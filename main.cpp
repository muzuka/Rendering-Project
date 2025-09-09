/* Depth dependent Point-based Cartoon Cloud rendering system
 *
 * Version 1.0
 *  - Main results achieved
 *  - Depth-based texture variation
 *  - Depth-based size variation
 *  - Texture mapping
 * Next Version 1.1
 *  - True depth buffer based outlining
 *
 * Controls:
 *
 * 1 -------- White Background
 * 2 -------- Black Background
 * a -------- decreases division of distance
 * s -------- increases division of distance
 * x -------- decreases size coefficient
 * z -------- increases size coefficient
 * e -------- decreases radius cutoff
 * r -------- increases radius cutoff
 * q -------- increase color divide distance
 * w -------- decrease color divide distance
 * RMB ------ menu
 *
 * Written by: Sean Brown
 * UCID: 10062604
 */



#ifdef __APPLE__
#include <OpenGL/glew.h>
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#endif

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include "VectorUtils.h"
#include "MatrixUtils.h"
#include "PixelMap.h" // <-- Includes freeglut
#include "Point.h"
#include "gltb.h"

#define MIDDLE_BUTTON_UP   3
#define MIDDLE_BUTTON_DOWN 4

#define WHITE_BACKGROUND 0
#define BLACK_BACKGROUND 1

#define ONE_GRADIENT   0
#define TWO_GRADIENT   1
#define THREE_GRADIENT 2
#define FIVE_GRADIENT  3

#define BLUE_CLOUD   2000
#define WHITE_CLOUD  2001
#define YELLOW_CLOUD 2002
#define BLACK_CLOUD  2003

using namespace std;

vector<Point> cloud;
unsigned int vao;
unsigned int vbo;

GLfloat width       = 800.0;
GLfloat height      = 600.0;
GLfloat nearPlane = 1.0;
GLfloat farPlane    = 5000.0;
GLfloat fieldOfView = 60.0;

GLfloat cameraX     = 600.0;
GLfloat cameraY     = -500.0;
GLfloat cameraZ     = -1500.0;
GLfloat cameraPosition3[3] = {cameraX, cameraY, cameraZ};
GLfloat cameraPosition4[4] = {cameraX, cameraY, cameraZ, 1.0};

GLfloat directionX = 655.21;
GLfloat directionY = -678.86;
GLfloat directionZ = -1770.98;

GLuint  gradientID           = ONE_GRADIENT;
GLuint	backgroundColorID    = BLACK_BACKGROUND;
GLfloat clearColorRedVal     = 0.0;
GLfloat clearColorGreenVal   = 0.0;
GLfloat clearColorBlueVal    = 0.0;

GLuint  numOfPoints			 = 0;
GLfloat pointSizeCoefficient = 1.0;
GLfloat pointSize;

GLfloat twoColorDivide       = 2.0;
GLfloat radiusCutOff         = 0.6;
GLfloat percentageOfdistance = 10000.0;
GLfloat percentageStep       = 10000.0;
GLuint  textureID			 = WHITE_CLOUD;
GLfloat zoom 				 = 0.0;
GLfloat size 				 = 20.0;
GLfloat zoomStep             = 25.0;
GLuint  shader_Program;

PixelMap pix[6];

// splits the string "target" into the "parts" array by the "delim" characters
// note: must know the size of "parts" you will need before calculation
//                 otherwise provide a bigger than necessary array for parts
void splitString(char* parts[], char* target, const char* delim) {
	char* tok;
	tok = strtok(target, delim);
	int i = 0;
	while(tok != NULL) {
		parts[i] = tok;
		tok = strtok(NULL, delim);
		i++;
	}
}

// sorting comparator
bool pointCompare(Point a, Point b) {
	return a.distance < b.distance;
}

//#############################################################################
//  OpenGL initialization
//#############################################################################

string readFileText(const char* path) {
	ifstream f(path, ios::binary);
	if (!f)
		throw runtime_error(string("Failed to open file: ") + path);

	stringstream ss;
	ss << f.rdbuf();
	return ss.str();
}

GLuint compileShader(GLenum type, const char* path) {
	string src;

	try {
		src = readFileText(path);
	}
	catch (const exception& e) {
		fprintf(stderr, "Shader error: %s", e.what());
		return 0;
	}
	if (src.empty()) {
		fprintf(stderr, "Shader error: File empty");
	}

	GLuint sh = glCreateShader(type);
	if (!sh) {
		fprintf(stderr, "glCreateShader failed for %s\n", path);
		return 0;
	}

	const char* cstr = src.c_str();
	GLint len = static_cast<GLint>(src.size());
	glShaderSource(sh, 1, &cstr, &len);   // pass explicit length
	glCompileShader(sh);

	GLint ok = GL_FALSE, logLen = 0;
	glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
	glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen > 1) {
		std::vector<GLchar> log(logLen);
		GLsizei written = 0;
		glGetShaderInfoLog(sh, logLen, &written, log.data());
		std::fprintf(stderr, "[%s] %.*s\n",
			(type == GL_VERTEX_SHADER ? "Vertex" :
				type == GL_FRAGMENT_SHADER ? "Fragment" : "Shader"),
			(int)written, log.data());
	}

	if (ok != GL_TRUE) {
		std::fprintf(stderr, "Compile failed: %s\n", path);
		glDeleteShader(sh);
		return 0;
	}
	return sh;
}

