from __future__ import annotations
import os
from pathlib import Path
from dataclasses import dataclass
from lts import LLVMIRProcessor
from looper.dcp_construction import build_dcp
from .bound_algorithm import *
from time import perf_counter_ns
from looper.config import Configuration
from looper.utils import analysis_logger

CLANG: Path = Path("/home/panekj/llvm-install/bin/clang")
CLANG_ARGS: str = "-Xclang -disable-O0-optnone -fno-discard-value-names"


class CompilationFailedException(Exception):
    pass

        
@dataclass
class FunctionAnalysisResult:
    function_name: str
    bound: str = 'UNKNOWN'
    message: str = ''
    analysis_time_in_ms: int = 0
    
    def to_dict(self) -> dict:
        return {self.function_name: {'bound': self.bound, 'message': self.message, 'analysis_time_in_ms': self.analysis_time_in_ms}}
        
def compile(filename) -> LLVMIRProcessor:
    # parse c source to LLVM IR

    status = os.system(f"{CLANG} {CLANG_ARGS} -emit-llvm -c {filename} -o llvm_ir.bc")
    if status:
        raise CompilationFailedException()
    #os.system(f"{CLANG} {CLANG_ARGS} -S -emit-llvm -c {filename} -o llvm_ir.ll")
    #os.system("opt -S -passes='dot-cfg' llvm_ir.ll -disable-output")
        
    m = LLVMIRProcessor()
    rv = m.load_module("llvm_ir.bc")
    if (rv == -1):
        raise CompilationFailedException()
    return m


def analyze_function(compiled_unit, function_name) -> FunctionAnalysisResult:
    start = perf_counter_ns()
    try:
        lts = compiled_unit.get_lts(function_name)
    except Exception:
        return FunctionAnalysisResult(function_name, "UNKNOWN", "Failed to construct LTS.")
    
    print(function_name)
    dcp, norms = build_dcp(lts)
    bound = total_bound(dcp, norms)
    
    if Configuration['graphs']:
        analysis_logger.save_in_directory('lts.dot', lts.convert_to_dot())
        analysis_logger.save_in_directory('dcp.dot', dcp.convert_to_dot())
    
    return FunctionAnalysisResult(function_name, str(bound) if bound else "Top", "", (perf_counter_ns() - start)/1000_000)
    
