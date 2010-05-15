#!/bin/bash

if [ -e obj ]; then
    echo ">>> Nothing";
else
    echo ">>> mkdir obj";
    mkdir obj;
fi;