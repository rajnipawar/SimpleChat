# SimpleChat - Ring Network Messaging Application

A Qt6-based distributed messaging application that implements a ring network topology for message routing between multiple chat instances.

## Overview

SimpleChat is a programming assignment implementation that demonstrates:
- Ring network topology with TCP-based communication
- Message serialization using QVariantMap
- Sequential message ordering and routing
- Multi-instance local deployment

## Features

### Core Requirements Implemented
- **GUI Implementation**: Qt6-based chat window with message log and input areas
- **Network Communication**: TCP-based messaging using QTcpSocket
- **Message Protocol**: Serialization/deserialization with QVariantMap containing:
  - ChatText: The actual message content
  - Origin: Unique identifier for each SimpleChat instance
  - Destination: Where the message should end up
  - Sequence number: For message ordering
- **Ring Topology**: Each process connects to the next in a static ring (9001→9002→9003→9004→9001)
- **Message Routing**: Messages are forwarded around the ring until they reach their destination

### Technical Features
- **Modern Dark Theme UI**: Professional dark theme with message bubbles and tabbed conversations
- **Smart Message Input**: Auto-focus, dropdown destination selection, tab-based messaging
- **Visual Message Alignment**: Sent messages (blue, right-aligned), received messages (gray, left-aligned)
- **Conversation Management**: Separate tabs for each node conversation with message history
- **Network Reliability**: Automatic retry connection mechanism with message queuing
- **Comprehensive Logging**: Debug output for network events and message routing

## Architecture

### Components

1. **Message Class** (`message.h/cpp`)
   - Handles message serialization/deserialization
   - QVariantMap-based protocol implementation
   - Validation and data integrity

2. **NetworkManager Class** (`networkmanager.h/cpp`)
   - TCP server and client functionality
   - Ring topology management
   - Message routing and forwarding
   - Connection management and retry logic

3. **ChatWindow Class** (`chatwindow.h/cpp`)
   - Modern dark theme Qt6 GUI implementation
   - Tabbed conversation interface with individual node conversations
   - Message bubble styling with proper left/right alignment
   - Smart destination selection (dropdown + tab-based messaging)
   - Professional dark color scheme

4. **SimpleChat Class** (`simplechat.h/cpp`)
   - Main application logic
   - Integration between GUI and network components
   - Node identification and setup

### Ring Network Topology

```
Node1 (9001) → Node2 (9002) → Node3 (9003) → Node4 (9004) → Node1 (9001)
```

Each node:
- Runs a TCP server on its assigned port
- Connects as a client to the next node in the ring
- Forwards messages that aren't destined for itself
- Delivers messages that are addressed to it

## Build Requirements

### Dependencies
- Qt6 (Core, Widgets, Network modules)
- CMake 3.16 or higher
- C++17 compatible compiler
- Unix-like system (Linux, macOS)

### Installing Qt6 on macOS
```bash
# Using Homebrew
brew install qt6

# Or download from Qt official website
# https://www.qt.io/download-qt-installer
```

### Installing Qt6 on Ubuntu/Debian
```bash
sudo apt update
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential
```

## Building the Project

### Quick Start
```bash
# Clone or extract the project
cd SimpleChat

# Build and run all instances
./launch_ring.sh
```

### Manual Build
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
make -j$(nproc)

# Run individual instance
./SimpleChat --port 9001
```

### Build Options
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

## Usage

### Starting the Ring Network
```bash
# Automated launch (recommended)
./launch_ring.sh

