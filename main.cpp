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

void ShowWelcomeWindow(bool& p_open, Airport& port, bool& show_game, int windowWidth, int windowHeight, bool& flagRunning) {
    ImVec2 centerPos = ImVec2(
        (windowWidth - 1500) * 0.5f,  // (ширина_экрана - ширина_окна) / 2
        (windowHeight - 800) * 0.5f  // (высота_экрана - высота_окна) / 2
    );
    ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always); // Всегда центрировать
    ImGui::SetNextWindowSize(ImVec2(1500, 800), ImGuiCond_Always); // Фиксированный размер
    
    ImGui::Begin("Welcome to Airport Manager", &p_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    
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
    
    // Center buttons
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
    if (ImGui::Button("Start Game", ImVec2(200, 40))) {
        p_open = false;
        show_game = true;
    }
    
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
    if (ImGui::Button("Exit", ImVec2(200, 40))) {
        p_open = false;
        flagRunning = false;
    }
    
    ImGui::End();
}

void ShowGameWindow(bool& p_open, Airport& port, bool& show_welcome, bool& show_level_complete, int& current_level, std::string& game_output, int windowWidth, int windowHeight) {
    static std::string output_text;
    static char input_buf[256] = "";
    static bool scroll_to_bottom = false;
    
    ImVec2 centerPos = ImVec2(
        (windowWidth - 1500) * 0.5f,  // (ширина_экрана - ширина_окна) / 2
        (windowHeight - 800) * 0.5f  // (высота_экрана - высота_окна) / 2
    );
    ImGui::SetNextWindowPos(centerPos, ImGuiCond_Always); // Всегда центрировать
    ImGui::SetNextWindowSize(ImVec2(1500, 800), ImGuiCond_Always); // Фиксированный размер
    ImGui::Begin("Airport Manager Game", &p_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    
    // Game output display
    ImGui::BeginChild("Output", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() - 120), true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.9f, 1.0f, 1.0f));
    ImGui::TextUnformatted(output_text.c_str());
    ImGui::PopStyleColor();
    if (scroll_to_bottom) {
        ImGui::SetScrollHereY(1.0f);
        scroll_to_bottom = false;
    }
    // Auto-scroll
    // if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    //     ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
    ImGui::Separator();
    
    // Runway selection buttons
    ImGui::Text("Select Runway:");
    auto progress = port.returnMemory();
    if (current_level < progress.size()) {
        LevelProgress* level_progress = progress[current_level];
        for (int i = 0; i < level_progress->vpp_count; i++) {
            if (i > 0) ImGui::SameLine();
            bool is_busy = level_progress->vpps[i]->getBusyTime() > 0;
            if (is_busy) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
            }
            if (ImGui::Button(("Runway " + std::to_string(i+1)).c_str(), ImVec2(100, 40))) {
                if (!is_busy) {
                    output_text += "> Selected Runway " + std::to_string(i+1) + "\n";
                    game_output += port.processCommand(std::to_string(i+1), current_level + 1);
                    scroll_to_bottom = true;
                }
            }
            if (is_busy) {
                ImGui::PopStyleColor(2);
            }
        }
    }
    
    // Action buttons
    ImGui::Spacing();
    if (ImGui::Button("Send to Next Circle", ImVec2(230, 40))) {
        output_text += "> Sent to next circle\n";
        game_output += port.processCommand("-1", current_level + 1);
        scroll_to_bottom = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Skip Request", ImVec2(230, 40))) {
        output_text += "> Skipped request\n";
        game_output += port.processCommand("0", current_level + 1);
        scroll_to_bottom = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Exit to Menu", ImVec2(230, 40))) {
        p_open = false;
        show_welcome = true;
        output_text.clear();
    }
    
    ImGui::Separator();
    
    // Status bar
    ImGui::Text("Level: %d | Requests processed: %d/%d", 
                current_level + 1, 
                (current_level < progress.size()) ? progress[current_level]->countOfProcessedRequests : 0,
                port.getLevelRequestCount(current_level));
    
    // Check if level is complete
    if (current_level < progress.size() && 
        progress[current_level]->countOfProcessedRequests >= port.getLevelRequestCount(current_level)) {
        p_open = false;
        show_level_complete = true;
    }

    ImGui::End();
}