//#############################################################################
//  load a shader file into opengl
//  This code is from the openGL tutorial:
//  http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
//#############################################################################
GLuint loadShader(const char * fragment_shader, const char * vertex_shader)
{
	// Sanity: ensure loader/ctx are alive
	const GLubyte* ver = glGetString(GL_VERSION);
	if (!ver) {
		std::fprintf(stderr, "No GL context (glGetString(GL_VERSION) returned null)\n");
		return 0;
	}
	std::printf("GL version: %s\n", ver);

	GLuint vs = compileShader(GL_VERTEX_SHADER, vertex_shader);
	if (!vs) return 0;
	GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragment_shader);
	if (!fs) { glDeleteShader(vs); return 0; }

	GLuint prog = glCreateProgram();
	if (!prog) {
		std::fprintf(stderr, "glCreateProgram failed\n");
		glDeleteShader(vs); glDeleteShader(fs);
		return 0;
	}

	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	glLinkProgram(prog);

	GLint ok = GL_FALSE, logLen = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &ok);
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLen);

	if (logLen > 1) {
		std::vector<GLchar> log(logLen);
		GLsizei written = 0;
		glGetProgramInfoLog(prog, logLen, &written, log.data());
		std::fprintf(stderr, "[Link] %.*s\n", (int)written, log.data());
	}

	// shaders can be detached/deleted after link
	glDetachShader(prog, vs);
	glDetachShader(prog, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	if (ok != GL_TRUE) {
		std::fprintf(stderr, "Program link failed\n");
		glDeleteProgram(prog);
		return 0;
	}

	return prog;
}

void loadPoints() {
	fstream in;
	float x, y, z, mag;
	
	in.open("Point-Data.csv", fstream::in);
	if(!in) {
		printf("csv File failed to open!\n");
		exit(1);
	}
	printf("Opened File!\n");
		
	char* data[5];
	string line;
	cloud = vector<Point>();
		
	while(getline(in, line)) {
		if(line[0] == '#') {
			continue;
		}
		splitString(data, (char*)line.c_str(), ",");
		x = atof(data[1]);
		y = atof(data[2]);
		z = atof(data[3]);
		mag = atof(data[4]);
			
		cloud.push_back(Point(x, y, z, mag));
	}
	in.close();
}

// Call once before glut loop
void init() {

	glewExperimental = GL_TRUE;           // needed on many drivers/profiles
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "GLEW init failed\n");
		return;
	}

	loadPoints();
	
	numOfPoints = cloud.size();
	printf("%d points\n", numOfPoints);
	
	glEnable(GL_DEPTH_TEST);
	
	gltbInit(GLUT_LEFT_BUTTON);
	
	pix[0].readBMPFile("textures/WhiteCloud.bmp");
	pix[0].setTexture(2000);
	
	shader_Program = loadShader("fragmentShader.fp", "vertexShader.vp");
	
	gluPerspective(fieldOfView, height/width, nearPlane, farPlane);
}

//#############################################################################
//  Point cloud initialization
//#############################################################################

// renders points based on gradient divide
void renderPoints() {
	int secondStep, thirdStep, fourthStep;
	int halfOfPoints, thirdOfPoints, fifthOfPoints;

	if (cloud.empty())
	{
		fprintf(stderr, "Error: No points to render!\n");
		return;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * cloud.size(), cloud.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, x));
	glBindVertexArray(vao);

	int step = numOfPoints / (gradientID + 1);
	int textureID = 2000;

	if (gradientID == 0)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawArrays(GL_POINTS, 0, numOfPoints);
	}
	else
	{
		for (int i = 0; i < gradientID + 1; i++)
		{
			int minStep = step * i;
			glBindTexture(GL_TEXTURE_2D, textureID);
			glDrawArrays(GL_POINTS, minStep, minStep + step);
			textureID++;
		}
	}
}

