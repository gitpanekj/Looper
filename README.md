# REBEL
Static complexity analysis tool for inference of tight upper bounds on a program execution cost.


## Installation Manual
A precompiled binary of REBEL is currently not available and building REBEL from the source is the only option.

First, Python version and virtual environment management system **pyenv** must be installed.
See https://github.com/pyenv/pyenv for more details.
Next, a virtual environment with Python version 3.10 or higher must be created and activated as follows:

```bash
pyenv install 3.12.2
pyenv virtualenv 3.12.2 rebel
pyenv activate rebel
```

At this point, the virtual environment **rebel** with Python version 3.12.2 is activated.

Subsequently, the Python modules implemented in C++ must be compiled.
The following dependencies must be installed first.
The specified versions of individual dependencies are those used throughout the development of REBEL.
- `cmake 3.31.5`
- `make 4.4.1`
- `clang 19.1.7`
- `llvm 19.1.7`


Note that the Python environment **rebel** must be activated at this point.
In order to build the Python modules implemented in C++ and install them in the activated environment, run the following commands from the top-level project directory:

```bash
cmake -S rebel -B rebel/build
make
make install
```


## User Manual

This section assumes that the REBEL tool is installed and the corresponding Python virtual environment is activated.
To test the functionality, several examples requiring amortized reasoning are located in the directory `examples`.

In order to run the analysis on the file `myfile.c`, use the following command:
```bash
rebel --file myfile.c
```

The directory in which the results will be stored can be specified as follows:
```bash
rebel --file myfile.c --results result_directory
```

Implicitly, a new folder `results` is created and the results are stored in `results/bounds.yaml`.