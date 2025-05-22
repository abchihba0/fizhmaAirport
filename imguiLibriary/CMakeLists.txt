cmake_minimum_required(VERSION 3.20)

# Создаем библиотеку ImGui из исходников
add_library(imgui STATIC
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

# Подключаем заголовки
target_include_directories(imgui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Опционально: добавить бэкенды (например, GLFW + OpenGL)
if (USE_GLFW)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp)
    target_link_libraries(imgui PRIVATE glfw OpenGL::GL)
endif()