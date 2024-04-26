cmake_minimum_required(VERSION 3.12.0)

project(DDirWatcher)
set(BIN_NAME DirWatchService) 
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if (WIN32)
    macro(get_WIN32_WINNT version)
        if (CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if ("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif ("${verMajor}" MATCHES "10")
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif(CMAKE_SYSTEM_VERSION)
    endmacro(get_WIN32_WINNT)

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
    add_definitions(-DBOOST_UUID_RANDOM_PROVIDER_FORCE_WINCRYPT)
endif(WIN32)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(SOURCES main.cpp
   DDirWatcher.hpp DDirWatcher.cpp
   DDirWatcherHandler.hpp DDirWatcherHandler.cpp
   DDirWatcherInputHandler.hpp DDirWatcherInputHandler.cpp
   DDirWatcherOutputHandler.hpp DDirWatcherOutputHandler.cpp
   DDirWatcherMarshallerHandler.hpp DDirWatcherMarshallerHandler.cpp
   DDirWatcherDataItem.hpp DDirWatcherDataItem.cpp)

add_executable(${BIN_NAME} ${SOURCES})

# target_sources(${BIN_NAME} PRIVATE ${SOURCES})

set_target_properties(${BIN_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${BIN_NAME} PROPERTIES LINK_FLAGS -pthread)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(g3log CONFIG REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(fswatch REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

find_package(ProcessorNode REQUIRED)


target_include_directories(${BIN_NAME} PUBLIC ${g3log_INCLUDE_DIRS} ${fswatch_INCLUDE_DIRS} ${tinyxml2_INCLUDE_DIRS} ${ProcessorNode_INCLUDE_DIRS})

target_link_libraries(${BIN_NAME} ${g3log_LIBRARIES} ${fswatch_LIBRARIES} tinyxml2::tinyxml2 ProcessorNode::ProcessorNode)

