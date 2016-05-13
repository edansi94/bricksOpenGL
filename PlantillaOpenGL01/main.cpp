/*
	Descripci�n:
		PROYECTO II - OPENGL.

		Archivo principal donde se desarrollar� el juego Bricks 2D
		utilizando la librer�a OpenGL.

	Alumno:
		Edward Fern�ndez	 - Carnet: 10-11121.
		Mar�a Lourdes Garcia - Carnet: 10-10264.
*/

#include <ctime>          // Librer�a para importar la variable time.
#include <vector>         // Libreria que me permite crear vectores.
#include <iostream>
#include <GL\glew.h>
#include <GL\freeglut.h>

using namespace std;

/*--------- Definiciones --------*/
// Indices de la lista que almacenar� los comandos para la creaci�n de los elementos del juego.
#define glPlataforma	1        
#define glParedLatIzq   2
#define glParedLatDer   3
#define glParedSup		4
#define glPelota		5
#define glBonus			6

/*--------- Estructuras .--------*/
typedef struct
{
	int  vida;					// Cuantos golpes puede recibir. 

	bool esEspecial;			// Indica si el bloque debe golpearse 2 veces para ser destruido.
	bool tieneBonus;			// Indica si al ser destruido el bloque generar� un bonus.
	bool estaActivo;			// Indica si el bloque no fue destruido. 
	bool esJugador;				// Indica si el bloque es la plataforma del jugador.

	vector<float> puntos[4];	// Arreglo de vectores que almacenar� los puntos del bloque.
} Bloque; // Estructura que representar� a los diferentes bloques que se motrar�n en pantalla.

typedef struct
{
	int tipo;					// Indica que tipo de bonus es.
	bool agarrado;				// Indica si el bonus ha sido agarrado.
	float centroXBonus;			// Coordenada en x del centro del Bonus
	float centroYBonus;			// Coordenada en y del centro del Bonus    
	vector<float> puntos[4];	// Arreglo de vectores que almacenar� los puntos del bonus.
} Bonus; // Estructura que representar� a los bonus.

/*------ Variables Globales ------*/
// Para la creaci�n de los bloques "enemigos".
int bloqueIJ = 0;				// Permitir� saber que bloque "enemigo" se generar�.
int numBloquesEsp = 5;			// Cantidad de bloques que pueden ser especiales.
int numBloquesBon = 6;			// Cantidad de bloques que tendr�n bonus.
int periodoRef    = 30;			// Tiempo para recargar en milisegundos.

// Para la creaci�n de las paredes laterales. 
float posXIniParedLat = 12.0;	// Punto X donde se dibujar� una pared lateral.
float largoXParedLat  =  0.5;	// Ancho de la pared lateral a dibujar.
float posYIniParedLat =  0.0;	// Punto Y donde se dibujar� la pared lateral.
float largoYParedLat  = 16.5;	// Largo de la pared lateral a dibujar.

// Para la creaci�n del techo.
float posXIniTecho = -12.5;		// Punto X donde se dibujar� el techo.
float largoXTecho  =  25.0;		// Ancho del techo a dibujar.
float posYIniTecho =  16.0;		// Punto Y donde se dibujar� el techo.
float largoYTecho  =   0.5;		// Largo del techo a dibujar.

// Para la creaci�n de la plataforma.
float posXIniPlat  = - 1.50;    // Punto X donde se dibujar� la plataforma.
float largoXPlat   =   3.00;    // Ancho de la plataforma a dibujar.
float posYIniPlat  =   0.25;    // Punto Y donde se dibujar� la plataforma.
float largoYPlat   =   0.25;    // Largo de la plataforma a dibujar.
float movPlat      =   0.00;    // Velocidad en X de la plataforma.

