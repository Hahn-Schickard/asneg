# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# prebuild
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

configure_file(
    "${PROJECT_SOURCE_DIR}/OpcUaServer/Build/BuildConfig.h.in"
    "${PROJECT_BINARY_DIR}/BuildConfig.h"
)

set(BUILD_DATE "${CURRENT_DATE}" CACHE STRING "current build an time")



# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# build 
# 
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
)

file(
    GLOB OpcUaServer_SRC 
    ${PROJECT_SOURCE_DIR}/OpcUaServer/*/*.cpp
)

add_executable(
    OpcUaServer${VERSION_MAJOR}
    ${OpcUaServer_SRC}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
    OpcUaServer${VERSION_MAJOR}
    OpcUaStackCore
    OpcUaStackServer
    OpcUaStackClient
    ${CMAKE_DL_LIBS}
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# install
# 
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
install(
    TARGETS OpcUaServer${VERSION_MAJOR} 
    DESTINATION /usr/bin
    COMPONENT applications
)
