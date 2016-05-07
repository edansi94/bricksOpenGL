/*
	Descripción:
		PROYECTO II - OPENGL.

		Archivo principal donde se desarrollará el juego Bricks 2D
		utilizando la librería OpenGL.

	Alumno:
		Edward Fernández - Carnet: 10-11121.
		María Lourdes    - Carnet: 10-10264.
*/

/*
	TO DO:
		+ Hacer que los bloques tengan las equinas no tan recta.
		+ Ubicar la plataforma (usar traslaciones).
*/

#include <iostream>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <vector>

using namespace std;

/*--------- Definiciones --------*/
// Indices de la lista.
#define glPlataforma 1        

/*--------- Estructuras .--------*/
typedef struct
{
	vector<float> puntos[4];   // Arreglo de vectores que almacenará los puntos del bloque.
	bool esEspecial;		   // Indica si el bloque al ser destruido creará un bonus.
	bool estaActivo;	       // Indica si el bloque no fue destruido. 
	bool esJugador;            // Indica si el bloque es la plataforma del jugador.
} Bloque; // Estructura que representará a los diferentes 
								// bloques que se motrarán en pantalla.

/*------ Variables Globales ------*/
Bloque prueba;
Bloque plataforma;				// Plataforma que utilizará el jugador para jugar.
Bloque listaBloques[7][5];      // Conjunto de todos los bloques "enemigos" que se
							    // mostrarán en pantalla.

/*---------- Funciones ----------*/
void generarPlataforma(void);
void compilarJuego(void);
void inicializarJuego(void);

/*
	Descripción:
		Permite almacenar los comandos necesarios para la generación
		de la plataforma.
*/
void generarPlataforma(void)
{
	// Se definen los puntos que generan a la plataforma.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	plataforma.puntos[0].push_back(0.0); // Posición x del primer punto. 
	plataforma.puntos[0].push_back(0.0); // Posición y del primer punto.
	plataforma.puntos[1].push_back(0.0); // Posición x del segundo punto. 
	plataforma.puntos[1].push_back(0.5); // Posición y del segundo punto.
	plataforma.puntos[2].push_back(2.0); // Posición x del tercer punto. 
	plataforma.puntos[2].push_back(0.5); // Posición y del tercer punto.
	plataforma.puntos[3].push_back(2.0); // Posición x del cuarto punto. 
	plataforma.puntos[3].push_back(0.0); // Posición y del cuarto punto.
	
	plataforma.esEspecial = false;
	plataforma.estaActivo = true;
	plataforma.esJugador  = true;

	// Se construye la plataforma.
	glNewList(glPlataforma, GL_COMPILE);
		glBegin(GL_LINE_LOOP);
			glVertex2f(plataforma.puntos[0][0], plataforma.puntos[0][1]);
			glVertex2f(plataforma.puntos[1][0], plataforma.puntos[1][1]);
			glVertex2f(plataforma.puntos[2][0], plataforma.puntos[2][1]);
			glVertex2f(plataforma.puntos[3][0], plataforma.puntos[3][1]);
		glEnd();
	glEndList();
}


/*
	Descripción:
		Permite compilar los comandos que se utilizarán para crear el 
		juego.
*/
void compilarJuego(void)
{
	generarPlataforma();
}

/*
	Descripción:
		Permite ejecutar los comandos que se utilizarán para crear el 
		juego.
*/
void inicializarJuego(void)
{
	glCallList(glPlataforma);  // Se muestra la plataforma.
}

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
	compilarJuego();
	inicializarJuego();

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