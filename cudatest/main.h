#ifndef main_h
#define main_h

#include <gl/glfw3.h>
#include "particle.h"

struct PARTICLE{
	float *px;
	float *py;
	float *pz;
	float *vx;
	float *vy;
	float *vz;
	float *ax;
	float *ay;
	float *az;
	float *m;
	unsigned char *texid;
};

void init(particle *cParticle);
void setTexID(particle *cParticle, int colorid);
void calcNbodyCpu(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *ax, float *ay, float *az, float *m);
void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void glfwCursorPosCallback(GLFWwindow *, double xpos, double ypos);
void glfwGetJoystick();
void glfwGetJoystickButton();
void setTexture(unsigned char *buffer, size_t width, size_t height, GLuint *texID);
void initTextureColor(unsigned char *buffer, size_t width, size_t height, GLubyte red, GLubyte green, GLubyte blue);
void glInit();
void draw(particle *cParticle);
#endif