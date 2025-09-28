#!/bin/bash

echo "SimpleChat Dependency Installation Script"
echo "========================================"
echo ""

# Detect operating system
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    echo "Detected macOS - Installing dependencies via Homebrew..."
    
    # Check if Homebrew is installed
    if ! command -v brew &> /dev/null; then
        echo "Homebrew not found. Installing Homebrew first..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    echo "Installing Qt6 and CMake..."
    brew install qt6 cmake
    
    # Set environment variables
    echo ""
    echo "Setting up environment..."
    QT_PATH=$(brew --prefix qt6)
    echo "export CMAKE_PREFIX_PATH=\"$QT_PATH:\$CMAKE_PREFIX_PATH\"" >> ~/.bash_profile
    echo "export PATH=\"$QT_PATH/bin:\$PATH\"" >> ~/.bash_profile
    
    # For zsh users
    if [ -f ~/.zshrc ]; then
        echo "export CMAKE_PREFIX_PATH=\"$QT_PATH:\$CMAKE_PREFIX_PATH\"" >> ~/.zshrc
        echo "export PATH=\"$QT_PATH/bin:\$PATH\"" >> ~/.zshrc
    fi
    
    echo "Qt6 installed at: $QT_PATH"
    echo "Please restart your terminal or run: source ~/.bash_profile"

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    echo "Detected Linux - Installing dependencies..."
    
    # Detect package manager
    if command -v apt-get &> /dev/null; then
        # Ubuntu/Debian
        echo "Using apt package manager..."
        sudo apt update
        sudo apt install -y qt6-base-dev qt6-tools-dev cmake build-essential
        
    elif command -v yum &> /dev/null; then
        # RHEL/CentOS/Fedora
        echo "Using yum package manager..."
        sudo yum install -y qt6-qtbase-devel qt6-qttools-devel cmake gcc gcc-c++
        
    elif command -v pacman &> /dev/null; then
        # Arch Linux
        echo "Using pacman package manager..."
        sudo pacman -S qt6-base qt6-tools cmake gcc
        
    else
        echo "Unsupported package manager. Please install Qt6, CMake, and build tools manually."
        exit 1
    fi

else
    echo "Unsupported operating system: $OSTYPE"
    echo "Please install Qt6, CMake, and a C++ compiler manually."
    exit 1
fi

echo ""
echo "Dependencies installation completed!"
echo ""
echo "To build and run SimpleChat:"
echo "1. cd SimpleChat"
echo "2. ./launch_ring.sh"
echo ""
echo "If you encounter Qt6 not found errors, try:"
echo "1. Restart your terminal"
echo "2. Set CMAKE_PREFIX_PATH manually"
echo "3. Use Qt5 as fallback (modify CMakeLists.txt)"