// Para la creaci�n de la pelota.
float radPelota	  =	 0.3;		// Radio de la pelota
float centroXPelota = 0.0;		// Centro de la pelota en X.
float centroYPelota = 0.0;		// Centro de la pelota en Y.
float velXPelota = 0.08;		// Velocidad en x que tendr� la pelota.
float velYPelota = 0.10;		// Velocidad en y que tendr� la pelota.

// Para la creacion de los bonus
float tamBonus	 = 0.2;			// Radio en caso de ser un circulo y mitad del lado en caso de ser un cuadrado
float velBonus	 = -0.10;		// Velovidad de caida del bonus
float aumentVel  = 1.40;		// Porcentaje de aumento de la velocidad de la pelota en el Bonus tipo 1
float disminTam  = 0.85;		// Porcentaje de disminuci�n del tama�o de la plataforma en el Bonus tipo 2

bool comenzoJuego = false;	   // Indica si la pelota ya se puede mover.

Bloque paredLateralIzq;        // Bloque que representar� la pared izquierda.
Bloque paredLateralDer;        // Bloque que representar� la pared derecha.
Bloque paredSuperior;          // Bloque que representar� a la pared superior del juego.
Bloque plataforma;			   // Plataforma que utilizar� el jugador.
Bloque listaBloques[5][7];     // Conjunto de todos los bloques "enemigos" que se
							   // mostrar�n en pantalla.
Bonus bonus; // --------------------------------------------------------------------------Revisar

/*---------- Definici�n de las funciones ----------*/
void generarPelota(void);
void generarParedLatIzq(void);
void generarParedLatDer(void);
void generarListaBloques(void);
void temporizador(int valor);
void ejesCoordenada(float w);
void flechaPresionada(int flecha, int x, int y);
void teclaPresionada(unsigned char tecla, int x, int y);
void dibujarPelota(float coordX, float coordY, float radio);
void dibujarBonus(float coordX, float coordY, float tam, int tipo);
void generarBonus(void);

bool randomBool(int numElementos);
bool colisionPlatPared(float limitePared, int direccion);

int calculoNumeroSegmento(float radio);

float lerp(float posInicial, float posFinal, float deltaTime);

/*---------- Desarrollo de las funciones ----------*/
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
		Permite refrescar la pantalla constantemente.
*/
void temporizador(int valor)
{
	glutPostRedisplay();
	glutTimerFunc(periodoRef, temporizador, 0);
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
bool colisionPlatPared(float limitePared, int direccion)
{
	// �Est� chocando con la pared izquierda?
	if (direccion == -1 && plataforma.puntos[0][0] + movPlat <= - posXIniParedLat) return true;
	
	// �Est� chocando con la pared derecha?
	if (direccion ==  1 && plataforma.puntos[3][0] + movPlat >= posXIniParedLat) return true;
	
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
			if (!colisionPlatPared(- posXIniParedLat, -1)){
				movPlat = lerp(movPlat, movPlat - 1, 0.5);
				if (!comenzoJuego)
					velXPelota = lerp(velXPelota, velXPelota - 1, 0.5);
			}
			break;
		case 'd':
		case 'D':
			if (!colisionPlatPared(posXIniParedLat, 1)){
				movPlat = lerp(movPlat, movPlat + 1, 0.5);
				if (!comenzoJuego)
					velXPelota = lerp(velXPelota, velXPelota + 1, 0.5);
			}
			break;
		case 32:
			comenzoJuego = true;
			break;
	}
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
			if (!colisionPlatPared(- posXIniParedLat, -1)){
				movPlat = lerp(movPlat, movPlat - 1, 0.5);
				if (!comenzoJuego)
					velXPelota = lerp(velXPelota, velXPelota - 1, 0.5);
			}	break;
		case GLUT_KEY_RIGHT:   // Flecha derecha.
			if (!colisionPlatPared(posXIniParedLat, 1)){
				movPlat = lerp(movPlat, movPlat + 1, 0.5);
				if (!comenzoJuego)
					velXPelota = lerp(velXPelota, velXPelota + 1, 0.5);
			}
			break;
	}
}

