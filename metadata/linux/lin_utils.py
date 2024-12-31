import os
import tarfile
import time
import sys
import subprocess


def ensure_executable(filepath):
    # Check if the file is executable
    if not os.access(filepath, os.X_OK):
        print(f"{filepath} is not executable. Making it executable...")
        # Run the chmod command to make the file executable
        subprocess.run(['chmod', '+x', filepath], check=True)


def un_targz_file(filepath, deleteTarGzFile=True):
    tarGzFilePath = os.path.abspath(filepath)  # Get full path of the file
    tarGzFileLocation = os.path.dirname(tarGzFilePath)

    # Initialize variables to track file sizes
    tarGzContent = dict()
    tarGzContentSize = 0

    # Open the tar.gz file
    with tarfile.open(tarGzFilePath, 'r:gz') as tarGzFile:
        # Get the list of files and their sizes
        for member in tarGzFile.getmembers():
            tarGzContent[member.name] = member.size
        tarGzContentSize = sum(tarGzContent.values())
        extractedContentSize = 0
        startTime = time.time()

        # Extract files and display progress
        for member in tarGzFile.getmembers():
            # Create the full path for the extracted file
            extractedFilePath = os.path.abspath(os.path.join(tarGzFileLocation, member.name))
            os.makedirs(os.path.dirname(extractedFilePath), exist_ok=True)

            # Extract the file
            if member.isfile():
                tarGzFile.extract(member, path=tarGzFileLocation)
                extractedContentSize += member.size

            # Update progress
            try:
                done = int(50 * extractedContentSize / tarGzContentSize)
                percentage = (extractedContentSize / tarGzContentSize) * 100
            except ZeroDivisionError:
                done = 50
                percentage = 100

            elapsedTime = time.time() - startTime
            try:
                avgKBPerSecond = (extractedContentSize / 1024) / elapsedTime
            except ZeroDivisionError:
                avgKBPerSecond = 0.0

            avgSpeedString = '{:.2f} KB/s'.format(avgKBPerSecond)
            if avgKBPerSecond > 1024:
                avgMBPerSecond = avgKBPerSecond / 1024
                avgSpeedString = '{:.2f} MB/s'.format(avgMBPerSecond)

            sys.stdout.write('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50 - done), percentage, avgSpeedString))
            sys.stdout.flush()

    sys.stdout.write('\n')

    # Optionally delete the tar.gz file
    if deleteTarGzFile:
        os.remove(tarGzFilePath)  # Delete the tar.gz file
