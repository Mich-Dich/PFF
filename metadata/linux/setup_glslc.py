import os
from pathlib import Path
import metadata.linux.lin_utils as lin_utils
from .. import utils

class glslc_configuration:
    glslc_version = "latest"  # You can specify a version if needed
    glslc_zip_url = "https://example.com/path/to/glslc-linux.tar.gz"  # Replace with actual URL
    glslc_directory = "./"

    @classmethod
    def validate(cls):
        if not cls.check_glslc():
            utils.print_c("glslc not installed correctly.", "red")
            return False
        else:
            lin_utils.ensure_executable(f"{cls.glslc_directory}glslc")
            return True

    @classmethod
    def check_glslc(cls):
        glslc_path = Path("/usr/bin/glslc")
        if glslc_path.exists():
            utils.print_c("Located glslc", "green")
            return True

        utils.print_c("You don't have glslc installed!", "orange")
        if cls.__install_glslc():
            input("Press enter to continue after installation ...")
        else:
            return False

        return True

    @classmethod
    def __install_glslc(cls):
        permission_granted = False
        while not permission_granted:
            reply = str(input("Would you like to download glslc? [Y/N]: ")).lower().strip()[:1]
            if reply == 'n':
                return False
            permission_granted = (reply == 'y')

        glslc_path = f"{cls.glslc_directory}/glslc-linux.tar.gz"
        print("Downloading {0:s} to {1:s}".format(cls.glslc_zip_url, glslc_path))
        utils.DownloadFile(cls.glslc_zip_url, glslc_path)
        print("Extracting", glslc_path)
        lin_utils.un_targz_file(glslc_path, deleteTarGzFile=True)
        print(f"glslc has been downloaded to '{cls.glslc_directory}'")

        return True

if __name__ == "__main__":
    glslc_configuration.validate()
