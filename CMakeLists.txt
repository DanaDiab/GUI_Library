# STUDENT CMakeLists.txt
#
# tabs: 8 space

# CMake initialization
	
cmake_minimum_required(VERSION 3.0)
project("ProjetC_IG")

# Paths

get_filename_component(ROOT_DIR		${CMAKE_CURRENT_LIST_FILE} PATH)
set(HEADER_DIRS				"${ROOT_DIR}/api")
set(TEST_DIR				"${ROOT_DIR}/tests")
set(DOCS_DIR				"${ROOT_DIR}/docs")
set(SRC_DIR				"${ROOT_DIR}/implem")

set(LIB_FLAGS				-lfreeimage -lSDL2 -lSDL2_ttf)

if(NOT DEFINED WIN32)
	set(CMAKE_C_FLAGS		"${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-parameter -Wno-unused-variable -Werror -pg")
endif(NOT DEFINED WIN32)

if(IG_ASAN)
	set(CMAKE_C_FLAGS		"${CMAKE_C_FLAGS} -fsanitize=address -O1 -DIG_ASAN=1")
endif(IG_ASAN)

# Platform specific definitions

if(${APPLE})
	set(HEADER_DIRS			${HEADER_DIRS}
					"/opt/local/include/SDL2"
					"/opt/local/include")
	set(PLATFORM_DIR		"${ROOT_DIR}/_macos")
	set(EIBASE			${PLATFORM_DIR}/libeibase.a)
	set(EXTCLASS			${PLATFORM_DIR}/libexternalclass.a)
	set(PLATFORM_LIB_FLAGS		${EIBASE} -L/opt/local/lib ${LIB_FLAGS})


	message(STATUS "Building for MacOS with eibase: ${EIBASE}")

elseif(${UNIX})

	set(HEADER_DIRS			${HEADER_DIRS}
					"/usr/include/SDL2")
	set(PLATFORM_DIR		"${ROOT_DIR}/_x11")
	set(EIBASE			${PLATFORM_DIR}/libeibase.a)
	set(EXTCLASS			${PLATFORM_DIR}/libexternalclass.a)
	set(PLATFORM_LIB_FLAGS		${EIBASE} -L${PLATFORM_DIR} ${LIB_FLAGS} -lm)

	message(STATUS "Building for Linux with eibase: ${EIBASE}")

else()

	set(SDL				"C:/projetc/SDL2_windows")
	set(HEADER_DIRS			${HEADER_DIRS}
					"${SDL}/include")
	add_definitions			(-D__WIN__=1 -D_CRT_SECURE_NO_WARNINGS=1 -D_AMD64_=1)
	add_compile_options		(/wd4068 /wd4142)

	set(PLATFORM_DIR		"${ROOT_DIR}/_win")
	set(EIBASE			${PLATFORM_DIR}/eibase.lib)
	set(EXTCLASS			${PLATFORM_DIR}/externalclass.lib)
	set(PLATFORM_LIB_FLAGS		${EIBASE}
					${SDL}/lib/SDL2.lib
					${SDL}/lib/SDL2_ttf.lib
					${SDL}/lib/FreeImage.lib)

	message(STATUS "Building for Windows with eibase: ${EIBASE}")

endif(${APPLE})

# General definitions for all targets

include_directories(${HEADER_DIRS} "${ROOT_DIR}/implem/draw" )


# target ei (libei)

set(LIB_EI_SOURCES
	${SRC_DIR}/draw/ei_draw_utils.h
	${SRC_DIR}/draw/ei_draw_utils.c
	${SRC_DIR}/draw/ei_draw_types.h
	${SRC_DIR}/draw/ei_draw_types.c
	
	${SRC_DIR}/draw/ei_draw_polygon.c
	${SRC_DIR}/draw/ei_draw_polyline.c
	${SRC_DIR}/draw/ei_draw_frame.h
	${SRC_DIR}/draw/ei_draw_frame.c
	${SRC_DIR}/draw/ei_draw_image.h
	${SRC_DIR}/draw/ei_draw_image.c
	${SRC_DIR}/draw/ei_draw.c
	

	${SRC_DIR}/ei_application.c
	${SRC_DIR}/ei_application_utils.h
	${SRC_DIR}/ei_application_utils.c
	${SRC_DIR}/ei_implementation.h
	${SRC_DIR}/ei_implementation.c
	${SRC_DIR}/ei_placer.c
	${SRC_DIR}/ei_widget_configure.c

	${SRC_DIR}/ei_widgetclass_toplevel.h
	${SRC_DIR}/ei_widgetclass_toplevel.c
	${SRC_DIR}/ei_widgetclass_frame.h
	${SRC_DIR}/ei_widgetclass_frame.c
	${SRC_DIR}/ei_widgetclass_button.h
	${SRC_DIR}/ei_widgetclass_button.c

	${SRC_DIR}/ei_widget_attributes.c
	${SRC_DIR}/ei_widget.c
	${SRC_DIR}/ei_widgetclass.c
	${SRC_DIR}/ei_event.c

#     ${SRC_DIR}/un_autre_fichier_source.c
)

add_library(ei STATIC			${LIB_EI_SOURCES})

# target minimal

add_executable(minimal 			${TEST_DIR}/minimal.c)
target_link_libraries(minimal		${PLATFORM_LIB_FLAGS})

# target lines

add_executable(lines			${TEST_DIR}/lines.c)
target_link_libraries(lines		ei ${PLATFORM_LIB_FLAGS})

# target lines_shape

add_executable(lines_shape			${TEST_DIR}/lines_shape.c)
target_link_libraries(lines_shape		ei ${PLATFORM_LIB_FLAGS})

# target frame

add_executable(frame			${TEST_DIR}/frame.c)
target_link_libraries(frame		ei ${PLATFORM_LIB_FLAGS})

# target button

add_executable(button			${TEST_DIR}/button.c)
target_link_libraries(button		ei ${PLATFORM_LIB_FLAGS})

# target hello_world

add_executable(hello_world		${TEST_DIR}/hello_world.c)
target_link_libraries(hello_world	ei ${PLATFORM_LIB_FLAGS})

# target puzzle

add_executable(puzzle			${TEST_DIR}/puzzle.c)
target_link_libraries(puzzle		ei ${PLATFORM_LIB_FLAGS})

# target two048

add_executable(two048			${TEST_DIR}/two048.c)
target_link_libraries(two048		ei ${PLATFORM_LIB_FLAGS})

# target minesweeper

add_executable(minesweeper		${TEST_DIR}/minesweeper.c)
target_link_libraries(minesweeper	ei ${PLATFORM_LIB_FLAGS})

# external_class

add_executable(external_class		${TEST_DIR}/external_class.c)
target_link_libraries(external_class ${EXTCLASS} ei ${PLATFORM_LIB_FLAGS})

# target to build the documentation 

add_custom_target(doc doxygen		${DOCS_DIR}/doxygen.cfg WORKING_DIRECTORY ${ROOT_DIR})

# all

add_custom_target(ig_all ALL)
add_dependencies(ig_all minimal frame button hello_world puzzle two048 minesweeper)
