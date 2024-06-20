#!/bin/sh
# Writer script for assignment 1 : it will write a string to a file
# Author: sbaans

# Test if there are exactly 2 paramters
if [ $# -ne 2 ]
then
    echo "Error : The number of parameters should be exactly 2"
    exit 1
fi

writefile=$1
writestr=$2

# Try to create the file
mkdir -p "$(dirname "$writefile")"
touch "$writefile"

# Test if the file is created

if [ ! -f "$writefile" ]
then
    echo "Error : The file $writefile could not be created"
    exit 1
fi


# Force overwriting of the file
echo $writestr >| "$writefile"
