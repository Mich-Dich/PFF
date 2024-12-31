
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
    from metadata.windows.setup_vulkan import vulkan_configuration as vulkan_requirements
    from metadata.windows.setup_premake import premake_configuration as premake_requirements
    import metadata.windows.register_icon as register_icon
elif platform.system() == "Linux":
    from metadata.linux.setup_vulkan import vulkan_configuration as vulkan_requirements
    from metadata.linux.setup_premake import premake_configuration as premake_requirements
else:
    raise Exception("Unsupported operating system")


try:
    utils.print_u("\nCHECK FOR ANY UPDATES")
    # subprocess.call(["git", "pull"])
    print("git submodule update --init --recursive")
    subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

    utils.print_u("\nCHECKING PYTHON SETUP")
    python_installed = python_requirements.validate()

    utils.print_u("\nCHECK VULKAN SETUP")
    vulkan_installed = vulkan_requirements.validate()

    utils.print_u("\nUPDATE WIKI")
    organize_engine_wiki.start(os.path.join(".github", "wiki"))

    # CLONE WIKI: git clone https://github.com/Mich-Dich/PFF.wiki.git .github/wiki

    utils.print_u("\nCHECK PREMAKE-5 SETUP")
    premake_installed = premake_requirements.validate()

    if (True == python_installed == vulkan_installed == premake_installed):
        if platform.system() == "Windows":

            utils.print_u("\nCHECK WORKSPACE SETUP")
            register_icon.register_icon()

            utils.print_u("\nBUILDING PFF (Procedurally Focused Framework) VS 2022 Solution:")
            premake_result = subprocess.run(['vendor/premake/premake5.exe', 'vs2022'], check=True)

            if premake_result.returncode != 0:
                utils.print_c(f"BUILD FAILED! the premake script encountered [{premake_result.returncode}] errors", "red")
            else:
                utils.print_c("BUILD SUCCESSFUL!", "green")

        else:               # because of [Load platform dependent script] only remaining option is Linux

            utils.print_u("\nCHECK WORKSPACE SETUP")
            result = subprocess.run(['./premake5', 'gmake'], text=True)
        
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
