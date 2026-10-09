#define GLFW_INCLUDE_GLU
#define GLEW_STATIC
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <process.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <deque>
#include <numeric>
#include <thread>
#include <libping/png.h>
#include <gl/glew.h>
#include <gl/glfw3.h>
#include <gl/glut.h>
#include <gl/AntTweakBar.h>
#include "resource.h"
#include "main.h"
#include "calcParticleParam.h"
#include "calcNbodyCuda.cuh"
#include "calcEnergyCuda.cuh"
#include "particle.h"
#include "node.h"
#include "tree.h"

#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glut32.lib")
#pragma comment(lib, "AntTweakBar.lib")
#pragma comment(lib, "libpng16.lib")
#pragma comment(lib, "zlib.lib")

using namespace std;

GLFWwindow *glfwWindow;
GLuint texID[12];
double panx = 0.0, pany = 0.0, panz = 0.0, zoom = 20.0;
double theta = M_PI_2, phi = 0.0;
bool leftButtonPressed = FALSE;
bool rightButtonPressed = FALSE;
bool centerButtonPressed = FALSE;
bool bPause = FALSE;
bool bCameraTracking = FALSE;
bool bCalcEnergy = FALSE;
bool bShowCrossHair = FALSE;
bool bCameraReset = TRUE;
bool bFullScreen = FALSE;
bool bRecord = FALSE;
bool bSave = FALSE;
bool bReplay = FALSE;
bool bBrightness = TRUE;
int showGrid = FALSE;
int particleColor = 0;
volatile bool wait = TRUE;
double preXpos, preYpos;
double rate = 0.008;
int mode = 0;
int size = 1;
particle *lpParticle;
node *rootNode;
static unsigned long long int frameCount = 0;
string outputStr[15];
deque<double> queue(1);
extern deque<double> queue_gpu;
ifstream ifs;
ofstream ofs;
unsigned int rec_frame = 0;
unsigned int file_num = 0;
string filename;
deque<string> command;

int x = 100, y = 100;

