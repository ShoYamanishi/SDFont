cmake_minimum_required( VERSION 3.10 )

project( sdfont )

include(CMakePrintHelpers)

find_package( Freetype REQUIRED )
find_package( PNG      REQUIRED )

find_package( GLEW     REQUIRED )
find_package( OpenGL   REQUIRED )
find_package( glfw3    REQUIRED )

# SDFONT_GENERATOR_LIB

add_library( sdfont_gen
    ${PROJECT_SOURCE_DIR}/src_lib_generator/free_type_utilities.cpp
    ${PROJECT_SOURCE_DIR}/src_lib_generator/generator.cpp
    ${PROJECT_SOURCE_DIR}/src_lib_generator/generator_config.cpp
    ${PROJECT_SOURCE_DIR}/src_lib_generator/generator_option_parser.cpp
    ${PROJECT_SOURCE_DIR}/src_lib_generator/internal_glyph_for_generator.cpp
    ${PROJECT_SOURCE_DIR}/src_lib_generator/png_loader.cpp
)

target_compile_features( sdfont_gen PRIVATE cxx_std_17 )

target_include_directories( sdfont_gen PUBLIC ${PROJECT_SOURCE_DIR}/include )

target_include_directories( sdfont_gen PRIVATE ${FREETYPE_INCLUDE_DIRS} )
target_include_directories( sdfont_gen PRIVATE ${PNG_INCLUDE_DIR}       )

target_link_directories( sdfont_gen PRIVATE "/usr/local/lib" )

target_link_libraries( sdfont_gen ${FREETYPE_LIBRARIES} )
target_link_libraries( sdfont_gen ${PNG_LIBRARIES}      )

# SDFONT_GENERATOR_COMMANDLINE

add_executable( sdfont_commandline ${PROJECT_SOURCE_DIR}/src_commandline/generator.cpp )
target_include_directories( sdfont_commandline PRIVATE ${FREETYPE_INCLUDE_DIRS} )
target_include_directories( sdfont_commandline PRIVATE ${PROJECT_SOURCE_DIR}/include )
target_compile_features( sdfont_commandline PRIVATE cxx_std_17 )
target_link_libraries( sdfont_commandline sdfont_gen )

# SDFONT_RUNTIME_HELPER_LIB

add_library( sdfont_rt
    ${PROJECT_SOURCE_DIR}/src_lib_runtime_helper/runtime_helper.cpp
    ${PROJECT_SOURCE_DIR}/src_lib_runtime_helper/metrics_parser.cpp
    ${PROJECT_SOURCE_DIR}/src_lib_runtime_helper/texture_loader.cpp
    ${PROJECT_SOURCE_DIR}/src_lib_runtime_helper/shader_manager.cpp
    ${PROJECT_SOURCE_DIR}/src_lib_runtime_helper/vanilla_shader_manager.cpp
)

target_compile_features( sdfont_rt PRIVATE cxx_std_17 )
target_include_directories( sdfont_rt PUBLIC ${PROJECT_SOURCE_DIR}/include )
target_include_directories( sdfont_rt PRIVATE ${PNG_INCLUDE_DIR}       )
target_link_libraries( sdfont_rt ${PNG_LIBRARIES}      )
target_link_libraries( sdfont_rt GLEW::glew )

if( ${CMAKE_SYSTEM_NAME} MATCHES Darwin )

    target_include_directories( sdfont_rt PRIVATE ${OPENGL_INCLUDE_DIR}/Headers )
    target_compile_definitions( sdfont_rt PRIVATE __MAC_LIB__ )
    target_link_libraries( sdfont_rt "-framework OpenGL" )

else()

    target_link_libraries( sdfont_rt OpenGL )

endif()

# DEMO

add_executable( sdfont_demo 
    ${PROJECT_SOURCE_DIR}/src_demo/demo.cpp
)

target_include_directories( sdfont_demo PRIVATE ${PROJECT_SOURCE_DIR}/include )
target_compile_features( sdfont_demo PRIVATE cxx_std_17 )
target_link_libraries( sdfont_demo sdfont_rt )

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
