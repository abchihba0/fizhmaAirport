#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "Airplane.cpp"
#include "Airport.cpp"
#include "VPP.cpp"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>



// Добавлено объявление для SetupImGuiStyle
void SetupImGuiStyle();

//GLFWwindow* window = nullptr;

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Custom styling function
void SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Colors
    style.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.11f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.35f, 0.35f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.45f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.50f, 1.00f);
    
    // Rounding
    style.WindowRounding = 5.0f;
    style.ChildRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;
    
    // Padding
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(10, 5);
}


int main() {
    // Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Create window with graphics context
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Airport Manager Game", primaryMonitor, NULL);
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight); // Получаем размеры основного окна
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.FontGlobalScale = 1.5f; // Увеличивает все шрифты в 1.5 раза
    SetupImGuiStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    Airport port;
    bool showMainMenu = true;

    // Основной цикл
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame (ТОЛЬКО ОДИН РАЗ ЗА КАДР)
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (showMainMenu) {
            // Отрисовка главного меню
            ImVec2 centerPos = ImVec2(
                (windowWidth - 1500) * 0.5f,  // (ширина_экрана - ширина_окна) / 2
                (windowHeight - 800) * 0.5f  // (высота_экрана - высота_окна) / 2
            );
            ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always); // Всегда центрировать
            ImGui::SetNextWindowSize(ImVec2(1500, 800), ImGuiCond_Always); // Фиксированный размер
            
            ImGui::Begin("Main Menu", nullptr, 
                ImGuiWindowFlags_NoTitleBar | 
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove);
            
            ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "Airport Manager Game");
            ImGui::Separator();
            
            ImGui::TextWrapped("Welcome to the Airport Manager simulation game! This is a game that simulates the work of an airport manager.");
            ImGui::Spacing();
            
            ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "Game Rules:");
            ImGui::BulletText("Process requests coming to the control center");
            ImGui::BulletText("Allow aircraft to take off or land on the appropriate runways");
            ImGui::BulletText("If necessary, send planes to the second circle");
            ImGui::BulletText("At each level there is a certain number of requests that you will receive, you need to process them all");
            
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Spacing();
            
            ImGui::Text("Type 'exit' at any time to return to this menu");
            
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
            if (ImGui::Button("Start Game", ImVec2(200, 40))) {
                showMainMenu = false;
            }
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
            if (ImGui::Button("Exit", ImVec2(200, 40))) {
                glfwSetWindowShouldClose(window, true);
            }
            
            ImGui::End();
        } else {
            // Запуск игровой логики
            static bool gameInitialized = false;
            if (!gameInitialized) {
                port.gameProcessing("Begin", 0, window);
                gameInitialized = true;
            }
            // Кнопка возврата в меню
            if (ImGui::Button("Back to Menu", ImVec2(200, 40))) {
                showMainMenu = true;
            }
        }

        // Rendering (ТОЛЬКО ОДИН РАЗ ЗА КАДР)
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.09f, 0.10f, 0.12f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
