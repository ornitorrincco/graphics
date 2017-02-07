from OpenGL.GLUT import *
from OpenGL.GL import *
from OpenGL.GLU import *


def draw():
    glClear(GL_COLOR_BUFFER_BIT)
    glFlush()
    
glutInit(sys.argv)
glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
glutInitWindowSize(720, 480)
glutInitWindowPosition(300, 200)
glutCreateWindow("Example01openGL")
glutDisplayFunc(draw)
glutMainLoop()
