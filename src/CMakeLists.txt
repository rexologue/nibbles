set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    scheme_model.cpp
    scheme_model.h
)

if (QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(nibbles
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(nibbles
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(nibbles
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        core              # <-- тянет include-путь из core/
)

if (WIN32)
    set_target_properties(nibbles PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if (APPLE)
    set_target_properties(nibbles PROPERTIES MACOSX_BUNDLE TRUE)
endif()

include(GNUInstallDirs)
install(TARGETS nibbles
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(nibbles)
endif()
