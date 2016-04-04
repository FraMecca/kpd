from setuptools import setup, find_packages
import os
setup (name='kpd',
        scripts=['kpd'],
        py_modules=['shell', 'util', 'config_file', 'MPDdatabase'],
        packages=find_packages())
