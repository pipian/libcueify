#!/usr/bin/env python
import os, shutil
from distutils.core import setup, Extension

if os.path.exists('swig/cueify.i'):
   if not os.path.exists('build/swig'):
       os.makedirs('build/swig')
   shutil.copyfile('swig/cueify.i', 'build/swig/cueify.i')

setup(
    name = 'cueify',
    version = '0.5.0',
    description = 'Python bindings for libcueify, a CD-metadata library'
    author = 'Ian Jacobi',
    author_email = 'pipian@pipian.com',
    url = 'https://github.com/pipian/libcueify',
    ext_modules=[Extension('_cueify', ['build/swig/cueify.i'],
                           include_dirs=['include', 'build/include'],
                           libraries=['cueify'],
                           swig_opts=['-modern', '-c++', '-Iinclude'])],
    packages = ['cueify']
)
