# This is a modefied version of: https://github.com/TheCherno/Hazel/blob/master/scripts/SetupVulkan.py

import os
import sys
import subprocess
from pathlib import Path
import tkinter as tk
from tkinter import filedialog

from .. import utils

from io import BytesIO
from urllib.request import urlopen

class vulkan_configuration:
    required_vulkan_version = "1.3."
    install_vulkan_version = "1.3.296.0"

    @classmethod
    def validate(cls):
        if (not cls.check_vulkan_SDK()):
            print("Vulkan SDK not installed correctly.")
            return False
            
        if (not cls.check_vulkan_SDK_debug_libs()):
            print("No Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.")
            print("(see docs.hazelengine.com/GettingStarted for more info).")
            print("Debug configuration disabled.")
            return False
        return True

    @classmethod
    def check_vulkan_SDK(cls):
        found = False

        while (not found):
            vulkan_SDK = os.environ.get("VULKAN_SDK")
            if (vulkan_SDK is None):
                print("\nYou don't have the Vulkan SDK installed!")
                cls.__install_vulkan_SDK()
                input("Press enter to continue after instalation ...")
            elif (cls.required_vulkan_version not in vulkan_SDK):
                print(f"You don't have the correct Vulkan SDK version! (Engine requires {cls.required_vulkan_version})")
                cls.__install_vulkan_SDK()
                input("Press enter to continue after re-instalation ...")
            else:
                found = True

        utils.print_c(f"Located Vulkan SDK at {vulkan_SDK}", "green")
        return True

    @classmethod
    def __install_vulkan_SDK(cls):
        permission_granted = False
        while not permission_granted:
            reply = str(input("Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(cls.install_vulkan_version))).lower().strip()[:1]
            if reply == 'n':
                return
            permission_granted = (reply == 'y')

        # Open a file dialog to select the save location
        root = tk.Tk()
        root.withdraw()  # Hide the root window
        vulkanDirectory = filedialog.askdirectory(title="Select Save Location for Vulkan SDK Installer")
        
        if not vulkanDirectory:  # If the user cancels the dialog
            print("Installation canceled.")
            return

        vulkanPath = os.path.join(vulkanDirectory, f"VulkanSDK-{cls.install_vulkan_version}-Installer.exe")
        vulkanInstallURL = f"https://sdk.lunarg.com/sdk/download/{cls.install_vulkan_version}/windows/VulkanSDK-{cls.install_vulkan_version}-Installer.exe"
        
        print("Downloading {0:s} to {1:s}".format(vulkanInstallURL, vulkanPath))
        utils.DownloadFile(vulkanInstallURL, vulkanPath)
        print("Running Vulkan SDK installer...")
        print("Dont forget to add Vulkan to you path!")
        print("Packages to include:")
        print("     Vulkan Memory Allocator header")
        print("     Volk header, source, and library")
        print("     Shader Toolchain Debug Symbols - 64-Bit")
        os.startfile(os.path.abspath(vulkanPath))

    @classmethod
    def check_vulkan_SDK_debug_libs(cls):
        vulkan_SDK = os.environ.get("VULKAN_SDK")
        shadercdLib = Path(f"{vulkan_SDK}/Lib/shaderc_sharedd.lib")
        
        return shadercdLib.exists()

if __name__ == "__main__":
    vulkan_configuration.validate()
