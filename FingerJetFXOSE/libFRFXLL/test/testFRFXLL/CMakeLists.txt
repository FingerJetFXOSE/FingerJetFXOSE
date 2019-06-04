project( testFRFXLL ) 

add_definitions( "-std=c++11" )

file( GLOB TEST_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" ) 
get_filename_component(CXXTEST_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/runner.cpp" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
include( "${FRFXLL_PATH}/test/cxxtestgen.cmake" )

if(EXISTS "${CXXTEST_OUTPUT}")
  message( STATUS "${Gn}Found runner.cpp${Na}")

  # add include directories
  include_directories("${CXXTEST_PATH}")
  include_directories("${FRFXLL_PATH}/include")
  include_directories("${FRFXLL_PATH}/test/TestVectors")

  add_executable( ${PROJECT_NAME}
    runner.cpp 
    dpfrUnitTest.cpp
  )
  target_link_libraries( ${PROJECT_NAME} "FRFXLL_static" "TestVectors" ) 

endif()


