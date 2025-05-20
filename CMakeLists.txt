cmake_minimum_required(VERSION 3.20)
project(MyImGuiApp)

# Настройка путей
set(GLFW_DIR "C:/msys641/mingw64")
include_directories(${GLFW_DIR}/include)
link_directories(${GLFW_DIR}/lib)

# Поиск пакетов
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED PATHS ${GLFW_DIR})

# Подключение ImGui
add_subdirectory(imguiLibrary/imgui)

# Создание исполняемого файла
add_executable(my_app 
    main.cpp
    # Добавляем файлы бэкендов ImGui
    imguiLibrary/imgui/backends/imgui_impl_glfw.cpp
    imguiLibrary/imgui/backends/imgui_impl_opengl3.cpp
)

# Линковка библиотек
target_link_libraries(my_app PRIVATE
    imgui
    glfw
    ${OPENGL_LIBRARIES}
    opengl32
)