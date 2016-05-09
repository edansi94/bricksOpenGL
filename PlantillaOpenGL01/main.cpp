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
		+ Hacer que se mueva con las flechas.
*/

#include <ctime>          // Librer�a para importar la variable time.
#include <vector>         // Libreria que me permite crear vectores.
#include <iostream>
#include <GL\glew.h>
#include <GL\freeglut.h>

using namespace std;

/*--------- Definiciones --------*/
// Indices de la lista que almacenar� los comandos para la creaci�n de los elementos del juego.
#define glPlataforma 1        
#define glParedLat   2
#define glParedSup   3

/*--------- Estructuras .--------*/
typedef struct
{
	int  vida;                 // Cuantos golpes puede recibir. 

	bool esEspecial;		   // Indica si el bloque debe golpearse 2 veces para ser destruido.
	bool tieneBonus;           // Indica si al ser destruido el bloque generar� un bonus.
	bool estaActivo;	       // Indica si el bloque no fue destruido. 
	bool esJugador;            // Indica si el bloque es la plataforma del jugador.

	vector<float> puntos[4];   // Arreglo de vectores que almacenar� los puntos del bloque.
} Bloque; // Estructura que representar� a los diferentes bloques que se motrar�n en pantalla.

/*------ Variables Globales ------*/
int bloqueIJ = 0;			   // Permitir� saber que bloque "enemigo" se generar�.
int numBloquesEsp = 5;         // Cantidad de bloques que pueden ser especiales.
int numBloquesBon = 6;         // Cantidad de bloques que tendr�n bonus.

float trasPared   = -12.0;     // Escalar correspondiente a la traslaci�n en X de las paredes.
float movimientoX =  -1.5;	   // Indicar� cuanto y hacia donde se mover� la plataforma.

Bloque plataforma;			   // Plataforma que utilizar� el jugador para jugar.
Bloque paredLateral;           // Bloque que representar� a la pared lateral (izquierda y derecha)  
							   // del juego.
Bloque paredSuperior;          //  Bloque que representar� a la pared superior del juego.
Bloque listaBloques[5][7];     // Conjunto de todos los bloques "enemigos" que se
							   // mostrar�n en pantalla.

/*---------- Funciones ----------*/
void teclaPresionada(unsigned char tecla, int x, int y);
void flechaPresionada(int flecha, int x, int y);
void generarParedLat(Bloque pared);
void generarParedSup(void);
void generarPlataforma(void);
void generarListaBloques(void);
void compilarJuego(void);
void ejecutarJuego(void);

bool randomBool(int numElmentos);
bool colisionPlatPared(Bloque pared, int direccion);

float lerp(float posInicial, float posFinal, float deltaTime);

/*--------------------------------*/
/*
	Descripci�n:
		Permite obtener un booleano de forma aleatoria partiendo de un
		n�mero de elementos.
*/
bool randomBool(int numElementos)
{
	int temp = rand() % numElementos;
	return temp == numElementos - 1 ? true : false;
}

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
		Permite saber si la plataforma est� chocando con una pared.
*/
bool colisionPlatPared(Bloque pared, int direccion)
{
	// �Est� chocando con la pared izquierda?
	if (direccion == -1 && plataforma.puntos[0][0] + movimientoX <= pared.puntos[3][0] + trasPared)
		return true;
	
	// �Est� chocando con la pared derecha?
	if (direccion ==  1 && plataforma.puntos[3][0] + movimientoX >= pared.puntos[0][0] - trasPared)
		return true;
	
	return false;
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
			if (!colisionPlatPared(paredLateral, -1))
				movimientoX = lerp(movimientoX, movimientoX - 1, 0.5);
			break;
		case 'd':
		case 'D':
			if (!colisionPlatPared(paredLateral, 1))
				movimientoX = lerp(movimientoX, movimientoX + 1, 0.5);
			break;
	}

	// Para realizar render al momento que se encadene la acci�n.
	glutPostRedisplay();
}

/*
	Descripci�n:
		Permite capturar que flecha es presionada y de esta forma
		se podr� encadenar la acci�n correspondiente.
*/
void flechaPresionada(int flecha, int x, int y)
{
	switch (flecha)
	{
		case GLUT_KEY_LEFT:    // Flecha izquierda.	
			if (!colisionPlatPared(paredLateral, -1))
				movimientoX = lerp(movimientoX, movimientoX - 1, 0.5);
				break;
		case GLUT_KEY_RIGHT:   // Flecha derecha.
			if (!colisionPlatPared(paredLateral, 1))
				movimientoX = lerp(movimientoX, movimientoX + 1, 0.5);
			break;
	}

	// Para realizar render al momento que se encadene la acci�n.
	glutPostRedisplay();
}

