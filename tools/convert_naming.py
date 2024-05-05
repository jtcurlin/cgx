import argparse
import os
from pathlib import Path
import gitignore_parser

# Define the mappings for replacements based on the command-line argument
replacements = {
    'cgx': {
        'sge': 'cgx',
        'SGE': 'CGX',
        'senior game engine': 'curlin graphics engine',
        'Copyright © 2024 Jacob Curlin, Connor Cotturone, Chip Bevil, William Osborne': 'Copyright © 2024 Jacob Curlin',
    },
    'sge': {
        'cgx': 'sge',
        'CGX': 'SGE',
        'curlin graphics engine': 'senior game engine',
        'Copyright © 2024 Jacob Curlin': 'Copyright © 2024 Jacob Curlin, Connor Cotturone, Chip Bevil, William Osborne',
    }
}

def parse_arguments():
    parser = argparse.ArgumentParser(description="Process project files for cgx or sge.")
    parser.add_argument("option", choices=["cgx", "sge"], help="Option to process files for ('cgx' or 'sge').")
    return parser.parse_args()

def should_ignore(file_path, ignore_matcher, external_path):
    # Check if the file is in the external directory
    try:
        file_path.relative_to(external_path)
        return True
    except ValueError:
        # The file is not in the external directory
        return ignore_matcher(str(file_path))

def rename_file_if_needed(file_path, option):
    if option == 'cgx':
        new_name = file_path.name.replace('sge', 'cgx')
    elif option == 'sge':
        new_name = file_path.name.replace('cgx', 'sge')
    else:
        return file_path

    new_file_path = file_path.with_name(new_name)
    if new_name != file_path.name and not new_file_path.exists():
        file_path.rename(new_file_path)
        print(f"Renamed: {file_path} to {new_name}")
        return new_file_path  # Return new file path for further processing
    return file_path  # Return original path if no renaming occurred or new name already exists

def process_file(file_path, mapping):
    print(f"processing: {file_path}")
    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()

    new_content = content
    for old, new in mapping.items():
        new_content = new_content.replace(old, new)

    if new_content != content:
        with open(file_path, 'w', encoding='utf-8') as file:
            file.write(new_content)
        print(f"Processed: {file_path}")

def main():
    args = parse_arguments()
    script_path = Path(__file__).resolve()
    project_root = script_path.parent.parent  # Assuming the script is under {project_root}/tools
    external_path = project_root / 'external'
    gitignore_path = project_root / '.gitignore'

    if gitignore_path.exists():
        ignore_matcher = gitignore_parser.parse_gitignore(gitignore_path)
    else:
        ignore_matcher = lambda x: False

    for root, dirs, files in os.walk(project_root):
        for file in files:
            file_path = Path(root) / file
            if not should_ignore(file_path, ignore_matcher, external_path) and file_path.suffix in ['.cpp', '.h', '.txt', '.md', '.h.in']:
                updated_file_path = rename_file_if_needed(file_path, args.option)
                process_file(updated_file_path, replacements[args.option])

if __name__ == "__main__":
    main()
