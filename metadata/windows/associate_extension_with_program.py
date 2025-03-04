

import winreg
import os
import sys
import ctypes


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


def check_extension_exists(extension):
    try:
        key_path = f".{extension}"
        with winreg.OpenKey(winreg.HKEY_CLASSES_ROOT, key_path, 0, winreg.KEY_READ) as key:
            return True
    except WindowsError:
        return False
    

def set_file_association(extension, program_path):
    print(f"Using icon file: [{program_path}]")
    try:

        with winreg.CreateKey(winreg.HKEY_CLASSES_ROOT, "PFF-Project\\shell\\open\\command") as key:
            command = f'"{program_path}" "%1"'
            winreg.SetValue(key, "", winreg.REG_SZ, command)

        print("File association and custom icon successfully registered.")
        return True
    except Exception as e:
        return str(e)


if __name__ == "__main__":    
    extension = "pffproj".replace(".", "")
    script_dir = os.path.dirname(os.path.abspath(__file__))                             # Get the current script's directory
    program_path = os.path.abspath(os.path.join("..", "bin", "Debug-windows-x86_64", "PFF_editor", "PFF_editor.exe"))

    # if check_extension_exists(extension):                                               # Check if extension exists before requesting admin
    #     print(f"The file extension .{extension} is already registered")
    #     sys.exit(0)

    if not os.path.exists(program_path):                                                   # Ensure icon file exists
        print("Error: The specified icon file [PFF_editor.exe] was not found.")
        input("Press any key to continue...")

    
    print("The PFF Game Engine uses the [.pffproj] file extension and requests to register a custom icon for it.")
    print("This action requires administrator privileges but is not strictly necessary for the engine's functionality.")
    elevate_privileges()
    result = set_file_association(extension, program_path)
    if result is True:
        print(f"Successfully registered the '.{extension}' file extension with the PFF-icon.")
    else:
        print(f"Error: {result}")

    input("Press any key to continue...")
