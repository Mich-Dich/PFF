import os
from pathlib import Path
import webbrowser
import time


url = "https://vulkan.lunarg.com/sdk/home#linux"

class vulkan_configuration:
    required_vulkan_version = "1.3."
    install_vulkan_version = "1.3.296.0"

    @classmethod
    def validate(cls):
        if (not cls.check_vulkan_SDK()):
            print("Vulkan SDK not installed correctly.")
            return False
            
        if (not cls.check_vulkan_SDK_debug_libs()):
            print("\nNo Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.")
            return False
        
        else:
            return True

    @classmethod
    def check_vulkan_SDK(cls):
        found = False

        while (not found):
            vulkan_SDK = os.environ.get("VULKAN_SDK")
            if (vulkan_SDK is None):
                print("You don't have the Vulkan SDK installed!")
            elif (cls.required_vulkan_version not in vulkan_SDK):
                print(f"You don't have the correct Vulkan SDK version! (Engine requires {cls.required_vulkan_version})")
            else:
                found = True
                break

            if (cls.__install_vulkan_SDK() == True):
                input("Press enter to continue after instalation ...")
            else:
                return False

        print(f"\nLocated Vulkan SDK at {vulkan_SDK}")
        return True

    @classmethod
    def __install_vulkan_SDK(cls):
        permission_granted = False
        while not permission_granted:
            reply = str(input("Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(cls.install_vulkan_version))).lower().strip()[:1]
            if reply == 'n':
                return False
            permission_granted = (reply == 'y')

        print("Dont forget to add Vulkan to you path!")
        print("Packages to include:")
        print("     Vulkan Memory Allocator header")
        print("     Volk header, source, and library")
        print("     Shader Toolchain Debug Symbols - 64-Bit")

        time.sleep(1)  # Wait for 1 second before opening the browser
        webbrowser.open_new_tab(url)
        time.sleep(1)  # Wait for 1 second befor exiting script
        return True


    @classmethod
    def check_vulkan_SDK_debug_libs(cls):
        vulkan_SDK = os.environ.get("VULKAN_SDK")
        shadercdLib = Path(f"{vulkan_SDK}/Lib/shaderc_sharedd.lib")        
        return shadercdLib.exists()

if __name__ == "__main__":
    vulkan_configuration.validate()