void init(particle *cParticle){
	cParticle->deleteAllParticle();

	if (mode == 0){
		if (bCameraReset)panx = 0.0, pany = 0.0, panz = 0.0, zoom = 140.0, theta = M_PI_2, phi = M_PI_2;
		float offset_px = 0.0f;
		float offset_py = 0.0f;
		float offset_pz = 0.0f;
		float offset_vx = 0.0f;
		float offset_vy = 0.0f;
		float offset_vz = 0.0f;

		cParticle->makeMiyamotoNagai(20480, 8.0, 0.5f, M * 20.0f, -0.0f, 0.0f, 0.0f, 0.0, 0.0f, -0.0f);
	}
	

	if (mode == 1){
		if (bCameraReset)panx = 0.0, pany = 0.0, panz = 0.0, zoom = 145.0, theta = M_PI_2, phi = M_PI_2;
		float offset_px = 0.0f;
		float offset_py = 0.0f;
		float offset_pz = 0.0f;
		float offset_vx = 0.0f;
		float offset_vy = 0.0f;
		float offset_vz = 0.0f;

		cParticle->makeMiyamotoNagai(20480, 7.0, 1.4f, M * 5.0f, -80.0f, 0.0f, 8.0f, 0.0f, 0.0f, -0.0f);
		for (unsigned int i = 0; i < cParticle->size; i++){
			cParticle->px[i] *= cos(-M_PI_4);
			cParticle->py[i] = cParticle->px[i] * sin(-M_PI_4);

			cParticle->vx[i] *= cos(-M_PI_4);
			cParticle->vy[i] = cParticle->vx[i] * sin(-M_PI_4);
		}

		cParticle->makeMiyamotoNagai(20480, 7.0f, 1.4f, M * 5.0f, -0, -0.0f, -4.0f, -2.0f, 0.0f, -0.0f);
		for (unsigned int i = 20480; i < 20480 * 2; i++){
			cParticle->pz[i] *= cos(M_PI_4);
			cParticle->py[i] = cParticle->pz[i] * sin(M_PI_4);

			cParticle->vz[i] *= cos(M_PI_4);
			cParticle->vy[i] = cParticle->vz[i] * sin(M_PI_4);

			cParticle->py[i] += 10.0f;
		}

		cParticle->makeMiyamotoNagai(20480, 7.0f, 1.4f, M * 5.0f, 80.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.0f);
		for (unsigned int i = 20480 * 2; i < 20480 * 3; i++){
			cParticle->px[i] *= cos(M_PI_4);
			cParticle->py[i] = cParticle->px[i] * sin(M_PI_4);

			cParticle->vx[i] *= cos(M_PI_4);
			cParticle->vy[i] = cParticle->vx[i] * sin(M_PI_4);
		}
		//cParticle->makeMiyamotoNagai(22528, 6.0f, 0.5f, M * 3.0f, 20.0f, 0.5f, -2.0f, -40.0f, 0.0f, 2.0f);
	}

	if (mode == 2){
		if (bCameraReset)panx = 20.0, pany = 0.0, panz = 0.0, zoom = 160.0f, theta = M_PI_2, phi = 0.0;
		float offset_px = 50.0f;
		float offset_py = 8.0f;
		float offset_pz = 0.0f;
		float offset_vx = -200.0f;
		float offset_vy = -16.0f;
		float offset_vz = 0.0f;

		cParticle->makeUniformSphere(20480, 2.0f, M, 0.5f, offset_px, offset_py, offset_pz, offset_vx, offset_vy, offset_vz);
		cParticle->px[0] = 0;
		cParticle->py[0] = 0;
		cParticle->pz[0] = 0;
		cParticle->vx[0] = 0;
		cParticle->vy[0] = 0;
		cParticle->vz[0] = 0;
		cParticle->m[0] = M * cParticle->size * 10;
	}

	if (mode == 3){
		if (bCameraReset)panx = 0.0, pany = 0.0, panz = 0.0, zoom = 45.0, theta = M_PI_2, phi = 0.0;
		float offset_px = 0.0f;
		float offset_py = 0.0f;
		float offset_pz = 0.0f;
		float offset_vx = 0.0f;
		float offset_vy = 0.0f;
		float offset_vz = 0.0f;

		cParticle->makeUniformSphere(1024 * 14, 3.0f, M * 20.0f, 0.5f, offset_px, offset_py, offset_pz, offset_vx, offset_vy, offset_vz);

		cParticle->makeUniformSphere(1024 * 14, 3.0f, M * 20.0f, 0.5f, 140.0f, offset_py, offset_pz, -335.0f, offset_vy, offset_vz);
	}

	if (mode == 4){
		if (bCameraReset)panx = 0.0, pany = 0.0, panz = 0.0, zoom = 45.0, theta = M_PI_2, phi = 0.0;
		for (int i = 0; i < 32; i++){
			float offset_px = sin(M_PI / 16.0 * (double)i) * 2.0;
			float offset_py = cos(M_PI / 16.0 * (double)i) * 2.0;
			float offset_pz = 0.0f;
			float offset_vx = cos(M_PI / 16.0 * (double)i) * 240.0;
			float offset_vy = -sin(M_PI / 16.0 * (double)i) * 240.0;
			float offset_vz = 0.0f;

			cParticle->makeUniformSphere(10240 / 32, 1.0f, M * 20.0f, 0.5f, offset_px, offset_py, offset_pz, offset_vx, offset_vy, offset_vz);
		}
	}


	if (mode == 5){
		if (bCameraReset)panx = 0.0, pany = 4.0, panz = 0.0, zoom = 58.0, theta = M_PI_2, phi = 0.0;
		float radius = 2.0f;
		float offset_px = 10.0f;
		float offset_py = 15.0f;
		float offset_pz = 0.0f;
		float offset_vx = -34.0f;
		float offset_vy = 10.5f;
		float offset_vz = 0.0f;

		cParticle->makeUniformSphere(5120, radius, M * 30.0f, 0.5f, offset_px, offset_py, offset_pz, offset_vx, offset_vy, offset_vz);

		offset_px = -10.0f;
		offset_py = -10.0f;
		offset_pz = 0.0f;
		offset_vx = 20.0f;
		offset_vy = 0.0f;
		offset_vz = 0.0f;

		cParticle->makeUniformSphere(5120, radius, M * 30.0f, 0.5f, offset_px, offset_py, offset_pz, offset_vx, offset_vy, offset_vz);
	}
	
	if (mode == 6){
		cParticle->makeUniformSphere(20480, 5.0f, M * 5.0f, 0.5f, -3, 0, 0, 0, 0, 0);

		cParticle->makeUniformSphere(20480, 5.0f, M * 5.0f, 0.5f, 3, 0, 0, 0, 0, 0);

		for (unsigned int i = 0; i < cParticle->size; i++)cParticle->py[i] *= 0.5;
		cParticle->px[0] = 15;
		cParticle->py[0] = 0;
		cParticle->pz[0] = 0;
		cParticle->vx[0] = -80;
		cParticle->vy[0] = 0;
		cParticle->vz[0] = 0;
		cParticle->m[0] *= 40000;

	}

	if (mode == 7){
		if (bCameraReset)panx = 0.0, pany = 0.0, panz = 0.0, zoom = 4.0, theta = M_PI_2, phi = 0.0;
		float offset_px = 0.0f;
		float offset_py = 0.0f;
		float offset_pz = 0.0f;
		float offset_vx = 0.0f;
		float offset_vy = 0.0f;
		float offset_vz = 0.0f;

		cParticle->makeUniformSphere(8, 4.0f, M * 1.0f, 0.5f, offset_px, offset_py, offset_pz, offset_vx, offset_vy, offset_vz);

		cParticle->px[0] = 0;
		cParticle->py[0] = 0;
		cParticle->pz[0] = 0;
		cParticle->vx[0] = 0;
		cParticle->vy[0] = 0;
		cParticle->vz[0] = 0;
		cParticle->m[0] *= 40000;


		for (int i = 1; i < 8; i++){
			cParticle->px[i] = 2.2f * (float)i;
			cParticle->py[i] = 0;
			cParticle->pz[i] = 0;
			float dx = cParticle->px[0] - cParticle->px[i];
			float dy = cParticle->py[0] - cParticle->py[i];
			float dz = cParticle->pz[0] - cParticle->pz[i];
			float r = sqrt(dx * dx + dy * dy + dz * dz);
			float v = sqrt(1.0f * G * cParticle->m[0] / r);
			cParticle->vx[i] = -cParticle->py[i] / r * v;
			cParticle->vy[i] = cParticle->px[i] / r * v;
			cParticle->vz[i] = 0;
		}
		
	}

	setTexID(cParticle, particleColor);
	
}

void setTexID(particle *cParticle, int colorid){
	random_device rd;
	mt19937_64 mt(rd());
	poisson_distribution<int>rand(0.5);
	int offset = colorid * 3;
	for (unsigned int i = 0; i < cParticle->size; i++){
		do{
			cParticle->texid[i] = rand(mt) + offset;
		} while (cParticle->texid[i] >= 3 + offset);
	}
}



