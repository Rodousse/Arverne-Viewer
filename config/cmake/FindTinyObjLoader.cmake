

find_path(TINYOBJLOADER_INCLUDE_DIR
NAMES "tiny_obj_loader.h"
PATHS
	"${PROJECT_SOURCE_DIR}/third_party/tinyobjloader/")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TinyObjLoader DEFAULT_MSG
TINYOBJLOADER_INCLUDE_DIR)
