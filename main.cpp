#include <GLFW/glfw3.h>
#include <iostream>
#include "source/File.h"

using namespace std;

int main()
{
    if (!glfwInit())
    {
        cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "BMP debug", nullptr, nullptr);
    if (!window)
    {
        cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Setup orthographic projection to match window pixels
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 640, 0, 480, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0, 0, 0, 1);

    BMP test = load_bmp("assets/charsets/player");

    cout << "width: " << test.width << " height: " << test.height << " size: " << test.size << endl;
    cout << "palette size: " << test.palette.size() << " pixels size: " << test.pixels.size() << endl;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, test.width, test.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, test.data());

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(0, 480 - test.height);
        glTexCoord2f(1, 0); glVertex2f(test.width, 480 - test.height);
        glTexCoord2f(1, 1); glVertex2f(test.width, 480);
        glTexCoord2f(0, 1); glVertex2f(0, 480);
        glEnd();

        glDisable(GL_TEXTURE_2D);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
