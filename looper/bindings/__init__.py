import ctypes
import os
__all__ = []

SO_FILES = [file for file in filter(lambda x: x.endswith(".so"), os.listdir())]

for so in SO_FILES:
    ctypes.CDLL(so)
    __all__.append(so.split(".")[0])