void ShowLevelCompleteWindow(bool* p_open, Airport* port, int level, bool* show_welcome, bool* show_game, std::vector<LevelProgress*>* progress_data) {
    ImGui::Begin("Level Complete!", p_open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowSize(ImVec2(500, 300));
    
    ImGui::Text("Congratulations on completing Level %d!", level + 1);
    ImGui::Spacing();
    
    if (level < progress_data->size()) {
        LevelProgress* level_progress = (*progress_data)[level];
        int total_requests = port->getLevelRequestCount(level);
        int processed = level_progress->countOfProcessedRequests;
        int correct = level_progress->countOfCorrectProcessedRequests;
        
        // Display level statistics
        ImGui::Text("Requests processed: %d/%d", processed, total_requests);
        ImGui::Text("Correctly processed: %d", correct);
        
        float progress = (float)correct / total_requests;
        ImGui::Text("Success rate: %.1f%%", progress * 100);
        ImGui::ProgressBar(progress, ImVec2(-1, 20));
        
        // Determine if player can advance to next level
        bool can_advance = progress >= 0.75f && level < progress_data->size() - 1;
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (can_advance) {
            if (ImGui::Button("Next Level", ImVec2(150, 40))) {
                *p_open = false;
                *show_game = true;
                level++; // Move to next level
            }
            ImGui::SameLine();
        }
        
        if (ImGui::Button("Replay Level", ImVec2(150, 40))) {
            // Reset level progress
            level_progress->countOfProcessedRequests = 0;
            level_progress->countOfCorrectProcessedRequests = 0;
            *p_open = false;
            *show_game = true;
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Main Menu", ImVec2(150, 40))) {
            *p_open = false;
            *show_welcome = true;
        }
    }
    
    ImGui::End();
}


int main() {
    // Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.FontGlobalScale = 1.5f; // Увеличивает все шрифты в 1.5 раза

    if (ImGui::IsKeyPressed(ImGuiKey_Equal)) {  // Увеличить при нажатии '+'
        io.FontGlobalScale *= 1.1f;
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Minus)) {  // Уменьшить при нажатии '-'
        io.FontGlobalScale *= 0.9f;
    }
    
    // Setup custom style
    SetupImGuiStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    // Our state
    Airport port;
    bool flagRunning = true;
    bool show_welcome = true;
    bool show_game = false;
    bool show_level_complete = false;
    int current_level = 0;
    std::string game_output;
    std::ostringstream output_stream;
    
    // Redirect cout to our string stream
    auto old_cout_buf = std::cout.rdbuf(output_stream.rdbuf());

    // Main loop
    while (!glfwWindowShouldClose(window) && flagRunning) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Welcome screen
        if (show_welcome) {
            ShowWelcomeWindow(show_welcome, port, show_game, windowWidth, windowHeight, flagRunning);
        }

        // Game screen - shows the console output
        if (show_game) {
            ShowGameWindow(show_game, port, show_welcome, show_level_complete, current_level, game_output, windowWidth, windowHeight);
        }

        if (show_level_complete) {
            auto progress_data = port.returnMemory();
            ShowLevelCompleteWindow(&show_level_complete, &port, current_level, &show_welcome, &show_game, &progress_data);
        }

        // Rendering
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
    std::cout.rdbuf(old_cout_buf); // Restore cout
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


// Display current level status
    // if (current_level > 0) {
    //     auto level_progress = port.returnMemory()[current_level-1];
    //     auto requests_info = port.returnRequestsInfo(); // Нужно добавить этот метод в класс Airport
        
    //     float progress = (float)level_progress->countOfCorrectProcessedRequests / 
    //                 (float)requests_info.first[current_level-1];
        
    //     ImGui::Text("Level %d Progress: ", current_level);
    //     ImGui::SameLine();
    //     ImGui::ProgressBar(progress, ImVec2(200, 20));
    //     ImGui::SameLine();
    //     ImGui::Text("%d/%d", level_progress->countOfCorrectProcessedRequests, 
    //             requests_info.first[current_level-1]);
        
    //     // Display runways status
    //     ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "Runways Status:");
    //     for (size_t i = 0; i < level_progress->vpps.size(); i++) {
    //         ImGui::Text("Runway %d: Length %dm - %s", 
    //                 i+1, 
    //                 level_progress->vpps[i]->get_lenght(),
    //                 level_progress->vpps[i]->getBusyTime() > 0 ? "BUSY" : "AVAILABLE");
    //     }
    // }