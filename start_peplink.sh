#!/bin/bash

# =========================================================================
# PEPLINK GPS & ALIGNMENT STARTUP SCRIPT (TMUX)
# =========================================================================

# Ensure we are in the correct directory
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR"

# Check if build exists, if not build it
if [ ! -f "install/setup.bash" ]; then
    echo "First time setup: Building ROS 2 workspace..."
    source /opt/ros/humble/setup.bash
    colcon build
    if [ $? -ne 0 ]; then
        echo "Build failed. Please check the errors."
        exit 1
    fi
fi

# Ensure tmux is installed
if ! command -v tmux &> /dev/null; then
    echo "Error: tmux is not installed. Please install it first:"
    echo "sudo apt install tmux"
    exit 1
fi

SESSION_NAME="peplink_session"

# --- Handle CLOSE ALL ---
if [ "$1" == "stop" ] || [ "$1" == "close" ]; then
    echo "Stopping Peplink nodes..."
    tmux kill-session -t $SESSION_NAME 2>/dev/null
    echo "✅ Successfully closed all Peplink nodes and tmux session."
    exit 0
fi

# Check if session already exists
tmux has-session -t $SESSION_NAME 2>/dev/null
if [ $? == 0 ]; then
  echo "Session $SESSION_NAME already exists. Attaching to it..."
  tmux attach -t $SESSION_NAME
  exit 0
fi

echo "Starting Peplink GPS and Alignment nodes in a new tmux session..."

# 1. Create a new tmux session in DETACHED mode. This creates Window 0, Pane 0.
tmux new-session -d -s $SESSION_NAME -n "system"

# 2. Setup Pane 0 (Top) - GPS Node
# Send commands directly to the pane. 'C-m' means 'Carriage Return' (Enter key).
tmux send-keys -t $SESSION_NAME:0.0 "cd \"$DIR\"" C-m
tmux send-keys -t $SESSION_NAME:0.0 "source /opt/ros/humble/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:0.0 "source install/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:0.0 "clear && echo -e '\033[1;32m[GPS NODE STARTED]\033[0m'" C-m
tmux send-keys -t $SESSION_NAME:0.0 "ros2 run peplink_gps_driver peplink_gps_node" C-m

# 3. Split Window vertically (creates bottom pane: Pane 1)
tmux split-window -t $SESSION_NAME:0.0 -v -p 40

# Setup Pane 1 (Bottom Left) - Alignment Node
tmux send-keys -t $SESSION_NAME:0.1 "cd \"$DIR\"" C-m
tmux send-keys -t $SESSION_NAME:0.1 "source /opt/ros/humble/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:0.1 "source install/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:0.1 "clear && echo -e '\033[1;36m[ALIGNMENT NODE STARTED]\033[0m'" C-m
tmux send-keys -t $SESSION_NAME:0.1 "ros2 run pttep_alignment alignment_node" C-m

# 4. Split Pane 1 horizontally (creates bottom right pane: Pane 2)
tmux split-window -t $SESSION_NAME:0.1 -h

# Setup Pane 2 (Bottom Right) - Command Line / Manual control
tmux send-keys -t $SESSION_NAME:0.2 "cd \"$DIR\"" C-m
tmux send-keys -t $SESSION_NAME:0.2 "source /opt/ros/humble/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:0.2 "source install/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:0.2 "clear" C-m
tmux send-keys -t $SESSION_NAME:0.2 "echo '=== ALIGNMENT CHEAT SHEET ==='" C-m
tmux send-keys -t $SESSION_NAME:0.2 "echo '1. Save Point: ros2 service call /save_location std_srvs/srv/Trigger'" C-m
tmux send-keys -t $SESSION_NAME:0.2 "echo '2. Calculate : ros2 service call /calculate_transformation pttep_alignment/srv/CalculateTransformation \"{reset: false}\"'" C-m
tmux send-keys -t $SESSION_NAME:0.2 "echo '3. View Data : ros2 topic echo /aligned_odom'" C-m
tmux send-keys -t $SESSION_NAME:0.2 "echo '============================='" C-m

# Focus on Pane 2 so when user attaches, they can type immediately
tmux select-pane -t $SESSION_NAME:0.2

# Attach to the session and bring it to foreground
tmux attach-session -t $SESSION_NAME