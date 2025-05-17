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

int main() {
    // Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
    #if defined(IMGUI_IMPL_OPENGL_ES2)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    #endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Airport Manager Game", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    Airport port;
    bool show_welcome = true;
    bool show_game = false;
    bool show_level_complete = false;
    int current_level = 0;
    std::string game_output;
    std::ostringstream output_stream;
    
    // Redirect cout to our string stream
    auto old_cout_buf = std::cout.rdbuf(output_stream.rdbuf());

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Welcome screen
        if (show_welcome) {
            ImGui::Begin("Welcome to Airport Manager", &show_welcome, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            ImGui::Text("Hello! This is a game that simulates the work of an airport manager.");
            ImGui::Text("During the game you need to process requests coming to the control center.");
            ImGui::Text("You can allow the takeoff or landing of the aircraft,");
            ImGui::Text("or you can send the aircraft on a second circle.");
            ImGui::Text("At each level there is a certain number of requests that you will receive,");
            ImGui::Text("you need to process them all.");
            ImGui::Text("To exit to the main menu, you can write exit at any time.");
            
            if (ImGui::Button("Start Game")) {
                show_welcome = false;
                show_game = true;
                port.gameProcessing("Begin", 0);
            }
            
            if (ImGui::Button("Exit")) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::End();
        }

        // Game screen - shows the console output
        if (show_game) {
            ImGui::Begin("Airport Manager Game", &show_game, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            
            // Display the game output
            ImGui::BeginChild("ConsoleOutput", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
            ImGui::TextUnformatted(output_stream.str().c_str());
            ImGui::SetScrollHereY(1.0f);
            ImGui::EndChild();
             
            // Input field
            static char input_buf[256] = "";
            ImGui::PushItemWidth(-1);
            if (ImGui::InputText("##Input", input_buf, IM_ARRAYSIZE(input_buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
                // Process input
                std::string input = input_buf;
                if (input == "exit") {
                    show_game = false;
                    show_welcome = true;
                    output_stream.str(""); // Clear output
                } else {
                    // Process the input (you'll need to adapt this to your game's input system)
                    output_stream << "> " << input << "\n";
                    // Here you would normally pass the input to your game logic
                }
                strcpy(input_buf, "");
            }
            ImGui::PopItemWidth();
            
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
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