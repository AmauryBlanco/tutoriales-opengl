set(ATB_DIR $ENV{ATB_DIR} CACHE PATH "AntTweakBar Directory")

find_package(ATB REQUIRED)

link_directories(${ATB_LIBRARY_DIRS})
include_directories( ${ATB_INCLUDE_DIRS})

add_executable( 11-AntTweakBar gui.cpp )						
target_link_libraries( 11-AntTweakBar ${GRAPHIC_LIBS})