/*
	Descripci�n:
		Permite almacenar los comandos necesarios para la generaci�n
		de la pared izquierda y derecha.
*/
void generarParedLat(void)
{
	// Se definen los puntos que generan a las paredes laterales.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	paredLateral.puntos[0].push_back( 0.0); // Posici�n x del primer punto. 
	paredLateral.puntos[0].push_back( 0.0); // Posici�n y del primer punto.
	paredLateral.puntos[1].push_back( 0.0); // Posici�n x del segundo punto. 
	paredLateral.puntos[1].push_back(16.5); // Posici�n y del segundo punto.
	paredLateral.puntos[2].push_back( 0.5); // Posici�n x del tercer punto. 
	paredLateral.puntos[2].push_back(16.5); // Posici�n y del tercer punto.
	paredLateral.puntos[3].push_back( 0.5); // Posici�n x del cuarto punto. 
	paredLateral.puntos[3].push_back( 0.0); // Posici�n y del cuarto punto.
	
	paredLateral.tieneBonus = false;
	paredLateral.esEspecial = false;
	paredLateral.estaActivo = true;
	paredLateral.esJugador  = false;
	paredLateral.vida = -1; // No puede ser destruido.

	// Se construye la pared lateral.
	glNewList(glParedLat, GL_COMPILE);
		glBegin(GL_LINE_LOOP);
			glPushMatrix();
				glColor3f(0.0, 1.0, 0.0);
				glVertex2f(paredLateral.puntos[0][0], paredLateral.puntos[0][1]);
				glVertex2f(paredLateral.puntos[1][0], paredLateral.puntos[1][1]);
				glVertex2f(paredLateral.puntos[2][0], paredLateral.puntos[2][1]);
				glVertex2f(paredLateral.puntos[3][0], paredLateral.puntos[3][1]);
			glPopMatrix();
		glEnd();
	glEndList();

}

/*
	Descripci�n:
		Permite almacenar los comandos necesarios para la generaci�n
		de la pared superior.
*/
void generarParedSup(void)
{
	// Se definen los puntos que generan la pared superior.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	paredSuperior.puntos[0].push_back( 0.0); // Posici�n x del primer punto. 
	paredSuperior.puntos[0].push_back( 0.0); // Posici�n y del primer punto.
	paredSuperior.puntos[1].push_back( 0.0); // Posici�n x del segundo punto. 
	paredSuperior.puntos[1].push_back( 0.5); // Posici�n y del segundo punto.
	paredSuperior.puntos[2].push_back(24.5); // Posici�n x del tercer punto. 
	paredSuperior.puntos[2].push_back( 0.5); // Posici�n y del tercer punto.
	paredSuperior.puntos[3].push_back(24.5); // Posici�n x del cuarto punto. 
	paredSuperior.puntos[3].push_back( 0.0); // Posici�n y del cuarto punto.
	
	paredSuperior.tieneBonus = false;
	paredSuperior.esEspecial = false;
	paredSuperior.estaActivo = true;
	paredSuperior.esJugador  = false;
	paredSuperior.vida = -1;  // No puede ser destruido.

	// Se construye la pared superior.
	glNewList(glParedSup, GL_COMPILE);
		glBegin(GL_LINE_LOOP);
			glPushMatrix();
				glColor3f(0.0, 1.0, 0.0);
				glVertex2f(paredSuperior.puntos[0][0], paredSuperior.puntos[0][1]);
				glVertex2f(paredSuperior.puntos[1][0], paredSuperior.puntos[1][1]);
				glVertex2f(paredSuperior.puntos[2][0], paredSuperior.puntos[2][1]);
				glVertex2f(paredSuperior.puntos[3][0], paredSuperior.puntos[3][1]);
			glPopMatrix();
		glEnd();
	glEndList();

}


/*
	Descripci�n:
		Permite almacenar los comandos necesarios para la generaci�n
		de la plataforma.
*/
void generarPlataforma(void)
{
	// Se definen los puntos que generan a la plataforma.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	plataforma.puntos[0].push_back(0.0);  // Posici�n x del primer punto. 
	plataforma.puntos[0].push_back(0.0);  // Posici�n y del primer punto.
	plataforma.puntos[1].push_back(0.0);  // Posici�n x del segundo punto. 
	plataforma.puntos[1].push_back(0.25); // Posici�n y del segundo punto.
	plataforma.puntos[2].push_back(3.0);  // Posici�n x del tercer punto. 
	plataforma.puntos[2].push_back(0.25); // Posici�n y del tercer punto.
	plataforma.puntos[3].push_back(3.0);  // Posici�n x del cuarto punto. 
	plataforma.puntos[3].push_back(0.0);  // Posici�n y del cuarto punto.
	
	plataforma.tieneBonus = false;
	plataforma.esEspecial = false;
	plataforma.estaActivo = true;
	plataforma.esJugador  = true;
	plataforma.vida = 1;

	// Se construye la plataforma.
	glNewList(glPlataforma, GL_COMPILE);
		glBegin(GL_LINE_LOOP);
			glPushMatrix();
				glColor3f(0.0, 0.0, 1.0);
				glVertex2f(plataforma.puntos[0][0], plataforma.puntos[0][1]);
				glVertex2f(plataforma.puntos[1][0], plataforma.puntos[1][1]);
				glVertex2f(plataforma.puntos[2][0], plataforma.puntos[2][1]);
				glVertex2f(plataforma.puntos[3][0], plataforma.puntos[3][1]);
			glPopMatrix();
		glEnd();
	glEndList();
}

