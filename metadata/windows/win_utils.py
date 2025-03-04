
import os
import winreg
from zipfile import ZipFile
import time
import sys


def enable_ansi_support():
    if os.name == 'nt':
        # Enable ANSI escape sequences in Windows
        import ctypes
        kernel32 = ctypes.windll.kernel32
        kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)


def GetSystemEnvironmentVariable(name):
    key = winreg.CreateKey(winreg.HKEY_LOCAL_MACHINE, r"System\CurrentControlSet\Control\Session Manager\Environment")
    try:
        return winreg.QueryValueEx(key, name)[0]
    except:
        return None


def GetUserEnvironmentVariable(name):
    key = winreg.CreateKey(winreg.HKEY_CURRENT_USER, r"Environment")
    try:
        return winreg.QueryValueEx(key, name)[0]
    except:
        return None


def UnzipFile(filepath, deleteZipFile=True):
    zipFilePath = os.path.abspath(filepath) # get full path of files
    zipFileLocation = os.path.dirname(zipFilePath)

    zipFileContent = dict()
    zipFileContentSize = 0
    with ZipFile(zipFilePath, 'r') as zipFileFolder:
        for name in zipFileFolder.namelist():
            zipFileContent[name] = zipFileFolder.getinfo(name).file_size
        zipFileContentSize = sum(zipFileContent.values())
        extractedContentSize = 0
        startTime = time.time()
        for zippedFileName, zippedFileSize in zipFileContent.items():
            UnzippedFilePath = os.path.abspath(f"{zipFileLocation}/{zippedFileName}")
            os.makedirs(os.path.dirname(UnzippedFilePath), exist_ok=True)
            if os.path.isfile(UnzippedFilePath):
                zipFileContentSize -= zippedFileSize
            else:
                zipFileFolder.extract(zippedFileName, path=zipFileLocation, pwd=None)
                extractedContentSize += zippedFileSize
            try:
                done = int(50*extractedContentSize/zipFileContentSize)
                percentage = (extractedContentSize / zipFileContentSize) * 100
            except ZeroDivisionError:
                done = 50
                percentage = 100
            elapsedTime = time.time() - startTime
            try:
                avgKBPerSecond = (extractedContentSize / 1024) / elapsedTime
            except ZeroDivisionError:
                avgKBPerSecond = 0.0
            avgSpeedString = '{:.2f} KB/s'.format(avgKBPerSecond)
            if (avgKBPerSecond > 1024):
                avgMBPerSecond = avgKBPerSecond / 1024
                avgSpeedString = '{:.2f} MB/s'.format(avgMBPerSecond)
            sys.stdout.write('\r[{}{}] {:.2f}% ({})     '.format('█' * done, '.' * (50-done), percentage, avgSpeedString))
            sys.stdout.flush()
    sys.stdout.write('\n')

    if deleteZipFile:
        os.remove(zipFilePath) # delete zip file