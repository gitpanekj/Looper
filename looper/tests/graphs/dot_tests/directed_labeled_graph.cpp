#include "graphs/directed_labeled_graph.hpp"
#include <iostream>
#include <fstream>

#define TEST_RECORD(fname) {#fname, fname}

using namespace graphs;

namespace directed_labeled_graph_dot_tests
{
    std::string empty_graph()
    {
        DirectedLabeledGraph<std::string, std::string> empty_graph;
        return empty_graph.convert_to_dot();
    }

    std::string single_node()
    {
        DirectedLabeledGraph<std::string, std::string> single_node_graph;
        single_node_graph.add_node("Node 1");
        return single_node_graph.convert_to_dot();
    }

    std::string single_node_self_edge()
    {
        DirectedLabeledGraph<std::string, std::string> single_node_graph;
        int id = single_node_graph.add_node("Node 1");
        single_node_graph.add_edge(id, id, "Label");
        return single_node_graph.convert_to_dot();
    }

    std::string two_nodes()
    {
        DirectedLabeledGraph<std::string, std::string> two_node_graph;
        int id1 = two_node_graph.add_node("Node 1");
        int id2 = two_node_graph.add_node("Node 2");
        return two_node_graph.convert_to_dot();
    }

    std::string multiple_nodes()
    {
        DirectedLabeledGraph<std::string, std::string> two_node_graph;
        int id1 = two_node_graph.add_node("Node 1");
        int id2 = two_node_graph.add_node("Node 2");
        int id3 = two_node_graph.add_node("Node 3");
        int id4 = two_node_graph.add_node("Node 4");
        return two_node_graph.convert_to_dot();
    }

    std::string two_connected_nodes()
    {
        DirectedLabeledGraph<std::string, std::string> two_connected_node_graph;
        int id1 = two_connected_node_graph.add_node("Node 1");
        int id2 = two_connected_node_graph.add_node("Node 2");
        two_connected_node_graph.add_edge(id1, id2, "Label");
        return two_connected_node_graph.convert_to_dot();
    }

    std::string multiple_connected_nodes()
    {
        DirectedLabeledGraph<std::string, std::string> multiple_node_graph;
        int id1 = multiple_node_graph.add_node("Node 1");
        int id2 = multiple_node_graph.add_node("Node 2");
        int id3 = multiple_node_graph.add_node("Node 3");
        int id4 = multiple_node_graph.add_node("Node 4");
        multiple_node_graph.add_edge(id1, id1, "Label");
        multiple_node_graph.add_edge(id2, id2, "Label");
        multiple_node_graph.add_edge(id3, id3, "Label");
        multiple_node_graph.add_edge(id4, id4, "Label");
        multiple_node_graph.add_edge(id1, id2, "Label");
        multiple_node_graph.add_edge(id2, id3, "Label");
        multiple_node_graph.add_edge(id3, id4, "Label");
        multiple_node_graph.add_edge(id4, id1, "Label");
        return multiple_node_graph.convert_to_dot();
    }

    std::string two_nodes_multiple_edges()
    {
        DirectedLabeledGraph<std::string, std::string> two_node_graph_multiple_edges;
        int id1 = two_node_graph_multiple_edges.add_node("Node 1");
        int id2 = two_node_graph_multiple_edges.add_node("Node 2");
        two_node_graph_multiple_edges.add_edge(id1, id2, "Label 1");
        two_node_graph_multiple_edges.add_edge(id1, id2, "Label 2");
        two_node_graph_multiple_edges.add_edge(id1, id2, "Label 3");
        return two_node_graph_multiple_edges.convert_to_dot();
    }

    typedef struct
    {
        std::string name;
        std::string (*run)();
    } TestRecord;

    TestRecord tests[] = {
        TEST_RECORD(empty_graph),
        TEST_RECORD(single_node),
        TEST_RECORD(single_node_self_edge),
        TEST_RECORD(two_nodes),
        TEST_RECORD(multiple_nodes),
        TEST_RECORD(two_connected_nodes),
        TEST_RECORD(multiple_connected_nodes),
        TEST_RECORD(two_nodes_multiple_edges)};
}