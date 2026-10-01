
add_library(server_lib SHARED src/server.cpp)

# Export symbols automatically if compiled on Windows
set_target_properties(server_lib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_include_directories(server_lib PUBLIC include)

target_link_libraries(server_lib PUBLIC common)