#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <algorithm>

// ImGui includes
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

// VPP (Взлётно-посадочная полоса) класс
class VPP {
private:
    int lenght;
    bool status;
    unsigned int busytime = 0;
public:
    VPP(int lenght_) : lenght(lenght_), status(false) {}

    void set_lenght(int lenght_) { lenght = lenght_; }
    void set_status(int status_) { status = status_; }
    int getBusyTime() const { return busytime; }
    void setBusyTime(int time) { busytime = time; }
    int get_lenght() const { return lenght; }
    bool get_status() const { return status; }
};

// Базовый класс самолета
class Airplane {
public:
    virtual ~Airplane() = default;
    virtual std::string getType() const = 0;
    virtual int getMaxCircle() const = 0;
    virtual int getVppLength() const = 0;
    virtual int getRequiredTime() const = 0;

    int getTime() const { return timeOnTheCircle; }
    int getCircle() const { return circle; }
    void increaseCircle() { circle += 1; }
    void setTime(int time) { timeOnTheCircle = time; }
protected:
    int circle = 0;
    int timeOnTheCircle = 0;
};

// Конкретные типы самолетов
class CargoPlane : public Airplane {
public:
    std::string getType() const override { return "CargoPlane"; }
    int getMaxCircle() const override { return MAX_CIRCLES; }
    int getVppLength() const override { return NEED_VPP_LENGTH; }
    int getRequiredTime() const override { return REQUIRED_TIME; }
    static const int MAX_CIRCLES = 2;
    static const int NEED_VPP_LENGTH = 4000;
    static const int REQUIRED_TIME = 6;
};

class PassengerPlane : public Airplane {
public:
    std::string getType() const override { return "PassengerPlane"; }
    int getMaxCircle() const override { return MAX_CIRCLES; }
    int getVppLength() const override { return NEED_VPP_LENGTH; }
    int getRequiredTime() const override { return REQUIRED_TIME; }
    static const int MAX_CIRCLES = 2;
    static const int NEED_VPP_LENGTH = 2000;
    static const int REQUIRED_TIME = 4;
};

class AgriculturePlane : public Airplane {
public:
    std::string getType() const override { return "AgriculturePlane"; }
    int getMaxCircle() const override { return MAX_CIRCLES; }
    int getVppLength() const override { return NEED_VPP_LENGTH; }
    int getRequiredTime() const override { return REQUIRED_TIME; }
    static const int MAX_CIRCLES = 2;
    static const int NEED_VPP_LENGTH = 500;
    static const int REQUIRED_TIME = 2;
};

class MilitaryPlane : public Airplane {
public:
    std::string getType() const override { return "MilitaryPlane"; }
    int getMaxCircle() const override { return MAX_CIRCLES; }
    int getVppLength() const override { return NEED_VPP_LENGTH; }
    int getRequiredTime() const override { return REQUIRED_TIME; }
    static const int MAX_CIRCLES = 2;
    static const int NEED_VPP_LENGTH = 1000;
    static const int REQUIRED_TIME = 3;
};

class BusinessPlane : public Airplane {
public:
    std::string getType() const override { return "BusinessPlane"; }
    int getMaxCircle() const override { return MAX_CIRCLES; }
    int getVppLength() const override { return NEED_VPP_LENGTH; }
    int getRequiredTime() const override { return REQUIRED_TIME; }
    static const int MAX_CIRCLES = 2;
    static const int NEED_VPP_LENGTH = 800;
    static const int REQUIRED_TIME = 3;
};

class RescuePlane : public Airplane {
public:
    std::string getType() const override { return "RescuePlane"; }
    int getMaxCircle() const override { return MAX_CIRCLES; }
    int getVppLength() const override { return NEED_VPP_LENGTH; }
    int getRequiredTime() const override { return REQUIRED_TIME; }
    static const int MAX_CIRCLES = 2;
    static const int NEED_VPP_LENGTH = 1500;
    static const int REQUIRED_TIME = 5;
};

// Компаратор для очереди
struct CompareByTimeOnTheCircle {
    bool operator()(Airplane* a, Airplane* b) {
        return a->getTime() > b->getTime();
    }
};

