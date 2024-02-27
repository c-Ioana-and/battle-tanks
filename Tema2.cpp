#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/Colors.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about FrameStart, Update, FrameEnd
 *  and the order in which they are called, see world.cpp.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    camera = new implemented::Camera1();
    camera->Set(glm::vec3(0, 4, -2), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    InitForward = camera->forward;

    {
        Mesh* mesh = new Mesh("tank_body");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "body.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("tank_turret");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "turret.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("tank_tracks");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "tracks.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("tank_cannon");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tank"), "cannon.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Light & material properties
    {
        lightPosition = glm::vec3(0, 5, 1);
        materialShininess = 30;
        materialKd = 3;
        materialKs = 0.5f;

        renderCameraTarget = false;
        projectionMatrix = glm::perspective(RADIANS(100.0f), window->props.aspectRatio, 0.01f, 200.0f);
    }

    // Place the buildings
    int nrBuildings = rand() % 8;

    for (int i = 0; i < nrBuildings; i++) {
        struct Building Building;
        Building.MoveBuilding = glm::mat4(1);
        Building.ofX = rand() % 40 - 20 + 0.01f;
        Building.ofY = rand() % 20 + 20 + 0.01f;
        Building.ofZ = rand() % 40 - 20 + 0.01f;

        if (Building.ofX == 0 && Building.ofZ == 0) {
            Building.ofX = rand() % 40 - 20 + 0.01f;
            Building.ofZ = rand() % 40 - 20 + 0.01f;
        }
        moveBuildings.push_back(Building);
    }

    int nrTanks = 4;

    for (int i = 0; i < nrTanks; i++) {
        struct Tank Tank;
        Tank.hp = 100;
        Tank.ofX = rand() % 40 - 20 + 0.01f;
        Tank.ofZ = rand() % 40 - 20 + 0.01f;
        Tank.rot = 0;
        Tank.rotTurret = Tank.rot;

        if (Tank.ofX == 0 && Tank.ofZ == 0) {
            Tank.ofX = rand() % 40 - 20 + 0.01f;
            Tank.ofZ = rand() % 40 - 20 + 0.01f;
        }

        // if this tanks position is the same as another tank
        moveTanks.push_back(Tank);
    }

}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    // render main tank
    {
        // check if the tank collided with a building
        for (int j = 0; j < moveBuildings.size(); j++) {
            float ofX_B = moveBuildings[j].ofX;
            float ofZ_B = moveBuildings[j].ofZ;

            float P = radTank + radBuilding - glm::distance(glm::vec3(ofX, 0, ofZ), glm::vec3(ofX_B, 0, ofZ_B));
            if (P > 0.01f) {
                glm::vec3 P_vect;
                // calculate the direction in which the tank must be moved
                if (P != radTank + radBuilding)
                    P_vect = abs(P) * glm::normalize(glm::vec3(ofX - ofX_B, 0.01f, ofZ - ofZ_B));
                else {
                    // the diff vector cant be normalized, both the tank and the building's center are in the same point
                    // so we move the tank in a random direction
                    glm::vec3 vect = glm::vec3(1, 0, 0);
                    P_vect = abs(P) * glm::normalize(vect);
                }

                ofX += P_vect.x * 0.5f;
                ofZ += P_vect.z * 0.5f;
            }
        }

        // check if the tank collided with another tank
        for (int j = 0; j < moveTanks.size(); j++) {
            float ofX_T = moveTanks[j].ofX;
            float ofZ_T = moveTanks[j].ofZ;

            float P = radTank * 2 - glm::distance(glm::vec3(ofX, 0, ofZ), glm::vec3(ofX_T, 0, ofZ_T));
            if (P > 0.01f) {
                glm::vec3 P_vect;
                // calculate the direction in which the tank must be moved
                if (P != radTank + radTank)
                    P_vect = abs(P) * glm::normalize(glm::vec3(ofX - ofX_T, 0.01f, ofZ - ofZ_T));
                else
                {
                    // the diff vector cant be normalized, both the tank and the building's center are in the same point
                    // so we move the tank in a random direction
                    glm::vec3 vect = glm::vec3(1, 0, 0);
                    P_vect = abs(P) * glm::normalize(vect);
                }
                ofX += P_vect.x * 0.5f;
                ofZ += P_vect.z * 0.5f;
            }
        }

        MoveTank = glm::mat4(1);
        MoveTank = glm::translate(MoveTank, glm::vec3(ofX, 1, ofZ));
        MoveTank = glm::rotate(MoveTank, rot, glm::vec3(0, 1, 0));

        RotateTurret = glm::rotate(MoveTank, rotTurret, glm::vec3(0, 1, 0));

        RenderSimpleMesh(meshes["tank_body"], shaders["LabShader"], MoveTank, darkerGreen, main_hp);
        RenderSimpleMesh(meshes["tank_tracks"], shaders["LabShader"], MoveTank, lightGrey, main_hp);
        RenderSimpleMesh(meshes["tank_cannon"], shaders["LabShader"], RotateTurret, lightGrey, main_hp);
        RenderSimpleMesh(meshes["tank_turret"], shaders["LabShader"], RotateTurret, darkGreen, main_hp);

        if (main_hp <= 0) {
			cout << "GAME OVER" << endl;
			
            moveTanks.clear();
            moveBuildings.clear();
            MoveShots.clear();

            // this will make the tank blue and sad :(
            main_hp = 200;
		}
    }

    // Render ground
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.1f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(10, 2, 10));
        RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, lightGreen);
    }

    // Place buildings randomly
    {
        for (int i = 0; i < moveBuildings.size(); i++) {
            int ofX = moveBuildings[i].ofX;
            int ofY = moveBuildings[i].ofY;
            int ofZ = moveBuildings[i].ofZ;
            glm::mat4 moveBuilding = glm::translate(moveBuildings[i].MoveBuilding, glm::vec3(ofX, 4, ofZ));
            moveBuilding = glm::scale(moveBuilding, glm::vec3(5, ofY / 2.0f, 5));
            RenderSimpleMesh(meshes["box"], shaders["LabShader"], moveBuilding, darkGreen);
        }
    }

    // Render shots
    {
        float current_time = Engine::GetElapsedTime();
        for (int i = 0; i < MoveShots.size(); i++) {
            // check if it collided with a building
            float ofX_S = MoveShots[i].MoveShot[3][0];
            float ofZ_S = MoveShots[i].MoveShot[3][2];

            bool collision = false;
            // check collision with buildings
            for (int j = 0; j < moveBuildings.size(); j++) {
                if (ofX_S >= moveBuildings[j].ofX - radBuilding && ofX_S <= moveBuildings[j].ofX + radBuilding) {
                    if (ofZ_S >= moveBuildings[j].ofZ - radBuilding && ofZ_S <= moveBuildings[j].ofZ + radBuilding) {
                        collision = true;
                        break;
                    }
                }
            }
            if (MoveShots[i].main == true) {
                // check collision with another tank
                for (int j = 0; j < moveTanks.size(); j++) {
                    if (ofX_S >= moveTanks[j].ofX - radTank && ofX_S <= moveTanks[j].ofX + radTank) {
                        if (ofZ_S >= moveTanks[j].ofZ - radTank && ofZ_S <= moveTanks[j].ofZ + radTank) {
                            collision = true;
                            moveTanks[j].hp -= 30;
                            break;
                        }
                    }
                }
            }
            else {
                // check collision with main tank
                if (ofX_S >= ofX - radTank && ofX_S <= ofX + radTank) {
                    if (ofZ_S >= ofZ - radTank && ofZ_S <= ofZ + radTank) {
                        collision = true;
                        main_hp -= 30;
                    }
                }
            }

            if (MoveShots[i].trans > 50 || current_time - last_time < 1 || collision == true) {
                MoveShots.erase(MoveShots.begin() + i);
            }
            else {
                // the shot must move in the same direction as the turret is facing
                last_time = deltaTimeSeconds;
                MoveShots[i].trans += deltaTimeSeconds * 4;
                MoveShots[i].MoveShot = glm::translate(MoveShots[i].InitMoveTurret, glm::vec3(0, 0, MoveShots[i].trans));
                MoveShots[i].MoveShot = glm::rotate(MoveShots[i].MoveShot, RADIANS(MoveShots[i].rotTurret), glm::vec3(0, 1, 0));
                MoveShots[i].MoveShot = glm::scale(MoveShots[i].MoveShot, glm::vec3(0.3f, 0.3f, 0.3f));

                RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], MoveShots[i].MoveShot, glm::vec3(0, 0, 0));
            }
        }
    }
    {
        // place the enemy tanks
        for (int i = 0; i < moveTanks.size(); i++) {
            if (moveTanks[i].hp <= 0) {
                moveTanks.erase(moveTanks.begin() + i);
                continue;
            }

           
            if (Engine::GetElapsedTime() - moveTanks[i].last_time_state > moveTanks[i].time) {
                // change the state of the tank
                moveTanks[i].currState = GetRandomTankState(moveTanks[i].currState);
                moveTanks[i].last_time_state = Engine::GetElapsedTime();

                if (moveTanks[i].closeToMain == true &&
                    (moveTanks[i].currState == TankState::rotateRight ||
                        moveTanks[i].currState == TankState::rotateLeft)) {

                    moveTanks[i].time = 8;
                }
                else {
                    moveTanks[i].time = 4;
                }
            }
            float speed = 2 * deltaTimeSeconds;
            float speed1 = deltaTimeSeconds;
            
            glm::vec2 dirToMain;
            dirToMain.x = ofX - moveTanks[i].ofX;
            dirToMain.y = ofZ - moveTanks[i].ofZ;
            glm::normalize(dirToMain);

            
            switch (moveTanks[i].currState) {
            case TankState::moveForward:
                // make the tank move forward in the direction it is facing
                moveTanks[i].ofX += speed1 * sin(moveTanks[i].rot);
                moveTanks[i].ofZ += speed1 * cos(moveTanks[i].rot);
                break;
            case TankState::moveBackward:
                // make the tank move backward in the direction it is facing
                moveTanks[i].ofX += speed1 * sin(moveTanks[i].rot);
                moveTanks[i].ofZ += speed1 * cos(moveTanks[i].rot);
                break;
            case TankState::rotateLeft:
                moveTanks[i].rot += speed * 10;

                // rotate turret to face the main tank if needed
                moveTanks[i].rotTurret = atan2(-dirToMain.y, dirToMain.x) + M_PI / 2;
                break;
            case TankState::rotateRight:
                moveTanks[i].rot -= speed * 10;

                // rotate turret to face the main tank if needed
                moveTanks[i].rotTurret = atan2(-dirToMain.y, dirToMain.x) + M_PI / 2;
                break;
            default:
                break;
            }
            float ofX_t1 = moveTanks[i].ofX;
            float ofZ_t1 = moveTanks[i].ofZ;
            float rot = moveTanks[i].rot;

            if (rot > 2 * M_PI)
                rot -= 2 * M_PI;

            // check if the tank collided with a building
            bool collision = false;
            float ofX_B;
            float ofZ_B;
            for (int j = 0; j < moveBuildings.size(); j++) {
                ofX_B = moveBuildings[j].ofX;
                ofZ_B = moveBuildings[j].ofZ;

                float P = radTank + radBuilding - glm::distance(glm::vec3(ofX_t1, 0, ofZ_t1), glm::vec3(ofX_B, 0, ofZ_B));
                if (P > 0.01f) {
                    glm::vec3 P_vect;
                    // calculate the direction in which the tank must be moved
                    if (P != radTank + radBuilding)
                        P_vect = abs(P) * glm::normalize(glm::vec3(ofX_t1 - ofX_B, 0.01f, ofZ_t1 - ofZ_B));
                    else
                    {
                        // the diff vector cant be normalized, both the tank and the building's center are in the same point
                        // so we move the tank in a random direction
                        glm::vec3 vect = glm::vec3(1, 0, 0);
                        P_vect = abs(P) * glm::normalize(vect);
                    }

                    moveTanks[i].ofX += P_vect.x * 0.5f;
                    moveTanks[i].ofZ += P_vect.z * 0.5f;
                }
            }

            // check if the tank collided with another tank
            for (int j = 0; j < i; j++) {
                if (i != j) {
                    float ofX_T = moveTanks[j].ofX;
                    float ofZ_T = moveTanks[j].ofZ;

                    float P = radTank * 2 - glm::distance(glm::vec3(ofX_t1, 0, ofZ_t1), glm::vec3(ofX_T, 0, ofZ_T));
                    if (P > 0.01f) {
                        glm::vec3 P_vect;
                        // calculate the direction in which the tank must be moved
                        if (P != radTank + radTank)
                            P_vect = abs(P) * glm::normalize(glm::vec3(ofX_t1 - ofX_T, 0.01f, ofZ_t1 - ofZ_T));
                        else {
                            // the diff vector cant be normalized, both the tank and the building's center
                            // are in the same point. so we move one of the tanks in a random direction
                            glm::vec3 vect = glm::vec3(1, 0, 0);
                            P_vect = abs(P) * glm::normalize(vect);
                        }
                        moveTanks[j].ofX += P_vect.x * 0.7f;
                        moveTanks[j].ofZ += P_vect.z * 0.7f;
                        moveTanks[i].ofX -= P_vect.x * 0.7f;
                        moveTanks[i].ofZ -= P_vect.z * 0.7f;
                    }
                }
            }

            glm::mat4 moveTank = glm::mat4(1);
            moveTank = glm::translate(moveTank, glm::vec3(moveTanks[i].ofX, 1, moveTanks[i].ofZ));
            moveTank = glm::rotate(moveTank, RADIANS(rot), glm::vec3(0, 1, 0));

            glm::mat4 rotateTurret = glm::mat4(1);
            rotateTurret = glm::translate(rotateTurret, glm::vec3(moveTanks[i].ofX, 1, moveTanks[i].ofZ));
            rotateTurret = glm::rotate(rotateTurret, moveTanks[i].rotTurret, glm::vec3(0, 1, 0));

            // render shots of the tank if it's close enough to the main tank
            if (glm::distance(glm::vec3(moveTanks[i].ofX, 0, moveTanks[i].ofZ), glm::vec3(ofX, 0, ofZ)) < 20) {
                if (moveTanks[i].currState != TankState::moveForward && moveTanks[i].currState != TankState::moveBackward) {
                    moveTanks[i].closeToMain = true;
                    float current_time = Engine::GetElapsedTime();
                    if (current_time - moveTanks[i].last_shot_t > 4) {
                        struct Shot Shot;
                        Shot.InitMoveTurret = rotateTurret;
                        Shot.MoveShot = rotateTurret;
                        Shot.rotTurret = moveTanks[i].rotTurret;
                        Shot.trans = 0;
                        MoveShots.push_back(Shot);
                        moveTanks[i].last_shot_t = current_time;
                    }
                }
            }

            RenderSimpleMesh(meshes["tank_body"], shaders["LabShader"], moveTank, darkBrown, moveTanks[i].hp);
            RenderSimpleMesh(meshes["tank_tracks"], shaders["LabShader"], moveTank, lightGrey, moveTanks[i].hp);
            RenderSimpleMesh(meshes["tank_cannon"], shaders["LabShader"], rotateTurret, lightGrey, moveTanks[i].hp);
            RenderSimpleMesh(meshes["tank_turret"], shaders["LabShader"], rotateTurret, Brown, moveTanks[i].hp);
        }
    }
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, float hp)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // TODO(student): Set light position uniform
    glUniform3fv(glGetUniformLocation(shader->program, "light_position"), 1, glm::value_ptr(lightPosition));

    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    // TODO(student): Set eye position (camera position) uniform
    glUniform3fv(glGetUniformLocation(shader->program, "eye_position"), 1, glm::value_ptr(eyePosition));

    // TODO(student): Set material property uniforms (shininess, kd, ks, object color)
    glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), materialShininess);
    glUniform1f(glGetUniformLocation(shader->program, "material_kd"), materialKd);
    glUniform1f(glGetUniformLocation(shader->program, "material_ks"), materialKs);
    glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));
    glUniform1f(glGetUniformLocation(shader->program, "hp"), hp);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see input_controller.h.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float cameraSpeed = 2 * deltaTime;
    float rotSpeed = 0.5f * deltaTime;

    if (window->KeyHold(GLFW_KEY_W)) {
        // Translate the camera forward
        ofX += cameraSpeed * sin(rot);
        ofZ += cameraSpeed * cos(rot);
        camera->TranslateForward(cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        // Rotate tank to the left
        rot += rotSpeed;
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        // Translate the camera backward
        ofX -= cameraSpeed * sin(rot);
        ofZ -= cameraSpeed * cos(rot);
        camera->TranslateForward(-cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        // Rotate tank to the right
        rot -= rotSpeed;
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    float distX = mouseX - 640;
    float distY = mouseY - 360;


    // Add mouse move event
    float sensivityOX = 0.001f;
    float sensivityOY = 0.001f;

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        camera->RotateThirdPerson_OX(-deltaY * sensivityOX, ofX, ofZ);
        camera->RotateThirdPerson_OY(-deltaX * sensivityOY, ofX, ofZ);
        camera->position = glm::vec3(ofX, 4, ofZ) - camera->forward * camera->distanceToTarget;
    }
    else {
        float angleCursor = atan2(distY, distX);
        float angleCamera = atan2(camera->forward.z, camera->forward.x);
        rotTurret = -angleCursor - angleCamera;
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event

    // if left click
    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
        // create and add a new matrix for each shot

        struct Shot Shot;
        Shot.InitMoveTurret = RotateTurret;
        Shot.rotTurret = rotTurret;
        Shot.MoveShot = RotateTurret;
        Shot.trans = 0;
        Shot.main = true;
        MoveShots.push_back(Shot);
    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Add mouse scroll event
}


void Tema2::OnWindowResize(int width, int height)
{
}