void calcNbody(float *px, float *py, float *pz, float *vx, float *vy, float *vz, float *ax, float *ay, float *az, float *m, unsigned int num, unsigned int num2, volatile unsigned int offset){

	for (unsigned int i = offset; i < offset + num2; i++){
		ax[i] = 0;
		ay[i] = 0;
		az[i] = 0;
	}

	for (unsigned int i = offset; i < offset + num2; i++){
		for (unsigned int j = 0; j < num; j++){
			if (i == j)continue;

			float dx = px[i] - px[j];
			float dy = py[i] - py[j];
			float dz = pz[i] - pz[j];
			float r = dx * dx + dy * dy + dz * dz + EPS * EPS;
			float a = (G * m[j]) / (sqrt(r) * r);

			ax[i] -= a * dx;
			ay[i] -= a * dy;
			az[i] -= a * dz;
		}
	}



	for (unsigned int i = offset; i < offset + num2; i++){
		float kx1, kx2, kx3, kx4, ky1, ky2, ky3, ky4, kz1, kz2, kz3, kz4;

		vx[i] += ax[i] * dt;
		vy[i] += ay[i] * dt;
		vz[i] += az[i] * dt;

		kx1 = vx[i];
		kx2 = (vx[i] + kx1 / 2.0f  * dt);
		kx3 = (vx[i] + kx2 / 2.0f * dt);
		kx4 = (vx[i] + kx3 * dt);

		px[i] += (kx1 + 2.0f * kx2 + 2.0f * kx3 + kx4) / 6.0f * dt;

		ky1 = vy[i];
		ky2 = (vy[i] + ky1 / 2.0f * dt);
		ky3 = (vy[i] + ky2 / 2.0f * dt);
		ky4 = (vy[i] + ky3 * dt);

		py[i] += (ky1 + 2.0f * ky2 + 2.0f * ky3 + ky4) / 6.0f * dt;

		kz1 = vz[i];
		kz2 = (vz[i] + kz1 / 2.0f * dt);
		kz3 = (vz[i] + kz2 / 2.0f * dt);
		kz4 = (vz[i] + kz3 * dt);

		pz[i] += (kz1 + 2.0f * kz2 + 2.0f * kz3 + kz4) / 6.0f * dt;
	}
}
void calcAcceralation(particle *cParticle){
	unsigned int size = cParticle->size / 8;
	thread t1(calcNbody, cParticle->px, cParticle->py, cParticle->pz, cParticle->vx, cParticle->vy, cParticle->vz, cParticle->ax, cParticle->ay, cParticle->az, cParticle->m, cParticle->size, size, size * 0);
	thread t2(calcNbody, cParticle->px, cParticle->py, cParticle->pz, cParticle->vx, cParticle->vy, cParticle->vz, cParticle->ax, cParticle->ay, cParticle->az, cParticle->m, cParticle->size, size, size * 1);
	thread t3(calcNbody, cParticle->px, cParticle->py, cParticle->pz, cParticle->vx, cParticle->vy, cParticle->vz, cParticle->ax, cParticle->ay, cParticle->az, cParticle->m, cParticle->size, size, size * 2);
	thread t4(calcNbody, cParticle->px, cParticle->py, cParticle->pz, cParticle->vx, cParticle->vy, cParticle->vz, cParticle->ax, cParticle->ay, cParticle->az, cParticle->m, cParticle->size, size, size * 3);
	thread t5(calcNbody, cParticle->px, cParticle->py, cParticle->pz, cParticle->vx, cParticle->vy, cParticle->vz, cParticle->ax, cParticle->ay, cParticle->az, cParticle->m, cParticle->size, size, size * 4);
	thread t6(calcNbody, cParticle->px, cParticle->py, cParticle->pz, cParticle->vx, cParticle->vy, cParticle->vz, cParticle->ax, cParticle->ay, cParticle->az, cParticle->m, cParticle->size, size, size * 5);
	thread t7(calcNbody, cParticle->px, cParticle->py, cParticle->pz, cParticle->vx, cParticle->vy, cParticle->vz, cParticle->ax, cParticle->ay, cParticle->az, cParticle->m, cParticle->size, size, size * 6);
	thread t8(calcNbody, cParticle->px, cParticle->py, cParticle->pz, cParticle->vx, cParticle->vy, cParticle->vz, cParticle->ax, cParticle->ay, cParticle->az, cParticle->m, cParticle->size, size, size * 7);

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();
}

void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)bPause = !bPause;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
		if (mode == 7)mode = 0;
		else mode++;
		init(lpParticle);
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
		if (mode == 0)mode = 7;
		else mode--;
		init(lpParticle);
	}
	if (key == GLFW_KEY_KP_0 && action == GLFW_PRESS)init(lpParticle);

	if (key == GLFW_KEY_C && action == GLFW_PRESS)bCameraTracking = !bCameraTracking;
	if (key == GLFW_KEY_E && action == GLFW_PRESS)bCalcEnergy = !bCalcEnergy;
	if (key == GLFW_KEY_R && action == GLFW_PRESS)bCameraReset = !bCameraReset;
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)bShowCrossHair = !bShowCrossHair;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS){
		if (showGrid == 3)showGrid = 0;
		else showGrid++;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS){
		if (particleColor == 4)particleColor = 0;
		else particleColor++;
		setTexID(lpParticle, particleColor);
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS){
		bFullScreen = !bFullScreen;
		static int width, height, posx, posy;
		if (bFullScreen){
			glfwGetWindowSize(glfwWindow, &width, &height);
			glfwGetWindowPos(glfwWindow, &posx, &posy);

			glfwDestroyWindow(glfwWindow);			
			const GLFWvidmode *glfwVideoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwWindow = glfwCreateWindow(glfwVideoMode->width, glfwVideoMode->height, "N-Body", glfwGetPrimaryMonitor(), NULL);
			glfwMakeContextCurrent(glfwWindow);

			glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
			glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
			glfwSetScrollCallback(glfwWindow, glfwScrollCallback);
			glfwSetCursorPosCallback(glfwWindow, glfwCursorPosCallback);
			glfwGetCursorPos(glfwWindow, &preXpos, &preYpos);
			glInit();
		}
		else{
			glfwDestroyWindow(glfwWindow);
			glfwWindow = glfwCreateWindow(width, height, "N-Body", NULL, NULL);
			glfwSetWindowPos(glfwWindow, posx, posy);
			glfwMakeContextCurrent(glfwWindow);
			
			glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
			glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
			glfwSetScrollCallback(glfwWindow, glfwScrollCallback);
			glfwSetCursorPosCallback(glfwWindow, glfwCursorPosCallback);
			glfwGetCursorPos(glfwWindow, &preXpos, &preYpos);
			glInit();
		}
	}

	static float size = 2.0f;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS){
		size += 0.2f;
		glPointParameterf(GL_POINT_SIZE_MIN, size);		
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS){
		size -= 0.2f;
		glPointParameterf(GL_POINT_SIZE_MIN, size);		
		if (size < 1.0f)size = 1.0f;
	}

	if (key == GLFW_KEY_B && action == GLFW_PRESS){
		if (bBrightness){
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			bBrightness = !bBrightness;
		}
		else{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			bBrightness = !bBrightness;
		}		
	}
}

void glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)leftButtonPressed = GLFW_PRESS;
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)leftButtonPressed = GLFW_RELEASE;
	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)rightButtonPressed = GLFW_PRESS;
	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE)rightButtonPressed = GLFW_RELEASE;
	if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS)centerButtonPressed = GLFW_PRESS;
	if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_RELEASE)centerButtonPressed = GLFW_RELEASE;
}

