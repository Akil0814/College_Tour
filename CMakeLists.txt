cmake_minimum_required(VERSION 3.16)

project(College_Tour VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

qt_add_executable(College_Tour
    MANUAL_FINALIZATION

    main.cpp
    home_page.cpp
    home_page.h
    homepage.ui

    resources.qrc

    data_manager.cpp
    data_manager.h
    manager.h
    data_types.h
)

target_link_libraries(College_Tour PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)

if(Qt6_VERSION VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.College_Tour)
endif()

set_target_properties(College_Tour PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE FALSE #FALSE for testing only
)

include(GNUInstallDirs)
install(TARGETS College_Tour
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(College_Tour)