void RenderScene(void) {
	glClearColor(clearColorRedVal, clearColorGreenVal, clearColorBlueVal, 1.0);
		
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(0.0, 0.0, zoom);
	
	gltbMatrix();

	gluLookAt(cameraX, cameraY, cameraZ, directionX, directionY, directionZ, 0.0, 1.0, 0.0);
	// update and sort points
	
	GLfloat modelview[16], finalVertex[4], currentVertex[4];
	GLfloat newDistance;
	
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	
	// update the distances
	for (int i = 0; i < (int)numOfPoints; i++) {
		currentVertex[0] = cloud[i].x;
		currentVertex[1] = cloud[i].y;
		currentVertex[2] = cloud[i].z;
		currentVertex[3] = 1.0;
		MatrixUtils::multiply4Matrix_4Vector(modelview, currentVertex, finalVertex);
		newDistance = VectorUtils::length(finalVertex);
		cloud[i].setDistance(newDistance);
	}
	
	sort(cloud.begin(), cloud.end(), pointCompare);
	
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_POINT_SPRITE);
	
	glEnable(GL_TEXTURE_2D);
	
	//glEnable(GL_PROGRAM_POINT_SIZE);
	
	GLuint cLoc       = glGetUniformLocation(shader_Program, "coefficient");
	GLuint percentLoc = glGetUniformLocation(shader_Program, "percentFactor");
	GLuint textLoc    = glGetUniformLocation(shader_Program, "sampler");
	GLuint radLoc     = glGetUniformLocation(shader_Program, "radii");
	
	glUseProgram(shader_Program);

	glUniform1f(radLoc, radiusCutOff);
	glUniform1f(percentLoc, percentageOfdistance);
	glUniform1i(textLoc, 0);
	glUniform1f(cLoc, pointSizeCoefficient);

	renderPoints();
	
	glDisable(GL_TEXTURE_2D);

	// Flush drawing commands
	glutSwapBuffers();
	glutPostRedisplay(); // Redraw
}

//#############################################################################
//  Menu functions
//#############################################################################

void ChangeSize(int width, int height) {
    gltbReshape(width, height);
		
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fieldOfView, (GLfloat)height / (GLfloat)width, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.0);
}

void menuFunc(int choice) {
	switch (choice) {
		case 1:
			gradientID = ONE_GRADIENT;
			pix[0].readBMPFile("textures/WhiteCloud.bmp");
			pix[0].setTexture(2000);
			break;
		case 2:
			gradientID = ONE_GRADIENT;
			pix[0].readBMPFile("textures/BlackCloud.bmp");
			pix[0].setTexture(2000);
			break;
		case 3:
			gradientID = ONE_GRADIENT;
			if (backgroundColorID == WHITE_BACKGROUND) {
				pix[0].readBMPFile("textures/Blue1OnWhite.bmp");
				pix[0].setTexture(2000);
			}
			else {
				pix[0].readBMPFile("textures/Blue1OnBlack.bmp");
				pix[0].setTexture(2000);
			}
			break;
		case 4:
			gradientID = ONE_GRADIENT;
			if (backgroundColorID == WHITE_BACKGROUND) {
				pix[0].readBMPFile("textures/Yellow1OnWhite.bmp");
				pix[0].setTexture(2000);
			}
			else {
				pix[0].readBMPFile("textures/Yellow1OnBlack.bmp");
				pix[0].setTexture(2000);
			}
			break;
		case 10:
			gradientID = TWO_GRADIENT;
			if (backgroundColorID == WHITE_BACKGROUND) {
				pix[0].readBMPFile("textures/Yellow1OnWhite.bmp");
				pix[0].setTexture(2000);
				pix[1].readBMPFile("textures/Blue1OnWhite.bmp");
				pix[1].setTexture(2001);
			}
			else {
				pix[0].readBMPFile("textures/Yellow1OnBlack.bmp");
				pix[0].setTexture(2000);
				pix[1].readBMPFile("textures/Blue1OnBlack.bmp");
				pix[1].setTexture(2001);
			}
			break;
		case 20:
			gradientID = THREE_GRADIENT;
			if (backgroundColorID == WHITE_BACKGROUND) {
				pix[0].readBMPFile("textures/Yellow1OnWhite.bmp");
				pix[0].setTexture(2000);
				pix[1].readBMPFile("textures/YellowBlueMiddleOnWhite.bmp");
				pix[1].setTexture(2001);
				pix[2].readBMPFile("textures/Blue1OnWhite.bmp");
				pix[2].setTexture(2002);
			}
			else {
				pix[0].readBMPFile("textures/Yellow1OnBlack.bmp");
				pix[0].setTexture(2000);
				pix[1].readBMPFile("textures/YellowBlueMiddleOnBlack.bmp");
				pix[1].setTexture(2001);
				pix[2].readBMPFile("textures/Blue1OnBlack.bmp");
				pix[2].setTexture(2002);
			}
			break;
		case 30:
			gradientID = FIVE_GRADIENT;
			if (backgroundColorID == WHITE_BACKGROUND) {
				pix[0].readBMPFile("textures/Yellow1OnWhite.bmp");
				pix[0].setTexture(2000);
				pix[1].readBMPFile("textures/Yellow2OnWhite.bmp");
				pix[1].setTexture(2001);
				pix[2].readBMPFile("textures/Yellow3OnWhite.bmp");
				pix[2].setTexture(2002);
				pix[3].readBMPFile("textures/Blue3OnWhite.bmp");
				pix[3].setTexture(2003);
				pix[4].readBMPFile("textures/Blue2OnWhite.bmp");
				pix[4].setTexture(2004);
				pix[5].readBMPFile("textures/Blue1OnWhite.bmp");
				pix[5].setTexture(2005);
			}
			else {
				pix[0].readBMPFile("textures/Yellow1OnBlack.bmp");
				pix[0].setTexture(2000);
				pix[1].readBMPFile("textures/Yellow2OnBlack.bmp");
				pix[1].setTexture(2001);
				pix[2].readBMPFile("textures/Yellow3OnBlack.bmp");
				pix[2].setTexture(2002);
				pix[3].readBMPFile("textures/Blue3OnBlack.bmp");
				pix[3].setTexture(2003);
				pix[4].readBMPFile("textures/Blue2OnBlack.bmp");
				pix[4].setTexture(2004);
				pix[5].readBMPFile("textures/Blue1OnBlack.bmp");
				pix[5].setTexture(2005);
			}
			break;
		default:
			break;
	}
}

