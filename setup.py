
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
    import metadata.windows.IDE_selection as IDE_setup
elif platform.system() == "Linux":
    from metadata.linux.setup_vulkan import vulkan_configuration
    from metadata.linux.setup_premake import premake_configuration
    from metadata.linux.setup_glslc import glslc_configuration
    import metadata.linux.IDE_setup as IDE_setup
else:
    raise Exception("Unsupported operating system")




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

            selected_ide = IDE_setup.prompt_ide_selection()
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
            
            selected_ide = IDE_setup.prompt_ide_selection()
            premake_action = "gmake"  # Default action
            if selected_ide == "JetBrains Rider":
                premake_action = "rider"
            elif "VSCode" in selected_ide:
                premake_action = "gmake"
            elif "Makefile" in selected_ide:
                premake_action = "gmake"
            
            premake_result = subprocess.run(['./premake5', premake_action], text=True)
            if premake_result.returncode != 0:
                utils.print_c(f"BUILD FAILED! the premake script encountered [{premake_result.returncode}] errors", "red")
            else:
                utils.print_c("BUILD SUCCESSFUL!", "green")


            if "VSCode" in selected_ide:                                                # generate build/compile tasks
                premake_action = "gmake"
                build_config = IDE_setup.prompt_build_config()
                IDE_setup.setup_vscode_configs(os.getcwd(), build_config)

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