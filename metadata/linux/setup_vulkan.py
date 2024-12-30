import os
from pathlib import Path
import webbrowser
import time
import metadata.utils as utils


url = "https://vulkan.lunarg.com/sdk/home#linux"

class vulkan_configuration:
    required_vulkan_version = "1.3."
    install_vulkan_version = "1.3.296.0"

    @classmethod
    def validate(cls):
        if (not cls.check_vulkan_SDK()):
            utils.print_c("Vulkan SDK not installed correctly.", "red")
            return False
            
        if (not cls.check_vulkan_SDK_debug_libs()):
            utils.print_c("No Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.", "red")
            return False
        
        else:
            return True

    @classmethod
    def check_vulkan_SDK(cls):
        found = False

        while (not found):
            vulkan_SDK = os.environ.get("VULKAN_SDK")
            if (vulkan_SDK is None):
                utils.print_c(f"You don't have the Vulkan SDK installed! [{vulkan_SDK}]", "orange")
            elif (cls.required_vulkan_version not in vulkan_SDK):
                utils.print_c(f"You don't have the correct Vulkan SDK version! (Engine requires {cls.required_vulkan_version})", "orange")
            else:
                found = True
                break

            if (cls.__install_vulkan_SDK() == True):
                input("Press enter to continue after instalation ...")
            else:
                return False

        print(f"Located Vulkan SDK at {vulkan_SDK}")
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
        return True                             # TODO: Need to check for debug stuff

if __name__ == "__main__":
    vulkan_configuration.validate()
