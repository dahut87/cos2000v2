#!/bin/sh
session="debug"
tmux start-server
tmux new-session -d -s $session -n gdb

tmux splitw -h -p 90
tmux splitw -t 0 -v -p 50
tmux splitw -t 2 -h -p 45
tmux splitw -v -p 50
tmux splitw -v -p 50

tmux send-keys -t 5 "gdb --init-command=./debug/gdbinit -x $1" Enter
tmux send-keys -t 3 "telnet 127.0.0.1 6666" Enter 
tmux send-keys -t 5 "dashboard stack -output /dev/pts/4" Enter
tmux send-keys -t 5 "dashboard threads -output /dev/null" Enter
tmux send-keys -t 5 "dashboard assembly -output /dev/pts/3" Enter 
tmux send-keys -t 5 "dashboard registers -output /dev/pts/2" Enter
tmux send-keys -t 5 "dashboard expressions -output /dev/pts/6" Enter
tmux send-keys -t 5 "dashboard memory -output /dev/pts/6" Enter
tmux send-keys -t 5 "dashboard source -output /dev/pts/3" Enter
tmux send-keys -t 5 "dashboard history -output /dev/pts/7" Enter
tmux send-keys -t 5 "dashboard threads -output /dev/null" Enter
tmux send-keys -t 5 "dashboard stack -output /dev/pts/4" Enter
tmux new-window -t $session:1 -n scratch
tmux select-window -t $session:0
tmux attach-session -t $session
