add_executable(shorturl shorturl.cc )
target_link_libraries(shorturl muduo_http)

add_executable(Testscript Testscript.cpp)
target_link_libraries(Testscript muduo_net)