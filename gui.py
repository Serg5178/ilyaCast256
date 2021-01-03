import os 

def getFilesInDirectory(dir: str):
    files = []
    for dirname, dirnames, filenames in os.walk(dir):
        for filename in filenames:
            files.append(os.path.join(dirname, filename))
    return files

directory = str(input("Укажите путь\n"))
print(getFilesInDirectory(directory))
