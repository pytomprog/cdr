#!/bin/bash

clear

cd "/home/tom/Desktop/cdr2024/programs/brain"

echo "Wake up, Archibald ...
The Matrix has you ...
Follow the pipou.
Knock, knock, Archibald.

Modes:
  1 - Running
  2 - Programming"

mode=0
read -p "Which mode do you want to choose ? " mode
while [[ $mode < 1 || $mode > 2 ]]
do
    read -p "Please enter valid value. " mode
done

if [ $mode == 1 ]
then
    clear
    sudo xmake run --root core
fi

if [ $mode == 2 ]
then
    /usr/bin/code &
    lxterminal -e "cd /home/tom/Desktop/cdr2024/programs/brain/"s &
fi