/*
	Descripci�n:
		Permite almacenar los comandos necesarios para la generaci�n
		de la pared izquierda.
*/
void generarParedLatIzq(void)
{
	// Se definen los puntos que permitir�n construir la pared izquierda.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	paredLateralIzq.puntos[0].push_back(- posXIniParedLat);					 // Posici�n x del primer punto. 
	paredLateralIzq.puntos[0].push_back(  posYIniParedLat);					 // Posici�n y del primer punto.
	paredLateralIzq.puntos[1].push_back(- posXIniParedLat);					 // Posici�n x del segundo punto. 
	paredLateralIzq.puntos[1].push_back(  posYIniParedLat + largoYParedLat); // Posici�n y del segundo punto.
	paredLateralIzq.puntos[2].push_back(- posXIniParedLat - largoXParedLat); // Posici�n x del tercer punto. 
	paredLateralIzq.puntos[2].push_back(  posYIniParedLat + largoYParedLat); // Posici�n y del tercer punto.
	paredLateralIzq.puntos[3].push_back(- posXIniParedLat - largoXParedLat); // Posici�n x del cuarto punto. 
	paredLateralIzq.puntos[3].push_back(  posYIniParedLat);                  // Posici�n y del cuarto punto.
	
	paredLateralIzq.tieneBonus = false;
	paredLateralIzq.esEspecial = false;
	paredLateralIzq.estaActivo = true;
	paredLateralIzq.esJugador  = false;
	paredLateralIzq.vida = -1; // No puede ser destruido.

	// Se construye la pared lateral izquierda.
	glNewList(glParedLatIzq, GL_COMPILE);
		glBegin(GL_QUADS);
			glPushMatrix();
				glColor3f(0.0, 1.0, 0.0);
				glVertex2f(paredLateralIzq.puntos[0][0], paredLateralIzq.puntos[0][1]);
				glVertex2f(paredLateralIzq.puntos[1][0], paredLateralIzq.puntos[1][1]);
				glVertex2f(paredLateralIzq.puntos[2][0], paredLateralIzq.puntos[2][1]);
				glVertex2f(paredLateralIzq.puntos[3][0], paredLateralIzq.puntos[3][1]);
			glPopMatrix();
		glEnd();
	glEndList();

}

