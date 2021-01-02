import os 

def getFilesInDirectory(dir):
    files = []
    for dirname, dirnames, filenames in os.walk(dir):
        for filename in filenames:
            files.append(os.path.join(dirname, filename))
    return files

directory = './'
print(getFilesInDirectory(directory))