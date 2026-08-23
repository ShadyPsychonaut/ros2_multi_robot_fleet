#!/usr/bin/env bash
set -m
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LAUNCH_FILE="$SCRIPT_DIR/launch_sim.py"

ros2 launch "$LAUNCH_FILE" &
LAUNCH_PID=$!

cleanup() {
    echo "Shutting down cleanly..."
    kill -INT -- -"$LAUNCH_PID" 2>/dev/null
    sleep 5
    kill -KILL -- -"$LAUNCH_PID" 2>/dev/null
    pkill -9 -f "gz sim" 2>/dev/null
    pkill -9 -f "component_container_isolated" 2>/dev/null
}
trap cleanup SIGINT SIGTERM
wait "$LAUNCH_PID"