void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	if (yoffset < 0)zoom *= 1.05;
	if (yoffset > 0)zoom *= 0.95;
}

void glfwCursorPosCallback(GLFWwindow *, double xpos, double ypos){
	//カメラ回転
	if (leftButtonPressed == GLFW_PRESS){
		if (cos(phi) >= 0){
			if (xpos > preXpos)theta += M_PI / 90.0;
			if (xpos < preXpos)theta -= M_PI / 90.0;
		}
		else{
			if (xpos > preXpos)theta -= M_PI / 90.0;
			if (xpos < preXpos)theta += M_PI / 90.0;
		}

		if (ypos > preYpos)phi += M_PI / 90.0;
		if (ypos < preYpos)phi -= M_PI / 90.0;
	}

	//カメラ
	if (rightButtonPressed == GLFW_PRESS){
		//横移動
		if (xpos > preXpos){
			panx += sin(theta) * rate * zoom;
			panz -= cos(theta) * rate * zoom;
		}
		if (xpos < preXpos){
			panx -= sin(theta) * rate * zoom;
			panz += cos(theta) * rate * zoom;
		}

		//縦移動
		if (ypos > preYpos){
			panx += cos(theta) * sin(phi) * rate * zoom;
			pany -= cos(phi) * rate * zoom;
			panz += sin(theta) * sin(phi) * rate * zoom;
		}
		if (ypos < preYpos){
			panx -= cos(theta) * sin(phi) * rate * zoom;
			pany += cos(phi) * rate * zoom;
			panz -= sin(theta) * sin(phi) * rate * zoom;
		}
	}

	//カメラ奥移動
	if (centerButtonPressed == GLFW_PRESS){
		if (ypos > preYpos){
			panx += cos(theta) * cos(phi) * rate * zoom;
			pany += sin(phi) * rate * zoom;
			panz += sin(theta) * cos(phi) * rate * zoom;
		}
		if (ypos < preYpos){
			panx -= cos(theta) * cos(phi) * rate * zoom;
			pany -= sin(phi) * rate * zoom;
			panz -= sin(theta) * cos(phi) * rate * zoom;
		}
	}

	preXpos = xpos;
	preYpos = ypos;
}

void glfwGetJoystick(){
	//ゲームパッドが刺さっていなければreturn。XBOX360の純正コントローラーでなければ落ちるか挙動がおかしいのでその都度修正
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_FALSE)return;

	int joyStickCount, buttonCount;
	const float *joystickAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &joyStickCount);
	const unsigned char *joystickButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);

	//カメラ縦回転
	if (joystickAxes[3] >= 0.2)phi -= M_PI / 45.0 * abs(joystickAxes[3]);
	if (joystickAxes[3] <= -0.2)phi += M_PI / 45.0 * abs(joystickAxes[3]);

	//カメラ横回転
	if (cos(phi) >= 0){
		if (joystickAxes[4] >= 0.2)theta -= M_PI / 45.0 * abs(joystickAxes[4]);
		if (joystickAxes[4] <= -0.2)theta += M_PI / 45.0 * abs(joystickAxes[4]);
	}
	else{
		if (joystickAxes[4] >= 0.2)theta += M_PI / 45.0 * abs(joystickAxes[4]);
		if (joystickAxes[4] <= -0.2)theta -= M_PI / 45.0 * abs(joystickAxes[4]);
	}


	//カメラズーム
	if (joystickAxes[2] >= 0.2)zoom *= (1.00 + 0.02 * abs(joystickAxes[2]));
	if (joystickAxes[2] <= -0.2)zoom *= (1.00 - 0.02 * abs(joystickAxes[2]));


	//カメラ横移動
	if (joystickAxes[0] >= 0.2){
		panx += sin(theta) * rate * zoom * abs(joystickAxes[0]);
		panz -= cos(theta) * rate * zoom * abs(joystickAxes[0]);
	}
	if (joystickAxes[0] <= -0.2){
		panx -= sin(theta) * rate * zoom * abs(joystickAxes[0]);
		panz += cos(theta) * rate * zoom * abs(joystickAxes[0]);
	}


	//カメラ縦移動
	if (joystickAxes[1] >= 0.2){
		panx += cos(theta) * sin(phi) * rate * zoom * abs(joystickAxes[1]);
		pany -= cos(phi) * rate * zoom * abs(joystickAxes[1]);
		panz += sin(theta) * sin(phi) * rate * zoom * abs(joystickAxes[1]);
	}
	if (joystickAxes[1] <= -0.2){
		panx -= cos(theta) * sin(phi) * rate * zoom * abs(joystickAxes[1]);
		pany += cos(phi) * rate * zoom * abs(joystickAxes[1]);
		panz -= sin(theta) * sin(phi) * rate * zoom * abs(joystickAxes[1]);
	}

	//カメラ奥移動
	if (joystickButtons[0] == 1){
		panx += cos(theta) * cos(phi) * rate * zoom;
		pany += sin(phi) * rate * zoom;
		panz += sin(theta) * cos(phi) * rate * zoom;
	}
	if (joystickButtons[1] == 1){
		panx -= cos(theta) * cos(phi) * rate * zoom;
		pany -= sin(phi) * rate * zoom;
		panz -= sin(theta) * cos(phi) * rate * zoom;
	}
}

void glfwGetJoystickButton(){
	//ゲームパッドが刺さっていなければreturn。XBOX360の純正コントローラーでなければ落ちるか挙動がおかしいのでその都度修正
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_FALSE)return;

	int joyStickCount, buttonCount;
	const float *joystickAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &joyStickCount);
	const unsigned char *joystickButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
	static BOOL pressed = FALSE;
	//リセット
	if (joystickButtons[6] == 1 && pressed == FALSE){
		init(lpParticle);
		pressed = TRUE;
		return;
	}
	else if (joystickButtons[6] == 1 && pressed == TRUE)return;
	
	//ポーズ
	if (joystickButtons[7] == 1 && pressed == FALSE){
		bPause = !bPause;
		pressed = TRUE;
		return;
	}
	else if (joystickButtons[7] == 1 && pressed == TRUE)return;

	//モード変更
	if (joystickButtons[5] == 1 && pressed == FALSE){
		if (mode == 3)mode = 0;
		else mode++;
		init(lpParticle);
		pressed = TRUE;
		return;
	}
	else if (joystickButtons[5] == 1 && pressed == TRUE)return;

	if (joystickButtons[4] == 1 && pressed == FALSE){
		if (mode == 0)mode = 3;
		else mode--;
		init(lpParticle);
		pressed = TRUE;
		return;
	}
	else if (joystickButtons[4] == 1 && pressed == TRUE)return;

	pressed = FALSE;
}

