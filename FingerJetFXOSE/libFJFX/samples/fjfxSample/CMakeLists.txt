project( fjfxSample ) 

# add include directories
include_directories("${FRFXLL_PATH}/include")
include_directories("${FJFX_PATH}/include")

add_executable( "${PROJECT_NAME}_static"
  fjfxSample.c
)
target_link_libraries( "${PROJECT_NAME}_static" "FJFX_static" "FRFXLL_static" ) 

add_executable( ${PROJECT_NAME}
  fjfxSample.c
)
target_link_libraries( "${PROJECT_NAME}" "FJFX" ) 

