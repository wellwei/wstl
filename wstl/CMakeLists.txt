add_library(wstl INTERFACE)

# 设置头文件路径
target_include_directories(wstl INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)