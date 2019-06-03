project( FRFXLLSample ) 

remove_definitions( "-std=c++11" )

# add include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../src/algorithm")

file( GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" ) 

add_executable( "${PROJECT_NAME}_static"
  ${SOURCE_FILES}
)
target_link_libraries( "${PROJECT_NAME}_static" "FRFXLL_static" ) 

add_executable( ${PROJECT_NAME}
  ${SOURCE_FILES}
)
target_link_libraries( ${PROJECT_NAME} "FRFXLL" ) 