/* 
	Descripci�n:
		Permite generar los bloques "enemigos".
*/
void generarListaBloques(void)
{
	srand(time(0));  // Para que se puedan generar n�meros aleatorios.
	float tempPosicionX = trasPared + 2.25;  // Almacenar� la posici�n actual X donde se generar� el bloque.
										     // tempPosicion = posicionParedIzq + anchoPared + separaci�n
	float tempPosicionY = 14;                // Almacenar� la posici�n actual Y donde se generar� el bloque.

	for(int i= 0; i < 5; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			bloqueIJ = 10 + 10*i + j;
			listaBloques[i][j].puntos[0].push_back(tempPosicionX);       // Posici�n x del primer punto. 
			listaBloques[i][j].puntos[0].push_back(tempPosicionY);       // Posici�n x del primer punto. 
			listaBloques[i][j].puntos[1].push_back(tempPosicionX);       // Posici�n x del segundo punto. 
			listaBloques[i][j].puntos[1].push_back(tempPosicionY - 0.6); // Posici�n y del segundo punto.
			listaBloques[i][j].puntos[2].push_back(tempPosicionX + 1.9); // Posici�n x del tercer punto. 
			listaBloques[i][j].puntos[2].push_back(tempPosicionY - 0.6); // Posici�n y del tercer punto.
			listaBloques[i][j].puntos[3].push_back(tempPosicionX + 1.9); // Posici�n x del cuarto punto. 
			listaBloques[i][j].puntos[3].push_back(tempPosicionY);       // Posici�n y del cuarto punto.
	
			// Se determina si el bloque tendr� bonus.
			if (numBloquesBon > 0){
				listaBloques[i][j].tieneBonus = randomBool(6);
				numBloquesBon = listaBloques[i][j].tieneBonus ? numBloquesBon - 1 :  numBloquesBon;
			}

			// Se determina si el bloque ser� especial.
			if (numBloquesEsp > 0){
				listaBloques[i][j].esEspecial = randomBool(5);
				numBloquesEsp = listaBloques[i][j].esEspecial ? numBloquesEsp - 1 :  numBloquesEsp;
			}

			listaBloques[i][j].estaActivo = true;
			listaBloques[i][j].esJugador  = false;
	
			tempPosicionX += 3;
			
			// Se construye el bloque enemigo.
			glNewList(bloqueIJ, GL_COMPILE);
				glBegin(GL_LINE_LOOP);
					glPushMatrix();
						if (listaBloques[i][j].esEspecial)
							glColor3f(1.0, 1.0, 0.0);
						else
							glColor3f(1.0, 0.0, 0.0);
						glVertex2f(listaBloques[i][j].puntos[0][0], listaBloques[i][j].puntos[0][1]);
						glVertex2f(listaBloques[i][j].puntos[1][0], listaBloques[i][j].puntos[1][1]);
						glVertex2f(listaBloques[i][j].puntos[2][0], listaBloques[i][j].puntos[2][1]);
						glVertex2f(listaBloques[i][j].puntos[3][0], listaBloques[i][j].puntos[3][1]);
					glPopMatrix();
				glEnd();
			glEndList();
		}

		tempPosicionX  = trasPared + 2.25;
		tempPosicionY -= 1.3;
	}
}


/*
	Descripci�n:
		Permite compilar los comandos que se utilizar�n para crear el 
		juego.
*/
void compilarJuego(void)
{
	generarParedLat();
	generarParedSup();
	generarPlataforma();
	generarListaBloques();
}

/*
	Descripci�n:
		Permite ejecutar los comandos que se utilizar�n para crear el 
		juego.
*/
void ejecutarJuego(void)
{
	// Transformaci�n para que se pueda ver mejor en la pantalla.
	glTranslatef(0.0, -7.0, 0.0);

	/*-------------------- Plataforma --------------------*/
	glPushMatrix();
		glTranslatef(movimientoX, 0, 0);
		glCallList(glPlataforma);  // Se muestra la plataforma.
	glPopMatrix();

		
	/*-------------------- L�mites del juego --------------------*/
	// Creaci�n de la pared izquierda.
	glPushMatrix();
		glTranslatef(trasPared, 0.0, 0.0);
		glCallList(glParedLat);
	glPopMatrix();

	// Creaci�n de la pared superior.
	glPushMatrix();
		glTranslatef(trasPared, 16.0, 0.0);
		glCallList(glParedSup);
	glPopMatrix();

	// Creaci�n de la pared derecha.
	glPushMatrix();
		glTranslatef(-trasPared, 0.0, 0.0);
		glCallList(glParedLat);
	glPopMatrix();

	/*-------------------- Lista de bloques "enemigos" --------------------*/
	glPushMatrix();
		bloqueIJ = 10;
		for (int i = 0; i < 5; i++)
		{
			for(int j = 0; j < 7; j++)
			{
				bloqueIJ = 10 + 10*i + j;
				glCallList(bloqueIJ);
			}
		}
	glPopMatrix();
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
	glutSpecialFunc(flechaPresionada);

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