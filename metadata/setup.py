# /home/mich/workspace/PFF/metadata/setup.py
from setuptools import setup, find_packages

setup(
    name="PFF-metadata",
    version="0.1.0",
    packages=find_packages(),         # finds `metadata` + subpackages
    include_package_data=True,
    # add any runtime deps here, if needed
    # install_requires=[],
)
