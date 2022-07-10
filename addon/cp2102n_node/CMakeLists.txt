project(cp2102n)

set(CMAKE_CXX_STANDARD 11)

set(SRCS "src/cp2102n.cc")

add_library(${PROJECT_NAME} SHARED
    ${SRCS}
    ${CMAKE_JS_SRC}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    SUFFIX ".node"
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})

target_compile_definitions(${PROJECT_NAME} PRIVATE -DNAPI_VERSION=6)

target_link_libraries(${PROJECT_NAME} cp2102_usb)
