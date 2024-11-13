cmake_minimum_required(VERSION 3.28)
project(PicEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "./")
include_directories(./)

find_package(Qt6 COMPONENTS REQUIRED  Widgets)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

qt_add_executable(PicEditor
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        canvaslabel.cpp
        canvaslabel.h
        showwidget.cpp
        showwidget.h
        cvcore.h
        cvzoom.cpp
        cvcore.cpp
        coverlabel.cpp
        coverlabel.h
)

target_link_libraries(PicEditor PRIVATE
        Qt6::Widgets ${OpenCV_LIBS}
)

include(GNUInstallDirs)
install(TARGETS PicEditor
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