# Manual launch (separate terminals)
./build/SimpleChat --port 9001
./build/SimpleChat --port 9002
./build/SimpleChat --port 9003
./build/SimpleChat --port 9004
```

### Sending Messages

#### Modern UI Approach (Current)
The application features a modern messaging interface:
- **System Tab**: Use the dropdown "To:" selector to choose destination node
- **Conversation Tabs**: Click on individual node tabs (💬 Node2, 💬 Node3, etc.) to send messages directly to that node
- **Message Bubbles**: Your sent messages appear as blue bubbles on the right, received messages as gray bubbles on the left
- **Auto-focusing**: Message input is automatically focused for immediate typing

#### Additional Features
- **Conversation History**: Each node conversation maintains separate message history
- **Enter Key Support**: Press Enter to send messages (Shift+Enter for new lines)
- **Visual Feedback**: Different bubble styles clearly distinguish sent vs received messages

Available nodes:
- Node1 (port 9001)
- Node2 (port 9002)
- Node3 (port 9003)
- Node4 (port 9004)

### Message Flow Example
If Node1 sends a message to Node3:
1. Node1 creates message with Origin=Node1, Destination=Node3
2. Node1 forwards to Node2
3. Node2 sees Destination≠Node2, forwards to Node3
4. Node3 receives and displays the message

## Testing

### Comprehensive Test Suite
```bash
# Run complete automated test suite
./run_tests.sh

# Or build and run tests manually
mkdir -p tests/build && cd tests/build
cmake -DBUILD_TESTS=ON ../..
make -j$(sysctl -n hw.ncpu)
ctest --output-on-failure
```

### Integration Testing
```bash
# Launch all 4 nodes for manual integration testing
./launch_ring.sh
```

### Manual Testing Steps
1. Start all 4 instances using `./launch_ring.sh`
2. Wait for all connections to establish (check console output)
3. Send test messages between different nodes
4. Verify messages are received at correct destinations
5. Test message ordering with multiple sequential messages

### Test Coverage
The test suite includes comprehensive unit tests (`tests/test_simple.cpp`) covering:

**Core Message Functionality:**
- Message creation and validation
- Default constructor behavior
- Getter/setter methods

**Content Validation:**
- Empty content handling
- Whitespace-only content  
- Multi-line messages
- Special characters and Unicode support (你好 🎉)
- Very long messages (1000+ characters)

**Sequence Number Testing:**
- Sequence numbers starting from 1 (requirement compliance)
- Zero sequence number validation (must be invalid)
- Negative sequence number validation (must be invalid) 
- Large sequence number handling
- Sequential ordering verification

**Node ID Validation:**
- Valid origin and destination IDs
- Empty/invalid node ID rejection

**Serialization Testing:**
- QVariantMap serialization/deserialization
- Round-trip data integrity
- Special character preservation
- Empty variant map handling

**Edge Cases:**
- Default message validity
- Boundary value testing
- Error condition handling

**Test Results:** 18 comprehensive test cases with 100% pass rate

## Project Structure

```
SimpleChat/
├── CMakeLists.txt          # Build configuration
├── README.md               # This documentation
├── launch_ring.sh          # Automated launcher script
├── run_tests.sh            # Automated test suite runner
├── install_dependencies.sh # Dependency installation
├── src/
│   ├── main.cpp            # Application entry point
│   ├── simplechat.h/cpp    # Main application class
│   ├── chatwindow.h/cpp    # GUI implementation
│   ├── networkmanager.h/cpp # Network and ring management
│   └── message.h/cpp       # Message protocol implementation
└── tests/
    ├── CMakeLists.txt      # Test build configuration
    ├── test_simple.cpp     # Comprehensive unit tests (18 test cases)  
    └── build/              # Test build directory (auto-generated)
