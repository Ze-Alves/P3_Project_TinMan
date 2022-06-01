#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
using namespace std;

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include "LoadShaders.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp> // glm::inverseTranspose()


#define WIDTH 800
#define HEIGHT 600


GLuint VAO;
GLuint Buffer;
GLuint programa;

glm::mat4 Model, View, Projection;
glm::mat3 NormalMatrix;
GLfloat angle = 0.0f;


void LerVert(ifstream& file, vector <GLfloat>& vert, int& i) {

	char d;
	string s = "";
	while (file.get(d)) {

		if (d == '\n') {
			if (s != "")
				vert.push_back(stof(s));
			break;
		}
		else if (d == ' ' && s != "") {
			i++;
			vert.push_back(stof(s));
			s = "";
		}
		else s += d;

	}
	
}

void LerTex(ifstream& file,vector <GLfloat> &tex,int& i) {
	char d;
	string s = "";
	while (file.get(d)) {

		if (d == '\n')
			break;

		if (d == ' ' && s != "") {
			tex.push_back(stof(s));
			s = "";
		}
		else s += d;

	}
	if (s != "")
		tex.push_back(stof(s));
}

void LerNor(ifstream& file,vector <GLfloat>& nor,int& i) {
	char d;
	string s = "";
	while (file.get(d)) {

		if (d == '\n')
			break;

		if (d == ' ' && s != "") {
			nor.push_back(stof(s));
			s = "";
		}
		else s += d;
	}
	if (s != "")
		nor.push_back( stof(s));

}

void LerInd(ifstream& file,vector  <GLint>& ind,int& i) {
	char d;
	string s = "";
	while (file.get(d)) {

		if (d == '\n')
			break;

		if ((d == '/' || d == ' ') && s != "") {
			ind.push_back( stoi(s));
			s = "";
		}
		else s += d;

	}
	if (s != "")
		ind.push_back(stoi(s));
}

void LerFicheiro(vector <GLfloat> &vert, vector <GLfloat> &tex, vector <GLfloat> &nor, vector <GLint> &ind){

	char d;
	ifstream file("Iron_Man.obj");
	string s = "";
	string trash;
	int vi = 0,ti=0,ni=0,ii=0;

	int i = 0;
	while (file.get(d)) {
		

		s = "";
		s += d;
		file.get(d);
		s += d;
		if (s == "v ") {
			LerVert(file, vert, vi);
			i++;
		}
		else if (s == "vt")
			LerTex(file, tex, ti);
		else if (s == "vn")
			LerNor(file, nor, ni);
		else if (s == "f ")
			LerInd(file, ind, ii);
		else
			getline(file, trash);
	}
	

	
}



void LoadVertices(vector <GLfloat>& vert, vector <GLfloat>& tex, vector <GLfloat>& nor) {
	
	


	GLuint buffer[3];




	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(3, buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

	glBufferStorage(GL_ARRAY_BUFFER, vert.size()*4, &vert[0], 0);


	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);

	glBufferStorage(GL_ARRAY_BUFFER, tex.size() * 4, &tex[0], 0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);

	glBufferStorage(GL_ARRAY_BUFFER, nor.size() * 4, &nor[0], 0);


	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER,   "shaders/light.vert" },
		{ GL_FRAGMENT_SHADER, "shaders/light.frag" },
		{ GL_NONE, NULL }
	};

	programa = LoadShaders(shaders);
	if (!programa) exit(EXIT_FAILURE);
	glUseProgram(programa);



	GLint coordsId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vPosition");

	GLint normalId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vNormal");

	GLint textId= glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vTexture");


	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	

	glVertexAttribPointer(coordsId, 3, GL_FLOAT, GL_FALSE, 0 , (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);

	glVertexAttribPointer(textId, 2, GL_FLOAT, GL_TRUE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glVertexAttribPointer(normalId, 3 , GL_FLOAT, GL_TRUE, 0, (void*)0);

	
	glEnableVertexAttribArray(coordsId);

	glEnableVertexAttribArray(textId);

	glEnableVertexAttribArray(normalId);

	Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	View = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 5.0f),	
		glm::vec3(0.0f, 0.0f, 0.0f),	
		glm::vec3(0.0f, 1.0f, 0.0f)		
	);
	Model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 ModelView = View * Model;
	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelView));


	GLint modelId = glGetProgramResourceLocation(programa, GL_UNIFORM, "Model");
	glProgramUniformMatrix4fv(programa, modelId, 1, GL_FALSE, glm::value_ptr(Model));

	GLint viewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "View");
	glProgramUniformMatrix4fv(programa, viewId, 1, GL_FALSE, glm::value_ptr(View));
	
	GLint modelViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "ModelView");
	glProgramUniformMatrix4fv(programa, modelViewId, 1, GL_FALSE, glm::value_ptr(ModelView));
	
	GLint projectionId = glGetProgramResourceLocation(programa, GL_UNIFORM, "Projection");
	glProgramUniformMatrix4fv(programa, projectionId, 1, GL_FALSE, glm::value_ptr(Projection));
	
	GLint normalViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "NormalMatrix");

	glProgramUniformMatrix3fv(programa, normalViewId, 1, GL_FALSE, glm::value_ptr(NormalMatrix));






	GLint locationTexSampler = glGetProgramResourceLocation(programa, GL_UNIFORM, "TinTexture");
	glProgramUniform1i(programa, locationTexSampler, 0);


	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); 
	glCullFace(GL_BACK);
}



