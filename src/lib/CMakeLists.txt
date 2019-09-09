project(${CMAKE_PROJECT_NAME})

list(APPEND ${PROJECT_NAME}_SOURCES
    logger.cpp
    statement.cpp
    statement_factory.cpp
    reader.cpp
    interpreter.cpp
    async.cpp)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})

target_link_libraries(${PROJECT_NAME} 
    ${CMAKE_THREAD_LIBS_INIT}
    CONAN_PKG::boost
    CONAN_PKG::range-v3)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION lib)