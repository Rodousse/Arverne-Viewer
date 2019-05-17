
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(HEADERS
    include/application/MainWindow.h
    include/application/ModelManager.h
    include/application/RendererWindow.h
)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/ModelManager.cpp
    src/RendererWindow.cpp
)

include_directories(include ${PLOG_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} renderer Qt5::Widgets Qt5::Gui)

##add resources in the exe folder
#add_custom_command(TARGET ${PROJECT_NAME}  PRE_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
