// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

// Include GLEW	
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/texture.hpp>

extern float power;		//control.cpp에도 선언됨
extern bool Launch;		//control.cpp에 선언됨

GLfloat *make_cylinder(GLfloat x, GLfloat y, GLfloat z, GLfloat h, GLfloat r, size_t resolution);		//원기둥 함수
GLfloat *make_cone(GLfloat x, GLfloat y, GLfloat z, GLfloat h, GLfloat r, size_t resolution);			//원뿔 함수



int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1600, 900, "Rocket_Launch", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	//별 
	static const GLfloat g_star_buffer_data[] = {
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};

	//별 색
	// One color for each vertex. They were generated randomly.
	static const GLfloat g_star_color_buffer_data[] = {
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f
	};

	//땅 지면
	static const GLfloat g_ground_data[] = {
		1000,0,1000,
		1000,0,-1000,
		-1000,0,-1000,

		-1000,0,-1000,
		-1000,0,1000,
		1000,0,1000
	};

	//땅 지면 색
	static const GLfloat g_ground_color_data[] = {
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	//낙하산
	static const GLfloat g_vertex_buffer_data4[] = {
		0.0f, -3.0f, 0.0f,
		0.0f, 0.0f, 3.0f,
		3.0f, 0.0f, 0.0f,

		0.0f, -3.0f, 0.0f,
		3.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -3.0f,

		0.0f, -3.0f, 0.0f,
		-3.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 3.0f,

		0.0f, -3.0f, 0.0f,
		-3.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -3.0f,

		0.0f, 0.0f, 3.0f,
		-3.0f, 0.0f, 0.0f,
		3.0f, 0.0f, 0.0f,

		0.0f, 0.0f, -3.0f,
		-3.0f, 0.0f, 0.0f,
		3.0f, 0.0f, 0.0f,

		2.9f, 0.0f, 0.1f,
		2.9f, 0.0f, -0.1f,
		1.0f, 4.0f, 0.0f,

		3.0f, 0.0f, 0.0f,
		1.0f, 4.0f, 0.0f,
		2.9f, 0.0f, 0.1f,

		2.9f, 0.0f, -0.1f,
		3.0f, 0.0f, 0.0f,
		1.0f, 4.0f, 0.0f,

		0.1f, 0.0f, 2.9f,
		-0.1f, 0.0f, 2.9f,
		0.0f, 4.0f, 1.0f,

		0.1f, 0.0f, 2.9f,
		0.0f, 0.0f, 3.0f,
		0.0f, 4.0f, 1.0f,

		-0.1f, 0.0f, 2.9f,
		0.0f, 0.0f, 3.0f,
		0.0f, 4.0f, 1.0f,

		-2.9f, 0.0f, 0.1f,
		-2.9f, 0.0f, -0.1f,
		-1.0f, 4.0f, 0.0f,

		-2.9f, 0.0f, 0.1f,
		-3.0f, 0.0f, 0.0f,
		-1.0f, 4.0f, 0.0f,

		-2.9f, 0.0f, -0.1f,
		-3.0f, 0.0f, 0.0f,
		-1.0f, 4.0f, 0.0f,

		-0.1f, 0.0f, -2.9f,
		0.1f, 0.0f, -2.9f,
		0.0f, 4.0f, -1.0f,

		-0.1f, 0.0f, -2.9f,
		0.0f, 0.0f, -3.0f,
		0.0f, 4.0f, -1.0f,

		0.1f, 0.0f, -2.9f,
		0.0f, 0.0f, -3.0f,
		0.0f, 4.0f, -1.0f
	};

	//낙하산 색
	static const GLfloat g_color_buffer_data4[] = {
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,

	};

	//날개
	static const GLfloat g_vertex_buffer_data[] = {
		0.0f,0.0f,-1.0f,
		1.0f,0.0f, 0.0f,
		0.0f,0.0f, 1.0f,

		0.0f,0.0f,-1.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,

		0.0f,1.0f,0.0f,
		0.0f, 0.0f,-1.0f,
		1.0f,0.0f,0.0f,

		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f,
		1.0f,0.0f,0.0f
	};

	//날개 색
	// One color for each vertex. They were generated randomly.
	static const GLfloat g_color_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};

	//로켓 색
	static const GLfloat g_color_buffer_data2[] = {
		1.0f, 0.0f, 0.0f,
	};


	GLfloat *g_cylinder_buffer_data = make_cylinder(0, 0, 0, 5, 1, 36);			//원기둥 함수  반지름 1, 높이 5
	GLfloat *g_cone_buffer_data = make_cone(0, 5, 0, 1, 1, 36);					//원뿔 함수		반지름 1, 높이 1

	//지면
	GLuint groundbuffer;
	glGenBuffers(1, &groundbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, groundbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_ground_data), g_ground_data, GL_STATIC_DRAW);

	//지면 색
	GLuint ground_color_buffer;
	glGenBuffers(1, &ground_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, ground_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_ground_color_data), g_ground_color_data, GL_STATIC_DRAW);


	//날개
	GLuint wing_buffer;
	glGenBuffers(1, &wing_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, wing_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	//날개 색
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);


	//원기둥
	GLuint cylinder_buffer;
	glGenBuffers(1, &cylinder_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, cylinder_buffer);
	glBufferData(GL_ARRAY_BUFFER, 36 * 18 * sizeof(GLfloat), g_cylinder_buffer_data, GL_STATIC_DRAW);	//직접 계산을 한듯한 소스코드가 있다. 왜 그럴까? sizeof는 변수의 크기 반환. GLfloat은 배열 주소라 4만 반환

	//원기둥 색
	GLuint colorbuffer2;
	glGenBuffers(1, &colorbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data2), g_color_buffer_data2, GL_STATIC_DRAW);

	//원뿔
	GLuint cone_buffer3;
	glGenBuffers(1, &cone_buffer3);
	glBindBuffer(GL_ARRAY_BUFFER, cone_buffer3);
	glBufferData(GL_ARRAY_BUFFER, 36 * 9 * sizeof(GLfloat), g_cone_buffer_data, GL_STATIC_DRAW);

	//원뿔 색
	GLuint colorbuffer3;
	glGenBuffers(1, &colorbuffer3);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data2), g_color_buffer_data2, GL_STATIC_DRAW);

	//낙하산
	GLuint parachute_buffer;
	glGenBuffers(1, &parachute_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, parachute_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data4), g_vertex_buffer_data4, GL_STATIC_DRAW);

	//낙하산 색
	GLuint colorbuffer4;
	glGenBuffers(1, &colorbuffer4);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data4), g_color_buffer_data4, GL_STATIC_DRAW);

	//별
	GLuint star_buffer;
	glGenBuffers(1, &star_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, star_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_star_buffer_data), g_star_buffer_data, GL_STATIC_DRAW);

	//별 색
	GLuint colorbuffer8;
	glGenBuffers(1, &colorbuffer8);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer8);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_star_color_buffer_data), g_star_color_buffer_data, GL_STATIC_DRAW);



	double lastTime = glfwGetTime();
	double lastFrameTime = lastTime;
	vec3 gOrientation1, gOrientation2, gOrientation3, gOrientation4, gOrientation5, gOrientation6;

	std::vector<glm::vec3> positions(100);
	std::vector<glm::quat> orientations(100);
	for (int i = 0; i < 100; i++) {
		positions[i] = glm::vec3(rand() % 200 - 50, rand() % 100 + 20, rand() % 200 - 50);
		orientations[i] = glm::normalize(glm::quat(glm::vec3(rand() % 360, rand() % 360, rand() % 360)));
	}

	int flag = 0;			//vx 속도 설정을 위한 flag
	int flag2 = 0;			//낙하시 속도 설정을 위한 flag2
	float y = 0;				//y좌표
	float x = 0;			//x좌표

	float rad = 3.141592f / 3;			//초기에 60도로 날아간다고 가정

	float vx = 0;		//x축 방향의 속도
	float vy = 0;		//y축 방향의 속도
	float vy2 = 0;		//y축 낙하시 속도
	float g = 5.0f;	//중력가속도

	bool parachute = false;			//낙하산 펴졌는지의 여부
	bool stop = false;				//지면에 닿았는지의 여부

	do {

		//지속적으로 회전하기 위해 deltaTime 값을 구한다
		double currentTime = glfwGetTime();
		float deltaTime = (float)(currentTime - lastFrameTime);		//시간 차
		lastFrameTime = currentTime;


		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();


		if (vy < 0 && (y - 5 * sin(atan(abs(vy) / vx)) + 0.4f) < 0) {			//지면에 닿았는지 확인
			stop = true;
		}


		//포물선
		vec3 gPosition8(0.0f + x, 0.0f + y, 0.0f);
		glm::mat4 TranslationMatrix8 = translate(mat4(), gPosition8);
		if (vx / vy >= 0) {
			gOrientation6.z = -atan(vx / vy);	//뒤에 숫자가 낮을 수록 빠른 회전
		}
		else {
			gOrientation6.z = 3.141592f - atan(vx / vy);	//뒤에 숫자가 낮을 수록 빠른 회전	//뒤에 숫자가 낮을 수록 빠른 회전
		}
		glm::mat4 RotationMatrix8 = eulerAngleYXZ(gOrientation6.y, gOrientation6.x, gOrientation6.z);


		//평면 모델
		glm::mat4 Model = glm::mat4(1.0f);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * Model;


		//낙하산 모델
		vec3 gPosition6(0.0f, -4.0f, 0.0f);
		glm::mat4 TranslationMatrix6 = translate(mat4(), gPosition6);
		//glm::mat4 ScalingMatrix6 = scale(mat4(), vec3(3.0f, 3.0f, 3.0f));
		glm::mat4 Model6 = glm::mat4(1.0f);


		//오른쪽 날개
		vec3 gPosition1(1.0f, 0.0f, 0.0f);
		//gOrientation1.x = 0.0f;
		gOrientation1.y += 3.14159f * deltaTime;
		glm::mat4 TranslationMatrix1 = translate(mat4(), gPosition1); // A bit to the left
		glm::mat4 RotationMatrix1 = eulerAngleYXZ(gOrientation1.y, gOrientation1.x, gOrientation1.z);		//회전
		glm::mat4 ScalingMatrix1 = scale(mat4(), vec3(1.0f, 2.0f, 1.0f));
		glm::mat4 Model1 = glm::mat4(1.0f);			//오른쪽 날개


		//왼쪽 날개
		vec3 gPosition4(-1.0f, 0.0f, 0.0f);
		gOrientation4.y = 3.14159f;
		glm::mat4 TranslationMatrix4 = translate(mat4(), gPosition4); // A bit to the left
		glm::mat4 RotationMatrix4 = eulerAngleYXZ(gOrientation4.y, gOrientation4.x, gOrientation4.z);
		glm::mat4 Model4 = glm::mat4(1.0f);


		//기둥
		vec3 gPosition2(0.0f, 0.0f, 0.0f);
		glm::mat4 TranslationMatrix2 = translate(mat4(), gPosition2); // A bit to the left
		glm::mat4 ScalingMatrix2 = scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
		glm::mat4 Model2 = glm::mat4(1.0f);


		glm::mat4 Model3 = glm::mat4(1.0f);





		if (Launch) {
			if (!flag) {
				vx = power * cos(rad);
				vy = power * sin(rad);
				flag = 1;
			}

			if (vy < -20) {
				parachute = true;
			}

			Model1 = Model1 * TranslationMatrix8 * RotationMatrix8 * RotationMatrix1 * TranslationMatrix1 * ScalingMatrix1;						//오른쪽 날개
			Model4 = Model4 * TranslationMatrix8 * RotationMatrix8 * RotationMatrix1 * TranslationMatrix4 * RotationMatrix4 * ScalingMatrix1;		//왼쪽 날개
			Model2 = Model2 * TranslationMatrix8 * RotationMatrix8 * TranslationMatrix2 * RotationMatrix1 * ScalingMatrix2;						//기둥
			Model3 = Model3 * TranslationMatrix8 * RotationMatrix8 * TranslationMatrix2 * RotationMatrix1 * ScalingMatrix2;						//원뿔
			Model6 = Model6 * TranslationMatrix8 * RotationMatrix8 * RotationMatrix1 * TranslationMatrix6;

			if (!stop) {


				if (parachute) {
					if (!flag2) {
						vy2 = vy + 19.0f;
						flag2 = true;
					}
					vy2 -= 0.1f *deltaTime;
					y += vy2 * deltaTime;
					x += deltaTime;
					cout << vy2 << "\n";
				}
				else {
					x += vx * deltaTime;
					vy -= g * deltaTime;
					y += vy * deltaTime;
					cout << vy << "\n";
				}
			}
		}
		else {
			Model1 = Model1 * ScalingMatrix1  * TranslationMatrix1;
			Model4 = Model4 * ScalingMatrix1 * TranslationMatrix4 * RotationMatrix4;
			Model2 = Model2 * ScalingMatrix2  * TranslationMatrix2;
			Model3 = Model3 * ScalingMatrix2 * TranslationMatrix2;
			Model6 = Model6 * TranslationMatrix6;
		}

		glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * Model1;
		glm::mat4 MVP5 = ProjectionMatrix * ViewMatrix * Model4;
		glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * Model2;
		glm::mat4 MVP4 = ProjectionMatrix * ViewMatrix * Model3;
		glm::mat4 MVP6 = ProjectionMatrix * ViewMatrix * Model6;




		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);


		for (int i = 0; i < 100; i++) {
			glm::mat4 RotationMatrix = glm::toMat4(orientations[i]);
			glm::mat4 TranslationMatrix = translate(mat4(), positions[i]);
			glm::mat4 ScalingMatrix = scale(mat4(), vec3(0.2f, 0.4f, 1.0f));
			glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix*ScalingMatrix;

			glm::mat4 MVP10 = ProjectionMatrix * ViewMatrix * ModelMatrix;

			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP10[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, star_buffer);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
									//shader의 layout번호와 일치해야한다

				3,                  // size, 각 정점은 float 3개로 구성되어있다
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer8);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 3 indices starting at 0 -> 1 triangle
											//0번째 정점부터 그린다, 정점은 총 3개 있다

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);


		}

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, groundbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
								//shader의 layout번호와 일치해야한다

			3,                  // size, 각 정점은 float 3개로 구성되어있다
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, ground_color_buffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle
										//0번째 정점부터 그린다, 정점은 총 3개 있다

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);



		if (parachute) {
			//낙하산

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP6[0][0]);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, parachute_buffer);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
									//shader의 layout번호와 일치해야한다

				3,                  // size, 각 정점은 float 3개로 구성되어있다
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, colorbuffer4);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// Draw the triangle !
			glDrawArrays(GL_TRIANGLES, 0, 18 * 3); // 3 indices starting at 0 -> 1 triangle
											//0번째 정점부터 그린다, 정점은 총 3개 있다

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}






		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);


		//// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, wing_buffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		//// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		//// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP5[0][0]);


		//// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, wing_buffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		//// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		//// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP3[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cylinder_buffer);
		glVertexAttribPointer(
			0,		  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,		  // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0,		  // stride
			(void *)0 // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
		glVertexAttribPointer(
			1,		  // attribute. No particular reason for 1, but must match the layout in the shader.
			3,		  // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0,		  // stride
			(void *)0 // array buffer offset
		);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 37 * 18); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP4[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cone_buffer3);
		glVertexAttribPointer(
			0,		  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,		  // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0,		  // stride
			(void *)0 // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer2);
		glVertexAttribPointer(
			1,		  // attribute. No particular reason for 1, but must match the layout in the shader.
			3,		  // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0,		  // stride
			(void *)0 // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLE_FAN, 0, 37 * 9); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &wing_buffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

GLfloat *make_cone(GLfloat x, GLfloat y, GLfloat z, GLfloat h, GLfloat r, size_t numberOfSides) {

	GLfloat *result = new GLfloat[numberOfSides * 9];

	GLfloat doublePi = 2.0f * 3.141592f;

	for (size_t i = 0; i < numberOfSides * 9; i += 9) {
		size_t f = i / 9;
		result[i + 0] = x + (r * cos(f * doublePi / numberOfSides));
		result[i + 1] = y;
		result[i + 2] = z + (r * sin(f * doublePi / numberOfSides));

		result[i + 3] = x + (r * cos((f + 1ll) * doublePi / numberOfSides));
		result[i + 4] = y;
		result[i + 5] = z + (r * sin((f + 1ll) * doublePi / numberOfSides));

		result[i + 6] = x;
		result[i + 7] = y + h;
		result[i + 8] = z;
	}
	return result;
}

GLfloat *make_cylinder(GLfloat x, GLfloat y, GLfloat z, GLfloat h, GLfloat r, size_t numberOfSides) {

	GLfloat *result = new GLfloat[numberOfSides * 18];

	GLfloat doublePi = 2.0f * 3.141592f;

	for (size_t i = 0; i < numberOfSides * 18; i += 18) {
		size_t f = i / 18;
		result[i + 0] = x + (r * cos(f * doublePi / numberOfSides));
		result[i + 1] = y;
		result[i + 2] = z + (r * sin(f * doublePi / numberOfSides));

		result[i + 3] = x + (r * cos((f + 1ll) * doublePi / numberOfSides));
		result[i + 4] = y;
		result[i + 5] = z + (r * sin((f + 1ll) * doublePi / numberOfSides));

		result[i + 6] = x + (r * cos(f * doublePi / numberOfSides));
		result[i + 7] = y + h;
		result[i + 8] = z + (r * sin(f * doublePi / numberOfSides));

		result[i + 9] = x + (r * cos(f * doublePi / numberOfSides));
		result[i + 10] = y + h;
		result[i + 11] = z + (r * sin(f * doublePi / numberOfSides));

		result[i + 12] = x + (r * cos((f + 1ll) * doublePi / numberOfSides));
		result[i + 13] = y + h;
		result[i + 14] = z + (r * sin((f + 1ll) * doublePi / numberOfSides));

		result[i + 15] = x + (r * cos((f + 1ll) * doublePi / numberOfSides));
		result[i + 16] = y;
		result[i + 17] = z + (r * sin((f + 1ll) * doublePi / numberOfSides));
	}
	return result;
}

