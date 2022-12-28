#pragma once
//joystick connection/deconnection
void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        // The joystick was connected
    }
    else if (event == GLFW_DISCONNECTED)
    {
        // The joystick was disconnected
    }
}

//textCallback
void character_callback(GLFWwindow* window, unsigned int codepoint)
{

}

//keyPressCallback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //if (key == GLFW_KEY_E && action == GLFW_PRESS)
    //    activate_airship();
}