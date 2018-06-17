import ctypes

lib=ctypes.cdll.LoadLibrary('./print.so')
lib.f()
