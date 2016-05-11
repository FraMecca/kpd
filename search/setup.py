from distutils.core import  Extension, setup
from Cython.Build import cythonize

ext = Extension (name = "cython_search", sources = ["list_kpd.c", "C_search.c", "cython_search.pyx", "filter.c"],extra_compile_args=["-O2"], extra_link_args=["-lz"] )
setup (ext_modules = cythonize (ext))