void keyboardFunc(unsigned char key, int x, int y) {
	switch(key) {
		case 'h':
			printf("Help -> Instructions:\n");
			printf("1 -------- White Background\n");
			printf("2 -------- Black Background\n");
			printf("a -------- decreases division of distance\n");
			printf("s -------- increases division of distance\n");
			printf("x -------- decreases size coefficient\n");
			printf("z -------- increases size coefficient\n");
			printf("e -------- decreases radius cutoff\n");
			printf("r -------- increases radius cutoff\n");
			printf("q -------- increase color divide distance\n");
			printf("w -------- decrease color divide distance\n");
			printf("RMB ------ menu");
			break;
		case '1':
			backgroundColorID = WHITE_BACKGROUND;
			clearColorRedVal = 1.0;
			clearColorGreenVal = 1.0;
			clearColorBlueVal = 1.0;
			break;
		case '2':
			backgroundColorID = BLACK_BACKGROUND;
			clearColorRedVal = 0.0;
			clearColorGreenVal = 0.0;
			clearColorBlueVal = 0.0;
			break;
		case 'a':
			percentageOfdistance -= percentageStep;
			break;
		case 's':
			percentageOfdistance += percentageStep;
			break;
		case 'z':
			pointSizeCoefficient += 0.1;
			if(pointSizeCoefficient <= 0.1)
				pointSizeCoefficient = 0.1;
			break;
		case 'x':
			pointSizeCoefficient -= 0.1;
			if(pointSizeCoefficient <= 0.1)
				pointSizeCoefficient = 0.1;
			break;
		case 'r':
			radiusCutOff += 0.05;
			break;
		case 'e':
			radiusCutOff -= 0.05;
			break;
		case 'q':
			twoColorDivide += 0.05;
			break;
		case 'w':
			twoColorDivide -= 0.05;
			if(twoColorDivide <= 0.1)
				twoColorDivide = 0.1;
			break;
	}
}
	
void mouseFunc(int button, int state, int x, int y) {
	
	gltbMouse(button, state, x, y);
	
	if(button == MIDDLE_BUTTON_UP) {

		zoom += zoomStep;
	}
	else if(button == MIDDLE_BUTTON_DOWN) {

		zoom -= zoomStep;
	}
}

int main(int argc, char* argv[]) {
	
	int oneGradientMenu, twoGradientMenu, threeGradientMenu, fiveGradientMenu;
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Point-Based Renderer");
	glutKeyboardFunc(keyboardFunc);
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(gltbMotion);
	
	oneGradientMenu = glutCreateMenu(menuFunc);
	glutAddMenuEntry("White", 1);
	glutAddMenuEntry("Black", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Yellow", 4);
	
	twoGradientMenu = glutCreateMenu(menuFunc);
	glutAddMenuEntry("Yellow/Blue", 10);
	
	threeGradientMenu = glutCreateMenu(menuFunc);
	glutAddMenuEntry("Yellow/Blue", 20);
	
	fiveGradientMenu = glutCreateMenu(menuFunc);
	glutAddMenuEntry("Yellow/Blue", 30);
	
	glutCreateMenu(menuFunc);
	glutAddMenuEntry("Available scenes:", 0);
	glutAddSubMenu("1 Color", oneGradientMenu);
	glutAddSubMenu("2 Color Gradient", twoGradientMenu);
	glutAddSubMenu("3 Color Gradient", threeGradientMenu);
	glutAddSubMenu("5 Color Gradient", fiveGradientMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
	return 0;
}
