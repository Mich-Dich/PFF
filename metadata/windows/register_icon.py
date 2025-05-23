import winreg
import os
import sys
import ctypes
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
            return True
        else:
            print(f"Admin privileges not granted.\n")
            return False
    else:
        return True


def check_extension_exists(extension):
    try:
        key_path = f".{extension}"
        with winreg.OpenKey(winreg.HKEY_CLASSES_ROOT, key_path, 0, winreg.KEY_READ) as key:
            return True
    except WindowsError:
        return False
    

def set_file_association(extension, icon_path):
    print(f"Using icon file: [{icon_path}]")
    try:
        with winreg.CreateKey(winreg.HKEY_CLASSES_ROOT, f".{extension}") as key:        # Create .pffproj file extension association
            winreg.SetValue(key, "", winreg.REG_SZ, "PFF-Project")
            
        with winreg.CreateKey(winreg.HKEY_CLASSES_ROOT, "PFF-Project") as key:          # Create file type information
            winreg.SetValue(key, "", winreg.REG_SZ, "PFF-Project File")
            
            with winreg.CreateKey(key, "DefaultIcon") as icon_key:                      # Set icon
                winreg.SetValue(icon_key, "", winreg.REG_SZ, icon_path)
        
        print("File association and custom icon successfully registered.")
        return True
    except Exception as e:
        return str(e)


def notify_shell():
    ctypes.windll.shell32.SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, None, None)


def register_icon():    
    extension = "pffproj"
    icon_path = "..\\assets\\logo.ico"
    script_dir = os.path.dirname(os.path.abspath(__file__))                             # Get the current script's directory
    icon_path = os.path.join(script_dir, icon_path)

    if check_extension_exists(extension):                                               # Check if extension exists before requesting admin
        print(f"The file extension .{extension} is already registered")
        return

    if not os.path.exists(icon_path):                                                   # Ensure icon file exists
        print("Error: The specified icon file [logo.ico] was not found in the [script/assets] directory.")
        return
    
    print("The PFF Game Engine uses the [.pffproj] file extension and requests to register a custom icon for it.")
    print("This action requires administrator privileges but is not strictly necessary for the engine's functionality.")
    if elevate_privileges() == False:
        return
    
    result = set_file_association(extension, icon_path)
    if result is True:
        print(f"Successfully registered the '.{extension}' file extension with the PFF-icon.\n")
        notify_shell()  # Notify the shell of the change
    else:
        print(f"Error: {result}\n")

    input("Press any key to continue...")
