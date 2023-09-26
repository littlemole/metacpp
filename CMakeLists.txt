############################################
# preamble
############################################
cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0054 NEW)
project(metacpp)
include(CTest)

############################################
# c++ std
############################################

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

############################################
# debug mode produces libs with d suffix
############################################

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

############################################
# basic dependecies
############################################
option(WITH_TEST "run tests (default:on)" ON)
if(WITH_TEST)
find_package(GTest REQUIRED)
endif()

############################################
# clang support
############################################

set(LINKERFLAGS "")
set(STDLIB "")
set(FLAGS "")


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(STDLIB "c++abi")
    set(LINKERFLAGS "-stdlib=libc++ -fcoroutines-ts ")
    set(FLAGS "-std=c++20 -stdlib=libc++ -fcoroutines-ts -D_RESUMABLE_FUNCTIONS_SUPPORTED")
endif()

############################################
# OS support (Linux/Windows)
############################################
if(WITH_TEST)
IF (WIN32)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

message("+++++++++++++++++")
message("${CMAKE_BUILD_TYPE}")
message("+++++++++++++++++")

	find_package(expat CONFIG REQUIRED)

	set(OSLIBS "Ws2_32" "Rpcrt4" "Shlwapi")
	set(BUILDFLAGS "-await ${FLAGS}")

	find_library(JSONCPP_LIBRARY NAMES jsoncpp)

	set(LIBS ${GTEST_LIBRARIES}  ${EXPAT_LIBRARIES} ${JSONCPP_LIBRARY}  ${STDLIB}  )

	if(${CMAKE_BUILD_TYPE} MATCHES "(Release)|(RelWithDebInfo)")

		find_library(PATEX_LIBRARY NAMES patex.lib   NO_CACHE)
		set(LIBS ${PATEX_LIBRARY} ${LIBS} )
        else()
		find_library(PATEX_LIBRARY NAMES patexd.lib PATH_SUFFIXES "debug/lib")
		set(LIBS ${PATEX_LIBRARY} ${LIBS})
	endif()

 ELSEIF (UNIX)

    set(OSLIBS "pthread")
	set(BUILDFLAGS "${FLAGS}")

    find_package(PkgConfig)
    pkg_check_modules(JSONCPP REQUIRED jsoncpp)
    pkg_check_modules(EXPAT REQUIRED expat)
    pkg_check_modules(PATEX REQUIRED patex)

    set(LINKER_FLAGS " ${LINKER_FLAGS} ${JSONCPP_LDFLAGS}")

    set(LIBS ${GTEST_LIBRARIES} ${JSONCPP_LIBRARIES} ${PATEX_LIBRARIES} ${EXPAT_LIBRARIES} ${STDLIB} )

ENDIF ()
endif()

message("+++++++++++++++++")
message("BUILDFLAGS: ${BUILDFLAGS}")
message("LIBS: ${LIBS}")
message("LDFLAGS: ${LINKER_FLAGS}")
message("+++++++++++++++++")


############################################
# set linker and compiler flags
############################################

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LINKERFLAGS}" )
set(CMAKE_CXX_FLAGS_DEBUG  	"${CMAKE_CXX_FLAGS_DEBUG} -DMOL_PROMISE_DEBUG")
set(CMAKE_CXX_FLAGS  		"${CMAKE_CXX_FLAGS} ${BUILDFLAGS}")


############################################
# include directories
############################################

include_directories(include)
if(WITH_TEST)
include_directories(${EXPAT_INCLUDE_DIR})
include_directories(${GTEST_INCLUDE_DIR})
endif()
############################################
# sources
############################################

file(GLOB SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/${CMAKE_PROJECT_NAME}/*.h")

############################################
# lib target
############################################

#add_library(${CMAKE_PROJECT_NAME}  STATIC ${SOURCES})

if(WITH_TEST)

############################################
# tests
############################################

add_executable(Tests t/test.cpp)
add_executable(SetterGetterTests t/getter_setter.cpp)
add_executable(JsonTests t/json.cpp)
add_executable(XmlTests t/xml.cpp)

############################################
# libs for tests
############################################

target_link_libraries(Tests ${LIBS} ${BACKEND} ${OSLIBS} )
target_link_libraries(SetterGetterTests ${LIBS} ${BACKEND} ${OSLIBS} )
target_link_libraries(JsonTests  ${LIBS} ${BACKEND} ${OSLIBS} )
target_link_libraries(XmlTests  ${LIBS} ${BACKEND} ${OSLIBS} )

target_link_options(Tests PRIVATE ${JSONCPP_LDFLAGS})
target_link_options(SetterGetterTests PRIVATE ${JSONCPP_LDFLAGS})
target_link_options(JsonTests PRIVATE ${JSONCPP_LDFLAGS})
target_link_options(XmlTests PRIVATE ${JSONCPP_LDFLAGS})

############################################
# run tests
############################################

	add_test(NAME Tests COMMAND Tests)
	add_test(NAME SetterGetterTests COMMAND SetterGetterTests)
	add_test(NAME JsonTests COMMAND JsonTests)
	add_test(NAME XmlTests COMMAND XmlTests)	
else()
endif()

 
############################################
# install rules
############################################

#install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION lib)
install(FILES ${HEADERS} DESTINATION include/metacpp)

INSTALL( DIRECTORY "include/${CMAKE_PROJECT_NAME}/" DESTINATION "include/${CMAKE_PROJECT_NAME}/" )

install(FILES ${CMAKE_PROJECT_NAME}.pc DESTINATION lib/pkgconfig)



