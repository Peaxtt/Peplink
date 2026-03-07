from setuptools import find_packages
from setuptools import setup

setup(
    name='pttep_alignment',
    version='1.2.0',
    packages=find_packages(
        include=('pttep_alignment', 'pttep_alignment.*')),
)
