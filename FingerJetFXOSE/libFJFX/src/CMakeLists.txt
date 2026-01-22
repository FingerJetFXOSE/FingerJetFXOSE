project( FJFX ) 

add_definitions( "-std=c++11" )

# add include directories
include_directories("${FRFXLL_PATH}/include")
include_directories("${FJFX_PATH}/include")

add_library( "${PROJECT_NAME}_static" STATIC
  FJFX.cpp
)

add_library( ${PROJECT_NAME} SHARED
  FJFX.cpp
)

if( USE_SANITIZER )
  target_link_libraries( ${PROJECT_NAME} "FRFXLL_static" "asan" ) 
else()
  target_link_libraries( ${PROJECT_NAME} "FRFXLL_static" ) 
endif()

set_target_properties( ${PROJECT_NAME} ${PROJECT_NAME}_static PROPERTIES
    PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/../include/FJFX.h"
)

include(GNUInstallDirs)
install( TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
