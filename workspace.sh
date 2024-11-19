#!/bin/bash

NAME=rhysos
SHELL_CMD=bash

# screen -dmS $NAME -t shell $SHELL_CMD
# screen -S $NAME -t vim_screen -X screen bash -c vim 
# screen -r $NAME 

SESSIONEXISTS=$(tmux list-sessions | grep $NAME)

if [ "$SESSIONEXISTS" = "" ]
then
    tmux new-session -d -s $NAME 'bash -c vim'
    tmux rename-window -t 0 'Vim'
    # tmux send-keys -t 'Vim' C-m C-l C-t C-m

    tmux new-window -t $NAME:1 -n 'Shell' $SHELL_CMD
    tmux send-keys -t 'Shell' C-m 'clear' C-m
fi

tmux attach-session -t $NAME:0
