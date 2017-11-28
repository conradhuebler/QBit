cmake_minimum_required(VERSION 3.1)
project(QBit)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND git log -1 --format=%aD
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_DATE
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions("-DGIT_COMMIT_HASH=${GIT_COMMIT_HASH}")
add_definitions("-DGIT_BRANCH=${GIT_BRANCH}")
add_definitions("-DGIT_COMMIT_DATE=${GIT_COMMIT_DATE}")


# configure_file (
#   "${PROJECT_SOURCE_DIR}/src/global_config.h.in"
#   "${PROJECT_BINARY_DIR}/src/global_config.h"
# )
# 
# configure_file(
#   "${CMAKE_SOURCE_DIR}/src/version.h.in"
#   "${CMAKE_BINARY_DIR}/src/version.h"
# )

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-reorder -pedantic \
                                           -Wextra -Wcast-align -Wcast-qual  -Wchar-subscripts  \
                                           -Wcomment -Wdisabled-optimization \
                                           -Wformat  -Wformat=2 -Wformat-nonliteral -Wformat-security\
                                           -Wformat-y2k  -Wimport  -Winit-self  -Winline -Winvalid-pch\
                                           -Wunsafe-loop-optimizations  -Wmissing-braces\
                                           -Wmissing-field-initializers -Wmissing-format-attribute  \
                                           -Wmissing-include-dirs -Wmissing-noreturn -Wpacked  -Wparentheses\
                                           -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point\
                                           -Wsign-compare  -Wstack-protector -Wstrict-aliasing\
                                           -Wstrict-aliasing=2 -Wswitch \
                                           -Wtrigraphs  -Wuninitialized -Wunknown-pragmas  -Wunreachable-code\
                                           -Wunused -Wunused-function  -Wunused-label  -Wunused-parameter\
                                           -Wunused-value  -Wunused-variable  -Wvariadic-macros\
                                           -Wvolatile-register-var  -Wwrite-strings -Wno-deprecated-declarations\
                                           -Wno-error=unused-local-typedefs -Wno-error=enum-compare")                                        
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} eigen/ libpeakpick/)

set(qbit_core_SRC
                kissfft/kiss_fft.c
                src/core/nmrspec.cpp
                src/core/filehandler.cpp
                src/func/fit_threaded.cpp
)

add_library(qbit_core STATIC ${qbit_core_SRC})
qt5_use_modules(qbit_core Core Gui)

set(qbit_gui 
                src/gui/dialogs/selectguess.cpp
                src/gui/dialogs/fitparameter.cpp
                src/gui/widgets/chartview.cpp
                src/gui/widgets/multispecwidget.cpp
                src/gui/widgets/peakwidget.cpp
                src/gui/widgets/fileswidget.cpp
                src/gui/widgets/peakcallout.cpp
                src/gui/qbit.cpp
                src/main.cpp)

add_executable(QBit ${qbit_gui})
qt5_use_modules(QBit Widgets Charts)

set_property(TARGET QBit PROPERTY CXX_STANDARD 14)

# generate proper GUI program on specified platform
if(WIN32) # Check if we are on Windows
	if(MSVC) # Check if we are using the Visual Studio compiler
		set_target_properties(QBit PROPERTIES
			WIN32_EXECUTABLE YES
			LINK_FLAGS "/ENTRY:mainCRTStartup"
		)
	elseif(CMAKE_COMPILER_IS_GNUCXX)
			 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested but seems to work
	else()
		message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
	endif(MSVC)
elseif(APPLE)
	set_target_properties(QBit PROPERTIES
			MACOSX_BUNDLE YES
	)
elseif(UNIX)
	# Nothing special required
	set (_Theme ON)
else()
	message(SEND_ERROR "You are on an unsupported platform! (Not Win32, Mac OS X or Unix)")
endif(WIN32)

target_link_libraries(QBit qbit_core ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

if(UNIX)
    target_link_libraries(QBit pthread dl )
endif(UNIX)

install(TARGETS QBit RUNTIME DESTINATION bin)
