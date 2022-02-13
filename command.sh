#!/bin/bash

command=$1

case "$command" in
  "init")
    cmake -S. -B.build;;
  "test")
    step_test;;
  "build")
    step_build;;
  "run"|"start")
    step_run
esac

function step_test() {
    cppcheck -q --enable=all ./*.c
}

function step_build() {
    step_test;
    cmake --build .build
}

function step_run() {
    step_build;
    ./.build/osLinux;;
}