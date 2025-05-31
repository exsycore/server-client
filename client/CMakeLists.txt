file(GLOB_RECURSE CLIENT_SOURCES "*.cpp")

add_executable(client
    ${CLIENT_SOURCES}
)
