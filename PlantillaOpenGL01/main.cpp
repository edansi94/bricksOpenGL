/*
	Descripci�n:
		PROYECTO II - OPENGL.

		Archivo principal donde se desarrollar� el juego Bricks 2D
		utilizando la librer�a OpenGL.

	Alumno:
		Edward Fern�ndez - Carnet: 10-11121.
		Mar�a Lourdes    - Carnet: 10-10264.
*/

/*
	TO DO:
		+ Hacer que los bloques tengan las equinas no tan recta.
		+ Ubicar la plataforma (usar traslaciones).
		+ Hacer que se mueva con las flechas.
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
	vector<float> puntos[4];   // Arreglo de vectores que almacenar� los puntos del bloque.
	bool esEspecial;		   // Indica si el bloque al ser destruido crear� un bonus.
	bool estaActivo;	       // Indica si el bloque no fue destruido. 
	bool esJugador;            // Indica si el bloque es la plataforma del jugador.
} Bloque; // Estructura que representar� a los diferentes 
								// bloques que se motrar�n en pantalla.

/*------ Variables Globales ------*/
float movimientoX = 0.0;			// Indicar� cuanto y hacia donde se mover� la plataforma.

Bloque prueba;
Bloque plataforma;				// Plataforma que utilizar� el jugador para jugar.
Bloque listaBloques[7][5];      // Conjunto de todos los bloques "enemigos" que se
							    // mostrar�n en pantalla.

/*---------- Funciones ----------*/
float lerp(float posInicial, float posFinal, float deltaTime);
void teclaPresionada(unsigned char tecla, int x, int y);
void generarPlataforma(void);
void compilarJuego(void);
void ejecutarJuego(void);

/*
	Descripci�n:
		Permite realizar la interpolaci�n lineal entre dos escalares dados en base al
		tiempo de ejecuci�n del programa.
*/
float lerp(float posInicial, float posFinal, float deltaTime)
{
	return( posInicial + deltaTime*(posFinal - posInicial));
}

/*
	Descripci�n:
		Permite capturar que tecla es presionada y de esta forma
		se podr� encadenar la acci�n correspondiente.
*/
void teclaPresionada(unsigned char tecla, int x, int y)
{
	switch (tecla)
	{
		case 'a':
		case 'A':
			movimientoX = lerp(movimientoX, movimientoX - 1, 0.5);
			break;
		case 'd':
		case 'D':
			movimientoX = lerp(movimientoX, movimientoX + 1, 0.5);
			break;
	}

	// Para realizar render al momento que se encadene la acci�n.
	glutPostRedisplay();
}

/*
	Descripci�n:
		Permite almacenar los comandos necesarios para la generaci�n
		de la plataforma.
*/


/*
	Descripci�n:
		Permite almacenar los comandos necesarios para la generaci�n
		de la plataforma.
*/
void generarPlataforma(void)
{
	// Se definen los puntos que generan a la plataforma.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	plataforma.puntos[0].push_back(0.0); // Posici�n x del primer punto. 
	plataforma.puntos[0].push_back(0.0); // Posici�n y del primer punto.
	plataforma.puntos[1].push_back(0.0); // Posici�n x del segundo punto. 
	plataforma.puntos[1].push_back(0.5); // Posici�n y del segundo punto.
	plataforma.puntos[2].push_back(2.0); // Posici�n x del tercer punto. 
	plataforma.puntos[2].push_back(0.5); // Posici�n y del tercer punto.
	plataforma.puntos[3].push_back(2.0); // Posici�n x del cuarto punto. 
	plataforma.puntos[3].push_back(0.0); // Posici�n y del cuarto punto.
	
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
	Descripci�n:
		Permite compilar los comandos que se utilizar�n para crear el 
		juego.
*/
void compilarJuego(void)
{
	generarPlataforma();
}

/*
	Descripci�n:
		Permite ejecutar los comandos que se utilizar�n para crear el 
		juego.
*/
void ejecutarJuego(void)
{
	glTranslatef(movimientoX, 0, 0);
	glCallList(glPlataforma);  // Se muestra la plataforma.
}

/*
	Descripci�n:
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

	// Matriz de proyecci�n.
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
	
	// Permite saber que tecla es seleccionada.
	glutKeyboardFunc(teclaPresionada);

	//ejesCoordenada(2.0);
	compilarJuego();
	ejecutarJuego();

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