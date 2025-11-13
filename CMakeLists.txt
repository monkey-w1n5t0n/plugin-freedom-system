cmake_minimum_required(VERSION 3.22)

# Set deployment target before project (macOS only)
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS deployment version")
endif()

project(JUCEPlugins VERSION 1.0.0)

# Add JUCE once at root
# Check environment variable first, then system config, then fallback to default
if(DEFINED ENV{JUCE_DIR})
    set(JUCE_PATH $ENV{JUCE_DIR})
    message(STATUS "Using JUCE from environment: ${JUCE_PATH}")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/.claude/system-config.json")
    # Try to parse JUCE path from system-config.json
    execute_process(
        COMMAND python3 -c "import json; print(json.load(open('${CMAKE_SOURCE_DIR}/.claude/system-config.json'))['juce_path'])"
        OUTPUT_VARIABLE JUCE_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(JUCE_PATH)
        message(STATUS "Using JUCE from system-config.json: ${JUCE_PATH}")
    else()
        # Fallback to default
        if(APPLE)
            set(JUCE_PATH "/Applications/JUCE")
        else()
            set(JUCE_PATH "$ENV{HOME}/JUCE")
        endif()
        message(STATUS "Using default JUCE path: ${JUCE_PATH}")
    endif()
else()
    # Fallback to default based on platform
    if(APPLE)
        set(JUCE_PATH "/Applications/JUCE")
    else()
        set(JUCE_PATH "$ENV{HOME}/JUCE")
    endif()
    message(STATUS "Using default JUCE path: ${JUCE_PATH}")
endif()

add_subdirectory(${JUCE_PATH} JUCE)

# Auto-discover plugins
file(GLOB PLUGIN_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/plugins/*")
foreach(PLUGIN_DIR ${PLUGIN_DIRS})
    if(IS_DIRECTORY ${PLUGIN_DIR} AND EXISTS "${PLUGIN_DIR}/CMakeLists.txt")
        add_subdirectory(${PLUGIN_DIR})
    endif()
endforeach()
