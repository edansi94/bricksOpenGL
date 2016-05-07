/*
	Descripción:
		PROYECTO II - OPENGL.

		Archivo principal donde se desarrollará el juego Bricks 2D
		utilizando la librería OpenGL.

	Alumno:
		Edward Fernández - Carnet: 10-11121.
		María Lourdes    - Carnet: 
*/


#include <iostream>
#include <GL\glew.h>
#include <GL\freeglut.h>

using namespace std;

/*
	Descripción:
		Permite generar un ejes de coordenadas.
*/
void ejesCoordenada(float w) {
	
	glLineWidth(w);
	glBegin(GL_LINES);
		glColor3f(1.0,0.0,0.0);
		glVertex2f(0,10);
		glVertex2f(0,-10);
		glColor3f(0.0,0.0,1.0);
		glVertex2f(10,0);
		glVertex2f(-10,0);
	glEnd();

	glLineWidth(w-1.0);
	int i;
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINES);
		for(i = -10; i <=10; i++){
			if (i!=0) {		
				if ((i%2)==0){	
					glVertex2f(i,0.4);
					glVertex2f(i,-0.4);

					glVertex2f(0.4,i);
					glVertex2f(-0.4,i);
				}else{
					glVertex2f(i,0.2);
					glVertex2f(i,-0.2);

					glVertex2f(0.2,i);
					glVertex2f(-0.2,i);

				}
			}
		}
		
	glEnd();

	glLineWidth(1.0);
}

void changeViewport(int w, int h) {
	float aspectradio;

	glViewport(0,0,w,h); // Cambia el aspecto de la pantalla.

	// Matriz de proyección.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();			// Identidad

	aspectradio = (float) w / (float) h;

	if (w<=h)
		glOrtho(-10,10,-10/aspectradio, 10/aspectradio, 1.0, -1.0);
	else
		glOrtho(-10*aspectradio,10*aspectradio,-10, 10, 1.0, -1.0);

}

void render(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//ejesCoordenada(2.0);

	glutSwapBuffers();
}

int main (int argc, char** argv) {

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitWindowSize(800,600);

	glutCreateWindow("Opengl");

	glutReshapeFunc(changeViewport);
	glutDisplayFunc(render);

	
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}
	

	glutMainLoop();
	return 0;

}