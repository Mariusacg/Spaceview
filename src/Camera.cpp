#include "Camera.hpp"

using glm::vec3, glm::mat4, glm::mat3, glm::vec4;

// std::ostream &operator<<(std::ostream &stream, const vec3 &other)
// {
//     stream << other.x << ", " << other.y << ", " << other.z;
//     return stream;
// }
// std::ostream &operator<<(std::ostream &stream, const vec4 &other)
// {
//     stream << other.x << ", " << other.y << ", " << other.z << ", " << other.w;
//     return stream;
// }
// bool operator==(const vec3 &other, const vec3 &other2)
// {
//     return other.x == other2.x && other.y == other2.y && other.z == other2.z;
// }

namespace gps
{

    Camera::Camera(vec3 cameraPosition, vec3 cameraTarget, vec3 cameraUp)
        : cameraPosition(cameraPosition), cameraTarget(cameraTarget), cameraUpDirection(glm::normalize(cameraUp)),
          cameraFrontDirection(-glm::normalize(this->cameraPosition - this->cameraTarget)),
          cameraFrontWrotate(this->cameraFrontDirection),
          cameraRightDirection(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)),
          cameraRightWrotate(this->cameraRightDirection)
    {
    }

    // return the view matrix, using the glm::lookAt() function
    mat4 Camera::getViewMatrix()
    {
        return glm::lookAt(this->cameraPosition, this->cameraTarget, this->cameraUpDirection);
    }

    // update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed)
    {
        switch (direction)
        {
        case MOVE_FORWARD:
            this->cameraPosition = this->cameraPosition + speed * this->cameraFrontWrotate;
            this->cameraTarget = this->cameraTarget + speed * this->cameraFrontWrotate;
            break;
        case MOVE_BACKWARD:
            this->cameraPosition = this->cameraPosition - speed * this->cameraFrontWrotate;
            this->cameraTarget = this->cameraTarget - speed * this->cameraFrontWrotate;
            break;
        case MOVE_RIGHT:
            this->cameraPosition = this->cameraPosition + speed * this->cameraRightWrotate;
            this->cameraTarget = this->cameraTarget + speed * this->cameraRightWrotate;
            break;
        case MOVE_LEFT:
            this->cameraPosition = this->cameraPosition - speed * this->cameraRightWrotate;
            this->cameraTarget = this->cameraTarget - speed * this->cameraRightDirection;
            break;
        }
    }

    // update the camera internal parameters following a camera rotate event
    // yaw - camera rotation around the y axis
    // pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw, float roll)
    {
        float p = glm::radians(pitch);
        float y = glm::radians(yaw);
        float r = glm::radians(roll);
        // std::cout << pitch << " " << p << " " << yaw << " " << y << " " << roll << " " << r << std::endl;
        // std::cout << this->cameraPosition.x << this->cameraPosition.y << this->cameraPosition.z << std::endl;
        // float mem[9] = {float(cos(r) * cos(y) - sin(r) * sin(p) * sin(y)),
        //                 float(sin(r) * cos(y) + cos(r) * sin(p) * sin(y)),
        //                 float(-cos(p) * sin(y)),
        //                 float(-sin(r) * cos(p)),
        //                 float(cos(r) * cos(p)),
        //                 float(sin(p)),
        //                 float(cos(r) * sin(y) + sin(r) * sin(p) * cos(y)),
        //                 float(sin(r) * sin(y) - cos(r) * sin(p) * cos(y)),
        //                 float(cos(p) * cos(y))};
        mat3 euler = mat3(glm::yawPitchRoll(y, p, r));
        this->cameraFrontDirection = glm::normalize((euler * this->cameraFrontDirection));
        this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
        this->cameraUpDirection = glm::cross(this->cameraRightDirection, this->cameraFrontDirection);
        this->cameraTarget = this->cameraPosition + this->cameraFrontDirection;
    }

    mat4 Camera::getViewMatrixWrotate(float pitch, float yaw, float roll)
    {
        float p = glm::radians(pitch);
        float y = glm::radians(yaw);
        float r = glm::radians(roll);

        mat3 euler = mat3(glm::yawPitchRoll(y, p, r));

        vec3 tempFrontDirection = glm::normalize((euler * this->cameraFrontDirection));
        vec3 tempRightDirection = glm::normalize(glm::cross(tempFrontDirection, this->cameraUpDirection));
        vec3 tempUpDirection = glm::cross(tempRightDirection, tempFrontDirection);
        vec3 tempTarget = this->cameraPosition + tempFrontDirection;

        this->cameraFrontWrotate = tempFrontDirection;
        this->cameraRightWrotate = tempRightDirection;

        return glm::lookAt(this->cameraPosition, tempTarget, tempUpDirection);
    }
    vec3 Camera::getPosition() const { return this->cameraPosition; }
    void Camera::setPosition(vec3 position) { this->cameraPosition = position; }
}