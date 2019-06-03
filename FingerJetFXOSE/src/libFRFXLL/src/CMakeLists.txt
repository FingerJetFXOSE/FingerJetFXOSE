project( FRFXLL ) 

# add include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/algorithm")

set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/libFRFXLL/*.cpp" ) 

add_library( "${PROJECT_NAME}_static" STATIC
  ${SOURCE_FILES}
)

add_library( ${PROJECT_NAME} SHARED
  ${SOURCE_FILES}
)
