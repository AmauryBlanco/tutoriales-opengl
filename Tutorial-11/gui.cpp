#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

#define px(x) x * ( fWidth / width )

int width = 1280;
int height = 720;
int fWidth = width;
int fHeight = height;

void onResizeWindow(GLFWwindow* window, int width, int height) {
    TwWindowSize(width, height);
}

void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
        TwKeyPressed(TW_KEY_BACKSPACE, TW_KMOD_NONE);
}

void onCharacter(GLFWwindow* window, unsigned int codepoint) {
    TwKeyPressed(codepoint, TW_KMOD_NONE);
}

void onMouseButton(GLFWwindow * window, int button, int action, int mods)
{
    auto a = action == GLFW_PRESS ? TW_MOUSE_PRESSED : TW_MOUSE_RELEASED;
    auto b = TW_MOUSE_LEFT;

    TwMouseButton(a, b);
}

void onMouseMotion(GLFWwindow * window, double xpos, double ypos)
{
    TwMouseMotion(px(static_cast<int>(xpos)), px(static_cast<int>(ypos)));
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "AntTweakBar en GLFW", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(fWidth, fHeight);

    glfwGetFramebufferSize(window, &fWidth, &fHeight);

    glfwSetCursorPosCallback(window, onMouseMotion);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetKeyCallback(window, onKeyPress);
    glfwSetCharCallback(window, onCharacter);
    glfwSetWindowSizeCallback(window, onResizeWindow);

    TwBar* bar = TwNewBar("Tweak Bar");

    double speed = 8.10;
    double acel = 10.25;
    int wire = 1;
    float color[] = { 0.1f, 0.2f, 0.4f };
    unsigned char color2[] = { 255, 0, 0, 128 };
    float g_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float g_LightDirection[] = { -0.57735f, -0.57735f, -0.57735f };

    TwAddVarRW(bar, "velocidad", TW_TYPE_DOUBLE, &speed,
        " label='Velocidad Rotacion' min=0 max=20 step=0.1 keyIncr=s keyDecr=S help='Aumenta o disminuye la velocidad de rotacion' ");

    TwAddVarRO(bar, "acelaracion", TW_TYPE_DOUBLE, &acel, " label='Aceleracion' help='Aumenta o disminuye la velocidad de rotacion' ");

    TwAddVarRW(bar, "wire", TW_TYPE_BOOL32, &wire, " label='Bool variable'  ");

    TwAddVarRW(bar, "color1", TW_TYPE_COLOR3F, &color, " label='Color de Fondo' ");

    TwAddVarRW(bar, "color2", TW_TYPE_COLOR32, &color2, " label='RGBA 32 bits color' alpha ");
    TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &g_LightDirection,
        " label='Light direction' opened=true help='Change the light direction.' ");

    TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation,
        " label='Object rotation' opened=true help='Change the object orientation.' ");

    glViewport(0, 0, fWidth, fHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, fWidth, fHeight, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(color[0], color[1], color[2], 1);
        glClear(GL_COLOR_BUFFER_BIT);

        TwDraw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    TwTerminate();
    glfwTerminate();

    return 0;
}