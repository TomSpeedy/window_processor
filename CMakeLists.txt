cmake_minimum_required(VERSION 3.10)

project(window_processor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts DataVisualization PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts DataVisualization PrintSupport)
#message(FATAL_ERROR ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT DEFINED ENV{CLUSTERER_PATH})
    set(ENV{CLUSTERER_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../)
endif()
if($ENV{CLUSTERER_PATH} MATCHES [^/\\]$)
    set(ENV{CLUSTERER_PATH} ENV{CLUSTERER_PATH}/)
endif()
set(clusterer_include_dir $ENV{CLUSTERER_PATH}clusterer/include/)
set(onnx_include_dir $ENV{CLUSTERER_PATH}clusterer/onnxruntime-linux-x64-1.14.1/include/)
if (NOT EXISTS ${clusterer_include_dir})
    message(FATAL_ERROR "Error - could not find clusterer library include path")
endif()


find_library(clusterer_library lcclusterer
    PATHS $ENV{CLUSTERER_PATH}build/lib/)

find_library(onnx_library onnxruntime
    PATHS $ENV{CLUSTERER_PATH}onnxruntime-linux-x64-1.14.1/lib/;$ENV{CLUSTERER_PATH}build/packages/Microsoft.ML.OnnxRuntime.1.15.1/runtimes/win-x64/native/;$ENV{ONNX_LIBRARY_PATH})



if (NOT CLUSTERER_LIBRARY)
    message(FATAL_ERROR "Clusterer library not found, please set CLUSTERER_PATH variable")
endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        windowdatatablemodel.h
        windowdatatablemodel.cpp
        controller.h controller.cpp
        fileedit.h fileedit.cpp
        filterselectionwindow.h filterselectionwindow.cpp filterselectionwindow.ui
        plot_drawing/qcustomplot/qcustomplot.h plot_drawing/qcustomplot/qcustomplot.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(window_processor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET window_processor APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(window_processor SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(window_processor
            ${PROJECT_SOURCES}
        )
    endif()
endif()
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#target_compile_features(window_processor PRIVATE cxx_std_17)

target_include_directories(window_processor PUBLIC ${clusterer_include_dir})
target_include_directories(window_processor PUBLIC ${onnx_include_dir})

target_link_libraries(window_processor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts Qt${QT_VERSION_MAJOR}::DataVisualization Qt${QT_VERSION_MAJOR}::PrintSupport ${clusterer_library} ${onnx_library})
set_target_properties(window_processor PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


install(TARGETS window_processor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(window_processor)
endif()
