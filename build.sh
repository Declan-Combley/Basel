#!/bin/bash

set -xe

cc=gcc
flags='-Wall -Wextra -Werror'

$cc main.c -o basel $flags
