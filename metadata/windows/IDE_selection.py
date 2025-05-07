
import os
import sys
import subprocess

def detect_visual_studio_versions():
    vs_version_map = {
        "17": "2022",  # VS 2022 (major version 17)
        "16": "2019",  # VS 2019 (major version 16)
        "15": "2017"   # VS 2017 (major version 15)
    }
    try:
        vswhere_path = os.path.join(os.environ["ProgramFiles(x86)"], "Microsoft Visual Studio", "Installer", "vswhere.exe")
        result = subprocess.run([vswhere_path, "-format", "json", "-products", "*", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64"], capture_output=True, text=True)
        if result.returncode == 0:
            import json
            installations = json.loads(result.stdout)
            versions = []
            for install in installations:
                major_version = install["installationVersion"].split('.')[0]
                premake_id = vs_version_map.get(major_version, None)
                if premake_id:
                    versions.append(premake_id)
            return list(set(versions))  # Remove duplicates
    except Exception as e:
        print(f"Error detecting Visual Studio versions: {e}")
    return []


def detect_rider():
    # Check common Rider installation paths
    rider_paths = [
        os.path.join(os.environ["ProgramFiles"], "JetBrains", "JetBrains Rider *"),
        os.path.join(os.environ["LocalAppData"], "JetBrains", "Toolbox", "apps", "Rider", "*")
    ]
    for path in rider_paths:
        if os.path.exists(path):
            return True
    return False


def detect_vscode():
    try:
        subprocess.run(["where", "code"], check=True, capture_output=True)
        return True
    except subprocess.CalledProcessError:
        pass

    # Fallback: check common installation paths
    paths = [
        os.path.join(os.environ.get("ProgramFiles", ""), "Microsoft VS Code", "Code.exe"),
        os.path.join(os.environ.get("LocalAppData", ""), "Programs", "Microsoft VS Code", "Code.exe")
    ]
    return any(os.path.exists(p) for p in paths)


def prompt_ide_selection():
    ides = []

    vs_versions = detect_visual_studio_versions()
    if vs_versions:
        ides.extend([f"Visual Studio {version}" for version in vs_versions])

    if detect_rider():
        ides.append("JetBrains Rider")

    if detect_vscode():
        ides.append("VSCode")

    if not ides:
        print("No supported IDEs detected.")
        sys.exit(1)

    print("Detected IDEs:")
    for i, ide in enumerate(ides):
        print(f"{i}. {ide}")

    if len(ides) == 1:
        print("only one IDE detected")
        return ides[0]

    choice = input("Select an IDE to use (enter the number): ")
    try:
        choice_index = int(choice)
        if 0 <= choice_index < len(ides):
            return ides[choice_index]
        else:
            print("Invalid selection.")
            sys.exit(1)
            
    except ValueError:
        print("Invalid input.")
        sys.exit(1)
