file(GLOB_RECURSE SERVER_SOURCES "*.cpp")

add_executable(server
    ${SERVER_SOURCES}
)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})