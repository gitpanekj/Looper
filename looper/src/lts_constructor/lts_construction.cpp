

#include <memory>
#include <stack>
#include <set>

#include "llvm/IR/Function.h" // Function

#include "lts_constructor/cfg_utils.hpp"        // getFunctionName, getFunctionReturnType, getFunctionParameters
#include "graphs/labeled_transition_system.hpp" // LabeledTransitionSystem
#include "graphs/lts_labels.hpp"                // LTSTransitionLabel
#include "lts_constructor/LLVMIRInterpreter/lts_label_builder.hpp"
#include "lts_constructor/LLVMIRInterpreter/LTS_execution_context.hpp"

using LTS = graphs::LabeledTransitionSystem;


LTS constructLTSFromCFG(llvm::Function *cfg)
{
    // TODO: refactor description of the STACK
    // Stack of "edges" to be processed during CFG traversal.
    // Edge is a tuple (LTS location, previous CFG basic block, CFG basic block).
    // A CFG basic block is direct or indirect successor of the basic block
    // the LTS location was created for during the previous CFG traversal.
    // A CFG basic block is indirect successor only in case when all predecessor
    // up to the basic block represented by location has exactly one predecossor and
    // exactly one successor.
    // At the beginning the (l_s, cfg_entry_block) is inserted.
    std::stack<std::tuple<int, llvm::BasicBlock *, llvm::BasicBlock *>> edges_to_process_stack;
    // visited basic blocks
    std::set<llvm::BasicBlock *> visited_basic_blocks;
    std::set<llvm::BasicBlock *> processed_basic_blocks;
    // mapping of basic blocks to locations
    std::unordered_map<llvm::BasicBlock *, int> basic_block_to_location_id;
    std::unordered_set<int> potential_loop_heads;

    // TODO: label mapping
    TransitionExecutionContext forming_label;
    LTSLabelBuilder::LTSLabelBuilder label_builder;

    // LTS = ({l_s, l_e}, {}, l_s, l_e)
    // auto lts = std::make_shared<LTS>();
    LTS _lts;
    LTS* lts = &_lts;
    lts->add_start_location("start location");
    lts->add_end_location("exit location");

    // Processing metadata of the function
    lts->set_function_name(getFunctionName(cfg));
    lts->set_return_type(getFunctionReturnType(cfg));
    lts->add_parameters(getFunctionParameters(cfg));

    // NOTE: probably as a result of performed clang optimizations
    // a function may be left without any basic block in that case,
    // then an empty LTS is returned
    // TODO: inspect this problem in depth and remove this workaround
    if (cfg->empty())
    {
        lts->add_edge(lts->add_start_location(), lts->add_end_location());
        return _lts;
    }

    // CFG Traversal
    int location_id;
    llvm::BasicBlock *basic_block;
    llvm::BasicBlock *previous_basic_block;


    edges_to_process_stack.push({lts->get_start_location(), &cfg->getEntryBlock(), &cfg->getEntryBlock()});
    while (!edges_to_process_stack.empty())
    {
        // Top
        std::tie(location_id, previous_basic_block, basic_block) = edges_to_process_stack.top();

        //std::cout << "Location: " << location_id << "  BasicBlock: " << basic_block->getName().str() << std::endl;

        // Type, visited
        BasicBlockType basic_block_type = getBasicBlockType(basic_block);

        // All the successors processed?
        if (isProcessed(processed_basic_blocks, basic_block)){
            edges_to_process_stack.pop();
            // operations to be performed after all the successors are processed
            potential_loop_heads.erase(basic_block_to_location_id[basic_block]);
            continue;
        }
        
        processed_basic_blocks.insert(basic_block);
        visited_basic_blocks.insert(basic_block);

        // Instruction processing
        //std::cout << "Processing instructions to unvisited block" << std::endl;
        forming_label.check_target_node_of_jump(basic_block->getName().str());
        forming_label.previous_basic_block = previous_basic_block;
        for (const auto &inst : *basic_block){
            // Instruction interpretation over the context
            try {
                label_builder.interpretInstruction(&inst, forming_label);
            } catch(const InstructionNotImplemented &e) 
            {
                // Invalidate  unsupported instruction and its result
                if (inst.getName().str().size() > 0)
                {
                    forming_label.invalidated_variables.insert(inst.getName().str());
                    //std::cout << "Unknown Instruciton -- > Invalidating " << inst.getName().str() << std::endl;
                }
            }  catch(const InvalidatedValue &e) 
            {
                // Invalidate instruction and its result because value of an operand is invalid
                if (inst.getName().str().size() > 0)
                {
                    forming_label.invalidated_variables.insert(inst.getName().str());
                    //std::cout << "Unknown Operand -- > Invalidating " << inst.getName().str() << std::endl;
                }
            }
        }

        if (!basic_block_type.is(BasicBlockType::Property::Intermediary)){
            // Generate new edge and location
            int src = location_id;
            int dst = lts->add_vertex(std::string(basic_block->getName()));
            basic_block_to_location_id[basic_block] = dst;

            
            int edge_id = lts->add_edge(src, dst, forming_label.get_transition_label());

            // if the basic block has exactly one predecessor and exactly one successor
            // location is not generated and the the edge will start
            // in the previous generated location
            location_id =  basic_block_to_location_id[basic_block]; // dst

            // potential loop heads
            if (basic_block_type.is(BasicBlockType::Property::Branching) &&
                basic_block_type.is(BasicBlockType::Property::Join) &&
                !lts->is_loop_head(location_id))
            {
                potential_loop_heads.insert(location_id);
            }
        }

        // add edge to LTS exit location for terminating basic block
        if (basic_block_type.is(BasicBlockType::Property::Terminating))
        {
            lts->add_edge(basic_block_to_location_id[basic_block], lts->get_end_location());
        }

        // Push edges to be processed
        for (int i = 0; i < basic_block->getTerminator()->getNumSuccessors(); i++)
        {
            llvm::BasicBlock* successor = basic_block->getTerminator()->getSuccessor(i);
            if (isVisited(visited_basic_blocks, successor)){
                // create new edge along with statements
                int src = location_id;
                int dst = basic_block_to_location_id[successor];
                
                //std::cout << "Processing instructions leading to visited block" << std::endl;
                // Processing the state changing instruction which are execuded before jump instruction NOTE:  works for testing example, may need extension in the future
                // check whether the node is not target of a conditional jump, if so, condition is added to label - NOTE: here covers edge (src, dst) where src=dst
                forming_label.check_target_node_of_jump(successor->getName().str()); // check whether the node is not target of a conditional jump, if so, condition is added to label
                forming_label.previous_basic_block = basic_block;
                for (const auto &inst : *successor){
                    // Instruction interpretation over the context
                    try {
                        label_builder.interpretInstruction(&inst, forming_label);
                    } catch(const InstructionNotImplemented &e) 
                    {
                        // Invalidate  unsupported instruction and its result
                        if (inst.getName().str().size() > 0)
                        {
                            forming_label.invalidated_variables.insert(inst.getName().str());
                            //std::cout << "Unknown Instruciton -- > Invalidating " << inst.getName().str() << std::endl;
                        }
                    }  catch(const InvalidatedValue &e) 
                    {
                        // Invalidate instruction and its result because value of an operand is invalid
                        if (inst.getName().str().size() > 0)
                        {
                            forming_label.invalidated_variables.insert(inst.getName().str());
                            //std::cout << "Unknown Operand -- > Invalidating " << inst.getName().str() << std::endl;
                        }
                    }
                }
                int edge_id = lts->add_edge(src, dst, forming_label.get_transition_label());

                // loop head confirmation
                if (potential_loop_heads.find(dst) != potential_loop_heads.end())
                {
                    lts->mark_loop_head(dst);
                    potential_loop_heads.erase(dst);
                }

                if (lts->is_loop_head(dst))
                {
                    lts->mark_back_edge(edge_id);
                }
                continue;
            }

            edges_to_process_stack.push({location_id, basic_block, successor});
        }
    }
    return _lts;
}


