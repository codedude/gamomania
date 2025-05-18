cmake_minimum_required(VERSION 3.31)


set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_NAME gamomania)
project(${PROJECT_NAME} VERSION 0.0.1)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendored/assimp EXCLUDE_FROM_ALL)
add_subdirectory(vendored/glm EXCLUDE_FROM_ALL)


set(SOURCES main shader demo gl_debug file texture camera event light material list mesh model hash)

list(TRANSFORM SOURCES PREPEND "src/")
list(TRANSFORM SOURCES APPEND ".cpp")

list(APPEND SOURCES vendored/glad/glad.c)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE /usr/include/)
target_include_directories(${PROJECT_NAME} PRIVATE vendored)
# target_include_directories(${PROJECT_NAME} PRIVATE vendored/glm)
# target_include_directories(${PROJECT_NAME} PRIVATE vendored/assimp/include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_directories(${PROJECT_NAME} PRIVATE /usr/lib64/)
target_link_directories(${PROJECT_NAME} PRIVATE vendored/assimp/bin)


set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -g")
add_compile_options(-fsanitize=address -g)
add_link_options(-fsanitize=address)

add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}")

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3 assimp::assimp glm::glm
    GL m
)

# copy shaders to build dir
# add_custom_command(
#   TARGET ${PROJECT_NAME} POST_BUILD
#   COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
#   ${CMAKE_CURRENT_SOURCE_DIR}/src/shader/
#     $<TARGET_FILE_DIR:${PROJECT_NAME}>/shader/)
