#!/bin/sh
# Finder script for assignment 1 : it will find the number of matching lines in a given directory
# Author: sbaans

# Test if there are exactly 2 paramters
if [ $# -ne 2 ]
then
    echo "Error : The number of parameters should be exactly 2"
    exit 1
fi

# Check if the directory is existing
filesdir=$1

if [ ! -d "$filesdir" ]
then
    echo "Error : The directory $filesdir is not existing"
    exit 1
fi

# Count the number of files in the directory and its subdirectories
filesnumber=$(find "$filesdir" -type f | wc -l)

# Check how many occurences of the string there are in the directory
searchstring=$2

matchinglines=$(grep -roh "$searchstring" "$filesdir" | wc -w)

echo  "The number of files are $filesnumber and the number of matching lines are $matchinglines"



