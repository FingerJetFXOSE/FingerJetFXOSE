project( FRFXLL ) 

add_definitions( "-std=c++11" )

# add include directories
include_directories("${FRFXLL_PATH}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/algorithm")

set(SOURCE_FILES "")
file( GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.cpp" ) 

add_library( "${PROJECT_NAME}_static" STATIC
  ${SOURCE_FILES}
)

add_library( ${PROJECT_NAME} SHARED
  ${SOURCE_FILES}
)
if( USE_SANITIZER )
  target_link_libraries( ${PROJECT_NAME} "asan" ) 
endif()

set(PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/FRFXLL.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/FRFXLL_Results.h"
)
set_target_properties( ${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
)
include(GNUInstallDirs)
install( TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)