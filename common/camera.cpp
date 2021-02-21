#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include <iostream>
#include <glfw3.h>

//#define LOG(x) std::cout << x << std::endl
//#define CLUNKY

using namespace glm;
//void joystick_callback(int jid, int event);

Camera::Camera(GLFWwindow* window) : window(window)
{
    position = glm::vec3(0, 0, 5);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    FoV = 45.0f;
    speed = 6.0f;
    mouseSpeed = 0.001f;
    fovSpeed = 2.0f;
}


void Camera::onMouseMove(double xPos, double yPos) {
    static double lastxPos = xPos;
    static double lastyPos = yPos;  
    if (active) {
        horizontalAngle += mouseSpeed * (lastxPos - xPos);
        verticalAngle += mouseSpeed * (lastyPos - yPos);
    }
    lastxPos = xPos;
    lastyPos = yPos;
}

void Camera::update()
{

    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);


    // Task 5.4: right and up vectors of the camera coordinate system
    // use spherical coordinates
	vec3 direction(
		cos(verticalAngle)*sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle)*cos(horizontalAngle)
    );

    // Right vector
	vec3 right(
        sin(horizontalAngle - 3.14f/2.0f),
        0.0f,
        cos(horizontalAngle - 3.14f / 2.0f)
    );

    // Up vector
    vec3 up = cross(right, direction);

    // Task 5.5: update camera position using the direction/right vectors
    // Move forward
#ifndef CLUNKY
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
		//position += vec3(direction.x, 0, direction.z) * deltaTime * speed; //No Up and Downs
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        position += up * deltaTime * speed;
        //std::cout << up.x << up.y << up.z << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        position -= up * deltaTime * speed;
    }

    // Task 5.6: handle zoom in/out effects
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		FoV -= fovSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		FoV += fovSpeed;
	}
#else
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += vec3(direction.x, 0, direction.z) * deltaTime * speed;
        //position += vec3(direction.x, 0, direction.z) * deltaTime * speed; //No Up and Downs
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= vec3(direction.x, 0, direction.z) * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += vec3(right.x, 0, right.z) * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= vec3(right.x, 0, right.z) * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        position += up * deltaTime * speed;
        //std::cout << up.x << up.y << up.z << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        position -= up * deltaTime * speed;
    }

    // Task 5.6: handle zoom in/out effects
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        FoV -= fovSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        FoV += fovSpeed;
    }
#endif
    //std::cout << glfwJoystickPresent(GLFW_JOYSTICK_1) << std::endl;
    if (glfwJoystickPresent(GLFW_JOYSTICK_1))
    {

        int axesCount;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);

        int buttonCount;
        const unsigned char* button = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        /*
        std::cout << "Left Stick X:" << axes[0] << std::endl;
        std::cout << "Left Stick Y:" << axes[1] << std::endl;
        std::cout << "Right Stick X:" << axes[2] << std::endl;
        std::cout << "RIght Stick Y:" << axes[5] << std::endl;
        std::cout << "L2:" << axes[3] << std::endl;
        std::cout << "R2:" << axes[4] << std::endl << std::endl << std::endl;
        */

        if (axes[1] > 0.2 /*BACKWARDS*/ || axes[1] < -0.2 /*FORWARD*/ ) {
            position += vec3(direction.x, 0, direction.z) * deltaTime * (speed + 4) * (-axes[1]);
            //position += vec3(direction.x, 0, direction.z) * deltaTime * speed; //No Up and Downs
        }
        // Strafe right
        if (axes[0] > 0.2 /*RIGHT*/ || axes[0] < -0.2 /*LEFT*/) {
            position += right * deltaTime * (speed + 4) * axes[0];
        }

        if (button[14] == GLFW_PRESS) {
            position += up * deltaTime * speed;
            //std::cout << up.x << up.y << up.z << std::endl;
        }
        if (button[16] == GLFW_PRESS) {
            position -= up * deltaTime * speed;
        }

        if (axes[2] > 0.2 /*LOOK RIGHT*/|| axes[2] < -0.2 /*LOOK LEFT*/)
        {
            horizontalAngle -= mouseSpeed * 20 * axes[2];
        }
        if (axes[5] > 0.2 /*LOOK RIGHT*/|| axes[5] < -0.2 /*LOOK LEFT*/)
        {
            verticalAngle -= mouseSpeed * 20 * axes[5];
        }
    }


    // Task 5.7: construct projection and view matrices
	projectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 200.0f);
    viewMatrix = lookAt(
		position,
        position + direction,
		up
    );
    //*/

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
    //glfwSetJoystickCallback(joystick_callback);
}

/*
void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        std::cout << "Connected" << std::endl;
    }
    else if (event == GLFW_DISCONNECTED)
    {
        std::cout << "Disconnected" << std::endl;
    }
}
//*/