import os
from pathlib import Path
import metadata.linux.lin_utils as lin_utils
from .. import utils

class premake_configuration:
    premake_version = "5.0.0-beta4"
    premake_zip_urls = f"https://github.com/premake/premake-core/releases/download/v{premake_version}/premake-{premake_version}-linux.tar.gz"
    premake_license_url = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premake_directory = "./"

    @classmethod
    def validate(cls):
        if (not cls.check_premake()):
            utils.print_c("premake not installed correctly.", "red")
            return False
        else:
            lin_utils.ensure_executable(f"{cls.premake_directory}premake5")
            return True

    @classmethod
    def check_premake(cls):
        while True:
            premake_path = Path(f"{cls.premake_directory}premake5")
            if premake_path.exists():
                break                                                               # found preamke5

            utils.print_c("You don't have premake5 installed!", "orange")
            if (cls.__install_premake() == False):
                return False

        utils.print_c(f"Located premake", "green")
        return True

    @classmethod
    def __install_premake(cls):
        permission_granted = False
        while not permission_granted:
            reply = str(input("Would you like to install premake {0:s}? [Y/N]: ".format(cls.premake_version))).lower().strip()[:1]
            if reply == 'n':
                return False
            permission_granted = (reply == 'y')

        premakePath = f"{cls.premake_directory}/premake-{cls.premake_version}-linux.tar.gz"
        print("Downloading {0:s} to {1:s}".format(cls.premake_zip_urls, premakePath))
        utils.DownloadFile(cls.premake_zip_urls, premakePath)
        print("Extracting", premakePath)
        lin_utils.un_targz_file(premakePath, deleteTarGzFile=True)
        print(f"Premake {cls.premake_version} has been downloaded to '{cls.premake_directory}'")

        premakeLicensePath = os.path.join(cls.premake_directory, 'vendor', 'premake', 'LICENSE.txt')
        license_directory = os.path.dirname(premakeLicensePath)
        os.makedirs(license_directory, exist_ok=True)

        premakeLicensePath = f"{cls.premake_directory}/vendor/premake/LICENSE.txt"
        print("Downloading Premake License file{0:s} to {1:s}".format(cls.premake_license_url, premakeLicensePath))
        utils.DownloadFile(cls.premake_license_url, premakeLicensePath)

        return True

if __name__ == "__main__":
    premake_configuration.validate()
