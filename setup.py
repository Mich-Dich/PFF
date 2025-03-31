
import os
import sys
import subprocess
import platform
import metadata.utils as utils
import metadata.organize_engine_wiki as organize_engine_wiki

# Load platform dependent script
from metadata.setup_python import python_configuration as python_requirements
if platform.system() == "Windows":
    import metadata.windows.win_utils as win_util
    win_util.enable_ansi_support()
    from metadata.windows.setup_vulkan import vulkan_configuration
    from metadata.windows.setup_premake import premake_configuration
    import metadata.windows.register_icon as register_icon
elif platform.system() == "Linux":
    from metadata.linux.setup_vulkan import vulkan_configuration
    from metadata.linux.setup_premake import premake_configuration
    from metadata.linux.setup_glslc import glslc_configuration
else:
    raise Exception("Unsupported operating system")














# 
# 
# 
# 
# 
# 














def detect_visual_studio_versions_windows():
    vs_version_map = {
        "17": "2022",  # VS 2022 (major version 17)
        "16": "2019",  # VS 2019 (major version 16)
        "15": "2017"   # VS 2017 (major version 15)
    }
    try:
        vswhere_path = os.path.join(os.environ["ProgramFiles(x86)"], "Microsoft Visual Studio", "Installer", "vswhere.exe")
        result = subprocess.run([vswhere_path, "-format", "json", "-products", "*", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64"], capture_output=True, text=True)
        if result.returncode == 0:
            import json
            installations = json.loads(result.stdout)
            versions = []
            for install in installations:
                major_version = install["installationVersion"].split('.')[0]
                premake_id = vs_version_map.get(major_version, None)
                if premake_id:
                    versions.append(premake_id)
            return list(set(versions))  # Remove duplicates
    except Exception as e:
        print(f"Error detecting Visual Studio versions: {e}")
    return []

def detect_rider_windows():
    # Check common Rider installation paths
    rider_paths = [
        os.path.join(os.environ["ProgramFiles"], "JetBrains", "JetBrains Rider *"),
        os.path.join(os.environ["LocalAppData"], "JetBrains", "Toolbox", "apps", "Rider", "*")
    ]
    for path in rider_paths:
        if os.path.exists(path):
            return True
    return False

def prompt_ide_selection_windows():
    ides = []
    vs_versions = detect_visual_studio_versions_windows()
    if vs_versions:
        ides.extend([f"Visual Studio {version}" for version in vs_versions])
    if detect_rider_windows():
        ides.append("JetBrains Rider")
    if not ides:
        print("No supported IDEs detected.")
        sys.exit(1)
    print("Detected IDEs:")
    for i, ide in enumerate(ides):
        print(f"{i}. {ide}")

    if len(ides) == 1:
        print("only one IDE detected")
        return ides[0]

    choice = input("Select an IDE to use (enter the number): ")
    try:
        choice_index = int(choice)
        if 0 <= choice_index < len(ides):
            return ides[choice_index]
        else:
            print("Invalid selection.")
            sys.exit(1)
    except ValueError:
        print("Invalid input.")
        sys.exit(1)














# 
# 
# 
# 
# 
# 













def detect_rider_linux():
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

def prompt_ide_selection_linux():
    ides = []
    
    # Detect available IDEs
    if detect_rider_linux():
        ides.append("JetBrains Rider")
    
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















# 
# 
# 
# 
# 
# 














def update_submodule(submodule_path, branch="main"):
    print(f"\nupdate submodule: {os.path.basename(submodule_path)}")
    try:
        os.chdir(submodule_path)
        subprocess.run(["git", "fetch", "origin"], check=True)
        subprocess.run(["git", "checkout", branch], check=True)
        subprocess.run(["git", "pull", "origin", branch], check=True)
        os.chdir(os.path.join("..", "..", ".."))
        utils.print_c(f"Successfully updated {submodule_path} to the latest commit.", "green")

    except subprocess.CalledProcessError as e:
        utils.print_c(f"Failed to update {submodule_path}: {e}", "red")
        sys.exit(1)


try:
    utils.print_u("\nCHECK FOR ANY UPDATES")
    print("updating main repo")
    try:
        subprocess.run(["git", "fetch", "origin"], check=True)
        subprocess.call(["git", "pull"])
        utils.print_c(f"Successfully updated main repo to the latest commit.", "green")
    except subprocess.CalledProcessError as e:
        utils.print_c(f"Failed to update main repo: {e}", "red")
        sys.exit(1)
    
    update_submodule("PFF/vendor/glfw", "master")
    update_submodule("PFF/vendor/imgui", "docking")
    update_submodule("PFF/vendor/ImGuizmo", "master")

    utils.print_u("\nCHECKING PYTHON SETUP")
    python_installed = python_requirements.validate()

    utils.print_u("\nCHECK VULKAN SETUP")
    vulkan_installed = vulkan_configuration.validate()

    utils.print_u("\nUPDATE WIKI")
    organize_engine_wiki.start(os.path.join(".github", "wiki"))

    # CLONE WIKI: git clone https://github.com/Mich-Dich/PFF.wiki.git .github/wiki

    utils.print_u("\nCHECK PREMAKE-5 SETUP")
    premake_installed = premake_configuration.validate()

    if (True == python_installed == vulkan_installed == premake_installed):
        if platform.system() == "Windows":

            utils.print_u("\nCHECK WORKSPACE SETUP")
            register_icon.register_icon()

            selected_ide = prompt_ide_selection_windows()
            if selected_ide.startswith("Visual Studio"):
                vs_year = selected_ide.split()[-1]  # Extract "2022" from "Visual Studio 2022"
                premake_action = f"vs{vs_year}"
                utils.print_u(f"\nBUILDING PFF (Procedurally Focused Framework) {selected_ide} Solution:")
                premake_result = subprocess.run(['vendor/premake/premake5.exe', premake_action], check=True)
            elif selected_ide == "JetBrains Rider":
                utils.print_u("\nBUILDING PFF (Procedurally Focused Framework) for JetBrains Rider:")
                premake_result = subprocess.run(['vendor/premake/premake5.exe', 'rider'], check=True)

            if premake_result.returncode != 0:
                utils.print_c(f"BUILD FAILED! the premake script encountered [{premake_result.returncode}] errors", "red")
            else:
                utils.print_c("BUILD SUCCESSFUL!", "green")

        else:               # because of [Load platform dependent script] only remaining option is Linux

            utils.print_u("\nCHECK GLSLC SETUP")
            glslc_installed = glslc_configuration.validate()
            
            utils.print_u("\nCHECK WORKSPACE SETUP")
            
            selected_ide = prompt_ide_selection_linux()
            premake_action = "gmake"  # Default action
            if selected_ide == "JetBrains Rider":
                premake_action = "rider"
            elif "Makefile" in selected_ide:
                premake_action = "gmake"
            
            premake_result = subprocess.run(['./premake5', premake_action], text=True)
            if premake_result.returncode != 0:
                utils.print_c(f"BUILD FAILED! the premake script encountered [{premake_result.returncode}] errors", "red")
            else:
                utils.print_c("BUILD SUCCESSFUL!", "green")

            utils.print_c("\nhelpful hints", "blue")
            print("  Cleanup all generated files:      rm -r bin bin-int && find . -name \"Makefile\" -exec rm -f {} +")
            print("  apply changed premake scripts:    ./premake5 gmake")
            print("  compile engine:                   make -j             (-j for multiprocessor compilation)")
            print("  for more help:                    ./premake5 --help OR visit[https://premake.github.io/docs/Using-Premake/]")
        
    else:
        # Print error message if any requirements failed
        failed_requirements = []
        if not python_installed:
            failed_requirements.append("Python")
        if not vulkan_installed:
            failed_requirements.append("Vulkan")
        if not premake_installed:
            failed_requirements.append("Premake5")

        failed_list = ", ".join(failed_requirements)
        utils.print_c(f"Missing requirements [{failed_list}] => ABORTING", "red")
except KeyboardInterrupt:
    print("\nProcess interrupted by user.")