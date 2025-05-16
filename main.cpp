#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <vector>

int main() {
//
    Airport port;
    bool showWelcome = true;
    bool inGame = false;
    bool showLevelComplete = false;
    char inputBuffer[256] = "";
    std::string consoleOutput;
    int currentLevel = 0;
    float scrollToBottom = false;
};

void AddToConsole(GameState& state, const std::string& text) {
    state.consoleOutput += text + "\n";
    state.scrollToBottom = true;
}

void ProcessGameInput(GameState& state, const std::string& input) {
    if (input == "exit") {
        state.inGame = false;
        state.showWelcome = true;
        AddToConsole(state, "Exiting to main menu...");
        return;
    }

    std::istringstream iss(input);
    std::ostringstream oss;
    std::streambuf* old_cout = std::cout.rdbuf(oss.rdbuf());

    if (state.showWelcome) {
        if (input == "Y" || input == "y") {
            state.port.gameProcessing("Begin", 0);
            state.showWelcome = false;
            state.inGame = true;
        }
    } 
    else if (state.inGame) {
        // Game processing logic here
    }

    std::cout.rdbuf(old_cout);
    AddToConsole(state, oss.str());
}

int main() {
    if (!glfwInit())
        return -1;

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Airport Manager", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    GameState state;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (state.showWelcome) {
            ImGui::Begin("Welcome to Airport Manager", &state.showWelcome, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            
            ImGui::TextWrapped("Hello! This is a game that simulates the work of an airport manager.");
            ImGui::TextWrapped("During the game you need to process requests coming to the control center.");
            ImGui::TextWrapped("You can allow the takeoff or landing of the aircraft, or you can send the aircraft on a second circle.");
            ImGui::TextWrapped("At each level there is a certain number of requests that you will receive, you need to process them all.");
            ImGui::TextWrapped("To exit to the main menu, you can write 'exit' at any time during the game.");
            
            ImGui::Separator();
            ImGui::Text("Are you ready to go? (Y/n)");
            
            static char answer[2] = "";
            ImGui::InputText("##answer", answer, 2, ImGuiInputTextFlags_CharsNoBlank);
            if (ImGui::Button("Start Game") || (ImGui::IsKeyPressed(ImGuiKey_Enter) && strlen(answer) > 0)) {
                if (answer[0] == 'Y' || answer[0] == 'y') {
                    ProcessGameInput(state, "Y");
                    answer[0] = '\0';
                }
            }
            
            ImGui::End();
        }

        if (state.inGame) {
            ImGui::Begin("Airport Manager Console", &state.inGame, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::TextUnformatted(state.consoleOutput.c_str());
            
            if (state.scrollToBottom) {
                ImGui::SetScrollHereY(1.0f);
                state.scrollToBottom = false;
            }
            ImGui::EndChild();
            
            ImGui::PushItemWidth(-1);
            if (ImGui::InputText("##Input", state.inputBuffer, IM_ARRAYSIZE(state.inputBuffer), 
                                ImGuiInputTextFlags_EnterReturnsTrue)) {
                ProcessGameInput(state, state.inputBuffer);
                memset(state.inputBuffer, 0, sizeof(state.inputBuffer));
                ImGui::SetKeyboardFocusHere();
            }
            ImGui::PopItemWidth();
            
            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}