cmake_minimum_required(VERSION 3.16)

project(SMO VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Charts)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

set(PROJECT_SOURCES
    main.cpp
    qcustomplot.cpp
    qcustomplot.h
)

set(APP_SOURCES
    app/simulator.cpp
    app/statistics.cpp
)
set(APP_HEADERS
    app/simulator.h
    app/statistics.h
)

set(UI_SOURCES
    ui/mainWindow.cpp
    ui/resultsdialog.cpp
    ui/simulatorwindow.cpp
)
set(UI_HEADERS
    ui/mainWindow.h
    ui/resultsdialog.h
    ui/simulatorwindow.h
)
set(UI_FORMS
    ui/mainWindow.ui
    ui/resultsdialog.ui
    ui/simulatorwindow.ui
)

set(MODELS_SOURCES
    models/request.cpp
    models/source.cpp
    models/dispatcher.cpp
    models/device.cpp
)
set(MODELS_HEADERS
    models/request.h
    models/source.h
    models/dispatcher.h
    models/device.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SMO
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SMO APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(SMO SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        find_package(Qt5 REQUIRED COMPONENTS Widgets)

        add_executable(SMO
            ${PROJECT_SOURCES}
            ${APP_SOURCES}
            ${APP_HEADERS}
            ${UI_SOURCES}
            ${UI_HEADERS}
            ${UI_FORMS}
            ${MODELS_SOURCES}
            ${MODELS_HEADERS}
            # request.h request.cpp
            # source.h source.cpp
            # device.h device.cpp
            # models/buffer.h models/buffer.cpp
            # dispatcher.h dispatcher.cpp

            # dispatcher.h dispatcher.cpp

            # statistics.h statistics.cpp

            # simulator.cpp simulator.h
            # simulatorwindow.h simulatorwindow.cpp simulatorwindow.ui
            # resultsdialog.h resultsdialog.cpp resultsdialog.ui
        )
    endif()
endif()

target_link_libraries(SMO PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)
target_link_libraries(SMO PRIVATE Qt5::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.SMO)
endif()
set_target_properties(SMO PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS SMO
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SMO)
endif()
