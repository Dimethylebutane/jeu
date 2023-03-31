#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <limits>

//maximum number of player
#define MAX_GAMEPAD	8

/*
#define NO_PLAYER	8 + 1
typedef uint8_t Player;
constexpr Player DefaultPlayer = 0;
bool playerIsValid(Player p)
{
	return p < (MAX_PLAYER - 1);
}*/



typedef uint8_t Gamepad;
#define NO_GAMEPAD std::numeric_limits<Gamepad>::max()

struct AxisInput
{
	uint32_t bot = GLFW_KEY_Q;
	uint32_t top = GLFW_KEY_D;
	uint32_t joyStickAxis = GLFW_GAMEPAD_AXIS_LEFT_X;
};

struct ButtonInput
{
	uint32_t key = GLFW_KEY_SPACE;
	uint32_t gamePadButton = GLFW_GAMEPAD_BUTTON_A;
};

bool isButtonPressed(ButtonInput b, GLFWgamepadstate gamepad = {});

float getAxis(AxisInput axis, GLFWgamepadstate gamepad = {});