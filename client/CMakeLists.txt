add_library(client_lib SHARED src/client.cpp)

set_target_properties(client_lib PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_include_directories(client_lib PUBLIC include)

target_link_libraries(client_lib PUBLIC common)



