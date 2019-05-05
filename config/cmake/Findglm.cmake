

find_path(GLM_INCLUDE_DIR
NAMES "glm/glm.hpp"
PATHS
    "${PROJECT_SOURCE_DIR}/third_party/glm/")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(glm DEFAULT_MSG
GLM_INCLUDE_DIR)
