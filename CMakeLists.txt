cmake_minimum_required(VERSION 3.31)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_NAME hello)

project(hello VERSION 0.0.1)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

set(SOURCES main demo shader gl_debug file)
list(TRANSFORM SOURCES PREPEND "src/")
list(TRANSFORM SOURCES APPEND ".c")

list(APPEND SOURCES vendor/glad/glad.c)


add_executable(hello ${SOURCES})

# Create your game executable target as usual
target_include_directories(hello PRIVATE vendor)
target_include_directories(hello PRIVATE src)

target_include_directories(hello PRIVATE /usr/include/)
target_link_directories(hello PRIVATE /usr/lib64/)

# Link to the actual SDL3 library.
target_link_libraries(hello PRIVATE SDL3 GL m)

add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}/src")
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
  ${CMAKE_CURRENT_SOURCE_DIR}/src/shader/
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/shader/)
