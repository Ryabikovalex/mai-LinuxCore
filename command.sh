#!/bin/bash

command=$1

case "$command" in
  "init")
    cmake -S. -B.build;;
  "build")
    cmake --build .build;;
  "run"|"start")
    cmake --build .build;
    ./.build/osLinux;;
esac
