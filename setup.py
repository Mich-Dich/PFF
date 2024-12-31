
import subprocess
import platform
import metadata.utils as utils

# Load platform dependent script
from metadata.setup_python import python_configuration as python_requirements
if platform.system() == "Windows":
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

    utils.print_u("\nCHECK PREMAKE-5 SETUP")
    premake_installed = premake_requirements.validate()

    utils.print_u("\nSETUP WORKSPACE")

    if (True == python_installed == vulkan_installed):
        if platform.system() == "Windows":

            # INVOKE REGISTER SCRIPT HERE
            # register_script_path = os.path.join(os.path.dirname(register_icon.__file__), 'register_icon.py')
            # result = subprocess.run([sys.executable, register_script_path], check=True)

            print("Should create VS Solution")      # should detect the most reason VS installed
        else:               # because of [Load platform dependent script] only remaining option is Linux

            # Run the command './premake5 gmake'
            result = subprocess.run(['./premake5', 'gmake'], text=True)

            print("Should setup workspace")
    else:
        # Print error message if any requirements failed
        failed_requirements = []
        if not python_installed:
            failed_requirements.append("Python")
        if not vulkan_installed:
            failed_requirements.append("Vulkan")

        failed_list = ", ".join(failed_requirements)
        utils.print_c(f"Missing requirements [{failed_list}] => ABORTING", "red")
except KeyboardInterrupt:
    print("\nProcess interrupted by user.")
