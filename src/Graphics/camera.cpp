#include "camera.h"

// Konstruktori
Camera::Camera(const glm::vec3& cameraPosition, const glm::vec3& cameraTarget, const glm::vec3& upVector)
{

    defaultPosition_ = cameraPosition;
    position_ = cameraPosition;
    target_ = cameraTarget;
    up_ = upVector;

    rotateCamera(0,0);
}

// Destruktori
Camera::~Camera()
{
}


/**
 * Palauttaa kameran matriisin
 * @return Matrix
 */
glm::mat4 Camera::getMatrix() const
{
    return view;
}

/**
 * Palauttaa kameran sijainnin
 * @return position
 */
glm::vec3 Camera::getPosition() const
{
    return position_;
}


/**
 * Asettaa kameran katsomaan annettua kohdetta
 * @param cameraTarget kameran kohdevektori
 */
void Camera::setView(const glm::vec3& cameraTarget)
{
    view = glm::lookAt(position_, cameraTarget, up_);
}


/**
 * Palauttaa kameran sijainnin alkuarvoihin
 */
void Camera::resetView()
{
    position_ = defaultPosition_;
    pitch     =  -45.0;
    yaw       =  -90.0;
    rotateCamera(0,0);
}


/**
 * Vaihtaa kameratyyppiä
 */
void Camera::toggleMode()
{
}


/**
 * Kameran sijainnin muutos
 * @param newPos uusi sijainti
 */
void Camera::translate(const glm::vec3& newPos)
{
    position_ = newPos;
}


/***
 * Kameran suunnan muutos hiirikoordinaattien perusteella.
 * @param xoffset x-muutos
 * @param xoffset y-muutos
 */
void Camera::rotateCamera(const float &xoffset, const float &yoffset)
{
    yaw   += xoffset;
    pitch += yoffset;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    front_.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front_.y = sin(glm::radians(pitch));
    front_.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front_ = glm::normalize(front_);
}


/**
 * Muuttaa kameran liikkumisnopeutta parametrin verran.
 * Minimi 1.0, maksimi 10.0
 * @param adjust muutos
 */
void Camera::adjustSpeed(const float &adjust)
{
    camSpeed += adjust;
    camSpeed = glm::clamp(camSpeed, 1.0f, 10.0f);
}


/**
 * Muuttaa kameran kääntymisnopeutta parametrin verran.
 * Minimi 0.1, maksimi 1.0
 * @param adjust muutos
 */
void Camera::adjustSensitivity(const float &adjust)
{
    camSensitivity += adjust;
    camSensitivity = glm::clamp(camSensitivity, 0.1f, 1.0f);
}


/**
 * Kameran päivitys
 * @param time aikakerroin, (currentTick / lastTick), välitetään program-luokasta
 */
void Camera::update(const float time)
{
        // WASD-kamera
        translate(position_);
        setView(position_ + front_);
}

/**
 * Näppäimistötapahtumien käsittely
 */
void Camera::handleKeyInput()
{
  uint32_t newTick = SDL_GetTicks();
  auto del = newTick - pPrevTick;
  if (del != 0)
  {
    deltaTime = float(newTick)/float(pPrevTick);
  }
  pPrevTick = newTick;
    /* Pyydetään SDL:n näppäintila */
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    /* Shift hidastaa liikkumisnopeutta */
    float speedMultiplier = camSpeed;
    if (keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT] )
    {
        speedMultiplier *= 0.3f;
    }

    /* WASD-näppäimet */
    if(keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W])
        position_ += front_ * speedMultiplier * deltaTime;

    if(keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S])
        position_ -= front_ * speedMultiplier * deltaTime;

    if(keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A])
        position_ -= glm::normalize(glm::cross(front_, up_)) * speedMultiplier * deltaTime;

    if(keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D])
        position_ += glm::normalize(glm::cross(front_, up_)) * speedMultiplier * deltaTime;

    /* Ylös ja alas liikkuminen */
    if(keystate[SDL_SCANCODE_V])
        position_ += glm::normalize(up_) * speedMultiplier * deltaTime;

    if(keystate[SDL_SCANCODE_C])
        position_ -= glm::normalize(up_) * speedMultiplier * deltaTime;
    update(deltaTime);

}

/**
 * Hiiritapahtumien käsittely. Program välittää tapahtumat.
 */
void Camera::handleMouseInput(SDL_Event &inputEvent)
{

    switch (inputEvent.type)
    {
    case (SDL_MOUSEBUTTONDOWN): /* Klikkaus. Sijainti talteen sulavampaa liikettä varten */
        lastMouseX = inputEvent.motion.x;
        lastMouseY = inputEvent.motion.y;
    case (SDL_MOUSEMOTION):
        switch (inputEvent.motion.state)
        {
        case (SDL_BUTTON_LMASK): /* Hiiren vasen nappi pohjassa */
            rotateCamera((inputEvent.motion.x - lastMouseX) * camSensitivity * deltaTime,
                         (lastMouseY - inputEvent.motion.y) * camSensitivity * deltaTime);
            /* Hiiren sijainti talteen muutoksen laskemista varten */
            lastMouseX = inputEvent.motion.x;
            lastMouseY = inputEvent.motion.y;
            break;
        }
    }
}
