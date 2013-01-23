#include <GL/glut.h>
#include "stdafx.h"
#include "Helpers.h"
#include "OBJImport.h"

OBJImport meshObj;

int screen_width = 800;
int screen_height = 600;

float SHRINK_FACTOR = 5;

int updatingAcceleration = 0;

float angleX = 0;
float angleY = 0;

int previousMouseX = 0;
int previousMouseY = 0;

float accelerationX = 0;
float accelerationY = 0;
char MODEL_TO_LOAD[16] = "vache.obj";
string fpsString = " ";

unsigned int fps_time;
unsigned int fps_frames = 0;
float fps = 0;

void draw_string(float x, float y, void *font, char *string)
{
    char *c;
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos3f(x, y, 0);
    for (c = string; *c; c++)
	{
        glutBitmapCharacter(font, *c);
	}
}

void display()
{
    int i, j;
    //char buf[16] = "stuff";

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0f * screen_width / screen_height, 0.1, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 2.0, 0.0,
              0.0, 0.0, -4.0,
              0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, -5.0);
	glRotatef(angleY, 1.0, 0.0, 0.0);
	glRotatef(angleX, 0.0, 1.0, 0.0);
//	printf("meshSize: %i\n",meshObj.meshFaces.size());
    glBegin(GL_TRIANGLES);
	int debugCount=0;
	while(1);
	for (i = 0; i < meshObj.meshFaces.size(); i++)
     {
		 debugCount++;
         glColor3f((GLfloat)fmod(i*i,1),(GLfloat)fmod(i*i,2),(GLfloat)fmod(i*i,3));
         
		 glVertex3f((GLfloat)meshObj.meshVertices.at((int)meshObj.meshFaces.at(i).x).x/SHRINK_FACTOR,
					(GLfloat)meshObj.meshVertices.at((int)meshObj.meshFaces.at(i).x).y/SHRINK_FACTOR,
					(GLfloat)meshObj.meshVertices.at((int)meshObj.meshFaces.at(i).x).z/SHRINK_FACTOR);

		 glVertex3f((GLfloat)meshObj.meshVertices.at((int)meshObj.meshFaces.at(i).y).x/SHRINK_FACTOR,
                    (GLfloat)meshObj.meshVertices.at((int)meshObj.meshFaces.at(i).y).y/SHRINK_FACTOR,
                    (GLfloat)meshObj.meshVertices.at((int)meshObj.meshFaces.at(i).y).z/SHRINK_FACTOR);

		 glVertex3f((GLfloat)meshObj.meshVertices.at((int)meshObj.meshFaces.at(i).z).x/SHRINK_FACTOR,
					(GLfloat)meshObj.meshVertices.at((int)meshObj.meshFaces.at(i).z).y/SHRINK_FACTOR,
					(GLfloat)meshObj.meshVertices.at((int)meshObj.meshFaces.at(i).z).z/SHRINK_FACTOR);
    }
    glEnd();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

   // snprintf(buf, 16, "FPS: %.1f", fps);
    draw_string(-0.9, -0.9, GLUT_BITMAP_HELVETICA_18, MODEL_TO_LOAD);
    draw_string(0.8, 0.9, GLUT_BITMAP_HELVETICA_18, (char*)fpsString.data());
    draw_string(0.8, -0.9, GLUT_BITMAP_HELVETICA_18, (char*)DecimalToString(SHRINK_FACTOR, 4).data());

    
    glutSwapBuffers();
}

void reshape(int width, int height)
{
    screen_width = width;
    screen_height = height;
    glViewport(0, 0, screen_width, screen_height);
}

void GetMouseAcceleration(int x, int y)
{
    int d_time = glutGet(GLUT_ELAPSED_TIME) - fps_time;
	//float d_time = 0.1f;
    if(x > previousMouseX)
	{
		accelerationX += d_time* 0.01f;
	}
	else if(x < previousMouseX)
	{
		accelerationX -= d_time* 0.01f;
	}

	if(y > previousMouseY)
	{
		accelerationY+= d_time* 0.01f;
	}
	else if (y < previousMouseY)
	{
		accelerationY -= d_time* 0.01f;
	}
	
	if(fabs(accelerationX) > 0.1f)
	{
		int sign = accelerationX < 0 ? -1 : 1;
		accelerationX = sign * 0.1f;
	}
	if(fabs(accelerationY) > 0.11f)
	{
		int sign = accelerationY < 0 ? -1 : 1;
		accelerationY = sign * 0.1f;
	}
    
	updatingAcceleration = 1;

	previousMouseX = x;
	previousMouseY = y;
}

void UpdateMouseAcceleration()
{
    glutPassiveMotionFunc( GetMouseAcceleration );
	if(!updatingAcceleration)
	{
        int d_time = glutGet(GLUT_ELAPSED_TIME) - fps_time;

		if(accelerationX != 0)
		{
			if(accelerationX < 0)
			{
				accelerationX += d_time*0.00001f;
			}
			else
			{
				accelerationX -=d_time*0.00001f;
			}
		}

		if(accelerationY != 0)
		{
			if(accelerationY < 0)
			{
				accelerationY += d_time*0.00001f;
			}
			else
			{
				accelerationY -=d_time*0.00001f;
			}
		}
	}

	updatingAcceleration = 0;

	//printf("%f, %f Mouse \n",accelerationX, accelerationY);

}

void UpdateKeyboardInput(unsigned char key,int mouseX,int mouseY)
{
    if(key == 'w')
    {
        SHRINK_FACTOR -= 0.1f;
    }
    if(key == 's')
    {
        SHRINK_FACTOR += 0.1f;
    }
    if(key == 'a')
    {
        accelerationX -= 0.1f;
    }
    if(key == 'd')
    {
        accelerationX += 0.1f;
    }
}

void idle()
{
    int d_time;
	UpdateMouseAcceleration();
    glutKeyboardFunc( UpdateKeyboardInput );
    
	angleX +=  accelerationX;
    angleY +=  accelerationY;

    fps_frames++;
    d_time = glutGet(GLUT_ELAPSED_TIME) - fps_time;
    if (d_time > 1000.0)
    {
        fps = fps_frames / (d_time / 1000.0);
        fps_time = glutGet(GLUT_ELAPSED_TIME);
        fps_frames = 0;
        fpsString.replace(0, 20,"Fps: " + DecimalToString((int)fps,4));
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{

    char* loadName = MODEL_TO_LOAD;
    
	meshObj = OBJImport();
	meshObj.ImportMesh(loadName);
	glutInit(&argc, argv);
    glutInitWindowSize(screen_width, screen_height);
    glutCreateWindow("Renderer");

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0, 0.0, 0.0, 0.0);


	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
		
	fps_time = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();

    return 0;
}

