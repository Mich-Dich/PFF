
import os
import subprocess
import platform

def print_underlined(text):
    print(f"\033[4m{text}\033[0m")

from metadata.setup_python import python_configuration as python_requirements


# CHANGE SO IT LOADS THE CORRECT INSTALL SCRIPT
if platform.system() == "Windows":
    from metadata.windows.setup_vulkan import vulkan_configuration as vulkan_requirements
    from metadata.windows.setup_premake import premake_configuration as premake_requirements
elif platform.system() == "Linux":
    from metadata.linux.setup_vulkan import vulkan_configuration as vulkan_requirements
    from metadata.linux.setup_premake import premake_configuration as premake_requirements
else:
    raise Exception("Unsupported operating system")


print_underlined("\nCHECK FOR ANY UPDATES")
# subprocess.call(["git", "pull"])
print("git submodule update --init --recursive")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

print_underlined("\nCHECKING PYTHON SETUP")
python_installed = python_requirements.validate()

print_underlined("\nCHECK VULKAN SETUP")
vulkan_installed = vulkan_requirements.validate()

print_underlined("\nCHECK PREMAKE-5 SETUP")
premake_installed = premake_requirements.validate()

if (True == python_installed == vulkan_installed == premake_installed):
    print("\nALLS GOOD")
else:
    print("\nALLS NOT GOOD")
