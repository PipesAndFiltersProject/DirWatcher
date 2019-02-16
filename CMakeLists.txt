cmake_minimum_required(VERSION 3.12.0)

project(DDirWatcher)
set(BIN_NAME DirWatchService) 
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(SOURCES main.cpp
   DDirWatcher.hpp DDirWatcher.cpp
   DDirWatcherHandler.hpp DDirWatcherHandler.cpp
   DDirWatcherMarshallerHandler.hpp DDirWatcherMarshallerHandler.cpp
   DDirWatcherDataItem.hpp DDirWatcherDataItem.cpp)
add_executable(${BIN_NAME} ${SOURCES})

# target_sources(${BIN_NAME} PRIVATE ${SOURCES})

set_target_properties(${BIN_NAME} PROPERTIES CXX_STANDARD 14)
set_target_properties(${BIN_NAME} PROPERTIES LINK_FLAGS -pthread)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(g3logger REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(fswatch REQUIRED)
find_package(OHARBaseLayer REQUIRED)

target_include_directories(${BIN_NAME} PUBLIC ${g3log_INCLUDE_DIRS} ${fswatch_INCLUDE_DIRS})

target_link_libraries(${BIN_NAME} ${g3log_LIBRARIES} ${fswatch_LIBRARIES} OHARBaseLayer::OHARBaseLayer)

