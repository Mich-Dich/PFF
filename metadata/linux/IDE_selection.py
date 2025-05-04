
import os
import sys
import subprocess

def detect_rider():
    # Check common Rider installation paths on Linux
    rider_paths = [
        os.path.expanduser("~/.local/share/JetBrains/Toolbox/apps/Rider"),
        "/opt/JetBrains Rider",
        os.path.expanduser("~/.local/share/applications/jetbrains-rider.desktop")
    ]
    
    # Check if rider command exists in PATH
    try:
        subprocess.run(["which", "rider"], check=True, capture_output=True)
        return True
    except subprocess.CalledProcessError:
        pass

    # Check physical paths
    for path in rider_paths:
        if os.path.exists(path) or len(glob.glob(path)) > 0:
            return True
    return False

def prompt_ide_selection():
    ides = []
    
    # Detect available IDEs
    if detect_rider():
        ides.append("JetBrains Rider")
    
    # Always include Makefile option
    ides.append("Makefile (CLion/Ninja compatible)")
    
    if not ides:
        print("No supported IDEs detected.")
        sys.exit(1)
        
    print("\nDetected IDEs:")
    for i, ide in enumerate(ides):
        print(f"{i}. {ide}")

    if len(ides) == 1:
        print("Only one IDE detected")
        return ides[0]

    while True:
        choice = input("Select an IDE to use (enter the number): ")
        try:
            choice_index = int(choice)
            if 0 <= choice_index < len(ides):
                return ides[choice_index]
            print("Invalid selection number.")
        except ValueError:
            print("Please enter a valid number.")
