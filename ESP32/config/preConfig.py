from pathlib import Path

filename = Path("config/userConfig.txt")
comPortSearchString = "# COM Ports:"
# !!! any other modifications go here, define new search strings

# check if config exists, create if not
if not filename.exists():
    filename.touch()  # creates an empty config file
    #the following code could be put into one .write call, however it's easier to read this way and this code will not be used often
    configFile = open(filename, "a")
    configFile.write("################################################################################################################################################\n")
    configFile.write("# V1\n")
    configFile.write("# ESP32 config file, this file holds all settings that change based on the device developers are using.\n")
    configFile.write("# This file will be auto generated if it does not exist, it should be excluded via gitignore.\n")
    configFile.write("# The very first time platformio.ini is initialized it might fail due to the way this configuration works.\n")
    configFile.write("# To disable something add a \';\' in front of it.")
    configFile.write("#\n")
    configFile.write("# IMPORTANT! Do not change lines that do start with a \'#\'!\n")
    configFile.write("################################################################################################################################################\n\n")
    # COM Ports
    configFile.write("# To set the COM Ports the script looks for \';### COM ports: ##############################\' in the platformio.ini,\n")
    configFile.write("# if such a line does not exist the ports will not be set.\n\n")
    configFile.write(comPortSearchString + '\n')
    configFile.write("upload_port = default\n")
    configFile.write("monitor_port = default\n")
    # !!! any other modifications go here:

# retrieve user configuration
with open(filename, "r") as configFile:
    configLines = configFile.readlines()

comPorts = ""
lineNumber = 0

while lineNumber < len(configLines):
    line = configLines[lineNumber]
    if comPortSearchString in line:
        # copies the next two lines:
        comPorts += configLines[lineNumber + 1]
        comPorts += configLines[lineNumber + 2]
        break
    lineNumber += 1

# COM port search string, this line is used to find the position for the COM port settings in the platformio.ini
comPortSearchString = ";### COM ports: ##############################"

# read content of platformio.ini
with open("platformio.ini", "r") as file:
	lines = file.readlines()

# modify platformio.ini
newLines = []
lineNumber = 0

while lineNumber < len(lines):
    line = lines[lineNumber]
    newLines.append(line)
    # set COM ports
    if comPortSearchString in line:  # found COM port position
        newLines.append(comPorts)  # insert configured comports
        lineNumber += 2  # skip old configuration
    # !!! any other modifications go here:
    lineNumber += 1  # move to next line

# write back to file
with open("platformio.ini", "w") as file:
    file.writelines(newLines)