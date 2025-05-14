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

set(SOURCES main demo shader gl_debug file texture camera event light material list mesh model hash)

list(TRANSFORM SOURCES PREPEND "src/")
list(TRANSFORM SOURCES APPEND ".c")
# list(APPEND SOURCES vendor/clay/renderers/SDL3/clay_renderer_SDL3.c)

list(APPEND SOURCES vendor/glad/glad.c)


add_executable(hello ${SOURCES})

target_link_libraries(hello PRIVATE cglm_headers)
add_subdirectory(vendor/cglm/ EXCLUDE_FROM_ALL)

# Create your game executable target as usual
target_include_directories(hello PRIVATE vendor)
target_include_directories(hello PRIVATE vendor/assimp/include)
target_include_directories(hello PRIVATE src)
# target_include_directories(hello PRIVATE vendor/SDL_ttf/include)
# target_include_directories(hello PRIVATE vendor/SDL_image/include)

target_include_directories(hello PRIVATE /usr/include/)
target_link_directories(hello PRIVATE /usr/lib64/)
target_link_directories(hello PRIVATE vendor/assimp/bin)
# target_link_directories(hello PRIVATE vendor/SDL_ttf/bin)
# target_link_directories(hello PRIVATE vendor/SDL_image/bin)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -g")
# add_compile_options(-fsanitize=address -g)
# add_link_options(-fsanitize=address)

# set(SDLTTF_VENDORED ON)
# add_subdirectory(vendor/SDL_ttf EXCLUDE_FROM_ALL)
# add_subdirectory(vendor/SDL_image EXCLUDE_FROM_ALL)

# Link to the actual SDL3 library. SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image
target_link_libraries(hello PRIVATE SDL3 GL m assimp)

add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}")
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
  ${CMAKE_CURRENT_SOURCE_DIR}/src/shader/
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/shader/)
