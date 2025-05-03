"""Checker entry point"""

import sys
from pathlib import Path
import click

from rebel.config import Configuration
from rebel.bound_analysis import *
from rebel.utils.output import YAMLStorageManager, analysis_logger
from rebel.profiling import ProfilingManager

@click.command()
@click.option('--file', help="File to be analyzed.")
@Configuration.option('results', default=".",   type=click.STRING, help="Directory where the results are stored.")
@Configuration.option('analysis', 'vfg', type=click.BOOL, is_flag=True, help="Enable variable flow graph extension.")
@Configuration.option('analysis', 'rc',  type=click.BOOL, is_flag=True, help="Enable reset chain extension.")
@Configuration.option('profiling',              type=click.BOOL, is_flag=True, help="Gather profiling statistics.")
@Configuration.option('logging',               type=click.BOOL, is_flag=True,   help="Log analysis.")
@Configuration.option('graphs',                 type=click.BOOL, is_flag=True, help="Generate .dot for intemediate representations.")
def cli_run_analysis(file: Path, **config) -> None:
    # Load configuration
    Configuration.load_configuration(Path(__file__).parent.parent / 'configuration.yaml', **config)
    # Setup Logger

    # Run analysis
    sys.exit(run_analysis(file))


def run_analysis(filename: Path) -> int:
    result_directory = Path(Configuration['results'])
    if not os.path.isdir(result_directory.as_posix()):
        print(f"looper: \033[1;31merror:\033[0m: \033[1mDirectory '{result_directory}' does not exist.\033[0m", file=sys.stderr)
        return 1
            
    # parse c source to LLVM IR
    try:
        compiled_unit = compile(filename)
    except CompilationFailedException:
        # TODO: logging
        print("looper: \033[1;31merror:\033[0m \033[1mCompilation failed.\033[0m", file=sys.stderr)
        return 1
    
    result_directory /= 'results'
    if not os.path.isdir(result_directory.as_posix()):
        os.mkdir(result_directory.as_posix())
    
    analysis_result_manager = YAMLStorageManager((result_directory / 'bounds.yaml').as_posix())
    
    for function_name in compiled_unit.get_functions():
        
        # setup output directory for logging
        function_analysis_log_directory = result_directory / function_name
        if Configuration['profiling'] or Configuration['logging'] or Configuration['graphs']:
            if not os.path.isdir(function_analysis_log_directory):
                os.mkdir(function_analysis_log_directory)
        analysis_logger.set_output_path(function_analysis_log_directory)
        
        # Function Analysis
        function_analysis_result = analyze_function(compiled_unit, function_name)
        
        # saving analysis result
        analysis_result_manager.write(function_analysis_result.to_dict())
        
        # saving profiling statistics
        if Configuration['profiling']:
            ProfilingManager.save(function_analysis_log_directory / 'profiling.yaml')
            ProfilingManager.reset()
    
    return 0
    
    