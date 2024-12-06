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
        response = input("Registering a file type requires admin privileges. Run as admin? (Y/N): ").lower()
        if response == 'y':
            script_path = os.path.abspath(sys.argv[0])
            ctypes.windll.shell32.ShellExecuteW(
                None, 
                "runas", 
                sys.executable, 
                f'"{script_path}"', 
                os.path.dirname(script_path), 
                1
            )
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
    
def set_file_association(extension, icon_path):
    script_dir = os.path.dirname(os.path.abspath(__file__))                             # Get the current script's directory
    icon_path = os.path.join(script_dir, icon_path)
    print(f"Icon file [{icon_path}]")
    
    if not os.path.exists(icon_path):                                                   # Ensure icon file exists
        print("Error: logo.ico not found in script directory")
        return

    try:
        with winreg.CreateKey(winreg.HKEY_CLASSES_ROOT, f".{extension}") as key:        # Create .pffproj file extension association
            winreg.SetValue(key, "", winreg.REG_SZ, "PFF-Project")
            
        with winreg.CreateKey(winreg.HKEY_CLASSES_ROOT, "PFF-Project") as key:          # Create file type information
            winreg.SetValue(key, "", winreg.REG_SZ, "PFF-Project File")
            
            with winreg.CreateKey(key, "DefaultIcon") as icon_key:                      # Set icon
                winreg.SetValue(icon_key, "", winreg.REG_SZ, icon_path)
        
        print("File association successfully set!")        
        return True
    except Exception as e:
        return str(e)

if __name__ == "__main__":    
    extension = "pffproj".replace(".", "")
    icon_path = "assets\\logo.ico"

    if check_extension_exists(extension):                                               # Check if extension exists before requesting admin
        print(f"File extension .{extension} is already registered")
        sys.exit(0)

    print(f"Registering File extension [.{extension}]")
    elevate_privileges()
    result = set_file_association(extension, icon_path)
    if result is True:
        print(f"Successfully registered .{extension} with custom icon")
    else:
        print(f"Error: {result}")

    input("Press any key to continue...")
