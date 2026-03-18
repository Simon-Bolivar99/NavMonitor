cmake_minimum_required( VERSION 3.16 )

project( NavMonitor VERSION 0.1.0 LANGUAGES CXX )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )

set( Qt6WebEngineWidgets_FOUND FALSE CACHE INTERNAL "Force disable WebEngine" FORCE )
set( Qt6WebEngineCore_FOUND   FALSE CACHE INTERNAL "Force disable WebEngine" FORCE )

set( TARGET NavMonitor )

find_package( Qt6 REQUIRED COMPONENTS Core Widgets )

# ---- Marble options -------------------------------------------------------

set( BUILD_MARBLE_APPS OFF CACHE BOOL "Disable Marble apps" FORCE )
set( BUILD_TESTING     OFF CACHE BOOL "Disable tests" FORCE )

# ---- Temporary include fix for Marble as subdirectory ---------------------

function( collect_header_dirs ROOT OUT_VAR )
    if( NOT EXISTS "${ROOT}" )
        set( ${OUT_VAR} "" PARENT_SCOPE )
        return()
    endif()

    file( GLOB_RECURSE header_files CONFIGURE_DEPENDS
        "${ROOT}/*.h"
        "${ROOT}/*.hpp"
    )

    set( result_dirs "" )
    foreach( header_file IN LISTS header_files )
        get_filename_component( header_dir "${header_file}" DIRECTORY )
        list( APPEND result_dirs "${header_dir}" )
    endforeach()

    list( REMOVE_DUPLICATES result_dirs )
    set( ${OUT_VAR} "${result_dirs}" PARENT_SCOPE )
endfunction()

collect_header_dirs(
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/marble/src/lib/marble"
    MARBLE_SRC_HEADER_DIRS
)

collect_header_dirs(
    "${CMAKE_CURRENT_BINARY_DIR}/extern/marble/src/lib/marble"
    MARBLE_BUILD_HEADER_DIRS
)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/marble/src/lib"
    "${CMAKE_CURRENT_BINARY_DIR}/extern/marble/src/lib"
    ${MARBLE_SRC_HEADER_DIRS}
    ${MARBLE_BUILD_HEADER_DIRS}
)

# ---- External dependencies ------------------------------------------------

add_subdirectory( extern/marble )

add_executable( ${TARGET}
    main.cpp
    MainWindow.cpp
    MainWindow.hpp
)

target_link_libraries( ${TARGET} PRIVATE
    Qt6::Core
    Qt6::Widgets
    marblewidget
)

# ---- GPS / gpsmm ----------------------------------------------------------

if( APPLE )
    find_library( GPS_LIB
        NAMES gps libgps
        PATHS /opt/homebrew/lib
        REQUIRED
    )

    if( NOT EXISTS "/opt/homebrew/include/libgpsmm.h" )
        message( FATAL_ERROR
            "libgpsmm.h not found: /opt/homebrew/include/libgpsmm.h" )
    endif()

    add_library( Gps::Gps SHARED IMPORTED )
    set_target_properties( Gps::Gps PROPERTIES
        IMPORTED_LOCATION "${GPS_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "/opt/homebrew/include"
    )

    target_link_libraries( ${TARGET} PRIVATE
        Gps::Gps
    )

elseif( UNIX )
    find_package( PkgConfig REQUIRED )
    pkg_check_modules( QGPSMM REQUIRED IMPORTED_TARGET Qgpsmm )

    target_link_libraries( ${TARGET} PRIVATE
        PkgConfig::QGPSMM
    )
endif()

target_compile_options( ${TARGET} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

set_target_properties( ${TARGET} PROPERTIES
    MACOSX_BUNDLE ON
    WIN32_EXECUTABLE ON
)