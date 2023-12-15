############################################
# preamble
############################################

cmake_minimum_required(VERSION 3.22)

project(metacpp)
include(CTest)
set(VERSION 0.0.5)

option(WITH_TEST "run tests (default:on)" ON)

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
# clang support
############################################

set(LINKERFLAGS "")
set(STDLIB "")

if(WIN32)
    set(FLAGS "/W3")
else()
    set(FLAGS "-Wall -Wextra")
endif()


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(STDLIB "c++abi")
    set(LINKERFLAGS "-stdlib=libc++ -fcoroutines-ts ")
    set(FLAGS "-std=c++20 -stdlib=libc++ -fcoroutines-ts ")
endif()


############################################
# OS support (Linux/Windows)
############################################

if(WITH_TEST)
IF (WIN32)

    if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    endif()

    find_package(GTest REQUIRED)
	find_package(jsoncpp REQUIRED)
	find_package(patex REQUIRED)
	find_package(expat REQUIRED)

	set(OSLIBS "Ws2_32" "Rpcrt4" "Shlwapi")
	set(BUILDFLAGS "${FLAGS}")

	find_library(JSONCPP_LIBRARY NAMES jsoncpp)

	set(LIBS patex ${GTEST_LIBRARIES}  ${EXPAT_LIBRARIES} ${JSONCPP_LIBRARY}  ${STDLIB}  )

 ELSEIF (UNIX)

    set(OSLIBS "pthread")
	set(BUILDFLAGS "${FLAGS}")

    find_package(expat REQUIRED)
    find_package(patex REQUIRED)

    set(LINKER_FLAGS " ${LINKER_FLAGS} ) #  ${JSONCPP_LDFLAGS}")

    set(LIBS patex jsoncpp_static expat gtest ${STDLIB} )

ENDIF ()
endif()



############################################
# set linker and compiler flags
############################################

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LINKERFLAGS}" )
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -DMOL_PROMISE_DEBUG")
set(CMAKE_CXX_FLAGS  	    "${CMAKE_CXX_FLAGS} ${BUILDFLAGS}")


############################################
# gtest & jsoncpp
############################################
if(UNIX AND WITH_TEST) 

    include(FetchContent)

    # gtest
    FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    #FetchContent_MakeAvailable(googletest)

    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

    # jsoncpp

    set(JSONCPP_LIB_BUILD_STATIC ON)
    set(BUILD_STATIC_LIBS On)
    set(BUILD_SHARED_LIBS Off)
    set(JSONCPP_WITH_TESTS Off) 

    FetchContent_Declare(
        jsoncpp
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/open-source-parsers/jsoncpp/archive/69098a18b9af0c47549d9a271c054d13ca92b006.zip
    )

    FetchContent_GetProperties(jsoncpp)
    if(NOT jsoncpp_POPULATED)
        FetchContent_Populate(jsoncpp)
        add_subdirectory(${jsoncpp_SOURCE_DIR} ${jsoncpp_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
    
endif()



message("+++++++++++++++++")
message("BUILDFLAGS: ${BUILDFLAGS}")
message("LIBS: ${LIBS}")
message("LDFLAGS: ${LINKER_FLAGS}")
message("+++++++++++++++++")

############################################
# include directories
############################################

include_directories(include)

if(UNIX AND WITH_TEST)
	get_target_property(JSONCPP_INCLUDE_DIR jsoncpp_static INTERFACE_INCLUDE_DIRECTORIES)
    message("JSONCPP: ${jsoncpp_INCLUDE_DIR})")
	include_directories(${jsoncpp_INCLUDE_DIR})
endif()


############################################
# sources
############################################

file(GLOB SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/${CMAKE_PROJECT_NAME}/*.h")



############################################
# tests
############################################

if(WITH_TEST)

add_executable(Tests t/test.cpp)
add_executable(SetterGetterTests t/getter_setter.cpp)
add_executable(JsonTests t/json.cpp)
add_executable(NJsonTests t/njson.cpp)
add_executable(XmlTests t/xml.cpp)

############################################
# libs for tests
############################################

target_link_libraries(Tests ${LIBS} ${BACKEND} ${OSLIBS} )
target_link_libraries(SetterGetterTests ${LIBS} ${BACKEND} ${OSLIBS} )
target_link_libraries(JsonTests  ${LIBS} ${BACKEND} ${OSLIBS} )
target_link_libraries(NJsonTests  ${LIBS} ${BACKEND} ${OSLIBS} )
target_link_libraries(XmlTests  ${LIBS} ${BACKEND} ${OSLIBS} )


############################################
# run tests
############################################

add_test(NAME Tests COMMAND Tests)
add_test(NAME SetterGetterTests COMMAND SetterGetterTests)
add_test(NAME JsonTests COMMAND JsonTests)
add_test(NAME NJsonTests COMMAND NJsonTests)
add_test(NAME XmlTests COMMAND XmlTests)	


endif()

 
############################################
# install rules
############################################

#install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION lib EXPORT metacpp-targets)
install(FILES ${HEADERS} DESTINATION include/metacpp)
install(DIRECTORY "include/${CMAKE_PROJECT_NAME}/" DESTINATION "include/${CMAKE_PROJECT_NAME}/" )
install(FILES ${CMAKE_PROJECT_NAME}.pc DESTINATION lib/pkgconfig)


############################################
# Create and install package configuration 
# and version files.
############################################

configure_file(
   ${metacpp_SOURCE_DIR}/metacpp-config.cmake.in
   ${metacpp_BINARY_DIR}/metacpp-config.cmake @ONLY)

configure_file(
   ${metacpp_SOURCE_DIR}/metacpp-config-version.cmake.in
   ${metacpp_BINARY_DIR}/metacpp-config-version.cmake @ONLY)

install(FILES ${metacpp_BINARY_DIR}/metacpp-config.cmake
         ${metacpp_BINARY_DIR}/metacpp-config-version.cmake
         DESTINATION lib/metacpp-${VERSION})

############################################
# packaging
############################################

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
include(package)