```

## Network Protocol

### Message Format
Messages are serialized using Qt's QVariantMap with the following structure:
```cpp
{
    "ChatText": "Hello World",      // String: The message content
    "Origin": "Node1",              // String: Sender identifier
    "Destination": "Node3",         // String: Recipient identifier
    "SequenceNumber": 42            // Integer: Message sequence number
}
```

### Connection Management
- Each node maintains one outgoing connection to the next node in the ring
- Incoming connections are accepted from any node
- Automatic retry mechanism with 3-second intervals
- Message queuing during connection outages

### Ring Ports Configuration
The ring uses fixed ports in sequence:
- Node1: 9001 → connects to → Node2: 9002
- Node2: 9002 → connects to → Node3: 9003
- Node3: 9003 → connects to → Node4: 9004
- Node4: 9004 → connects to → Node1: 9001

## Troubleshooting

### Common Issues

1. **Build Errors**
   ```bash
   # Ensure Qt6 is properly installed
   cmake --version
   qmake6 --version  # or qmake --version
   
   # Clean build
   rm -rf build
   mkdir build && cd build
   cmake ..
   ```

2. **Connection Issues**
   ```bash
   # Check if ports are available
   netstat -ln | grep 900[1-4]
   
   # Kill existing processes
   killall SimpleChat
   ```

3. **Qt6 Not Found**
   ```bash
   # Set Qt6 path manually
   export CMAKE_PREFIX_PATH=/path/to/qt6
   cmake ..
   ```

### Debug Mode
Enable debug output by setting environment variable:
```bash
export QT_LOGGING_RULES="*.debug=true"
./build/SimpleChat --port 9001
```

## Implementation Details

### Message Routing Algorithm
1. **Message Creation**: Create message with Origin, Destination, and auto-assigned sequence number
2. **Unique Identification**: Messages are uniquely identified by three components:
   - Origin: Unique identifier for each SimpleChat instance
   - Destination: Where the message should end up
   - Sequence number: Starting from 1, incrementing for each message from that origin
3. **Ring Propagation**: Forward message around static ring of peers until it reaches destination
4. **Sequence Ordering**: Messages are delivered in sequence number order at destination

### Sequence Number Management & Ordering
- **Starting from 1**: Each node's sequence counter begins at 1 and increments for each outgoing message
- **Automatic Assignment**: NetworkManager automatically assigns sequence numbers when sending
- **Order Enforcement**: Messages must be delivered in sequence order (e.g., message 3 before message 4)
- **Out-of-Order Buffering**: Messages arriving out of sequence are buffered until their turn
- **Sequence Validation**: Only messages with sequence numbers ≥ 1 are considered valid

### Ring Topology Message Forwarding
- **Destination Check**: If message destination matches current node → process with sequence ordering
- **Forward Logic**: If message destination ≠ current node → forward to next hop in ring
- **Ring Completion**: Messages propagate around the ring until they reach their intended destination
- **Logging**: Comprehensive debug output tracks message flow through the ring

### Connection Recovery
- Automatic reconnection with exponential backoff
- Message queuing during disconnection periods
- Graceful handling of network failures

## Assignment Compliance

This implementation fulfills all requirements specified in the Programming Assignment Part 1:

✅ **Enhanced GUI Implementation**
- Modern dark theme chat interface using Qt6
- Tabbed conversation system with individual node conversations  
- Professional message bubbles with proper left/right alignment
- Smart destination selection (dropdown + tab-based messaging)
- Dark theme with professional color scheme (#0B141A background, #007AFF sent messages, #2A2F32 received messages)
- Auto-focus on text input area when launched
- Responsive design with proper message spacing and typography

✅ **Network Communication**
- TCP-based messaging using QTcpSocket
- Message serialization/deserialization using QVariantMap
- Message format with ChatText, Origin, Destination, Sequence number
- Ring network topology with two peers per process

✅ **Messaging Protocol**
- Messages uniquely identified by Origin, Destination, Sequence number
- Sequential message ordering
- Message propagation around static ring until destination reached

✅ **Technical Requirements**
- C++ systems language implementation
- Qt6 framework
- CMake build system
- Git version control
- Support for running multiple instances locally
- Launch scripts for automation

✅ **Testing Requirements**
- Verified with 2-4 instances locally
- Message propagation testing
- Message ordering verification

✅ **Submission Requirements**
- Comprehensive documentation (this README)
- Build instructions
- Deployment and testing scripts
- Complete git repository structure

## Version Control

This project uses Git for version control. Key commits include:
- Initial project structure setup
- Core message protocol implementation
- GUI and network components
- Ring topology and routing logic
- Testing and deployment scripts
- Documentation and final integration

## Future Enhancements

Potential improvements for future versions:
- Dynamic ring topology with node discovery
- Message encryption and authentication
- Persistent message history
- Network partition tolerance
- Load balancing and redundancy
- RESTful API interface
- Web-based GUI option

---

**Author**: Programming Assignment Implementation  
**Course**: CS 550 Advanced Operating Systems  
**Date**: September 2025