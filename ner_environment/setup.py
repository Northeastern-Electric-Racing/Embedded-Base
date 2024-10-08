from setuptools import setup, find_packages
import os

# Read the contents of requirements.txt
def parse_requirements(filename):
    with open(filename, 'r') as file:
        return [line.strip() for line in file if line.strip() and not line.startswith('#')]

cur_directory = os.path.abspath(os.path.dirname(__file__))
setup(
    name='ner-setup', 
    version='0.1',  
    packages=find_packages(include=['ner_environment', 'ner_environment.*']),  
    #packages=['build_system'],
    install_requires=parse_requirements('requirements.txt'),  
    entry_points={
        'console_scripts': [
            'ner=build_system:main', 
        ],
    },
)
