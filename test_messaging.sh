#!/bin/bash

# SimpleChat Testing Script
# This script tests the messaging functionality

echo "SimpleChat Testing Script"
echo "========================="
echo ""

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Build directory not found. Please run ./launch_ring.sh first to build the project."
    exit 1
fi

cd build

# Check if executable exists
if [ ! -f "./SimpleChat" ]; then
    echo "Error: SimpleChat executable not found! Please build the project first."
    exit 1
fi

echo "Starting test with 4 nodes..."
echo ""

# Function to cleanup background processes
cleanup() {
    echo ""
    echo "Cleaning up test processes..."
    killall SimpleChat 2>/dev/null || true
    exit 0
}

trap cleanup INT

# Start nodes
echo "Starting Node 1 (port 9001)..."
./SimpleChat --port 9001 &
NODE1_PID=$!
sleep 2

echo "Starting Node 2 (port 9002)..."
./SimpleChat --port 9002 &
NODE2_PID=$!
sleep 2

echo "Starting Node 3 (port 9003)..."
./SimpleChat --port 9003 &
NODE3_PID=$!
sleep 2

echo "Starting Node 4 (port 9004)..."
./SimpleChat --port 9004 &
NODE4_PID=$!
sleep 3

echo ""
echo "All nodes started. Test cases:"
echo "1. Each node should connect to the next node in the ring"
echo "2. Messages should be routed through the ring to reach their destination"
echo "3. Sequence numbers should be maintained properly"
echo ""
echo "Manual testing required:"
echo "- Send messages using '@NodeX message' format"
echo "- Verify messages reach their intended destinations"
echo "- Check that message ordering is preserved"
echo ""
echo "Press Ctrl+C to stop all test instances"

# Wait for user input
while true; do
    sleep 1
done