void display(int vertices) {

	static const GLfloat black[] = {
		0.0f, 0.0f, 0.0f, 0.0f
	};

	glClearBufferfv(GL_COLOR, 0, black);

	glClear(GL_DEPTH_BUFFER_BIT);

	Model = glm::rotate(glm::mat4(1.0f), angle += 0.0002f, glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f)));
	glm::mat4 ModelView = View * Model;
	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelView));

	GLint modelId = glGetProgramResourceLocation(programa, GL_UNIFORM, "Model");
	glProgramUniformMatrix4fv(programa, modelId, 1, GL_FALSE, glm::value_ptr(Model));

	GLint viewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "View");
	glProgramUniformMatrix4fv(programa, viewId, 1, GL_FALSE, glm::value_ptr(View));

	GLint modelViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "ModelView");
	glProgramUniformMatrix4fv(programa, modelViewId, 1, GL_FALSE, glm::value_ptr(ModelView));

	GLint normalViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "NormalMatrix");
	glProgramUniformMatrix3fv(programa, normalViewId, 1, GL_FALSE, glm::value_ptr(NormalMatrix));








	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, vertices/3);
}

void OrganizarIndex(vector <GLfloat>& vert, vector <GLfloat>& tex, vector <GLfloat>& nor, vector <GLint>& ind,
	vector <GLfloat>& ivert, vector <GLfloat>& itex, vector <GLfloat>& inor,int size) {

	for (short i = 0; i < ind.size(); i+=9) {

		ivert.push_back(vert[(ind[i]*3)-3]);
		ivert.push_back(vert[(ind[i] * 3) - 2]);
		ivert.push_back(vert[(ind[i] * 3) - 1]);

		ivert.push_back(vert[(ind[i+3] * 3) - 3]);
		ivert.push_back(vert[(ind[i+3] * 3) - 2]);
		ivert.push_back(vert[(ind[i+3] * 3) - 1]);

		ivert.push_back(vert[(ind[i + 6] * 3) - 3]);
		ivert.push_back(vert[(ind[i + 6] * 3) - 2]);
		ivert.push_back(vert[(ind[i + 6] * 3) - 1]);

		itex.push_back(tex[(ind[i+ 1] * 2) - 2]);
		itex.push_back(tex[(ind[i+ 1] * 2) - 1]);

		itex.push_back(tex[(ind[i+ 4] * 2) - 2]);
		itex.push_back(tex[(ind[i+ 4] * 2) - 1]);

		itex.push_back(tex[(ind[i+7] * 2) - 2]);
		itex.push_back(tex[(ind[i+7] * 2) - 1]);

		inor.push_back(nor[(ind[i + 2] * 3) - 3]);
		inor.push_back(nor[(ind[i + 2] * 3) - 2]);
		inor.push_back(nor[(ind[i + 2] * 3) - 1]);
					   
		inor.push_back(nor[(ind[i + 5] * 3) - 3]);
		inor.push_back(nor[(ind[i + 5] * 3) - 2]);
		inor.push_back(nor[(ind[i + 5] * 3) - 1]);
					   
		inor.push_back(nor[(ind[i + 8] * 3) - 3]);
		inor.push_back(nor[(ind[i + 8] * 3) - 2]);
		inor.push_back(nor[(ind[i + 8] * 3) - 1]);
	}
}


void LoadTexture(string text) {
	GLuint TextureName;
	glGenTextures(1, &TextureName);
	glBindTexture(GL_TEXTURE_2D, TextureName);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);


	int width, height, nChannels;

	unsigned char* imageData = stbi_load(text.c_str(), &width, &height, &nChannels, 0);


	if (imageData) {
		// Carrega os dados da imagem para o Objeto de Textura vinculado ao target da face
		glTexImage2D(GL_TEXTURE_2D,
			0,					// Nível do Mipmap
			GL_RGB,				// Formato interno do OpenGL
			width, height,		// width, height
			0,					// border
			nChannels == 4 ? GL_RGBA : GL_RGB,	// Formato da imagem
			GL_UNSIGNED_BYTE,	// Tipos dos dados da imagem
			imageData);			// Apontador para os dados da imagem de textura


		// Liberta a imagem da memória do CPU
		stbi_image_free(imageData);
	}
	else cout << "Erro";

}

int main() {

	GLFWwindow* window;

	vector <GLfloat> vertices;
	vector <GLfloat> tvertices;
	vector <GLfloat> nvertices;
	vector <GLint> ivertices;
	
	

	LerFicheiro(vertices, tvertices, nvertices, ivertices);

	vector <GLfloat> indexedvertices;
	vector <GLfloat> indexedtvertices;
	vector <GLfloat> indexednvertices;

	OrganizarIndex(vertices,tvertices,nvertices,ivertices,indexedvertices,indexedtvertices,indexednvertices,ivertices.size());

	string textfile = "Iron_Man_D.tga";

	

	//for (int i = 0; i < indexedvertices.size(); i += 9) {

	//	cout << "Face "<<i/9<<" V1 " << indexedvertices[i] << " " << indexedvertices[i + 1] <<" "<< indexedvertices[i+2]<<" "
	//		<<" V2 " << indexedvertices[i + 3] << " " << indexedvertices[i + 4]  <<" " << indexedvertices[i + 5]
	//		<<" V3 " << indexedvertices[i + 6] << " " << indexedvertices[i + 7] << " " << indexedvertices[i + 8] << "\n";
	//}


	if (!glfwInit()) return -1;

	window = glfwCreateWindow(WIDTH, HEIGHT, "TinMan", NULL, NULL);

	if (window == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewInit();

	LoadTexture(textfile);

	LoadVertices(indexedvertices, indexedtvertices, indexednvertices);


	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		display(ivertices.size());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

}
