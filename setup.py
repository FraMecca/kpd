from setuptools import setup, find_packages
import subprocess
import os

os.chdir ("./search")
subprocess.call (["python3", "setup.py", "install"])
os.chdir ('..')
setup (name='kpd',
        scripts=['kpd'],
        py_modules=['shell', 'util', 'config_file', 'MPDdatabase', 'formatter', 'cython_search'],
        packages=find_packages())
