import os
from pathlib import Path
import webbrowser
import time


url = "https://premake.github.io/download"

class premake_configuration:
    required_premake_version = "1.3."
    install_premake_version = "1.3.296.0"

    @classmethod
    def validate(cls):
        if (not cls.check_premake()):
            print("premake SDK not installed correctly.")
            return False
            
        if (not cls.check_premake_debug_libs()):
            print("\nNo premake SDK debug libs found. Install premake SDK with debug libs.")
            return False
        
        else:
            return True

    @classmethod
    def check_premake(cls):
        found = False

        while (not found):
            premake = os.environ.get("premake")
            if (premake is None):
                print("You don't have the premake SDK installed!")
            elif (cls.required_premake_version not in premake):
                print(f"You don't have the correct premake SDK version! (Engine requires {cls.required_premake_version})")
            else:
                found = True
                break

            if (cls.__install_premake() == True):
                input("Press enter to continue after instalation ...")
            else:
                return False

        print(f"\nLocated premake SDK at {premake}")
        return True

    @classmethod
    def __install_premake(cls):
        permission_granted = False
        while not permission_granted:
            reply = str(input("Would you like to install premakeSDK {0:s}? [Y/N]: ".format(cls.install_premake_version))).lower().strip()[:1]
            if reply == 'n':
                return False
            permission_granted = (reply == 'y')

        print("Dont forget to add Premak to you path!")
        time.sleep(1)  # Wait for 1 second before opening the browser
        webbrowser.open_new_tab(url)
        time.sleep(1)  # Wait for 1 second befor exiting script
        return True

    @classmethod
    def check_premake_debug_libs(cls):
        premake = os.environ.get("premake")
        shadercdLib = Path(f"{premake}/Lib/shaderc_sharedd.lib")        
        return shadercdLib.exists()

if __name__ == "__main__":
    premake_configuration.validate()
