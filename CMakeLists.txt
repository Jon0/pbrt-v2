project(pbrt-v2)
cmake_minimum_required (VERSION 2.8)

add_definitions(
	-DPBRT_HAS_OPENEXR
	-DPBRT_PROBES_NONE
)

# place to find cmake modules
set(CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/cmake"
	${CMAKE_MODULE_PATH}
)

# get all header files
file(GLOB H_FILES
	"src"
	"src/accelerators"
	"src/cameras"
	"src/core"
	"src/film"
	"src/filters"
	"src/integrators"
	"src/lights"
	"src/materials"
	"src/renderers"
	"src/samplers"
	"src/shapes"
	"src/textures"
	"src/volumes"
)

# get all source files
file(GLOB SRC_FILES
	"src/accelerators/*.cpp"
	"src/cameras/*.cpp"
	"src/core/*.c"
	"src/core/*.cpp"
	"src/film/*.cpp"
	"src/filters/*.cpp"
	"src/integrators/*.cpp"
	"src/lights/*.cpp"
	"src/materials/*.cpp"
	"src/renderers/*.cpp"
	"src/samplers/*.cpp"
	"src/shapes/*.cpp"
	"src/textures/*.cpp"
	"src/volumes/*.cpp"
)

# find required packages
find_package(Threads)
find_package(OpenEXR REQUIRED)

include_directories(
	${H_FILES}
	${OPENEXR_INCLUDE_PATHS}
)

add_definitions(-std=c++11)

# build and link
add_library (pbrt ${SRC_FILES})
target_link_libraries (
	pbrt
	${OPENEXR_LIBRARIES}
)

add_executable (pbrtmain src/main/pbrt.cpp)
target_link_libraries (
	pbrtmain
	pbrt
	${CMAKE_THREAD_LIBS_INIT}
)
