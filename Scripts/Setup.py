
import os
import subprocess
import platform

from SetupPython import PythonConfiguration as PythonRequirements

PythonRequirements.Validate()

from SetupPremake import PremakeConfiguration as PremakeRequirements
os.chdir('./../')

premakeInstalled = PremakeRequirements.Validate()

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

if (premakeInstalled):
    if platform.system() == "Windows":
        print("\nRunning premake...")
        subprocess.call([os.path.abspath("./Scripts/WindowsGenerateProjects.bat"), "nopause"])

    print("\nSetup completed!")
else:
    print("Premake required to generate project files.")

