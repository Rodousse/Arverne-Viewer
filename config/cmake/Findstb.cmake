

find_path(STB_INCLUDE_DIR
NAMES "stb.h"
PATHS
    "${PROJECT_SOURCE_DIR}/third_party/stb/")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(stb DEFAULT_MSG
STB_INCLUDE_DIR)
