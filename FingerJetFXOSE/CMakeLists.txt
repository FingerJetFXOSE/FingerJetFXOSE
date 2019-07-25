cmake_minimum_required (VERSION 3.14)

project( FingerJetFXOSE )

set( NO_SEARCH TRUE )

# set path vars
set( ROOT_PATH ${PROJECT_SOURCE_DIR} )
set( FRFXLL_PATH ${ROOT_PATH}/libFRFXLL )
set( FJFX_PATH ${ROOT_PATH}/libFJFX )
get_filename_component(CXXTEST_PATH "../cxxtest" ABSOLUTE BASE_DIR "${ROOT_PATH}")

# set colors for cmake comand line
include( "${ROOT_PATH}/cmake/colors.cmake" )

# detect target platform
include( "${ROOT_PATH}/cmake/target.cmake" )

# setup compiler
include( "${ROOT_PATH}/cmake/compiler.cmake" )

# create build and distribution path
include( "${ROOT_PATH}/cmake/folders.cmake" )

# set build version
include( "${ROOT_PATH}/cmake/version.cmake" )

# write summary
message( STATUS "----- Configuring ${PROJECT_NAME} -----" ) 
message( STATUS "-- Project root path: '${ROOT_PATH}'" )
message( STATUS "-- Project build path: '${BUILD_PATH}'" )
message( STATUS "-- Project distribution path: '${DIST_PATH}'" )

add_subdirectory("${ROOT_PATH}/libFRFXLL/src/" "${BUILD_PATH}/libFRFXLL/src/")
add_subdirectory("${ROOT_PATH}/libFRFXLL/samples/FRFXLLSample/" "${BUILD_PATH}/libFRFXLL/samples/FRFXLLSample/")
add_subdirectory("${ROOT_PATH}/libFRFXLL/test/TestVectors/" "${BUILD_PATH}/libFRFXLL/test/TestVectors/")
add_subdirectory("${ROOT_PATH}/libFRFXLL/test/testFRFXLL/" "${BUILD_PATH}/libFRFXLL/test/testFRFXLL/")
add_subdirectory("${ROOT_PATH}/libFRFXLL/test/testFRFXLLInternals/" "${BUILD_PATH}/libFRFXLL/test/testFRFXLLInternals/")

add_subdirectory("${ROOT_PATH}/libMINEX/src/" "${BUILD_PATH}/libMINEX/src")

add_subdirectory("${ROOT_PATH}/libFJFX/src/" "${BUILD_PATH}/libFJFX/src")
add_subdirectory("${ROOT_PATH}/libFJFX/samples/fjfxSample/" "${BUILD_PATH}/libFJFX/samples/fjfxSample/")




