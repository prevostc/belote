#!/bin/bash
SESSION=belote

tmux -2 new-session -d -s $SESSION

##########
# CONFIG #
##########

# allow mouse clicks to select pane
tmux set-option -t $SESSION mouse on
tmux set-option -t $SESSION mouse-select-pane on

# prevent window title from becoming the current running command
tmux set-option -t $SESSION automatic-rename off
# with zsh, also uncomment DISABLE_AUTO_TITLE=true in your .zshrc config file

################
# WINDOW SETUP #
################

# Setup a window for api
tmux rename-window -t $SESSION:0 'api-server'
tmux split-window -h
tmux select-pane -t 0
tmux send-keys "npm run api.watch" C-m
tmux select-pane -t 1
tmux send-keys "npm run engine.watch" C-m

# Setup a front window
tmux new-window -t $SESSION:1 -n 'frontend'
tmux split-window -h
tmux select-pane -t 0
tmux send-keys "cd frontend" C-m
tmux send-keys "npm start" C-m
tmux select-pane -t 1
tmux send-keys "cd frontend" C-m

# Setup a git window
tmux new-window -t $SESSION:2 -n 'git'
tmux select-pane -t 0
tmux send-keys "git status" C-m

########
# INIT #
########

# Set default window and pane
tmux select-window -t $SESSION:0
tmux select-pane -t 0

# Attach to session
tmux -2 attach-session -t $SESSION
