cmake_minimum_required( VERSION 3.10 )

project( sdfont )

include(CMakePrintHelpers)

#include( /Users/shoichiroyamanishi/workarea/tmp/libgd-2.3.3/cmake/modules/FindGD.cmake )

message( "" )
message( "    [WARNING ON LIBGD]: libdg does not install any export modules at any common locations. However the Git repository contain FindGD.cmake. It's been copied from libgd-2.3.3/cmake/modules to ${PROJECT_SOURCE_DIR} for convenience." )
message( "" )

include( FindGD.cmake )

find_package( Freetype REQUIRED )
find_package( PNG      REQUIRED )

find_package( GLEW     REQUIRED )
find_package( OpenGL   REQUIRED )
find_package( glfw3    REQUIRED )

# SDFONT_LIB

add_library( sdfont_lib 
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_free_type_utilities.cpp
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_generator.cpp
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_generator_config.cpp
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_generator_option_parser.cpp
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_internal_glyph_for_generator.cpp
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_metrics_parser.cpp
)

target_compile_features( sdfont_lib PRIVATE cxx_std_17 )

target_include_directories( sdfont_lib PUBLIC ${PROJECT_SOURCE_DIR}/include )

target_include_directories( sdfont_lib PRIVATE ${GD_INCLUDE_DIR}        )
target_include_directories( sdfont_lib PRIVATE ${FREETYPE_INCLUDE_DIRS} )
target_include_directories( sdfont_lib PRIVATE ${PNG_INCLUDE_DIR}       )

target_link_directories( sdfont_lib PRIVATE "/usr/local/lib" )

target_link_libraries( sdfont_lib ${GD_LIBRARIES}       )
target_link_libraries( sdfont_lib ${FREETYPE_LIBRARIES} )
target_link_libraries( sdfont_lib ${PNG_LIBRARIES}      )
#target_link_libraries( sdfont_lib GLEW::glew )

# SDFONT_GENERATOR

add_executable( sdfont_generator ${PROJECT_SOURCE_DIR}/src_bin/sdfont_generator.cpp )
target_include_directories( sdfont_generator PRIVATE ${FREETYPE_INCLUDE_DIRS} )
target_include_directories( sdfont_generator PRIVATE ${PROJECT_SOURCE_DIR}/include )
target_compile_features( sdfont_generator PRIVATE cxx_std_17 )
target_link_libraries( sdfont_generator sdfont_lib )

# SDFONT_RUNTIME

add_library( sdfont_runtime
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_runtime_helper.cpp
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_metrics_parser.cpp
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_texture_loader.cpp
)

target_compile_features( sdfont_runtime PRIVATE cxx_std_17 )
target_include_directories( sdfont_runtime PUBLIC ${PROJECT_SOURCE_DIR}/include )
target_include_directories( sdfont_runtime PRIVATE ${PNG_INCLUDE_DIR}       )
target_link_libraries( sdfont_runtime ${PNG_LIBRARIES}      )
target_link_libraries( sdfont_runtime GLEW::glew )

if( ${CMAKE_SYSTEM_NAME} MATCHES Darwin )
    target_include_directories( sdfont_runtime PRIVATE ${OPENGL_INCLUDE_DIR}/Headers )
    target_compile_definitions( sdfont_runtime PRIVATE __MAC_LIB__ )
    target_link_libraries( sdfont_runtime "-framework OpenGL" )

else()
    target_link_libraries( sdfont_runtime OpenGL )
endif()

# DEMO

add_executable( sdfont_demo 
    ${PROJECT_SOURCE_DIR}/src_bin/sdfont_demo.cpp
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_shader_manager.cpp
    ${PROJECT_SOURCE_DIR}/src_lib/sdfont_vanilla_shader_manager.cpp
)
target_include_directories( sdfont_demo PRIVATE ${PROJECT_SOURCE_DIR}/include )
target_compile_features( sdfont_demo PRIVATE cxx_std_17 )
target_link_libraries( sdfont_demo sdfont_runtime )

target_link_libraries( sdfont_demo GLEW::glew )


if( ${CMAKE_SYSTEM_NAME} MATCHES Darwin )
   
    target_link_directories( sdfont_demo PRIVATE "/usr/local/lib" ) # for GLFW3
    target_include_directories( sdfont_demo PRIVATE ${OPENGL_INCLUDE_DIR}/Headers )
    target_compile_definitions( sdfont_demo PRIVATE __MAC_LIB__ )

    target_link_libraries( sdfont_demo glfw3 )
    target_link_libraries( sdfont_demo "-framework Cocoa" )
    target_link_libraries( sdfont_demo "-framework IOKit" )
    target_link_libraries( sdfont_demo "-framework OpenGL" )

else()
    target_link_libraries( sdfont_demo glfw )
    target_link_libraries( sdfont_demo OpenGL )

endif()
