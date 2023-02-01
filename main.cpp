#include "linux.hpp"
#define GLFW_DLL
#include <iostream>

int main()
{

    platspec_init();

    do
    {
        platspec_glclear();
    } while (!platspec_termprog());

    platspec_term();
    return 0;
}
