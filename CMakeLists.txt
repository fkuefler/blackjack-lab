cmake_minimum_required(VERSION 3.10)

project(BlackjackLab CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set up the source files
set(SOURCE_FILES
    src/BlackjackLab.cpp
    src/BlackjackGame.cpp
    src/Card.cpp
    src/Deck.cpp
    src/Hand.cpp
    src/BlackjackUtils.cpp
    src/EVCalculator.cpp
    src/StrategyGenerator.cpp
)

# Set up the include directories
include_directories(include)

# Add the executable
add_executable(BlackjackLab ${SOURCE_FILES})

# Set C++20 standard
target_compile_features(BlackjackLab PRIVATE cxx_std_20)