void setTexture(unsigned char *buffer, size_t width, size_t height, GLuint *texID){
	glBindTexture(GL_TEXTURE_2D, *texID);

	/* テクスチャ画像はバイト単位に詰め込まれている */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	/* テクスチャを拡大・縮小する方法の指定 */
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	/* テクスチャの繰り返し方法の指定 */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/* テクスチャ環境 */
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

	/* テクスチャの割り当て */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
}

void initTextureColor(unsigned char *buffer, size_t width, size_t height, GLubyte red, GLubyte green, GLubyte blue){
	for (unsigned int i = 0; i < width * height; i++){
		*(buffer + 0) = red;
		*(buffer + 1) = green;
		*(buffer + 2) = blue;
		buffer += 4;
	}
}

void glInit(){
#if defined(WIN32)
	glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVPROC)wglGetProcAddress("glPointParameterfvEXT");
#endif
#if defined(WIN32)
	glPointParameterf = (PFNGLPOINTPARAMETERFPROC)wglGetProcAddress("glPointParameterf");
#endif

	/* テクスチャの読み込みに使う配列 */
	HBITMAP texBmp = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP6), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	BITMAP texImage;
	GetObject(texBmp, sizeof(BITMAP), &texImage);

	BYTE *src_image = (BYTE*)texImage.bmBits;
	BYTE *dst_image = (BYTE*)malloc(sizeof(BYTE)* texImage.bmWidth * texImage.bmHeight * 4);
		
	for (int i = 0; i < texImage.bmWidth * texImage.bmHeight; i++)dst_image[i * 4 + 3] = (BYTE)((src_image[i * 3 + 0] + src_image[i * 3 + 1] + src_image[i * 3 + 2]) / 3.0);
	
	glGenTextures(12, texID);

	//オレンジ
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 240, 160, 70);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[0]);
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 250, 180, 140);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[1]);
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 160, 120, 120);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[2]);

	//青
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 120, 120, 240);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[3]);
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 150, 150, 250);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[4]);
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 120, 120, 160);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[5]);

	//緑
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 80, 200, 140);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[6]);
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 180, 255, 210);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[7]);
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 80, 200, 140);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[8]);

	//白
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 200, 200, 200);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[9]);
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 255, 255, 255);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[10]);
	initTextureColor((unsigned char*)dst_image, texImage.bmWidth, texImage.bmHeight, 150, 150, 150);
	setTexture(dst_image, texImage.bmWidth, texImage.bmHeight, &texID[11]);


	/* 距離に対する点の大きさの制御 */
	float distance[] = { 0.0, 0.0025, 0.0 };
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, distance);
	glPointParameterf(GL_POINT_SIZE_MIN, 2.0);
	glPointParameterf(GL_POINT_SIZE_MAX, 2000.0);

	/* 初期設定 */
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	/* 陰影付けを無効にする */
	glDisable(GL_LIGHTING);

	//点にアンチエリアシング処理を行う
	glEnable(GL_POINT_SMOOTH);

	/* アンチエリアシング処理の品質を最高にする */
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	
	/* アルファブレンディングを有効にする */
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	//メモリ解放
	DeleteObject(texBmp);
	free(dst_image);
}

