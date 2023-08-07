cmake_minimum_required( VERSION 3.10 )

set( CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_BINARY_DIR}/conan_toolchain.cmake" )

cmake_policy( SET CMP0076 NEW )
cmake_policy( SET CMP0079 NEW )

project( "SDFont" )

# Create library target for sdfont
#===============================================

# Create target
set( TARGET_NAME sdfont )
add_library( ${TARGET_NAME} SHARED )

target_compile_definitions( ${TARGET_NAME} PUBLIC
    USE_LIBPNG
)

target_compile_features( ${TARGET_NAME} PUBLIC
    cxx_std_14
)

# Link libraries
find_package( Freetype REQUIRED )
find_package( GLEW REQUIRED )
find_package( PNG REQUIRED )
target_link_libraries( ${TARGET_NAME} PUBLIC
    Freetype::Freetype
    GLEW::GLEW
    PNG::PNG
)

# Set include directories
target_include_directories( ${TARGET_NAME} PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

# Set sources
add_subdirectory( src_lib )

# Create sdfont_generator executable
#===============================================

# Create target
add_executable(
    sdfont_generator
    ${CMAKE_CURRENT_LIST_DIR}/src_bin/sdfont_generator.cpp
)

# Link libraries
target_link_libraries( sdfont_generator PRIVATE
        sdfont
)


# Create demo executable
#===============================================

# Create target
add_executable(
    demo
    ${CMAKE_CURRENT_LIST_DIR}/src_bin/demo.cpp
)

# Link libraries
find_package( glfw3 REQUIRED )
target_link_libraries( demo PRIVATE
    sdfont
    glfw
)


# Configure targets for installation
#===============================================

install( TARGETS sdfont )
