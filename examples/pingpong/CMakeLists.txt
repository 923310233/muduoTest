add_executable(tuntuliang_client client.cc)
target_link_libraries(tuntuliang_client muduo_net)

add_executable(tuntuliang_server server.cc)
target_link_libraries(tuntuliang_server muduo_net)

add_executable(pingpong_bench bench.cc)
target_link_libraries(pingpong_bench muduo_net)

add_executable(HTTPTestServer HTTPResonpseTest.cpp)
target_link_libraries(HTTPTestServer muduo_net muduo_http)
