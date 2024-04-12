#==============================
# Requirements
#==============================

cmake_minimum_required(VERSION 3.10)

#==============================
# Project Settings
#==============================

if (NOT PROJECT_DECLARED)
    project("Ymir" LANGUAGES CXX)
    set(PROJECT_DECLARED)
endif()

#==============================
# Unit Settings
#==============================

set(TARGET_NAME "Ymir")

#==============================
# Command Lines
#==============================

set(CMAKE_CXX_STANDARD 20)

#==============================
# Source
#==============================

# Macro which is used to find .cpp files recursively.
macro(find_cpp path list_name)
    file(GLOB_RECURSE _tmp_list RELATIVE ${path} ${path}/*.cpp)
    set(${list_name})
    foreach(f ${_tmp_list})
        if(NOT f MATCHES "cmake-*")
            list(APPEND ${list_name} ${f})
        endif()
    endforeach()
endmacro()

# Macro which is used to find .hpp files recursively.
macro(find_hpp path list_name)
    file(GLOB_RECURSE _tmp_list RELATIVE ${path} ${path}/*.hpp)
    set(${list_name})
    foreach(f ${_tmp_list})
        if(NOT f MATCHES "cmake-*")
            list(APPEND ${list_name} ${f})
        endif()
    endforeach()
endmacro()

# Macro for adding a third-party package to a specific target.
macro(add_package target_name visibility package_name)
    find_package(${package_name} REQUIRED)
    target_include_directories(${target_name} ${visibility} ${${package_name}_INCLUDE_DIRS})
    target_link_libraries(${target_name} ${visibility} ${${package_name}_LIBRARIES})
endmacro()

# Macro for adding a gaia module to a specific target.
macro(add_custom_module target_name visibility module_name)
    find_path(${module_name}_INCLUDE_DIRS "${module_name}")
    find_library(${module_name}_LIBRARIES "${module_name}")
    target_include_directories(${target_name} ${visibility} ${${module_name}_INCLUDE_DIRS})
    target_link_libraries(${target_name} ${visibility} ${${module_name}_LIBRARIES})
endmacro()

# Macro for adding a custom module of a custom package to a specific target.
macro(add_custom_package_module target_name visibility package_name module_name)
    find_path(${package_name}_INCLUDE_ROOT_DIR "${package_name}")
    set(${module_name}_INCLUDE_DIRS "${${package_name}_INCLUDE_ROOT_DIR}/${package_name}")
    find_library(${module_name}_LIBRARIES} "${module_name}")
    target_include_directories(${target_name} ${visibility} ${${module_name}_INCLUDE_DIRS})
    target_link_libraries(${target_name} ${visibility} ${${module_name}_LIBRARIES}})
endmacro()

#------------------------------
# C++
#------------------------------

# C++ Source Files
find_cpp(${CMAKE_CURRENT_SOURCE_DIR} TARGET_SOURCE)
# C++ Header Files
find_hpp(${CMAKE_CURRENT_SOURCE_DIR} TARGET_HEADER)

#==============================
# Compile Targets
#==============================

# To lower the running cost, this module will be compiled as a static library.
add_executable(${TARGET_NAME} ${TARGET_SOURCE} ${TARGET_HEADER})

# Enable 'DEBUG' Macro in Debug Mode
if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(${TARGET_NAME} PRIVATE -DDEBUG)
endif()

#==============================
# Dependencies
#==============================

# Gaia Modules
add_custom_module(${TARGET_NAME} PUBLIC GaiaExceptions)
add_custom_module(${TARGET_NAME} PUBLIC GaiaCameraInterface)
add_custom_module(${TARGET_NAME} PUBLIC GaiaCameraForDaheng)
add_custom_module(${TARGET_NAME} PUBLIC GaiaSerialIO)
add_custom_module(${TARGET_NAME} PUBLIC GaiaByteUtility)
add_custom_module(${TARGET_NAME} PUBLIC GaiaDNNHelper)
add_custom_module(${TARGET_NAME} PUBLIC EitrFramework)

add_package(${TARGET_NAME} PUBLIC OpenCV)
add_custom_module(${TARGET_NAME} PUBLIC tbb)
add_package(${TARGET_NAME} PUBLIC Protobuf)

find_package(Wt REQUIRED Wt HTTP)
target_include_directories(${TARGET_NAME} PUBLIC ${Wt_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC Wt::Wt Wt::HTTP)

# Boost
find_package(Boost 1.71 REQUIRED COMPONENTS system thread filesystem json program_options)
target_include_directories(${TARGET_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC ${Boost_LIBRARIES})

# In Linux, 'Threads' need to explicitly linked.
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(Threads)
    target_link_libraries(${TARGET_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif()

#===============================
# Install Scripts
#===============================

# Install executable files and libraries to 'default_path/'.
install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
# Install header files to 'default_path/TARGET_NAME/'
install(FILES ${TARGET_HEADER} DESTINATION  ${CMAKE_INSTALL_PREFIX}/include/${TARGET_NAME}/)