void drawTree(node *node){
	if (node->depth == 0){
		/*printf("%d %f %f %f %f %f \n%f %f %f %f %f \n%f\n", node->depth, node->center_x,
			node->node1->center_x * node->node1->sum_mass,
			node->node2->center_x * node->node2->sum_mass,
			node->node3->center_x * node->node3->sum_mass,
			node->node4->center_x * node->node4->sum_mass,
			node->node5->center_x * node->node5->sum_mass,
			node->node6->center_x * node->node6->sum_mass,
			node->node7->center_x * node->node7->sum_mass,
			node->node8->center_x * node->node8->sum_mass,
			node->sum_mass,
			(node->node1->center_x * node->node1->sum_mass+
			node->node2->center_x * node->node2->sum_mass+
			node->node3->center_x * node->node3->sum_mass+
			node->node4->center_x * node->node4->sum_mass+
			node->node5->center_x * node->node5->sum_mass+
			node->node6->center_x * node->node6->sum_mass+
			node->node7->center_x * node->node7->sum_mass+
			node->node8->center_x * node->node8->sum_mass) / node->sum_mass
			
			);*/
	}

	glColor4d(1.0f, 0.7f - (float)node->depth * 0.1f, 0.7f - (float)node->depth * 0.1f, 0.7f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(node->x, node->y, node->z);
	glVertex3f(node->x + node->distance, node->y, node->z);
	glVertex3f(node->x + node->distance, node->y + node->distance, node->z);
	glVertex3f(node->x, node->y + node->distance, node->z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(node->x, node->y, node->z + node->distance);
	glVertex3f(node->x + node->distance, node->y, node->z + node->distance);
	glVertex3f(node->x + node->distance, node->y + node->distance, node->z + node->distance);
	glVertex3f(node->x, node->y + node->distance, node->z + node->distance);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(node->x, node->y, node->z);
	glVertex3f(node->x + node->distance, node->y, node->z);
	glVertex3f(node->x + node->distance, node->y, node->z + node->distance);
	glVertex3f(node->x, node->y, node->z + node->distance);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(node->x, node->y + node->distance, node->z);
	glVertex3f(node->x + node->distance, node->y + node->distance, node->z);
	glVertex3f(node->x + node->distance, node->y + node->distance, node->z + node->distance);
	glVertex3f(node->x, node->y + node->distance, node->z + node->distance);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(node->x, node->y, node->z);
	glVertex3f(node->x, node->y + node->distance, node->z);
	glVertex3f(node->x, node->y + node->distance, node->z + node->distance);
	glVertex3f(node->x, node->y, node->z + node->distance);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(node->x + node->distance, node->y, node->z);
	glVertex3f(node->x + node->distance, node->y + node->distance, node->z);
	glVertex3f(node->x + node->distance, node->y + node->distance, node->z + node->distance);
	glVertex3f(node->x + node->distance, node->y, node->z + node->distance);
	glEnd();
		

	if (node->size <= 32)return;

	drawTree(node->node1);
	drawTree(node->node2);
	drawTree(node->node3);
	drawTree(node->node4);
	drawTree(node->node5);
	drawTree(node->node6);
	drawTree(node->node7);
	drawTree(node->node8);
}

void draw(particle *cParticle){
	float *px = cParticle->px;
	float *py = cParticle->py;
	float *pz = cParticle->pz;
	float *vx = cParticle->vx;
	float *vy = cParticle->vy;
	float *vz = cParticle->vz;
	float *m = cParticle->m;
	unsigned char *id = cParticle->texid;

	float ratio;
	int width, height;
	glfwGetFramebufferSize(glfwWindow, &width, &height);

	ratio = width / (float)height;
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0, (float)width / (float)height, 0, 100);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (bCameraTracking){
		long double sum_px = 0, sum_py = 0, sum_pz = 0, sum_m = 0;
		for (unsigned int i = 0; i < cParticle->size; i++){
			sum_px += (long double)px[i];
			sum_py += (long double)py[i];
			sum_pz += (long double)pz[i];
			sum_m += (long double)m[i];
		}
		panx = sum_px / cParticle->size;
		pany = sum_py / cParticle->size;
		panz = sum_pz / cParticle->size;
	}

	if (0 <= cos(phi))gluLookAt(cos(theta) * cos(phi) * zoom + panx, sin(phi) * zoom + pany, sin(theta) * cos(phi) * zoom + panz, panx, pany, panz, 0.0, 1.0, 0.0);
	else gluLookAt(cos(theta) * cos(phi) * zoom + panx, sin(phi) * zoom + pany, sin(theta) * cos(phi) * zoom + panz, panx, pany, panz, 0.0, -1.0, 0.0);


	if (particleColor != 4){
		/* テクスチャマッピング開始 */
		glEnable(GL_TEXTURE_2D);

		/* Point Sprite を有効にする */
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

		/* アルファテストを有効にする */
		glEnable(GL_ALPHA_TEST);
	}
	
	
	//粒子を表示
	glBindTexture(GL_TEXTURE_2D, 0 * 3 + 1);
	glBegin(GL_POINTS);
	glPointSize((GLfloat)size);
	for (unsigned int i = 0; i < cParticle->size / 3; i++){				
		glVertex3d(px[i], py[i], pz[i]);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 1 * 3 + 1);
	glBegin(GL_POINTS);
	glPointSize((GLfloat)size);
	for (unsigned int i = cParticle->size / 3; i < cParticle->size / 3 * 2; i++){		
		glVertex3d(px[i], py[i], pz[i]);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0 * 3 + 3);
	glBegin(GL_POINTS);
	glPointSize((GLfloat)size);
	for (unsigned int i = cParticle->size / 3 * 2; i < cParticle->size; i++){
		glVertex3d(px[i], py[i], pz[i]);
	}
	glEnd();
	

	/* アルファテストを無効にする */
	glDisable(GL_ALPHA_TEST);

	/* Point Sprite を無効にする */
	glDisable(GL_POINT_SPRITE);

	/* テクスチャマッピング終了 */
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);


	glColor4d(0.5, 0.5, 0.5, 0.7);

	//処理時間計測開始
	//clock_t start, end;
	//start = clock();

	/*rootNode->createCell(cParticle);
	drawTree(rootNode);
	rootNode->clearCell();
	rootNode->sort(cParticle);/
	//処理時間計測修了
	end = clock();
	printf("\r%.2f  ",(double)(end - start));
	//Sleep(100000);
	//delete rootNode;

	/*for (int i = 0; i < cParticle->size; i++){
		printf("%d %d\n", i % 32, cParticle->node_num[i]);
		Sleep(20);
		}*/

	if (showGrid == 1){
		for (float i = -500.0; i <= 500.0; i += 5.0){
			glBegin(GL_LINES);
			glVertex3f(500.0, i, 0.0); glVertex3f(-500.0, i, 0.0);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(i, 500.0, 0.0); glVertex3f(i, -500.0, 0.0);
			glEnd();
		}
	}
	else if (showGrid == 2){
		for (float i = -500.0; i <= 500.0; i += 5.0){
			glBegin(GL_LINES);
			glVertex3f(500.0, 0.0, i); glVertex3f(-500.0, 0.0, i);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(i, 0.0, 500.0); glVertex3f(i, 0.0, -500.0);
			glEnd();
		}
	}
	else if (showGrid == 3){
		for (float i = -500.0; i <= 500.0; i += 5.0){
			glBegin(GL_LINES);
			glVertex3f(0.0, 500.0, i); glVertex3f(0.0, -500.0, i);
			glEnd();
			glBegin(GL_LINES);
			glVertex3f(0.0, i, 500.0); glVertex3f(0, i, -500.0);
			glEnd();
		}
	}


	//クロスヘア表示
	if (bShowCrossHair){
		glBegin(GL_LINES);
		glColor3d(1.0, 0, 0);
		glVertex3d(-2 + panx, 0 + pany, 0 + panz);
		glVertex3d(2 + panx, 0 + pany, 0 + panz);
		glEnd();

		glBegin(GL_LINES);
		glColor3d(0, 1.0, 0);
		glVertex3d(0 + panx, -2 + pany, 0 + panz);
		glVertex3d(0 + panx, 2 + pany, 0 + panz);
		glEnd();

		glBegin(GL_LINES);
		glColor3d(0, 0, 1.0);
		glVertex3d(0 + panx, 0 + pany, -2 + panz);
		glVertex3d(0 + panx, 0 + pany, 2 + panz);
		glEnd();
	}

	
	//画面にテキスト描画	
	glColor3d(1.0, 1.0, 1.0);
	int glfwWidth, glfwHeight;
	glfwGetWindowSize(glfwWindow, &glfwWidth, &glfwHeight);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, glfwWidth, glfwHeight, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2f(6, 16);
	for (unsigned int i = 0; i < outputStr[0].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[0][i]);
	glRasterPos2f(6, 38);
	for (unsigned int i = 0; i < outputStr[1].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[1][i]);
	glRasterPos2f(6, 60);
	for (unsigned int i = 0; i < outputStr[2].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[2][i]);
	glRasterPos2f(6, 76);
	for (unsigned int i = 0; i < outputStr[3].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[3][i]);
	glRasterPos2f(6, 96);
	for (unsigned int i = 0; i < outputStr[4].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[4][i]);
	glRasterPos2f(6, 116);
	for (unsigned int i = 0; i < outputStr[5].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[5][i]);
	glRasterPos2f(6, 136);
	for (unsigned int i = 0; i < outputStr[6].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[6][i]);
	glRasterPos2f(6, 156);
	for (unsigned int i = 0; i < outputStr[7].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[7][i]);
	glRasterPos2f(6, 176);
	for (unsigned int i = 0; i < outputStr[8].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[8][i]);
	glRasterPos2f(6, 196);
	for (unsigned int i = 0; i < outputStr[9].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[9][i]);
	glRasterPos2f(6, 216);
	for (unsigned int i = 0; i < outputStr[10].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[10][i]);
	glRasterPos2f(6, 236);
	for (unsigned int i = 0; i < outputStr[11].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[11][i]);
	glRasterPos2f(6, 256);
	for (unsigned int i = 0; i < outputStr[12].size(); i++)glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, outputStr[12][i]);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glfwSwapBuffers(glfwWindow);
	glfwPollEvents();
}

