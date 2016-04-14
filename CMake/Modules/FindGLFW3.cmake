set(GLFW3_DIR $ENV{GLFW3_DIR} CACHE PATH "GLFW3 Directorio de instalacion")

find_path(GLFW3_INCLUDE_DIRS GLFW/glfw3.h HINTS ${GLFW3_DIR}  PATH_SUFFIXES include)
find_library(GLFW3_LIBRARY NAMES glfw3 HINTS ${GLFW3_DIR} PATH_SUFFIXES lib64 lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW3 DEFAULT_MSG GLFW3_LIBRARY GLFW3_INCLUDE_DIRS)