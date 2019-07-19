

find_path(PLOG_INCLUDE_DIR
NAMES "plog/Log.h"
PATH_SUFFIXES "include"
PATHS
    "${PROJECT_SOURCE_DIR}/third_party/plog/")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(plog DEFAULT_MSG
PLOG_INCLUDE_DIR)
