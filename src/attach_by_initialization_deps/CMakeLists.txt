add_library(server server.h server.cpp)

add_subdirectory("thirdparty/message_queue")
target_link_libraries(server message_queue)
