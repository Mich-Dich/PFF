
import os
import sys
import glob
import platform
import subprocess
import stat
import json
import datetime

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
import utils

def detect_rider():
    # Check common Rider installation paths on Linux
    rider_paths = [
        os.path.expanduser("~/.local/share/JetBrains/Toolbox/apps/Rider"),
        "/opt/JetBrains Rider",
        os.path.expanduser("~/.local/share/applications/jetbrains-rider.desktop")
    ]
    
    # Check if rider command exists in PATH
    try:
        subprocess.run(["which", "rider"], check=True, capture_output=True)
        return True
    except subprocess.CalledProcessError:
        pass

    # Check physical paths
    for path in rider_paths:
        if os.path.exists(path) or len(glob.glob(path)) > 0:
            return True
    return False


def detect_vscode():
    # Check if the `code` CLI is available
    try:
        subprocess.run(["which", "code"], check=True, capture_output=True)
        return True
    except subprocess.CalledProcessError:
        pass

    # Check common VSCode install paths (Linux)
    vscode_paths = [
        "/usr/share/code",
        "/opt/visual-studio-code",
        os.path.expanduser("~/.vscode"),
        os.path.expanduser("~/.var/app/com.visualstudio.code"),  # Flatpak
    ]
    for path in vscode_paths:
        if os.path.exists(path) or glob.glob(path):
            return True

    return False


def prompt_ide_selection():
    ides = []
    
    # Detect available IDEs
    if detect_rider():
        ides.append("JetBrains Rider")
    
    if detect_vscode():
        ides.append("VSCode")

    # Always include Makefile option
    ides.append("Makefile (CLion/Ninja compatible)")
    
    if not ides:
        print("No supported IDEs detected.")
        sys.exit(1)
        
    print("\nDetected IDEs:")
    for i, ide in enumerate(ides):
        print(f"{i}. {ide}")

    if len(ides) == 1:
        print("Only one IDE detected")
        return ides[0]

    while True:
        choice = input("Select an IDE to use (enter the number): ")
        try:
            choice_index = int(choice)
            if 0 <= choice_index < len(ides):
                return ides[choice_index]
            print("Invalid selection number.")
        except ValueError:
            print("Please enter a valid number.")


def prompt_build_config():
    configs = ["Debug", "RelWithDebInfo", "Release"]
    print("\nAvailable build configurations:")
    for i, cfg in enumerate(configs):
        print(f"{i}. {cfg}")
    
    while True:
        choice = input("Select a configuration (enter the number): ")
        try:
            index = int(choice)
            if 0 <= index < len(configs):
                return configs[index]
            else:
                print("Invalid number.")
        except ValueError:
            print("Please enter a valid number.")


def setup_vscode_configs(project_root, build_config):
    vscode_dir = os.path.join(project_root, ".vscode")
    os.makedirs(vscode_dir, exist_ok=True)

    # Use project_root consistently instead of hardcoding
    arch = "x86_64"
    system = platform.system().lower()  # "linux" or "windows"
    output_dir = f"{build_config}-{system}-{arch}"
    bin_dir = os.path.join(project_root, "bin", output_dir)
    
    editor_exe = os.path.join(bin_dir, "PFF_editor", "PFF_editor")
    test_project = os.path.abspath(os.path.join(project_root, "..", "PFF_test_projects", "test_project.pffproj"))

    # 1. Create pff_build.sh
    build_script_path = os.path.join(vscode_dir, "pff_build.sh")

    timestamp = datetime.datetime.now().strftime("%Y-%m-%d--%H:%M:%S")
    stage_name = f"PFF_{build_config}_{timestamp}"
    build_script_content = f"""#!/usr/bin/env bash
set -e

# staging folder name
STAGE_DIR="{bin_dir}/{stage_name}"

# create it
mkdir -p "$STAGE_DIR"

# move all previous artifacts into staging
mv "{bin_dir}/PFF"              "$STAGE_DIR/" 2>/dev/null || true
mv "{bin_dir}/PFF_helper"       "$STAGE_DIR/" 2>/dev/null || true
mv "{bin_dir}/PFF_editor"       "$STAGE_DIR/" 2>/dev/null || true
mv "{bin_dir}/vendor"           "$STAGE_DIR/" 2>/dev/null || true

# now trash the entire staging directory
gio trash "$STAGE_DIR" --force || true

# back to project root and regenerate
cd "{project_root}"
find . -name "Makefile" -delete
./premake5 gmake
gmake -j
    """
    with open(build_script_path, "w") as f:
        f.write(build_script_content)

    os.chmod(build_script_path, os.stat(build_script_path).st_mode | stat.S_IEXEC)

    # 2. tasks.json
    tasks_json_path = os.path.join(vscode_dir, "tasks.json")
    tasks_data = {
        "version": "2.0.0",
        "tasks": [
            {
                "label": "Clean & Build PFF",
                "type": "shell",
                "command": "${workspaceFolder}/.vscode/pff_build.sh",
                "problemMatcher": [],
                "group": { "kind": "build", "isDefault": True },
                "presentation": {
                    "reveal": "always",
                    "panel": "shared"
                }
            }
        ]
    }
    with open(tasks_json_path, "w") as f:
        json.dump(tasks_data, f, indent=4)

    # 3. launch.json
    launch_json_path = os.path.join(vscode_dir, "launch.json")
    program_path = os.path.join("${workspaceFolder}", "bin", output_dir, "PFF_editor", "PFF_editor")
    cwd_path = os.path.join("${workspaceFolder}", "bin", output_dir)

    launch_data = {
        "version": "0.2.0",
        "configurations": [
            {
                "name": f"Launch PFF_editor ({build_config})",
                "type": "cppdbg",
                "request": "launch",
                "program": program_path,
                "args": [test_project],
                "cwd": cwd_path,
                "preLaunchTask": "Clean & Build PFF",
                "stopAtEntry": False,
                "environment": [],
                "externalConsole": False,
                "MIMode": "gdb",
                "miDebuggerPath": "/usr/bin/gdb",
                "setupCommands": [
                    {
                        "description": "Enable pretty-printing for gdb",
                        "text": "-enable-pretty-printing",
                        "ignoreFailures": True
                    }
                ]
            }
        ]
    }
    with open(launch_json_path, "w") as f:
        json.dump(launch_data, f, indent=4)

    print("VSCode integration files generated in .vscode/")
    utils.print_c("\nVSCode usage", "blue")
    print("  Build the project:           Ctrl+Shift+B (runs 'Clean & Build PFF')")
    print("  Launch the editor:           F5 (runs the debugger with selected config)")
