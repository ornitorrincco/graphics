/*
Example 02  openGL-GLUT
This prograwim display a black window, and can change of color using the function
glutKeyboardFunc(key) where any case use a glClearColor() function to change the background function .
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>

static void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glClearColor(0,0,0,1);

    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;
			
		case 'w':
			glClearColor(1,0,0,1);
			break;
		case 'e':
			glClearColor(0,1,0,1);
			break;
		case 'r':
			glClearColor(0,0,1,1);
			break;
		case 't':
			glClearColor(0,1,1,1);
			break;
		case 'y':
			glClearColor(1,1,0,1);
			break;
		case 'u':
			glClearColor(0.5,0.2,0.7,1);
			break;
    }

    glutPostRedisplay();
}


/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(640,480);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("OpenGL example 02 Window creator ");

    glutDisplayFunc(display);
    glutKeyboardFunc(key);

    glutMainLoop();

    return EXIT_SUCCESS;
}
