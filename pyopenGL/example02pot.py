#This program display a pot 

from OpenGL.GLUT import *
from OpenGL.GL import *
from OpenGL.GLU import *
def draw():
    glClear(GL_COLOR_BUFFER_BIT)
    glutWireTeapot(0.5)
    glFlush()
    
glutInit(sys.argv)
glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
glutInitWindowSize(350, 350)
glutInitWindowPosition(100, 100)
glutCreateWindow("My pot")
glutDisplayFunc(draw)
glutMainLoop()
