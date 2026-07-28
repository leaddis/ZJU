# 每一个 test_xxx.cpp 都会编译成 test_xxx 可执行程序
# add_executable 表示增加一个可执行程序的目标
file(GLOB TEST_SOURCE test_*.cpp)
foreach(f ${TEST_SOURCE})
    get_filename_component(tg ${f} NAME_WE)
    add_executable(${tg} ${f} ${SOURCE})
    target_link_libraries(${tg} bex baltam_core)
    if(WIN32)
        target_link_libraries(${tg} openblas)
    else()
        target_link_libraries(${tg} lapacke)
    endif()
    # 添加到 test 目标
    add_test(NAME ${tg} COMMAND ${tg})
endforeach()

