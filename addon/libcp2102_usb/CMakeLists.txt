project(cp2102_usb)

set(SRCS)

list(APPEND SRCS "src/cp2102_libusb.c")

if(APPLE)
    list(APPEND SRCS "src/tty_utils_darwin.c")
elseif(UNIX)
    list(APPEND SRCS "src/tty_utils_linux.c")
else()
    list(APPEND SRCS "src/tty_utils_dummy.c")
endif()

add_library(${PROJECT_NAME} STATIC ${SRCS})

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

find_package(PkgConfig)

pkg_search_module(USB REQUIRED IMPORTED_TARGET libusb-1.0)
target_link_libraries(${PROJECT_NAME} PkgConfig::USB)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework CoreFoundation")
endif()
