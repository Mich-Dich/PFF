
import subprocess
import os
import shutil
import platform
import metadata.utils as utils

def parse_sidebar(sidebar_file):
    structure = {}
    current_path = []
    current_section = None

    with open(sidebar_file, 'r') as file:
        for line in file:
            line = line.rstrip()                                                            # Remove trailing whitespace
            stripped_line = line.lstrip()                                                   # Remove leading whitespace

            if stripped_line.startswith('## '):                                             # Section header
                current_section = stripped_line[3:]                                         # Remove '## ' prefix
                structure[current_section] = []
                current_path = [current_section]                                            # Reset current path to the new section
            elif stripped_line.startswith('- ['):                                           # Page link
                if current_section is not None:
                    page_name = stripped_line.split('](')[1].rstrip(')')                    # Get the part after '](' and remove the closing ')'
                    structure[current_section].append((current_path.copy(), page_name))     # Store the path and page name
            elif stripped_line.startswith("- **"):                                          # Subdirectory
                sub_dir_name = stripped_line.split('**')[1].strip()                         # Get the subdirectory name
                current_path.append(sub_dir_name)                                           # Add the subdirectory to the current path
            elif stripped_line.startswith('- '):                                            # Regular sub-item (not a page link)
                # Check the indentation level to determine if it's a subdirectory or a page
                indent_level = (len(line) - len(stripped_line)) // 2                        # Assuming 2 spaces per indent
                if indent_level > 0:                                                        # If there's indentation, it's a subdirectory
                    sub_dir_name = stripped_line[2:].strip()                                # Remove the '- ' prefix
                    current_path.append(sub_dir_name)                                       # Add the subdirectory to the current path
                else:
                    current_path = [current_section]                                        # If no indentation, reset the current path to the section
    return structure


def run_silent_subprocess(command):
    try:
        subprocess.run(command, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        pass


def start(target_dir):
    if os.path.exists(target_dir):
        shutil.rmtree(target_dir)
    
    run_silent_subprocess(fr'rmdir /s /q {os.path.join(target_dir, ".git")}')
    run_silent_subprocess(fr'git clone https://github.com/Mich-Dich/PFF.wiki.git {target_dir}')

    # Parse the sidebar to get the structure
    sidebar_file = os.path.join(target_dir, '_Sidebar.md')
    sidebar_structure = parse_sidebar(sidebar_file)

    # Organize files into directories
    for section, pages in sidebar_structure.items():
        for path, page in pages:
            # Create the full directory path based on the current path
            section_dir = os.path.join(target_dir, *path)                                   # Join the path components
            os.makedirs(section_dir, exist_ok=True)                                         # Create the directory if it doesn't exist
            
            source_file = os.path.join(target_dir, f"{page}.md")                            # Define the source file path
            
            if os.path.isfile(source_file):                                                 # Check if the file exists before moving
                shutil.move(source_file, os.path.join(section_dir, f"{page}.md"))           # Move the file to the corresponding section directory
            else:
                print(f"File not found: {source_file}")

    if platform.system() == "Windows":
        run_silent_subprocess(fr'rmdir /s /q {os.path.join(target_dir, ".git")}')
    elif platform.system() == "Linux":
        run_silent_subprocess(fr'rm -fr {os.path.join(target_dir, ".git")}')
    else:
        raise Exception("Unsupported operating system")

    try:
        os.remove(sidebar_file)
    except :
        pass

    utils.print_c(f"Downloaded and unpacked newest Engine Wiki", "green")
