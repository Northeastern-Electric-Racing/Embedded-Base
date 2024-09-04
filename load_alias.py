import os
import platform
import sys

def load_aliases(venv_path, aliases_file):
    os_type = platform.system()
    if os_type == 'Windows':
        activate_path = os.path.join(venv_path, 'Scripts', 'activate')  # Bash script for Git Bash on Windows
    else:
        activate_path = os.path.join(venv_path, 'bin', 'activate')  # bash script for Unix-like systems
    
    try:
        # Read existing aliases from the activation script
        if os.path.exists(activate_path):
            with open(activate_path, 'r') as activate_file:
                existing_aliases = activate_file.readlines()
        else:
            existing_aliases = []

        # Convert the existing aliases to a set for easy comparison
        existing_aliases_set = set()
        for line in existing_aliases:
            if line.startswith('alias '):
                alias_definition = line.strip()
                alias_name = alias_definition.split('=')[0].replace('alias ', '')
                existing_aliases_set.add(alias_name)

        # Read aliases from the provided aliases file
        with open(aliases_file, 'r') as f:
            aliases = f.readlines()

        # Prepare to write new aliases that aren't already in the activate script
        new_aliases = []
        for alias in aliases:
            alias_name, alias_command = alias.strip().split('=', 1)
            alias_name = alias_name.strip()
            alias_command = alias_command.strip('"')

            if alias_name not in existing_aliases_set:
                new_aliases.append(f'alias {alias_name}="{alias_command}"\n')

        # Append new aliases to the activation script if there are any
        if new_aliases:
            with open(activate_path, 'a') as activate_file:
                activate_file.write('\n# Aliases\n')
                activate_file.writelines(new_aliases)
            print(f"Added {len(new_aliases)} new alias(es) to the activation script.")
        else:
            print("No new aliases to add; all are already present.")

    except Exception as e:
        print(f"Failed to update aliases: {e}", file=sys.stderr)
        sys.exit(1)

def main():
    current_directory = os.path.dirname(os.path.abspath(__file__))
    parent_directory = os.path.dirname(current_directory)
    venv_path = os.path.join(parent_directory, 'ner-venv')
    aliases_file = os.path.join(current_directory, 'aliases.txt')
    
    load_aliases(venv_path, aliases_file)
    print("Close and reopen your terminal or the venv for changes to take effect.")
    
if __name__ == "__main__":
    main()