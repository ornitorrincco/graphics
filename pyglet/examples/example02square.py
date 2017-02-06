from pyglet.gl import *

class Triangle:
    def __init__(self):
        self.vertices = pyglet.graphics.vertex_list(3, ('v3f',[-0.5,-0.5,0.0, 0.5,-0.5,0.0, 0.0,0.5,0.0 ]),
                                                       ('c3B',[100, 200, 220, 200, 110, 100, 100, 250, 100]))
class Quad:
    def __init__(self):
        self.vertices = pyglet.graphics.vertex_list_indexed(4, [0,1,2, 2,3,0],
                                                            ('v3f',[-0.5,-0.5,0.0, 0.5,-0.5,0.0, 0.5,0.5,0.0, -0.5,0.5,0.0]),
                                                            ('c3f',[1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0, 1.0,1.0,1.0]))
                                                                
class Quad2:
    def __init__(self):
        self.indices = [0,1,2, 2,3,0]
        self.vertex = [-0.5,-0.5,0.0, 0.5,-0.5,0.0, 0.5,0.5,0.0, -0.5,0.5,0.0]
        self.color = [1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0, 1.0,1.0,1.0]
        
        self.vertices = pyglet.graphics.vertex_list_indexed(4, self.indices, ('v3f',self.vertex), ('c3f',self.color))
        
class MyWindow(pyglet.window.Window):
    def __init__(self, *args, **kwargs):
        super().__init__( *args, **kwargs)
        self.set_minimum_size(100,100)
        glClearColor(0.2, 0.3, 0.2, 1.0)

        self.triangle = Triangle()
        self.quad2 = Quad2()
        
        
    def on_draw(self):
        self.clear()
        #self.triangle.vertices.draw(GL_TRIANGLES)
        self.quad2.vertices.draw(GL_TRIANGLES)

    def on_resize(self, width, height):
        glViewport(0, 0, width, height)

        
if __name__== "__main__":
    window = MyWindow(500, 500, "el mejor cuadrado", resizable = True)
    pyglet.app.run()
 
