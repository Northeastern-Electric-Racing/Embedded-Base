import subprocess
import sys
import os

def get_staged_files():
    result = subprocess.run(['git', 'diff', '--cached', '--name-only'], stdout=subprocess.PIPE, text=True)
    return result.stdout.splitlines()

def restage_files(files):
    if files:
        subprocess.run(['git', 'add'] + files)
    else:
        print("No files to restage.")

def main():
    # Get a list of staged files
    staged_files = get_staged_files()
    
    # Run clang-format on staged files
    result = subprocess.run(['clang-format', '-i', '--style=file', '--fallback-style=Google'] + staged_files, 
                            )

    if result.returncode == 0:
        restage_files(staged_files)
    else:
        print("clang-format failed. Please fix the issues and commit again.(Most likely, just try commiting again)")
        sys.exit(1)

if __name__ == '__main__':
    main()