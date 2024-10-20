from setuptools import setup, find_packages

# Read the contents of requirements.txt
def parse_requirements(filename):
    with open(filename, 'r') as file:
        return [line.strip() for line in file if line.strip() and not line.startswith('#')]

setup(
    name='ner-enviroment',
    version='0.2',  
    packages=find_packages(include=['build_system', 'build_system.*']), 
    install_requires=parse_requirements('requirements.txt'),  
    entry_points={
        'console_scripts': [
            'ner=build_system:app',  
            'clang_restage=build_system.clang_restage:main'
        ],
    },
)