void load(particle *cParticle){
	if (ifs.eof()){
		bReplay = FALSE;
		bPause = TRUE;
		ifs.close();
		return;
	}

	ifs.read((char *)cParticle->px, sizeof(float)* cParticle->size);
	ifs.read((char *)cParticle->py, sizeof(float)* cParticle->size);
	ifs.read((char *)cParticle->pz, sizeof(float)* cParticle->size);		
}

void saveRepray(particle *cParticle){
	for (unsigned int i = 0; i < cParticle->size; i++)
		ofs.write((char *)&cParticle->px[i], sizeof(float));
	for (unsigned int i = 0; i < cParticle->size; i++)
		ofs.write((char *)&cParticle->py[i], sizeof(float));
	for (unsigned int i = 0; i < cParticle->size; i++)
		ofs.write((char *)&cParticle->pz[i], sizeof(float));
	rec_frame++;
}

void saveScreenShot(){
	png_bytep raw1D;
	png_bytepp raw2D;

	int width, height;
	glfwGetWindowSize(glfwWindow, &width, &height);


	FILE *fp;
	char str[256];
	sprintf_s(str, "./%d.png", file_num);
	fopen_s(&fp, str, "wb");
	png_structp pp = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop ip = png_create_info_struct(pp);

	png_init_io(pp, fp);
	png_set_IHDR(pp, ip, width, height,	8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	raw1D = (png_bytep)malloc(height * png_get_rowbytes(pp, ip));
	raw2D = (png_bytepp)malloc(height * sizeof(png_bytep));
	for (int i = 0; i < height; i++)
		raw2D[i] = &raw1D[i*png_get_rowbytes(pp, ip)];

	glReadBuffer(GL_FRONT);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height,GL_RGB, GL_UNSIGNED_BYTE, (void*)raw1D);

	for (int i = 0; i < height / 2; i++){
		png_bytep swp = raw2D[i];
		raw2D[i] = raw2D[height - i - 1];
		raw2D[height - i - 1] = swp;
	}

	png_write_info(pp, ip);
	png_write_image(pp, raw2D);
	png_write_end(pp, ip);

	png_destroy_write_struct(&pp, &ip);
	fclose(fp);
	free(raw1D);
	free(raw2D);
	file_num++;
}

void operateCommand(){
	string str = command.front();
	command.pop_front();
	if (str == "rec" && bRecord == FALSE){
		cout << "連番PNGとして保存しますか？ yes/no" << endl;
		cin >> str;
		if (str == "yes"){
			bRecord = TRUE;
			file_num = 0;
		}
	}

	if (str == "save" && bSave == FALSE){
		cout << "保存するファイル名を入力してください" << endl;
		cin >> str;		
		rec_frame = 0;
		ofs.open(str, ios_base::out | ios_base::binary | ios::trunc);
		if (ofs.fail()){
			cout << "ファイルを保存できません" << endl;
			ofs.close();
			return;
		}
		else{
			bSave = TRUE;
			ofs.write((char *)&lpParticle->size, sizeof(unsigned int));
		}		
	}
	if (str == "stop"){
		ofs.close();
		ifs.close();
		bSave = FALSE;
		bReplay = FALSE;
		bRecord = FALSE;
	}
	if (str == "open" && bReplay == FALSE){
		cout << "ファイル名を入力してください" << endl;
		cin >> str;
		ifs.open(str, ios::binary);
		if (ifs.fail()){
			cout << "ファイルを開けません" << endl;
			return;
		}
		else{
			bReplay = TRUE;
			lpParticle->deleteAllParticle();			
			unsigned int num;
			ifs.read((char *)&num, sizeof(unsigned int));
			lpParticle->addParticle(num);
			frameCount = 0;
		}
	}

	if (str == "soft"){
		cin >> str;
		if(stof(str) > 0)lpParticle->eps2 = stof(str);
	}
}

unsigned __stdcall input_key(void *p){
	while (1){
		Sleep((DWORD)16.66666666666667);
		if (command.size() == 0){
			string str;
			cin >> str;
			command.push_back(str);
		}
	}
	return 0;
}

unsigned __stdcall input_joystick(void *p){
	while (1){
		glfwGetJoystick();
		Sleep((DWORD)16.66666666666667);
	}
	return 0;
}

