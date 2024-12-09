set(LT_LLVM_INSTALL_DIR "" CACHE PATH "/home/panekj/llvm-install")
set(LT_LLVM_CMAKE_CONFIG_DIR "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/")
list(APPEND CMAKE_PREFIX_PATH "${LT_LLVM_CMAKE_CONFIG_DIR}")


find_package(LLVM 20.0.0 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/cpp_modules/include)
message(STATUS "Using LLVM_INCLUDE_DIRS in: ${LLVM_INCLUDE_DIRS}")

# if(NOT LLVM_ENABLE_RTTI)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
# endif()

add_library(lts_constructor STATIC lts_constructor.cpp cfg_utils.cpp)
target_link_libraries(lts_constructor graphs expression "LLVMWindowsManifest;LLVMXRay;LLVMLibDriver;LLVMDlltoolDriver;LLVMTextAPIBinaryReader;LLVMCoverage;LLVMLineEditor;LLVMX86TargetMCA;LLVMX86Disassembler;LLVMX86AsmParser;LLVMX86CodeGen;LLVMX86Desc;LLVMX86Info;LLVMOrcDebugging;LLVMOrcJIT;LLVMWindowsDriver;LLVMMCJIT;LLVMJITLink;LLVMInterpreter;LLVMExecutionEngine;LLVMRuntimeDyld;LLVMOrcTargetProcess;LLVMOrcShared;LLVMDWP;LLVMDebugInfoLogicalView;LLVMDebugInfoGSYM;LLVMOption;LLVMObjectYAML;LLVMObjCopy;LLVMMCA;LLVMMCDisassembler;LLVMLTO;LLVMPasses;LLVMHipStdPar;LLVMCFGuard;LLVMCoroutines;LLVMipo;LLVMVectorize;LLVMSandboxIR;LLVMLinker;LLVMInstrumentation;LLVMFrontendOpenMP;LLVMFrontendOffloading;LLVMFrontendOpenACC;LLVMFrontendHLSL;LLVMFrontendDriver;LLVMExtensions;LLVMDWARFLinkerParallel;LLVMDWARFLinkerClassic;LLVMDWARFLinker;LLVMGlobalISel;LLVMMIRParser;LLVMAsmPrinter;LLVMSelectionDAG;LLVMCodeGen;LLVMTarget;LLVMObjCARCOpts;LLVMCodeGenTypes;LLVMCGData;LLVMIRPrinter;LLVMInterfaceStub;LLVMFileCheck;LLVMFuzzMutate;LLVMScalarOpts;LLVMInstCombine;LLVMAggressiveInstCombine;LLVMTransformUtils;LLVMBitWriter;LLVMAnalysis;LLVMProfileData;LLVMSymbolize;LLVMDebugInfoBTF;LLVMDebugInfoPDB;LLVMDebugInfoMSF;LLVMDebugInfoDWARF;LLVMObject;LLVMTextAPI;LLVMMCParser;LLVMIRReader;LLVMAsmParser;LLVMMC;LLVMDebugInfoCodeView;LLVMBitReader;LLVMFuzzerCLI;LLVMCore;LLVMRemarks;LLVMBitstreamReader;LLVMBinaryFormat;LLVMTargetParser;LLVMTableGen;LLVMSupport;LLVMDemangle")


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bindings)


# Python bindings
pybind11_add_module(lts py_lts_constructor.cpp)
target_link_libraries(lts PRIVATE lts_constructor)
target_include_directories(lts PRIVATE ${CMAKE_SOURCE_DIR}/include)
set_target_properties(lts PROPERTIES
    OUTPUT_NAME "lts"
    PREFIX ""
    SUFFIX ".so")