// Прогресс уровня
struct LevelProgress {
    int Level = 1;
    int vpp_count;
    std::vector<VPP*> vpps;
    std::priority_queue<Airplane*, std::vector<Airplane*>, CompareByTimeOnTheCircle> managerLanding;
    std::priority_queue<Airplane*, std::vector<Airplane*>, CompareByTimeOnTheCircle> managerRise;
    int countOfProcessedRequests = 0;
    int countOfCorrectProcessedRequests = 0;
};

// Класс аэропорта
class Airport {
private:
    std::vector<int> countOfReQuestsOnTheLevel = {15, 20, 25, 30, 35};
    const std::vector<int> runwaysPerLevel = {3, 5, 7, 7, 7};
    std::vector<LevelProgress*> MemoryAboutLevelsProgress;

public:
    Airport() {
        const std::vector<int> requiredLengths = {
            CargoPlane::NEED_VPP_LENGTH,
            PassengerPlane::NEED_VPP_LENGTH,
            AgriculturePlane::NEED_VPP_LENGTH,
            MilitaryPlane::NEED_VPP_LENGTH,
            BusinessPlane::NEED_VPP_LENGTH,
            RescuePlane::NEED_VPP_LENGTH
        };

        for (int i = 0; i < countOfReQuestsOnTheLevel.size(); ++i) {
            MemoryAboutLevelsProgress.push_back(new LevelProgress());
            MemoryAboutLevelsProgress[i]->Level = (i+1);
            MemoryAboutLevelsProgress[i]->vpp_count = runwaysPerLevel[i];
        }

        for (int level = 0; level < countOfReQuestsOnTheLevel.size(); ++level) {
            int runwaysNeeded = runwaysPerLevel[level];
            std::vector<int> lengths;
            std::vector<int> remainingLengths = requiredLengths;
            
            std::srand(static_cast<unsigned int>(std::time(nullptr)));

            for (int i = 0; i < runwaysNeeded; ++i) {
                bool canGenerateRunway = false;
                int randomLength;

                do {
                    canGenerateRunway = false;
                    if (!remainingLengths.empty()) {
                        int randomIndex = std::rand() % remainingLengths.size();
                        randomLength = remainingLengths[randomIndex] + (std::rand() % 5) * 100;
                    } else {
                        int minLength = *std::min_element(requiredLengths.begin(), requiredLengths.end());
                        randomLength = minLength + (std::rand() % 10) * 100;
                    }

                    for (int len : requiredLengths) {
                        if (randomLength >= len) {
                            canGenerateRunway = true;
                            break;
                        }
                    }

                    if (canGenerateRunway) {
                        for (auto it = remainingLengths.begin(); it != remainingLengths.end(); ) {
                            if (randomLength >= *it) {
                                it = remainingLengths.erase(it);
                            } else {
                                ++it;
                            }
                        }
                    }
                } while (!canGenerateRunway);

                lengths.push_back(randomLength);
                MemoryAboutLevelsProgress[level]->vpps.push_back(new VPP(randomLength));
            }

            if (level > 0) {
                MemoryAboutLevelsProgress[level]->vpps = MemoryAboutLevelsProgress[level-1]->vpps;
                for (int i = MemoryAboutLevelsProgress[level-1]->vpps.size(); i < runwaysNeeded; ++i) {
                    MemoryAboutLevelsProgress[level]->vpps.push_back(new VPP(lengths[i]));
                }
            }
        }
    }

    Airplane* set_manager(LevelProgress* ourLevel) {
        std::vector<Airplane*> typesOfPlanesToGenerate = {
            new CargoPlane(),
            new PassengerPlane(),
            new AgriculturePlane(),
            new MilitaryPlane(),
            new BusinessPlane(),
            new RescuePlane()
        };
        
        bool can_we_generate_this_type;
        int count_of_generated_types = 6;
        int random_type;
    
        do {
            can_we_generate_this_type = false;
            random_type = rand() % count_of_generated_types;
    
            for(auto vpp : ourLevel->vpps) {
                if (vpp->get_lenght() >= typesOfPlanesToGenerate[random_type]->getVppLength()) {
                    if(vpp->getBusyTime() < typesOfPlanesToGenerate[random_type]->getRequiredTime()*typesOfPlanesToGenerate[random_type]->getMaxCircle()) {
                        can_we_generate_this_type = true;
                        break;
                    }
                }
            }
            if (!can_we_generate_this_type)
                typesOfPlanesToGenerate[random_type] = typesOfPlanesToGenerate[--count_of_generated_types];
    
        } while (!can_we_generate_this_type && count_of_generated_types != 0);
    
        if (can_we_generate_this_type)
            return typesOfPlanesToGenerate[random_type];
        else
            return nullptr;
    }

