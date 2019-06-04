project( frfxllSample ) 

# add include directories
include_directories("${FRFXLL_PATH}/include")
include_directories("${FRFXLL_PATH}/src/algorithm")

file( GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" ) 

add_executable( "${PROJECT_NAME}_static"
  ${SOURCE_FILES}
)
if( USE_SANITIZER )
  target_link_libraries( "${PROJECT_NAME}_static" "FRFXLL_static" "asan") 
else()
  target_link_libraries( "${PROJECT_NAME}_static" "FRFXLL_static" ) 
endif()

add_executable( ${PROJECT_NAME}
  ${SOURCE_FILES}
)
if( USE_SANITIZER )
  target_link_libraries( ${PROJECT_NAME} "FRFXLL" "asan" ) 
else()
  target_link_libraries( ${PROJECT_NAME} "FRFXLL" ) 
endif()
