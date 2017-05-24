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
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#endif

#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <math.h>
#include "Vector.h"
#include "PixelMap.h"
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

GLfloat width       = 800.0;
GLfloat height      = 600.0;
GLfloat nearPlane   = 1.0;
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
GLuint  backgroundColorID    = BLACK_BACKGROUND;
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

PixelMap pix[5];

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

bool pointCompare(Point a, Point b) {
	return a.getDistance() < b.getDistance();
}

//#############################################################################
//  load a shader file into opengl
//  This code is from the openGL tutorial:
//  http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
//#############################################################################
GLuint loadShader(const char * fragment_shader, const char * vertex_shader)
{
	
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	
	string Line;
	
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_shader, ios::in);
	if(VertexShaderStream.is_open()) {
		Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_shader, ios::in);
	if(FragmentShaderStream.is_open()) {
		Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	
	GLint Result = GL_FALSE;
	int InfoLogLength;
	
	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_shader);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShaderID);
	
	// Check Vertex Shader
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(vertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	
	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_shader);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShaderID);
	
	// Check Fragment Shader
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(fragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	
	// link program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vertexShaderID);
	glAttachShader(ProgramID, fragmentShaderID);
	glLinkProgram(ProgramID);
	
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	
	return ProgramID;
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

// renders all points in sequence
void renderGroupOfPoints(int begin, int end) {
	glBegin(GL_POINTS);
	for (int i = begin; i < end; i++) {
		glVertex3f(cloud[i].getX(), cloud[i].getY(), cloud[i].getZ());
	}
	glEnd();
}

// renders points based on gradient divide
void renderPoints() {
	int secondStep, thirdStep, fourthStep;
	int halfOfPoints, thirdOfPoints, fifthOfPoints;
	
	switch (gradientID) {
		case 0:
			glBindTexture(GL_TEXTURE_2D, 2000);
			renderGroupOfPoints(0, (int)numOfPoints);
			break;
		case 1:
			halfOfPoints = (int)numOfPoints / twoColorDivide;
			
			glBindTexture(GL_TEXTURE_2D, 2000);
			renderGroupOfPoints(0, halfOfPoints);
			
			glBindTexture(GL_TEXTURE_2D, 2001);
			renderGroupOfPoints(halfOfPoints + 1, (int)numOfPoints);
			break;
		case 2:
			thirdOfPoints = (int)numOfPoints / (twoColorDivide + 1);
			secondStep = thirdOfPoints * 2;
			
			glBindTexture(GL_TEXTURE_2D, 2000);
			renderGroupOfPoints(0, thirdOfPoints);
			
			glBindTexture(GL_TEXTURE_2D, 2001);
			renderGroupOfPoints(thirdOfPoints + 1, secondStep);
			
			glBindTexture(GL_TEXTURE_2D, 2002);
			renderGroupOfPoints(secondStep + 1, (int)numOfPoints);
			break;
		case 3:
			fifthOfPoints = (int)numOfPoints / (twoColorDivide + 3);
			secondStep = fifthOfPoints * 2;
			thirdStep = fifthOfPoints * 3;
			fourthStep = fifthOfPoints * 4;
			
			glBindTexture(GL_TEXTURE_2D, 2000);
			renderGroupOfPoints(0, fifthOfPoints);
			
			glBindTexture(GL_TEXTURE_2D, 2001);
			renderGroupOfPoints(fifthOfPoints + 1, secondStep);
			
			glBindTexture(GL_TEXTURE_2D, 2002);
			renderGroupOfPoints(secondStep + 1, thirdStep);
			
			glBindTexture(GL_TEXTURE_2D, 2003);
			renderGroupOfPoints(thirdStep + 1, fourthStep);
			
			glBindTexture(GL_TEXTURE_2D, 2004);
			renderGroupOfPoints(fourthStep + 1, (int)numOfPoints);
			break;
		default:
			break;
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
		currentVertex[0] = cloud[i].getX();
		currentVertex[1] = cloud[i].getY();
		currentVertex[2] = cloud[i].getZ();
		currentVertex[3] = 1.0;
		Vector::multiply4Matrix_4Vector(modelview, currentVertex, finalVertex);
		newDistance = Vector::length(finalVertex);
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