unsigned __stdcall calc(void *p){
	particle *lpParticle = (particle*)p;
	float *px = lpParticle->px;
	float *py = lpParticle->py;
	float *pz = lpParticle->pz;
	float *vx = lpParticle->vx;
	float *vy = lpParticle->vy;
	float *vz = lpParticle->vz;
	float *ax = lpParticle->ax;
	float *ay = lpParticle->ay;
	float *az = lpParticle->az;
	float *m = lpParticle->m;

	while (1){
		while (wait){}
		lpParticle->calcNbodyCuda();
		wait = TRUE;
	}
		
	return 0;
}

class data
{
	

public :
	int x;
	int y;

	data(){
		x = 1;
		y = 1;
	}

	void add(int あああ){
		x = あああ;
		y = あああ;
		z = あああ;
		w = あああ;
	}

private:
	int z;
	int w;
};

int main(int argc, char *argv[]){
	char cBuff[512] = "";
	GetConsoleTitle(cBuff, 512);

	HWND hWnd = FindWindow(0, cBuff);
	if (!hWnd)return 0;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_LAYERED);
	SetLayeredWindowAttributes(hWnd, 0, 192, LWA_ALPHA);
	SetConsoleTextAttribute(handle, 0x0008 | 0x000b);
	
	glfwInit();
	glfwSwapInterval(0);

	glfwWindow = glfwCreateWindow(1280, 720, "N-Body", NULL, NULL);
	glfwMakeContextCurrent(glfwWindow);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	TwInit(TW_OPENGL, NULL);
	glewInit();
	
	glfwSetKeyCallback(glfwWindow, glfwKeyCallback);
	glfwSetMouseButtonCallback(glfwWindow, glfwMouseButtonCallback);
	glfwSetScrollCallback(glfwWindow, glfwScrollCallback);
	glfwSetCursorPosCallback(glfwWindow, glfwCursorPosCallback);
	glfwGetCursorPos(glfwWindow, &preXpos, &preYpos);
	data ああa[10];
	for (int i = 0; i < 10; i++){
		ああa[i].add(i);
		ああa[i].add(i+1);
	}
	int *いいい = (int*)ああa;

	for (int i = 0; i < 40; i++){
		cout << いいい[i] << endl;
	}

	glInit();

	particle *cParticle = new particle;
	init(cParticle);

	lpParticle = cParticle;

	HANDLE subThread[2];
	//subThread[0] = (HANDLE)_beginthreadex(NULL, 0, input_joystick, NULL, 0, NULL);
	subThread[1] = (HANDLE)_beginthreadex(NULL, 0, input_key, NULL, 0, NULL);

	rootNode = new node;

	while (!glfwWindowShouldClose(glfwWindow)){
		//処理時間計測開始
		clock_t start, end;
		start = clock();

		if (command.size() > 0)operateCommand();

		if (bPause){
			draw(cParticle);
			glfwGetJoystick();
			glfwGetJoystickButton();
			goto text;
		}
		glfwGetJoystickButton();

		if (bRecord){
			saveScreenShot();
		}

		if (bReplay){
			load(cParticle);
			goto draw;
		}

		if (bSave){
			saveRepray(cParticle);
		}

		cParticle->calcNbodyCuda();

	draw:
		glfwGetJoystick();
		draw(cParticle);

		float pe = 0, ke = 0;		
		if (bCalcEnergy){
			pe = -calcPotentialEnergyCuda(cParticle->px, cParticle->py, cParticle->pz, cParticle->m, PARTICLE_NUM);
			ke = calcKineticEnergyCuda(cParticle->px, cParticle->py, cParticle->pz, cParticle->vx, cParticle->vy, cParticle->vz, cParticle->m, PARTICLE_NUM);
		}

		//処理時間計測修了
		end = clock();
		queue.push_back(((double)(end - start) / CLOCKS_PER_SEC));
		queue.pop_front();		
		double ave = accumulate(queue.begin(), queue.end(), 0.0) / queue.size();
		double ave_gpu = accumulate(queue_gpu.begin(), queue_gpu.end(), 0.0) / queue_gpu.size();
		frameCount++;

text:
		//画面に表示するテキストを格納
		char str[256];
		sprintf_s(str, "%f fps Total:%f ms GPU:%f ms %llu frames      ", 1.0 / ave, ave * 1000.0, ave_gpu, frameCount);
		outputStr[0] = string(str);
		sprintf_s(str, "Camera:(%f,%f,%f)>(%f,%f,%f)           ", cos(theta) * cos(phi) * zoom + panx, sin(phi) * zoom + pany, sin(theta) * cos(phi) * zoom + panz, panx, pany, panz);
		outputStr[1] = string(str);
		sprintf_s(str, "%d particles ", cParticle->size);
		outputStr[2] = string(str);
		sprintf_s(str, "pe:%f   ", pe);
		outputStr[3] = string(str);
		sprintf_s(str, "ke:%f   ", ke);
		outputStr[4] = string(str);
		sprintf_s(str, "Softning:%f   ", cParticle->eps2);
		outputStr[5] = string(str);
		sprintf_s(str, "Pause:%s   ", bPause ? "true" : "false");
		outputStr[6] = string(str);
		sprintf_s(str, "CrossHair:%s   ", bShowCrossHair ? "true" : "false");
		outputStr[7] = string(str);
		sprintf_s(str, "Grid:%s   ", showGrid == 0 ? "false" : showGrid == 1 ? "x-y" : showGrid == 2 ? "x-z" : showGrid == 3 ? "y-z" : "false");
		outputStr[8] = string(str);
		sprintf_s(str, "CameraReset:%s   ", bCameraReset ? "true" : "false");
		outputStr[9] = string(str);
		sprintf_s(str, "CameraTracking:%s   ", bCameraTracking ? "true" : "false");
		outputStr[10] = string(str);
		sprintf_s(str, "CalcEnergy:%s   ", bCalcEnergy ? "true" : "false");
		outputStr[11] = string(str);
		sprintf_s(str, "ParticleColor:%d   ", particleColor);
		outputStr[12] = string(str);
	}

	
	// Windowの削除
	glfwDestroyWindow(glfwWindow);
	// GLFWの終了処理
	glfwTerminate();


	//メモリ解放
	delete cParticle;
	delete rootNode;
	//CloseHandle(subThread[0]);
	CloseHandle(subThread[1]);
	//ofs.close();

	return 0;
}