add_executable(sandbox 
    # headers
    include/sandbox.h

    # source
    src/sandbox.cpp
)

target_include_directories(sandbox PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(sandbox PRIVATE
    cgx 
)