/*
	Descripci�n:
		Permite almacenar los comandos necesarios para la generaci�n
		de la pared derecha.
*/
void generarParedLatDer(void)
{
	// Se definen los puntos que permitir�n construir la pared derecha.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	paredLateralDer.puntos[0].push_back(posXIniParedLat);				   // Posici�n x del primer punto. 
	paredLateralDer.puntos[0].push_back(posYIniParedLat);			       // Posici�n y del primer punto.
	paredLateralDer.puntos[1].push_back(posXIniParedLat);				   // Posici�n x del segundo punto. 
	paredLateralDer.puntos[1].push_back(posYIniParedLat + largoYParedLat); // Posici�n y del segundo punto.
	paredLateralDer.puntos[2].push_back(posXIniParedLat + largoXParedLat); // Posici�n x del tercer punto. 
	paredLateralDer.puntos[2].push_back(posYIniParedLat + largoYParedLat); // Posici�n y del tercer punto.
	paredLateralDer.puntos[3].push_back(posXIniParedLat + largoXParedLat); // Posici�n x del cuarto punto. 
	paredLateralDer.puntos[3].push_back(posYIniParedLat);                  // Posici�n y del cuarto punto.
	
	paredLateralDer.tieneBonus = false;
	paredLateralDer.esEspecial = false;
	paredLateralDer.estaActivo = true;
	paredLateralDer.esJugador  = false;
	paredLateralDer.vida = -1; // No puede ser destruido.

	// Se construye la pared lateral.
	glNewList(glParedLatDer, GL_COMPILE);
		glBegin(GL_QUADS);
			glPushMatrix();
				glColor3f(0.0, 1.0, 0.0);
				glVertex2f(paredLateralDer.puntos[0][0], paredLateralDer.puntos[0][1]);
				glVertex2f(paredLateralDer.puntos[1][0], paredLateralDer.puntos[1][1]);
				glVertex2f(paredLateralDer.puntos[2][0], paredLateralDer.puntos[2][1]);
				glVertex2f(paredLateralDer.puntos[3][0], paredLateralDer.puntos[3][1]);
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
	paredSuperior.puntos[0].push_back( posXIniTecho);				// Posici�n x del primer punto. 
	paredSuperior.puntos[0].push_back( posYIniTecho);				// Posici�n y del primer punto.
	paredSuperior.puntos[1].push_back( posXIniTecho + largoXTecho); // Posici�n x del segundo punto. 
	paredSuperior.puntos[1].push_back( posYIniTecho);				// Posici�n y del segundo punto.
	paredSuperior.puntos[2].push_back( posXIniTecho + largoXTecho); // Posici�n x del tercer punto. 
	paredSuperior.puntos[2].push_back( posYIniTecho + largoYTecho); // Posici�n y del tercer punto.
	paredSuperior.puntos[3].push_back( posXIniTecho);				// Posici�n x del cuarto punto. 
	paredSuperior.puntos[3].push_back( posYIniTecho + largoYTecho); // Posici�n y del cuarto punto.
	
	paredSuperior.tieneBonus = false;
	paredSuperior.esEspecial = false;
	paredSuperior.estaActivo = true;
	paredSuperior.esJugador  = false;
	paredSuperior.vida = -1;  // No puede ser destruido.

	// Se construye la pared superior.
	glNewList(glParedSup, GL_COMPILE);
		glBegin(GL_QUADS);
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
	plataforma.puntos[0].push_back(posXIniPlat);			        	// Posici�n x del primer punto. 
	plataforma.puntos[0].push_back(posYIniPlat);						// Posici�n y del primer punto.
	plataforma.puntos[1].push_back(posXIniPlat);	        			// Posici�n x del segundo punto. 
	plataforma.puntos[1].push_back(posYIniPlat - largoYPlat);			// Posici�n y del segundo punto.
	plataforma.puntos[2].push_back(posXIniPlat + largoXPlat);           // Posici�n x del tercer punto. 
	plataforma.puntos[2].push_back(posYIniPlat - largoYPlat);			// Posici�n y del tercer punto.
	plataforma.puntos[3].push_back(posXIniPlat + largoXPlat);           // Posici�n x del cuarto punto. 
	plataforma.puntos[3].push_back(posYIniPlat);						// Posici�n y del cuarto punto.
	
	plataforma.tieneBonus = false;
	plataforma.esEspecial = false;
	plataforma.estaActivo = true;
	plataforma.esJugador  = true;
	plataforma.vida = 1;

	// Se construye la plataforma.
	glNewList(glPlataforma, GL_COMPILE);
		glBegin(GL_QUADS);
			glPushMatrix();
				glColor3f(0.0, 0.0, 1.0);
				glVertex2f(plataforma.puntos[0][0] + movPlat, plataforma.puntos[0][1]);
				glVertex2f(plataforma.puntos[1][0] + movPlat, plataforma.puntos[1][1]);
				glVertex2f(plataforma.puntos[2][0] + movPlat, plataforma.puntos[2][1]);
				glVertex2f(plataforma.puntos[3][0] + movPlat, plataforma.puntos[3][1]);
			glPopMatrix();
		glEnd();
	glEndList();
}

/*
	Descripci�n:
		Permite calcular el n�mero de segmentos a utilizar en la generaci�n
		de un c�rculo.
 */
int calculoNumeroSegmento(float radio){
	return 70 * sqrtf(radio);
}