    int processing(Airplane *tempPlane, LevelProgress* ourLevel) {
        int typeOfRequest = rand() % 2;
        std::cout << "Type of request Plane: " << tempPlane->getType() << "  Request type: " << ((typeOfRequest==0) ? "landing " : "takeoff ") << " Max Circle: "
        << tempPlane->getMaxCircle() << "  Need Length: " << tempPlane->getVppLength() << "  Time for next circle: " << tempPlane->getRequiredTime() << "  Current circle: " << tempPlane->getCircle() << std::endl;
        std::cout << "busyness of vpp: " << std::endl;
        for(int i = 0; i < ourLevel->vpp_count; i++) {
            std::cout << "vpp number " << i+1 << ": " << "time busy: " << ourLevel->vpps[i]->getBusyTime() << ", length: " << ourLevel->vpps[i]->get_lenght() << std::endl;
        }
        int choice = 1000;
        std::string input;
        int tempVpp;
        std::string vppInput;
        while (choice != -1 && choice != 0 && (choice < 1 || choice > ourLevel->vpp_count))
        {
            std::cin >> input; 
            if (input == "exit") {
                return 2; 
            }
            choice = std::stoi(input);

            if(choice > 0 && choice <= ourLevel->vpp_count) {
                tempVpp = choice;
                while(ourLevel->vpps[tempVpp-1]->getBusyTime() != 0 || ourLevel->vpps[tempVpp-1]->get_lenght() < tempPlane->getVppLength()) {
                    if(ourLevel->vpps[tempVpp-1]->getBusyTime() != 0) {
                        std::cout << "This vpp is busy. Please, choose another vpp." << std::endl;
                    }
                    else {
                        std::cout << "Unfortunately, you cannot accept the aircraft on this runway because its length is not sufficient. Please, choose another vpp." << std::endl;
                    }
                    std::cin >> vppInput;
                    if (vppInput == "exit") {
                        return 2;
                    }
                    tempVpp = std::stoi(vppInput);
                }
                ourLevel->vpps[tempVpp-1]->setBusyTime(tempPlane->getRequiredTime());
                std::cout << "Successful request processing" << std::endl;
                return 1;
            }
            else if(choice == -1 && typeOfRequest == 0) {
                if (!(tempPlane->getCircle() == tempPlane->getMaxCircle())) {
                    std::cout << "Sending the plane on a second circle..." << std::endl;
                }
                return -1;
            }
            else if(choice == -1 && typeOfRequest == 1) {
                if (!(tempPlane->getCircle() == tempPlane->getMaxCircle())) {
                    std::cout << "Sending the plane on a second circle..." << std::endl;
                }
                return -2;
            }
            else if(choice == 0) {
                std::cout << "Skipping this request..." << std::endl;
                return 0;
            }
            else {
                std::cout << "Wrong format." << std::endl;
                continue;
            }
        }
        return 0;
    }

    void game(LevelProgress* ourLevel) {
        std::ostringstream oss;
        std::streambuf* old_cout = std::cout.rdbuf(oss.rdbuf());
        srand(time(0));
        oss << "You have " << ourLevel->vpp_count << " vpps on " << ourLevel->Level << " level" << std::endl;
        double percentageOfCurrentLevelPassed = 0.0;
        int requests = ourLevel->countOfProcessedRequests;
        std::vector<Airplane*> tempmanager;
        int result;
        std::vector<Airplane*> PlanesWhoFlewInFromTheCircle;
        std::vector<Airplane*> PlanesWhoWantToFly;
        
        for(int i = 0; i < countOfReQuestsOnTheLevel[ourLevel->Level-1] - requests; i++) {
            for (auto vpp : ourLevel->vpps) {
                if (vpp->getBusyTime() > 0) {
                    vpp->setBusyTime(vpp->getBusyTime() - 1);
                }
            }
            
            // ... (остальная логика игры)
        }
        
        gameProcessing("LevelComplete", ourLevel->Level);
        std::cout.rdbuf(old_cout);
    }

    void gameProcessing(std::string point, int tempLvl) {
        // ... (логика обработки игры)
    }

    std::vector<LevelProgress*> returnMemory() { return MemoryAboutLevelsProgress; }
};

// Game state
struct GameState {
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