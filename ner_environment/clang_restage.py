import subprocess
import sys
import os

def get_staged_files():
    # Run the git command to get staged files
    result = subprocess.run(['git', 'diff', '--cached', '--name-only'], stdout=subprocess.PIPE, text=True)
    
    # Get the list of staged files from stdout
    staged_files = result.stdout.splitlines()
    
    # Filter files based on their extensions
    filtered_files = [f for f in staged_files if f.endswith(('.c', '.cpp', '.h'))]
    
    return filtered_files

def restage_files(files):
    if files:
        subprocess.run(['git', 'add'] + files)
    else:
        print("No files to restage.")

def main():

    current_directory = os.getcwd()
    
    # Get a list of staged files
    staged_files = get_staged_files()
    
    # Run clang-format on staged files

    if "bedded" in current_directory:
        clang_format_path = os.path.join(current_directory, 'ner_environment', 'clang-format')
        result = subprocess.run(['clang-format', f'--style=file:{clang_format_path}', '-i'] + staged_files, 
                            capture_output=True, text=True)
    else:

        clang_format_path = os.path.join(current_directory, 'Drivers', 'Embedded-Base', 'ner_environment', 'clang-format')
        result = subprocess.run(['clang-format', f'--style=file:{clang_format_path}', '-i'] + staged_files, 
                            capture_output=True, text=True)

    if result.returncode == 0:
        print("clang-format passed. Restaging files.")
        restage_files(staged_files)
    else:
        print(current_directory)
        print(f"clang-format failed. Please fix the issues and commit again.(Most likely, just try commiting again) {clang_format_path}")
        sys.exit(1)

if __name__ == '__main__':
    main()