/*
	Descripci�n:
		Permite crear la pelota.
 */
void dibujarPelota(float coordX, float coordY, float radio){
	int numSegmentos = calculoNumeroSegmento(radio);
	float theta = 2 * 3.1415926 / float(numSegmentos);
	float coseno = cosf(theta);
	float seno   = sinf(theta);
	float temp;

	float x = radio; // Se comienza con 0 grados.
	float y = 0;
	glBegin(GL_POLYGON);
		for(int i = 0; i < numSegmentos; i++)
		{
			glVertex2d(x + coordX + velXPelota, y + coordY + 0.55 + centroYPelota);

			temp = x;
			x = coseno*x - seno*y;
			y = seno*temp + coseno*y;
		}
	glEnd();
}

/*
	Descripci�n:
		Permite almacenar los comandos necesarios para la generaci�n
		de la pared superior.
*/
void generarPelota(void)
{
	glNewList(glPelota, GL_COMPILE);
		glPushMatrix();
			glColor3f(0.99, 0.89, 0.65);
			dibujarPelota(centroXPelota, centroYPelota, radPelota);
		glPopMatrix();
	glEndList();
}

/* 
	Descripci�n:
		Permite generar los bloques "enemigos".
*/
void generarListaBloques(void)
{
	srand(time(0));  // Para que se puedan generar n�meros aleatorios.
	float tempPosicionX = - posXIniParedLat + 2;  // Almacenar� la posici�n actual X donde se generar� el bloque.
							    				  // tempPosicion = posicionParedIzq + anchoPared + separaci�n
	float tempPosicionY = 14;                     // Almacenar� la posici�n actual Y donde se generar� el bloque.

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
			listaBloques[i][j].vida = 1;

			tempPosicionX += 3;
			
			// Se construye el bloque enemigo.
		}

		tempPosicionX  = - posXIniParedLat + 2;
		tempPosicionY -= 1.3;
	}
}

/*
	Descripci�n:
		Permite verificar si la pelota est� chocando con los diferentes objetos del juego.
*/
void revisarColisionPelota(void)
{
	if (centroXPelota + radPelota + 0.1 > posXIniParedLat)
	{
		centroXPelota = posXIniParedLat - radPelota - 0.1;
		velXPelota = -velXPelota;

	}else if(centroXPelota - radPelota < - posXIniParedLat)
	{
		centroXPelota = - posXIniParedLat + radPelota;
		velXPelota = -velXPelota;
	}

	// Colisi�n techo.
	if (centroYPelota + radPelota > posYIniTecho - 8.15)
	{
		centroYPelota = posYIniTecho - radPelota - 8.15;
		velYPelota = -velYPelota;
	}

	// Colisi�n plataforma en X.
	if  ((centroYPelota < 0) && (posXIniPlat + movPlat <= centroXPelota) && 
		 (- posXIniPlat + movPlat >= centroXPelota)){
		velYPelota = -velYPelota;
	}

	/*------ FILA 1 ------ */
	if (listaBloques[0][0].estaActivo && (listaBloques[0][0].puntos[0][0] < centroXPelota) && (listaBloques[0][0].puntos[3][0] + 0.1 > centroXPelota)
		&& (listaBloques[0][0].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][0].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][0].vida -= 1;
		listaBloques[0][0].vida == 0 ? listaBloques[0][0].estaActivo = false : listaBloques[0][0].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;
	}

	if (listaBloques[0][1].estaActivo && (listaBloques[0][1].puntos[0][0] < centroXPelota) && (listaBloques[0][1].puntos[3][0] + 0.1 > centroXPelota)
		&& (listaBloques[0][1].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][1].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][1].vida -= 1;
		listaBloques[0][1].vida == 0 ? listaBloques[0][1].estaActivo = false : listaBloques[0][1].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;
	}

	if (listaBloques[0][2].estaActivo && (listaBloques[0][2].puntos[0][0] < centroXPelota) && (listaBloques[0][2].puntos[3][0] + 0.1 > centroXPelota)
		&& (listaBloques[0][2].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][2].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][2].vida -= 1;
		listaBloques[0][2].vida == 0 ? listaBloques[0][2].estaActivo = false : listaBloques[0][2].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;
	}

	if (listaBloques[0][3].estaActivo && (listaBloques[0][3].puntos[0][0] < centroXPelota) && (listaBloques[0][3].puntos[3][0] + 0.1 > centroXPelota)
		&& (listaBloques[0][3].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][3].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][3].vida -= 1;
		listaBloques[0][3].vida == 0 ? listaBloques[0][3].estaActivo = false : listaBloques[0][3].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;
	}

	if (listaBloques[0][4].estaActivo && (listaBloques[0][4].puntos[0][0] < centroXPelota) && (listaBloques[0][4].puntos[3][0] + 0.1 > centroXPelota)
		&& (listaBloques[0][4].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][4].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][4].vida -= 1;
		listaBloques[0][4].vida == 0 ? listaBloques[0][4].estaActivo = false : listaBloques[0][4].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;
	}

	if (listaBloques[0][5].estaActivo && (listaBloques[0][5].puntos[0][0] < centroXPelota) && (listaBloques[0][5].puntos[3][0] + 0.1 > centroXPelota)
		&& (listaBloques[0][5].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][5].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][5].vida -= 1;
		listaBloques[0][5].vida == 0 ? listaBloques[0][5].estaActivo = false : listaBloques[0][5].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

	}

	if (listaBloques[0][6].estaActivo && (listaBloques[0][6].puntos[0][0] < centroXPelota) && (listaBloques[0][6].puntos[3][0] + 0.1 > centroXPelota)
		&& (listaBloques[0][6].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][6].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][6].vida -= 1;
		listaBloques[0][6].vida == 0 ? listaBloques[0][6].estaActivo = false : listaBloques[0][6].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;
	}
}

