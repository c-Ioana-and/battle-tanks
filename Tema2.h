#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"
#include "lab_m1/Tema2/camera.h"
#include "lab_m1/Tema2/TankState.h"

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;
        struct Tank {
            float hp;
            float ofX, ofZ;
            float rot, rotTurret;
            float last_shot_t = 0;
            float target_rot = 0;
            bool closeToMain = false;
            TankState currState = TankState::moveForward;
            float last_time_state = 0;
            float time = 4;
        };
        struct Shot {
            glm::mat4 InitMoveTurret;
            glm::mat4 MoveShot;
            float rotTurret;
            float trans;
            bool main;
        };
        struct Building {
            int ofX;
            int ofY;
            int ofZ;
            glm::mat4 MoveBuilding;
        };

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1), float hp = -1);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        implemented::Camera1* camera;
        glm::vec3 InitForward;
        bool renderCameraTarget;
        float lastMouseX = 0;
        glm::mat4 projectionMatrix;

        glm::vec3 lightPosition;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;

        glm::mat4 MoveTank = glm::mat4(1);
        glm::mat4 RotateTurret = glm::mat4(1);
        float radTank = 1.5f;
        float main_hp = 100;

        std::vector<struct Shot> MoveShots;
        float ofX = 0;
        float ofZ = 0;
        float rot = 0;
        float rotTurret = 0;

        float last_time = 0;
        float last_time_state = 0;

        std::vector<struct Tank> moveTanks;

        std::vector<struct Building> moveBuildings;
        float radBuilding = 2.5f;

        TankState currState = TankState::moveForward;
    };
} // namespace m1