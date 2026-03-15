cmake_minimum_required( VERSION 3.16 )

project( NavMonitor VERSION 0.1.0 LANGUAGES CXX)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )

set(Qt6WebEngineWidgets_FOUND FALSE CACHE INTERNAL "Force disable WebEngine" FORCE)
set(Qt6WebEngineCore_FOUND   FALSE CACHE INTERNAL "Force disable WebEngine" FORCE)

set ( TARGET NavMonitor )

find_package( Qt6 REQUIRED COMPONENTS Widgets )
find_package(Marble REQUIRED)

add_executable( ${TARGET}
    main.cpp
    MainWindow.cpp
    MainWindow.hpp
)

find_package( PkgConfig REQUIRED )

pkg_check_modules( QGPSMM REQUIRED IMPORTED_TARGET Qgpsmm )

target_link_libraries( ${TARGET} PRIVATE Marble Qt6::Core Qt6::Widgets PkgConfig::QGPSMM  )

target_compile_options( ${TARGET} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

set_target_properties( ${TARGET} PROPERTIES
    MACOSX_BUNDLE ON
    WIN32_EXECUTABLE ON
)
