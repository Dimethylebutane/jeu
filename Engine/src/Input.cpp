#include "../include/GameStruct/Input.hpp"


#include <assert.h>     /* assert */

GLFWwindow* window;

bool isButtonPressed(ButtonInput b, GLFWgamepadstate gamepad)
{
	bool kb = glfwGetKey(window, b.key) == GLFW_PRESS;
	bool gp = gamepad.buttons[b.gamePadButton] == GLFW_PRESS;

	return kb || gp;
}

float getAxis(AxisInput axis, GLFWgamepadstate gamepad)
{
	float kbv = (glfwGetKey(window, axis.top) == GLFW_PRESS) - (glfwGetKey(window, axis.bot) == GLFW_PRESS);
	float gpv = gamepad.axes[axis.joyStickAxis];

	bool gp = (gpv != 0.f);

	//gamePad has priority over keyboard -> more control with axis float and not ternary

	return gp ? gpv : kbv;
}