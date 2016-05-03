set(ATB_DIR $ENV{ATB_DIR} CACHE PATH "AntTweakBar directorio de instalacion")

find_path(ATB_INCLUDE_DIRS AntTweakBar.h HINTS ${ATB_DIR}/include  PATH_SUFFIXES include)
find_path(ATB_LIBRARY_DIRS AntTweakBar.lib AntTweakBar64.lib HINTS ${ATB_DIR}/lib  PATH_SUFFIXES lib)
find_library(ATB_LIBRARY NAMES AntTweakBar AntTweakBar64 HINTS ${ATB_DIR} PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ATB DEFAULT_MSG ATB_LIBRARY ATB_INCLUDE_DIRS)