void prueba(){
	for(int i= 0; i < 5; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			bloqueIJ = 10 + 10*i + j;
			glNewList(bloqueIJ, GL_COMPILE);
				glBegin(GL_QUADS);
					glPushMatrix();
						if (listaBloques[i][j].estaActivo){
							if (listaBloques[i][j].esEspecial)
								glColor3f(1.0, 0.0, 0.0);
							else
								glColor3f(0.96, 0.67, 0.2);
						}	
						else{
							glColor3f(0.0, 0.0, 0.0);
						}
						glVertex2f(listaBloques[i][j].puntos[0][0], listaBloques[i][j].puntos[0][1]);
						glVertex2f(listaBloques[i][j].puntos[1][0], listaBloques[i][j].puntos[1][1]);
						glVertex2f(listaBloques[i][j].puntos[2][0], listaBloques[i][j].puntos[2][1]);
						glVertex2f(listaBloques[i][j].puntos[3][0], listaBloques[i][j].puntos[3][1]);
					glPopMatrix();
				glEnd();
			glEndList();
		}
	}
}

/*
		Para general el bonus al colisionar con un bloque con bonus, en el if de la colision con el bloque, se agrega generarBonus
		con coordX y coordY iguales a las coordenadas del centro del bloque y se agrega lo siguiente:
			actualizar centro del bonus += velBonus
			verificar colision con la plataforma, cambiando agarrado por true(si colisiona) y drestruir el bonus
			en caso de agarrarlo:
				si es de tipo 1 --> velXpelota = velXPelota*aumentVel
									velYpelota = velYPelota*aumentVel
				si es de tipo 2 --> largoXPlat = largoXPlat*disminTam
	*/

