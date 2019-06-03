project( testFRFXLL ) 

file( GLOB TEST_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.h" ) 
get_filename_component(CXXTEST_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/runner.cpp" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

execute_process(
  COMMAND python --version
)
execute_process(
  COMMAND python cxxtestgen --version
  WORKING_DIRECTORY "${CXXTEST_PATH}/bin"
)
execute_process(
  COMMAND python cxxtestgen --runner=ParenPrinter --no-static-init --no-eh --no-std --output=${CXXTEST_OUTPUT} --world=${PROJECT_NAME} ${TEST_HEADER_FILES}
  WORKING_DIRECTORY "${CXXTEST_PATH}/bin"
)
if(EXISTS "${CXXTEST_OUTPUT}")
  message( STATUS "${Gn}Generated runner.cpp${Na}")
endif()

# add include directories
include_directories("${CXXTEST_PATH}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../TestVectors")

add_executable( ${PROJECT_NAME}
  runner.cpp 
  dpfrUnitTest.cpp
)
target_link_libraries( ${PROJECT_NAME} "FRFXLL_static" "TestVectors" ) 



