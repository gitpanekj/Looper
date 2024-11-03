"""Checker entry point"""

import sys, os
from pathlib import Path
import click
from bindings import lts

CLANG: Path = Path("/home/panekj/llvm-install/bin/clang")
CLANG_ARGS: str = "-Xclang -disable-O0-optnone -fno-discard-value-names"

@click.command()
@click.option('--file', help="File to be analyzed.")
def cli_run_analysis(file: Path) -> None:
    status = run_analysis(file)
    sys.exit(status)


def run_analysis(filename: Path) -> int:
    # parse c source to LLVM IR
    os.system(f"{CLANG} {CLANG_ARGS} -emit-llvm -c {filename} -o llvm_ir.bc")
    
    m = lts.LLVM_Module()

    rv = m.load_module("llvm_ir.bc")
    if (rv == -1):
        return -1
    
    funcs = m.get_functions()
    for f in funcs:
        l = m.get_lts(f)
        print(l.convert_to_dot())
    
    
 
    return 0
    
    