/*
	Descripci�n:
		permite crear el bonus
*/
void dibujarBonus(float coordX, float coordY, float tam, int tipo)
{
	bonus.tipo = tipo;

	// Se definen los puntos que generan al bonus.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	if (tipo == 1)
	{
		int numSegmentos = calculoNumeroSegmento(tam);
		float theta = 2 * 3.1415926 / float(numSegmentos);
		float coseno = cosf(theta);
		float seno   = sinf(theta);
		float temp;

		float x = tam; // Se comienza con 0 grados.
		float y = 0;
		glBegin(GL_POLYGON);
			for(int i = 0; i < numSegmentos; i++)
			{
				glVertex2d(x + coordX, y + coordY);

				temp = x;
				x = coseno*x - seno*y;
				y = seno*temp + coseno*y;
			}
		glEnd();
	}
	else if (tipo == 2)
	{
		bonus.puntos[0].push_back(coordX - tamBonus);			        // Posici�n x del primer punto. 
		bonus.puntos[0].push_back(coordY - tamBonus);					// Posici�n y del primer punto.
		bonus.puntos[1].push_back(coordX - tamBonus);        			// Posici�n x del segundo punto. 
		bonus.puntos[1].push_back(coordY + tamBonus);					// Posici�n y del segundo punto.
		bonus.puntos[2].push_back(coordX + tamBonus);				    // Posici�n x del tercer punto. 
		bonus.puntos[2].push_back(coordY + tamBonus);					// Posici�n y del tercer punto.
		bonus.puntos[3].push_back(coordX + tamBonus);					// Posici�n x del cuarto punto. 
		bonus.puntos[3].push_back(coordY - tamBonus);					// Posici�n y del cuarto punto.

		glBegin(GL_QUADS);
			glVertex2f(bonus.puntos[0][0], bonus.puntos[0][1]);
			glVertex2f(bonus.puntos[1][0], bonus.puntos[1][1]);
			glVertex2f(bonus.puntos[2][0], bonus.puntos[2][1]);
			glVertex2f(bonus.puntos[3][0], bonus.puntos[3][1]);
		glEnd();
	}
	
	bonus.agarrado = false;
}

void generarBonus(void)
{
	// TODO ver como asignar que tipo de bonus es
	glNewList(glBonus, GL_COMPILE);
		glPushMatrix();
			glColor3f(0.93, 0.0, 0.93);
			dibujarBonus(bonus.centroXBonus,bonus.centroYBonus,tamBonus,1);
		glPopMatrix();
	glEndList();
}

/*
	Descripci�n:
		Permite compilar los comandos que se utilizar�n para crear el 
		juego.
*/
void compilarJuego(void)
{
	generarParedLatIzq();
	generarParedLatDer();
	generarParedSup();
	generarPlataforma();
	generarPelota();
	generarBonus();
	prueba();
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

	glCallList(glPlataforma);

	glPushMatrix();	
		glCallList(glParedLatIzq);
		glCallList(glParedSup);
		glCallList(glParedLatDer);
	glPopMatrix();

	glPushMatrix();
		bloqueIJ = 10;
		for (int i = 0; i < 1; i++)
		{
			for(int j = 0; j < 7; j++)
			{
				bloqueIJ = 10 + 10*i + j;
				glCallList(bloqueIJ);
			}
		}
	glPopMatrix();

	glPushMatrix();
		glCallList(glPelota);

		// Si se comenz� el juego.
		if (comenzoJuego) {
			centroXPelota += velXPelota;
			centroYPelota += velYPelota;

			revisarColisionPelota();
		}
	glPopMatrix();
	glPushMatrix();
		glCallList(glBonus);
	glPopMatrix();
}

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
	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//mostrar();
	
	// Permite saber que tecla es seleccionada.
	glutKeyboardFunc(teclaPresionada);
	glutSpecialFunc(flechaPresionada);

	//ejesCoordenada(2.0);
	if (!comenzoJuego) generarListaBloques();
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
	glutTimerFunc(0, temporizador, 0);
	
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error");
		return 1;
	}
	

	glutMainLoop();
	return 0;

}