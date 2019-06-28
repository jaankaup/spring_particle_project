#ifndef CAMERA2_H
#define CAMERA2_H

#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "programstate.h"
#include "textureManager.h"
#include "modelManager.h"
#include "../Utils/log.h"
#include "../Utils/misc.h"

//#include "global.h"

class Camera
{
    public:
        Camera(const glm::vec3& cameraPosition, const glm::vec3& targetPosition, const glm::vec3& upVector);
        ~Camera();

        glm::mat4 getMatrix() const;
        glm::vec3 getPosition() const;
        void translate(const glm::vec3& newPos);
        void rotateCamera(const float &xoffset, const float &yoffset);
        void update(const float time);
        void handleKeyInput();
        void handleMouseInput(SDL_Event &mouseEvent);
        void setView(const glm::vec3& cameraTarget);
        void resetView();
        void toggleMode();
        void adjustSpeed(const float &adjust);
        void adjustSensitivity(const float &adjust);

    private:
        glm::mat4 view;

        glm::vec3 defaultPosition_; // Kameran aloitussijainti
        glm::vec3 position_;        // Kameran sijainti
        glm::vec3 target_;          // Kameran peruskohde
        glm::vec3 front_ = glm::vec3(0.0f, 0.0f,  -1.0f);
        glm::vec3 up_    = glm::vec3(0.0f, 1.0f,  0.0f);

        float deltaTime = 0.0f; // Aikakerroin ohjelmassa. Kompensoi päivitysvälin muutoksia.
        float movementInterp = 0.0f; // Kuvastaa ajan muutosta Tick-arvojen perusteella
                                     // käytetään floating, orbit -kameroissa

        // Katselukulma
        float pitch = -45.0f;
        float yaw   = -90.0f;

        // Kursorin sijainti ruudulla
        float lastMouseX = 0.0f;
        float lastMouseY = 0.0f;

        // Kameran liikkumisnopeuden kertoimet
        float camSpeed = 1.0;       // 1.0 - 10.0
        float camSensitivity = 0.1; // 0.1 - 1.0

        uint32_t pPrevTick;
//        enum class CameraMode { Free, Orbit, Floating };
//        CameraMode camMode = CameraMode::Free;
};

#endif // CAMERA2_H
