#!/bin/bash

# =========================================================================
# PEPLINK GPS & ALIGNMENT STARTUP SCRIPT (TMUX)
# =========================================================================

SESSION_NAME="peplink_alignment"

# Check if session already exists
tmux has-session -t $SESSION_NAME 2>/dev/null
if [ $? == 0 ]; then
  echo "Session $SESSION_NAME already exists. Attaching to it..."
  tmux attach -t $SESSION_NAME
  exit 0
fi

echo "Starting Peplink GPS and Alignment nodes in a new tmux session..."

# Create a new tmux session detached
tmux new-session -d -s $SESSION_NAME -n "system"

# Split window into 3 panes
# Pane 0 (Top): GPS Node
# Pane 1 (Bottom Left): Alignment Node
# Pane 2 (Bottom Right): Service Calls / User Commands

# Split horizontally (creates Pane 1 at bottom)
tmux split-window -v -p 40

# Split the bottom pane vertically (creates Pane 2 at bottom right)
tmux split-window -h

# --- Pane 0: Peplink GPS Node ---
tmux send-keys -t $SESSION_NAME:0 "source /opt/ros/humble/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:0 "source install/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:0 "ros2 run peplink_gps_node peplink_gps_driver" C-m

# --- Pane 1: PTTEP Alignment Node ---
tmux send-keys -t $SESSION_NAME:1 "source /opt/ros/humble/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:1 "source install/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:1 "ros2 run pttep_alignment alignment_node" C-m

# --- Pane 2: User Commands (Services) ---
tmux send-keys -t $SESSION_NAME:2 "source /opt/ros/humble/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:2 "source install/setup.bash" C-m
tmux send-keys -t $SESSION_NAME:2 "clear" C-m
tmux send-keys -t $SESSION_NAME:2 "echo '=== ALIGNMENT COMMANDS ==='" C-m
tmux send-keys -t $SESSION_NAME:2 "echo '1. To save a point (do this 3+ times at diff locations):'" C-m
tmux send-keys -t $SESSION_NAME:2 "echo '   ros2 service call /save_location std_srvs/srv/Trigger'" C-m
tmux send-keys -t $SESSION_NAME:2 "echo '2. To calculate and start real-time aligned odometry:'" C-m
tmux send-keys -t $SESSION_NAME:2 "echo '   ros2 service call /calculate_transformation pttep_alignment/srv/CalculateTransformation \"{reset: false}\"'" C-m
tmux send-keys -t $SESSION_NAME:2 "echo '3. To view the aligned output:'" C-m
tmux send-keys -t $SESSION_NAME:2 "echo '   ros2 topic echo /aligned_odom'" C-m
tmux send-keys -t $SESSION_NAME:2 "" C-m

# Attach to the session
tmux attach -t $SESSION_NAME