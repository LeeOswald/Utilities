#!/usr/bin/python

import os
import sys
import fnmatch


def printHelp():
    print("This script replaces tabs with spaces")
    print("Usage: tabs2spaces.py <mask> [<mask> [<mask>...]]")
    print("Example: tabs2spaces.py \*.c \*.h")
    print("Note that in Unix-like shells wildcards must be escaped (\*.c)")


def matchByMasks(root, files, masks):
    names = []
    for mask in masks:
        for file in fnmatch.filter(files, mask):
            name = root + '/' + file
            names.append(name)
    
    return names


def walk(root, masks):
    names = []
    for root, dirs, files in os.walk('.'):
        names = names + matchByMasks(root, files, masks)
    
    return names

    
def replaceTabs(pathFrom, pathTo):
    with open(pathFrom, 'r') as src:
        with open(pathTo, 'w') as dst:
            for line in src:
                line = line.rstrip()
                line = line.replace('\t','    ')
                dst.write(line + '\n')



def processFile(path):
    out = path + '.out'
    try:
        replaceTabs(path, out)
        old = path + '.old'
        os.rename(path, old)
        os.rename(out, path)
        os.remove(old)
        print(path + ': processed')

    except:
        print("Unexpected error: ", sys.exc_info()[0])


def processFiles(paths):
    for path in paths:
        processFile(path)    


def main():

    if len(sys.argv) < 2:
        return printHelp()

    masks = []
    for i in(1, len(sys.argv) - 1):
        masks.append(sys.argv[i])

    processFiles(walk('.', masks))



if __name__ == '__main__':
    main()
