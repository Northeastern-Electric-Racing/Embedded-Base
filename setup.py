from setuptools import setup, find_packages

# Read the contents of requirements.txt
def parse_requirements(filename):
    with open(filename, 'r') as file:
        return [line.strip() for line in file if line.strip() and not line.startswith('#')]

setup(
    name='ner-setup',  # The name of the package
    version='0.1',  # The package version
    packages=find_packages(),  # Automatically find packages in the current directory
    py_modules=['ner_setup', 'launchpad', 'clang_restage'],  # List of standalone modules
    install_requires=parse_requirements('requirements.txt'),  # Install dependencies from requirements.txt
    entry_points={
        'console_scripts': [
            'ner_setup=ner_setup:main',  # Command 'ner_setup' runs 'main' function in ner_setup.py
            'clang_restage=clang_restage:main',
            'launchpad=launchpad:main',
            'ner=build_system:main',  # Adding the entry point for 'ner'
        ],
    },
)
