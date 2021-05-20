#!/bin/bash

if ./build.sh -DDEBUG=TRUE -DTESTS=TRUE; then
echo "Starting tests...";
else 
echo "Could not start tests";
exit 1 
fi

./build/Test/Test
