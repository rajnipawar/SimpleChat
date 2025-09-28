#!/bin/bash

# SimpleChat Ring Network Launcher
# This script builds and launches multiple instances of SimpleChat

set -e

echo "Building SimpleChat..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure and build the project
cmake ..
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

echo "Build completed successfully!"
echo ""

# Check if executable exists
if [ ! -f "./SimpleChat" ]; then
    echo "Error: SimpleChat executable not found!"
    exit 1
fi

echo "Launching SimpleChat ring network..."
echo "Starting 4 nodes on ports 9001, 9002, 9003, 9004"
echo ""

# Launch instances in background with proper delays
echo "Starting Node 1 (port 9001)..."
./SimpleChat --port 9001 &
sleep 3

echo "Starting Node 2 (port 9002)..."
./SimpleChat --port 9002 &
sleep 3

echo "Starting Node 3 (port 9003)..."
./SimpleChat --port 9003 &
sleep 3

echo "Starting Node 4 (port 9004)..."
./SimpleChat --port 9004 &
sleep 3

echo "Waiting for all connections to establish..."
sleep 5

echo ""
echo "All nodes started successfully!"
echo ""
echo "Usage:"
echo "- Select destination node from dropdown menu"
echo "- Type your message in the text input area"
echo "- Click 'Send' or press Enter to send message"
echo "- Available nodes: Node1, Node2, Node3, Node4"
echo ""
echo "Press Ctrl+C to stop all instances"

# Wait for user to stop
trap 'echo ""; echo "Stopping all instances..."; killall SimpleChat 2>/dev/null || true; exit 0' INT

# Keep script running
while true; do
    sleep 1
done