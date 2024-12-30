import winreg
import os
import sys
import ctypes

# Define constants for SHChangeNotify
SHCNE_ASSOCCHANGED = 0x8000000
SHCNF_IDLIST = 0x0


def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False


def elevate_privileges():
    if not is_admin():
        response = input("Would you like to give admin privileges? (Y/N): ").lower()
        if response == 'y':
            script_path = os.path.abspath(sys.argv[0])
            ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, f'"{script_path}"', os.path.dirname(script_path), 1)
            sys.exit(0)
        else:
            print(f"Error getting admin privileges")
            sys.exit(1)


def remove_file_association(extension):
    try:
        # Remove the file extension association
        key_path = f".{extension}"
        winreg.DeleteKey(winreg.HKEY_CLASSES_ROOT, key_path)

        # Remove the file type information
        winreg.DeleteKey(winreg.HKEY_CLASSES_ROOT, "PFF-Project")

        print(f"Successfully unregistered the '.{extension}' file extension and its associated icon.")
        return True
    except WindowsError as e:
        return str(e)


def notify_shell():
    ctypes.windll.shell32.SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, None, None)


if __name__ == "__main__":
    extension = "pffproj"

    print("This script will unregister the file extension and icon for the PFF-Project.")
    elevate_privileges()
    result = remove_file_association(extension)
    if result is True:
        print(f"Successfully unregistered the '.{extension}' file extension.")
        notify_shell()  # Notify the shell of the change
    else:
        print(f"Error: {result}")

    input("Press any key to continue...")