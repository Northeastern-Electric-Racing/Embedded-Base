from setuptools import setup

# Read the contents of requirements.txt
def parse_requirements(filename):
    with open(filename, 'r') as file:
        return [line.strip() for line in file if line.strip() and not line.startswith('#')]

setup(
    name='ner-setup',
    version='0.1',
    py_modules=['ner_setup', 'launchpad', 'clang_restage', 'miniterm'],
    install_requires=parse_requirements('requirements.txt'),
    entry_points={
        'console_scripts': [
            'ner_setup=ner_setup:main',  # Command 'ner_setup' runs 'main' function in ner_setup.py
            'clang_restage=clang_restage:main',
            'serial=miniterm:main',
            'launchpad=launchpad:main',
           
        ],
    },

    
)