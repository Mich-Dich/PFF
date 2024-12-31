
import metadata.utils as utils
import sys
import subprocess
import importlib.util as importlib_util

class python_configuration:
    @classmethod
    def validate(cls):
        if not cls.__validate_python():
            return False

        for packageName in ["requests"]:
            if not cls.__validate_package(packageName):
                return False
        return True

    @classmethod
    def __validate_python(cls, versionMajor = 3, versionMinor = 3):
        if sys.version is not None:
            utils.print_c("Python version {0:d}.{1:d}.{2:d} detected.".format(sys.version_info.major, sys.version_info.minor, sys.version_info.micro), "green")
            if sys.version_info.major < versionMajor or (sys.version_info.major == versionMajor and sys.version_info.minor < versionMinor):
                print("Python version too low, expected version {0:d}.{1:d} or higher.".format( \
                    versionMajor, versionMinor))
                return False
            return True

    @classmethod
    def __validate_package(cls, packageName):
        if importlib_util.find_spec(packageName) is None:
            return cls.__install_package(packageName)
        return True

    @classmethod
    def __install_package(cls, packageName):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("Would you like to install Python package '{0:s}'? [Y/N]: ".format(packageName))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')
        
        print(f"Installing {packageName} module...")
        subprocess.check_call(['python', '-m', 'pip', 'install', packageName])

        return cls.__validate_package(packageName)

if __name__ == "__main__":
    python_configuration.validate()
