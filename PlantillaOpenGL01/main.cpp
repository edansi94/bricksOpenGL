/*
	Descripción:
		PROYECTO II - OPENGL.

		Archivo principal donde se desarrollará el juego Bricks 2D
		utilizando la librería OpenGL.

	Alumno:
		Edward Fernández	 - Carnet: 10-11121.
		María Lourdes Garcia - Carnet: 10-10264.
*/

#include <ctime>          // Librería para importar la variable time.
#include <vector>         // Libreria que me permite crear vectores.
#include <iostream>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <map>            // Librería que me permite utilizar hash.

using namespace std;

/*--------- Definiciones --------*/
// Indices de la lista que almacenará los comandos para la creación de los elementos del juego.
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
	bool tieneBonus;			// Indica si al ser destruido el bloque generará un bonus.
	bool estaActivo;			// Indica si el bloque no fue destruido. 
	bool esJugador;				// Indica si el bloque es la plataforma del jugador.

	vector<float> puntos[4];	// Arreglo de vectores que almacenará los puntos del bloque.
} Bloque; // Estructura que representará a los diferentes bloques que se motrarán en pantalla.

typedef struct
{
	int id;                     // Identificador del bonus.
	int tipo;					// Indica que tipo de bonus es.
	bool agarrado;				// Indica si el bonus ha sido agarrado.
	bool lanzado;               // Indica si el bonus puede moverse.
	float centroXBonus;			// Coordenada en x del centro del Bonus
	float centroYBonus;			// Coordenada en y del centro del Bonus    
	vector<float> puntos[4];	// Arreglo de vectores que almacenará los puntos del bonus.
} Bonus; // Estructura que representará a los bonus.

/*------ Variables Globales ------*/
// Para la creación de los bloques "enemigos".
int bloqueIJ = 0;				// Permitirá saber que bloque "enemigo" se generará.
int bonusJ   = 0;               // Permitirá saber que bloque  se generará
int direccion = 0;              // Para donde se mueve la plataforma. 
int numBloquesEsp = 5;			// Cantidad de bloques que pueden ser especiales.
int numBloquesBon = 6;			// Cantidad de bloques que tendrán bonus.
int periodoRef    = 15;			// Tiempo para recargar en milisegundos.

// Para la creación de las paredes laterales. 
float posXIniParedLat = 12.0;	// Punto X donde se dibujará una pared lateral.
float largoXParedLat  =  0.5;	// Ancho de la pared lateral a dibujar.
float posYIniParedLat =  0.0;	// Punto Y donde se dibujará la pared lateral.
float largoYParedLat  = 16.5;	// Largo de la pared lateral a dibujar.

// Para la creación del techo.
float posXIniTecho = -12.5;		// Punto X donde se dibujará el techo.
float largoXTecho  =  25.0;		// Ancho del techo a dibujar.
float posYIniTecho =  16.0;		// Punto Y donde se dibujará el techo.
float largoYTecho  =   0.5;		// Largo del techo a dibujar.

// Para la creación de la plataforma.
float posXIniPlat  = - 1.50;    // Punto X donde se dibujará la plataforma.
float largoXPlat   =   3.00;    // Ancho de la plataforma a dibujar.
float posYIniPlat  =   0.25;    // Punto Y donde se dibujará la plataforma.
float largoYPlat   =   0.25;    // Largo de la plataforma a dibujar.
float movPlat      =   0.00;    // Velocidad en X de la plataforma.

// Para la creación de la pelota.
float radPelota	  =	 0.3;		// Radio de la pelota
float centroXPelota = 0.0;		// Centro de la pelota en X.
float centroYPelota = 0.0;		// Centro de la pelota en Y.
float velXPelota = 0.04;		// Velocidad en x que tendrá la pelota.
float velYPelota = 0.05;		// Velocidad en y que tendrá la pelota.

// Para la creacion de los bonus
float tamBonus	 = 0.2;			// Radio en caso de ser un circulo y mitad del lado en caso de ser un cuadrado
float velBonus	 = -0.05;		// Velovidad de caida del bonus
float aumentVel  = 1.40;		// Porcentaje de aumento de la velocidad de la pelota en el Bonus tipo 1
float disminTam  = 0.85;		// Porcentaje de disminución del tamaño de la plataforma en el Bonus tipo 2

bool comenzoJuego = false;	   // Indica si la pelota ya se puede mover.
bool cargoBloques = false;     // Indica si se cargaron los bloques.

Bloque paredLateralIzq;        // Bloque que representará la pared izquierda.
Bloque paredLateralDer;        // Bloque que representará la pared derecha.
Bloque paredSuperior;          // Bloque que representará a la pared superior del juego.
Bloque plataforma;			   // Plataforma que utilizará el jugador.
Bloque listaBloques[5][7];     // Conjunto de todos los bloques "enemigos" que se
							   // mostrarán en pantalla.
Bonus bonus[6];
map<int, Bonus> listaBonus;    // Conjunto de todos los bloques a mostrar. 

int idBonus;	

/*---------- Definición de las funciones ----------*/
void generarBonus(void);
void generarPelota(void);
void temporizador(int valor);
void generarParedLatIzq(void);
void generarParedLatDer(void);
void generarListaBloques(void);
void actualizacionBloques(void);
void colisionPelotaBloques(void);
void dibujarBonus(int id, float tam);
void flechaPresionada(int flecha, int x, int y);
void teclaPresionada(unsigned char tecla, int x, int y);
void dibujarPelota(float coordX, float coordY, float radio);


bool randomBool(int numElementos);
bool colisionPlatPared(float limitePared, int direccion);

int calculoNumeroSegmento(float radio);

float lerp(float posInicial, float posFinal, float deltaTime);

Bonus crearBonus(float centroX, float centroY, int tipoBonus);

/*---------- Desarrollo de las funciones ----------*/
/*
	Descripción:
		Permite obtener un booleano de forma aleatoria partiendo de un
		número de elementos.
*/
bool randomBool(int numElementos)
{
	int temp = rand() % numElementos;
	return temp == numElementos - 1 ? true : false;
}

/*
	Descripción:
		Permite refrescar la pantalla constantemente.
*/
void temporizador(int valor)
{
	glutPostRedisplay();
	glutTimerFunc(periodoRef, temporizador, 0);
}

/*
	Descripción:
		Permite realizar la interpolación lineal entre dos escalares dados en base al
		tiempo de ejecución del programa.
*/
float lerp(float posInicial, float posFinal, float deltaTime)
{
	return( posInicial + deltaTime*(posFinal - posInicial));
}

/*
	Descripción:
		Permite saber si la plataforma está chocando con una pared.
*/
bool colisionPlatPared(float limitePared, int direccion)
{
	// ¿Está chocando con la pared izquierda?
	if (direccion == -1 && plataforma.puntos[0][0] + movPlat <= - posXIniParedLat) return true;
	
	// ¿Está chocando con la pared derecha?
	if (direccion ==  1 && plataforma.puntos[3][0] + movPlat >= posXIniParedLat) return true;
	
	return false;
}

/*
	Descripción:
		Permite capturar que tecla es presionada y de esta forma
		se podrá encadenar la acción correspondiente.
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
	Descripción:
		Permite capturar que flecha es presionada y de esta forma
		se podrá encadenar la acción correspondiente.
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
	Descripción:
		Permite almacenar los comandos necesarios para la generación
		de la pared izquierda.
*/
void generarParedLatIzq(void)
{
	// Se definen los puntos que permitirán construir la pared izquierda.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	paredLateralIzq.puntos[0].push_back(- posXIniParedLat);					 // Posición x del primer punto. 
	paredLateralIzq.puntos[0].push_back(  posYIniParedLat);					 // Posición y del primer punto.
	paredLateralIzq.puntos[1].push_back(- posXIniParedLat);					 // Posición x del segundo punto. 
	paredLateralIzq.puntos[1].push_back(  posYIniParedLat + largoYParedLat); // Posición y del segundo punto.
	paredLateralIzq.puntos[2].push_back(- posXIniParedLat - largoXParedLat); // Posición x del tercer punto. 
	paredLateralIzq.puntos[2].push_back(  posYIniParedLat + largoYParedLat); // Posición y del tercer punto.
	paredLateralIzq.puntos[3].push_back(- posXIniParedLat - largoXParedLat); // Posición x del cuarto punto. 
	paredLateralIzq.puntos[3].push_back(  posYIniParedLat);                  // Posición y del cuarto punto.
	
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
	Descripción:
		Permite almacenar los comandos necesarios para la generación
		de la pared derecha.
*/
void generarParedLatDer(void)
{
	// Se definen los puntos que permitirán construir la pared derecha.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	paredLateralDer.puntos[0].push_back(posXIniParedLat);				   // Posición x del primer punto. 
	paredLateralDer.puntos[0].push_back(posYIniParedLat);			       // Posición y del primer punto.
	paredLateralDer.puntos[1].push_back(posXIniParedLat);				   // Posición x del segundo punto. 
	paredLateralDer.puntos[1].push_back(posYIniParedLat + largoYParedLat); // Posición y del segundo punto.
	paredLateralDer.puntos[2].push_back(posXIniParedLat + largoXParedLat); // Posición x del tercer punto. 
	paredLateralDer.puntos[2].push_back(posYIniParedLat + largoYParedLat); // Posición y del tercer punto.
	paredLateralDer.puntos[3].push_back(posXIniParedLat + largoXParedLat); // Posición x del cuarto punto. 
	paredLateralDer.puntos[3].push_back(posYIniParedLat);                  // Posición y del cuarto punto.
	
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
	Descripción:
		Permite almacenar los comandos necesarios para la generación
		de la pared superior.
*/
void generarParedSup(void)
{
	// Se definen los puntos que generan la pared superior.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	paredSuperior.puntos[0].push_back( posXIniTecho);				// Posición x del primer punto. 
	paredSuperior.puntos[0].push_back( posYIniTecho);				// Posición y del primer punto.
	paredSuperior.puntos[1].push_back( posXIniTecho + largoXTecho); // Posición x del segundo punto. 
	paredSuperior.puntos[1].push_back( posYIniTecho);				// Posición y del segundo punto.
	paredSuperior.puntos[2].push_back( posXIniTecho + largoXTecho); // Posición x del tercer punto. 
	paredSuperior.puntos[2].push_back( posYIniTecho + largoYTecho); // Posición y del tercer punto.
	paredSuperior.puntos[3].push_back( posXIniTecho);				// Posición x del cuarto punto. 
	paredSuperior.puntos[3].push_back( posYIniTecho + largoYTecho); // Posición y del cuarto punto.
	
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
	Descripción:
		Permite almacenar los comandos necesarios para la generación
		de la plataforma.
*/
void generarPlataforma(void)
{
	// Se definen los puntos que generan a la plataforma.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	plataforma.puntos[0].push_back(posXIniPlat);			        	// Posición x del primer punto. 
	plataforma.puntos[0].push_back(posYIniPlat);						// Posición y del primer punto.
	plataforma.puntos[1].push_back(posXIniPlat);	        			// Posición x del segundo punto. 
	plataforma.puntos[1].push_back(posYIniPlat - largoYPlat);			// Posición y del segundo punto.
	plataforma.puntos[2].push_back(posXIniPlat + largoXPlat);           // Posición x del tercer punto. 
	plataforma.puntos[2].push_back(posYIniPlat - largoYPlat);			// Posición y del tercer punto.
	plataforma.puntos[3].push_back(posXIniPlat + largoXPlat);           // Posición x del cuarto punto. 
	plataforma.puntos[3].push_back(posYIniPlat);						// Posición y del cuarto punto.
	
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
	Descripción:
		Permite calcular el número de segmentos a utilizar en la generación
		de un círculo.
 */
int calculoNumeroSegmento(float radio)
{
	return 70 * sqrtf(radio);
}

/*
	Descripción:
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
	Descripción:
		Permite almacenar los comandos necesarios para la generación
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
	Descripción:
		Permite crear un bonus con el centro en X, el centro en Y y
		que tipo de bonus será.
*/
Bonus crearBonus(float centroX, float centroY, int tipoBonus)
{
	Bonus nuevoB;

	nuevoB.centroXBonus = centroX;
	nuevoB.centroYBonus = centroY;
	nuevoB.tipo = tipoBonus;
	nuevoB.agarrado = false;
	nuevoB.lanzado = false;

	if (tipoBonus == 1){
		nuevoB.puntos[0].push_back(centroX - tamBonus);			        // Posición x del primer punto. 
		nuevoB.puntos[0].push_back(centroY - tamBonus);					// Posición y del primer punto.
		nuevoB.puntos[1].push_back(centroX - tamBonus);        			// Posición x del segundo punto. 
		nuevoB.puntos[1].push_back(centroY + tamBonus);					// Posición y del segundo punto.
		nuevoB.puntos[2].push_back(centroX + tamBonus);				    // Posición x del tercer punto. 
		nuevoB.puntos[2].push_back(centroY + tamBonus);					// Posición y del tercer punto.
		nuevoB.puntos[3].push_back(centroX + tamBonus);					// Posición x del cuarto punto. 
		nuevoB.puntos[3].push_back(centroY - tamBonus);
	}

	return(nuevoB);
}

/* 
	Descripción:
		Permite generar los bloques "enemigos".
*/
void generarListaBloques(void)
{
	srand(time(0));  // Para que se puedan generar números aleatorios.
	float tempPosicionX = - posXIniParedLat + 2;  // Almacenará la posición actual X donde se generará el bloque.
							    				  // tempPosicion = posicionParedIzq + anchoPared + separación
	float tempPosicionY = 14;                     // Almacenará la posición actual Y donde se generará el bloque.

	int posBonus = 0;							  // Posición en el arreglo de bonus del bonus que se está creando.

	for(int i= 0; i < 5; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			bloqueIJ = 10 + 10*i + j;
			listaBloques[i][j].puntos[0].push_back(tempPosicionX);       // Posición x del primer punto. 
			listaBloques[i][j].puntos[0].push_back(tempPosicionY);       // Posición y del primer punto. 
			listaBloques[i][j].puntos[1].push_back(tempPosicionX);       // Posición x del segundo punto. 
			listaBloques[i][j].puntos[1].push_back(tempPosicionY - 0.6); // Posición y del segundo punto.
			listaBloques[i][j].puntos[2].push_back(tempPosicionX + 1.9); // Posición x del tercer punto. 
			listaBloques[i][j].puntos[2].push_back(tempPosicionY - 0.6); // Posición y del tercer punto.
			listaBloques[i][j].puntos[3].push_back(tempPosicionX + 1.9); // Posición x del cuarto punto. 
			listaBloques[i][j].puntos[3].push_back(tempPosicionY);       // Posición y del cuarto punto.
	
			// Se determina si el bloque tendrá bonus.
			if (numBloquesBon > 0){
				listaBloques[i][j].tieneBonus = randomBool(1); // Es 3
				numBloquesBon = listaBloques[i][j].tieneBonus ? numBloquesBon - 1 :  numBloquesBon;
			}

			// Se determina si el bloque será especial.
			if (numBloquesEsp > 0){
				listaBloques[i][j].esEspecial = randomBool(5);
				numBloquesEsp = listaBloques[i][j].esEspecial ? numBloquesEsp - 1 :  numBloquesEsp;
			}

			if (listaBloques[i][j].esEspecial) listaBloques[i][j].vida = 2;
			else listaBloques[i][j].vida = 1;

			if (listaBloques[i][j].tieneBonus){
				int bonusSeleccionado = randomBool(2);
				listaBonus[bloqueIJ] = crearBonus(listaBloques[i][j].puntos[1][0]+0.95,
												  listaBloques[i][j].puntos[1][1], 0);
			} 

			listaBloques[i][j].estaActivo = true;
			listaBloques[i][j].esJugador  = false;
			

			tempPosicionX += 3;
		}

		tempPosicionX  = - posXIniParedLat + 2;
		tempPosicionY -= 1.3;
	}

	cargoBloques = true;
}

/*
	Descripción:
		Permite verificar si existe colisión entre la pelota y los bloques "enemigos". En caso
		de que se produzca una colisión la pelota podrá cambiar su dirección en X y/o Y.
*/
void colisionPelotaBloques(void)
{
	/*------ FILA 1 ------ */
	if (listaBloques[0][0].estaActivo && (listaBloques[0][0].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[0][0].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[0][0].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][0].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][0].vida -= 1;
		listaBloques[0][0].vida == 0 ? listaBloques[0][0].estaActivo = false : listaBloques[0][0].estaActivo = true;

		velYPelota = -velYPelota;

		if (posXIniPlat + movPlat + (largoXPlat / 2) >= centroXPelota){ 
			if (listaBloques[0][0].puntos[2][1] - 7.2 < centroYPelota) velXPelota = -velXPelota;
		}else{
			if (listaBloques[0][0].puntos[0][1] - 7.2 > centroYPelota) velXPelota = -velXPelota;
		}

		if (listaBonus.find(10) != listaBonus.end()) 
		{
			listaBonus[10].lanzado = true;
			idBonus = 10;
			generarBonus();
		}

	}

	if (listaBloques[0][1].estaActivo && (listaBloques[0][1].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[0][1].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[0][1].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][1].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][1].vida -= 1;
		listaBloques[0][1].vida == 0 ? listaBloques[0][1].estaActivo = false : listaBloques[0][1].estaActivo = true;

		velYPelota = -velYPelota;

		if (posXIniPlat + movPlat + (largoXPlat / 2) >= centroXPelota){ 
			if (listaBloques[0][1].puntos[2][1] - 7.2 < centroYPelota) velXPelota = -velXPelota;
		}else{
			if (listaBloques[0][1].puntos[0][1] - 7.2 > centroYPelota) velXPelota = -velXPelota;
		}

		if (listaBonus.find(11) != listaBonus.end()) 
		{
			listaBonus[11].lanzado = true;
			idBonus = 11;
			generarBonus();
		}
	}

	if (listaBloques[0][2].estaActivo && (listaBloques[0][2].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[0][2].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[0][2].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][2].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][2].vida -= 1;
		listaBloques[0][2].vida == 0 ? listaBloques[0][2].estaActivo = false : listaBloques[0][2].estaActivo = true;

		velYPelota = -velYPelota;

		if (posXIniPlat + movPlat + (largoXPlat / 2) >= centroXPelota){ 
			if (listaBloques[0][2].puntos[2][1] - 7.2 < centroYPelota) velXPelota = -velXPelota;
		}else{
			if (listaBloques[0][2].puntos[0][1] - 7.2 > centroYPelota) velXPelota = -velXPelota;
		}

		if (listaBonus.find(12) != listaBonus.end()) 
		{
			listaBonus[12].lanzado = true;
			idBonus = 12;
			generarBonus();
		}
	}

	if (listaBloques[0][3].estaActivo && (listaBloques[0][3].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[0][3].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[0][3].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][3].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][3].vida -= 1;
		listaBloques[0][3].vida == 0 ? listaBloques[0][3].estaActivo = false : listaBloques[0][3].estaActivo = true;

		velYPelota = -velYPelota;

		if (posXIniPlat + movPlat + (largoXPlat / 2) >= centroXPelota){ 
			if (listaBloques[0][3].puntos[2][1] - 7.2 < centroYPelota) velXPelota = -velXPelota;
		}else{
			if (listaBloques[0][3].puntos[0][1] - 7.2 > centroYPelota) velXPelota = -velXPelota;
		}

		if (listaBonus.find(13) != listaBonus.end()) 
		{	
			listaBonus[13].lanzado = true;
			idBonus = 13;
			generarBonus();
		}
	}

	if (listaBloques[0][4].estaActivo && (listaBloques[0][4].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[0][4].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[0][4].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][4].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][4].vida -= 1;
		listaBloques[0][4].vida == 0 ? listaBloques[0][4].estaActivo = false : listaBloques[0][4].estaActivo = true;

		velYPelota = -velYPelota;

		if (posXIniPlat + movPlat + (largoXPlat / 2) >= centroXPelota){ 
			if (listaBloques[0][4].puntos[2][1] - 7.2 < centroYPelota) velXPelota = -velXPelota;
		}else{
			if (listaBloques[0][4].puntos[0][1] - 7.2 > centroYPelota) velXPelota = -velXPelota;
		}

		if (listaBonus.find(14) != listaBonus.end()) 
		{	
			listaBonus[14].lanzado = true;
			idBonus = 14;
			generarBonus();
		}
	}

	if (listaBloques[0][5].estaActivo && (listaBloques[0][5].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[0][5].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[0][5].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][5].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][5].vida -= 1;
		listaBloques[0][5].vida == 0 ? listaBloques[0][5].estaActivo = false : listaBloques[0][5].estaActivo = true;

		if (posXIniPlat + movPlat + (largoXPlat / 2) >= centroXPelota){ 
			if (listaBloques[0][5].puntos[2][1] - 7.2 < centroYPelota) velXPelota = -velXPelota;
		}else{
			if (listaBloques[0][5].puntos[0][1] - 7.2 > centroYPelota) velXPelota = -velXPelota;
		}

		velYPelota = -velYPelota;

		if (listaBonus.find(15) != listaBonus.end()) 
		{	
			listaBonus[15].lanzado = true;
			idBonus = 15;
			generarBonus();
		}
	}

	if (listaBloques[0][6].estaActivo && (listaBloques[0][6].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[0][6].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[0][6].puntos[0][1] - 7.2 > centroYPelota) && (listaBloques[0][6].puntos[2][1] - 7.2 < centroYPelota))
	{
		listaBloques[0][6].vida -= 1;
		listaBloques[0][6].vida == 0 ? listaBloques[0][6].estaActivo = false : listaBloques[0][6].estaActivo = true;

		velYPelota = -velYPelota;

		if (posXIniPlat + movPlat + (largoXPlat / 2) >= centroXPelota){ 
			if (listaBloques[0][6].puntos[2][1] - 7.2 < centroYPelota) velXPelota = -velXPelota;
		}else{
			if (listaBloques[0][6].puntos[0][1] - 7.2 > centroYPelota) velXPelota = -velXPelota;
		}

		if (listaBonus.find(16) != listaBonus.end()) 
		{	
			listaBonus[16].lanzado = true;
			idBonus = 16;
			generarBonus();
		}
	}
	/*------ FILA 2 ------ */
	/*if (listaBloques[1][0].estaActivo && (listaBloques[1][0].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[1][0].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[1][0].puntos[0][1] - 6.5 > centroYPelota) && (listaBloques[1][0].puntos[2][1] - 6.5 < centroYPelota))
	{
		listaBloques[1][0].vida -= 1;
		listaBloques[1][0].vida == 0 ? listaBloques[1][0].estaActivo = false : listaBloques[1][0].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(20) != listaBonus.end()){ 
			listaBonus[20].lanzado = true;
			idBonus = 20;
			generarBonus();
		}
	}

	if (listaBloques[1][1].estaActivo && (listaBloques[1][1].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[1][1].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[1][1].puntos[0][1] - 6.5 > centroYPelota) && (listaBloques[1][1].puntos[2][1] - 6.5 < centroYPelota))
	{
		listaBloques[1][1].vida -= 1;
		listaBloques[1][1].vida == 0 ? listaBloques[1][1].estaActivo = false : listaBloques[1][1].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(21) != listaBonus.end()){ 
			listaBonus[21].lanzado = true;
			idBonus = 21;
			generarBonus();
		}
	}

	if (listaBloques[1][2].estaActivo && (listaBloques[1][2].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[1][2].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[1][2].puntos[0][1] - 6.5 > centroYPelota) && (listaBloques[1][2].puntos[2][1] - 6.5 < centroYPelota))
	{
		listaBloques[1][2].vida -= 1;
		listaBloques[1][2].vida == 0 ? listaBloques[1][2].estaActivo = false : listaBloques[1][2].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(22) != listaBonus.end()){ 
			listaBonus[22].lanzado = true;
			idBonus = 22;
			generarBonus();
		}
	}

	if (listaBloques[1][3].estaActivo && (listaBloques[1][3].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[1][3].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[1][3].puntos[0][1] - 6.5 > centroYPelota) && (listaBloques[1][3].puntos[2][1] - 6.5 < centroYPelota))
	{
		listaBloques[1][3].vida -= 1;
		listaBloques[1][3].vida == 0 ? listaBloques[1][3].estaActivo = false : listaBloques[1][3].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(23) != listaBonus.end()){ 
			listaBonus[23].lanzado = true;
			idBonus = 23;
			generarBonus();
		}
	}

	if (listaBloques[1][4].estaActivo && (listaBloques[1][4].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[1][4].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[1][4].puntos[0][1] - 6.5 > centroYPelota) && (listaBloques[1][4].puntos[2][1] - 6.5 < centroYPelota))
	{
		listaBloques[1][4].vida -= 1;
		listaBloques[1][4].vida == 0 ? listaBloques[1][4].estaActivo = false : listaBloques[1][4].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(24) != listaBonus.end()){ 
			listaBonus[24].lanzado = true;
			idBonus = 24;
			generarBonus();
		}
	}

	if (listaBloques[1][5].estaActivo && (listaBloques[1][5].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[1][5].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[1][5].puntos[0][1] - 6.5 > centroYPelota) && (listaBloques[1][5].puntos[2][1] - 6.5 < centroYPelota))
	{
		listaBloques[1][5].vida -= 1;
		listaBloques[1][5].vida == 0 ? listaBloques[1][5].estaActivo = false : listaBloques[1][5].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(25) != listaBonus.end()){ 
			listaBonus[25].lanzado = true;
			idBonus = 25;
			generarBonus();
		}
	}

	if (listaBloques[1][6].estaActivo && (listaBloques[1][6].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[1][6].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[1][6].puntos[0][1] - 6.5 > centroYPelota) && (listaBloques[1][6].puntos[2][1] - 6.5 < centroYPelota))
	{
		listaBloques[1][6].vida -= 1;
		listaBloques[1][6].vida == 0 ? listaBloques[1][6].estaActivo = false : listaBloques[1][6].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(26) != listaBonus.end()){ 
			listaBonus[26].lanzado = true;
			idBonus = 26;
			generarBonus();
		}
	}*/
	/*------ FILA 3 ------ */
	/*if (listaBloques[2][0].estaActivo && (listaBloques[2][0].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[2][0].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[2][0].puntos[0][1] - 5.8 > centroYPelota) && (listaBloques[2][0].puntos[2][1] - 5.8 < centroYPelota))
	{
		listaBloques[2][0].vida -= 1;
		listaBloques[2][0].vida == 0 ? listaBloques[2][0].estaActivo = false : listaBloques[2][0].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(30) != listaBonus.end()){ 
			listaBonus[30].lanzado = true;
			idBonus = 30;
			generarBonus();
		}
	}

	if (listaBloques[2][1].estaActivo && (listaBloques[2][1].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[2][1].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[2][1].puntos[0][1] - 5.8 > centroYPelota) && (listaBloques[2][1].puntos[2][1] - 5.8 < centroYPelota))
	{
		listaBloques[2][1].vida -= 1;
		listaBloques[2][1].vida == 0 ? listaBloques[2][1].estaActivo = false : listaBloques[2][1].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(31) != listaBonus.end()){ 
			listaBonus[31].lanzado = true;
			idBonus = 31;
			generarBonus();
		}
	}

	if (listaBloques[2][2].estaActivo && (listaBloques[2][2].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[2][2].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[2][2].puntos[0][1] - 5.8 > centroYPelota) && (listaBloques[2][2].puntos[2][1] - 5.8 < centroYPelota))
	{
		listaBloques[2][2].vida -= 1;
		listaBloques[2][2].vida == 0 ? listaBloques[2][2].estaActivo = false : listaBloques[2][2].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(32) != listaBonus.end()){ 
			listaBonus[32].lanzado = true;
			idBonus = 32;
			generarBonus();
		}
	}

	if (listaBloques[2][3].estaActivo && (listaBloques[2][3].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[2][3].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[2][3].puntos[0][1] - 5.8 > centroYPelota) && (listaBloques[2][3].puntos[2][1] - 5.8 < centroYPelota))
	{
		listaBloques[2][3].vida -= 1;
		listaBloques[2][3].vida == 0 ? listaBloques[2][3].estaActivo = false : listaBloques[2][3].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(33) != listaBonus.end()){ 
			listaBonus[33].lanzado = true;
			idBonus = 33;
			generarBonus();
		}
	}

	if (listaBloques[2][4].estaActivo && (listaBloques[2][4].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[2][4].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[2][4].puntos[0][1] - 5.8 > centroYPelota) && (listaBloques[2][4].puntos[2][1] - 5.8 < centroYPelota))
	{
		listaBloques[2][4].vida -= 1;
		listaBloques[2][4].vida == 0 ? listaBloques[2][4].estaActivo = false : listaBloques[2][4].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(34) != listaBonus.end()){ 
			listaBonus[34].lanzado = true;
			idBonus = 34;
			generarBonus();
		}
	}

	if (listaBloques[2][5].estaActivo && (listaBloques[2][5].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[2][5].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[2][5].puntos[0][1] - 5.8 > centroYPelota) && (listaBloques[2][5].puntos[2][1] - 5.8 < centroYPelota))
	{
		listaBloques[2][5].vida -= 1;
		listaBloques[2][5].vida == 0 ? listaBloques[2][5].estaActivo = false : listaBloques[2][5].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(35) != listaBonus.end()){ 
			listaBonus[35].lanzado = true;
			idBonus = 35;
			generarBonus();
		}
	}

	if (listaBloques[2][6].estaActivo && (listaBloques[2][6].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[2][6].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[2][6].puntos[0][1] - 5.8 > centroYPelota) && (listaBloques[2][6].puntos[2][1] - 5.8 < centroYPelota))
	{
		listaBloques[2][6].vida -= 1;
		listaBloques[2][6].vida == 0 ? listaBloques[2][6].estaActivo = false : listaBloques[2][6].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(36) != listaBonus.end()){ 
			listaBonus[36].lanzado = true;
			idBonus = 36;
			generarBonus();
		}
	}*/
	
	/*------ FILA 4 ------ */
	/*if (listaBloques[3][0].estaActivo && (listaBloques[3][0].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[3][0].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[3][0].puntos[0][1] - 5.2 > centroYPelota) && (listaBloques[3][0].puntos[2][1] - 5.2 < centroYPelota))
	{
		listaBloques[3][0].vida -= 1;
		listaBloques[3][0].vida == 0 ? listaBloques[3][0].estaActivo = false : listaBloques[3][0].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(40) != listaBonus.end()){ 
			listaBonus[40].lanzado = true;
			idBonus = 40;
			generarBonus();
		}
	}

	if (listaBloques[3][1].estaActivo && (listaBloques[3][1].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[3][1].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[3][1].puntos[0][1] - 5.2 > centroYPelota) && (listaBloques[3][1].puntos[2][1] - 5.2 < centroYPelota))
	{
		listaBloques[3][1].vida -= 1;
		listaBloques[3][1].vida == 0 ? listaBloques[3][1].estaActivo = false : listaBloques[3][1].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(41) != listaBonus.end()){ 
			listaBonus[41].lanzado = true;
			idBonus = 41;
			generarBonus();
		}
	}

	if (listaBloques[3][2].estaActivo && (listaBloques[3][2].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[3][2].puntos[3][0] + 0.1 > centroXPelota)
		&& (listaBloques[3][2].puntos[0][1] - 5.2 > centroYPelota) && (listaBloques[3][2].puntos[2][1] - 5.2 < centroYPelota))
	{
		listaBloques[3][2].vida -= 1;
		listaBloques[3][2].vida == 0 ? listaBloques[3][2].estaActivo = false : listaBloques[3][2].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(42) != listaBonus.end()){ 
			listaBonus[42].lanzado = true;
			idBonus = 42;
			generarBonus();
		}
	}

	if (listaBloques[3][3].estaActivo && (listaBloques[3][3].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[3][3].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[3][3].puntos[0][1] - 5.2 > centroYPelota) && (listaBloques[3][3].puntos[2][1] - 5.2 < centroYPelota))
	{
		listaBloques[3][3].vida -= 1;
		listaBloques[3][3].vida == 0 ? listaBloques[3][3].estaActivo = false : listaBloques[3][3].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(43) != listaBonus.end()){ 
			listaBonus[43].lanzado = true;
			idBonus = 43;
			generarBonus();
		}
	}

	if (listaBloques[3][4].estaActivo && (listaBloques[3][4].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[3][4].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[3][4].puntos[0][1] - 5.2 > centroYPelota) && (listaBloques[3][4].puntos[2][1] - 5.2 < centroYPelota))
	{
		listaBloques[3][4].vida -= 1;
		listaBloques[3][4].vida == 0 ? listaBloques[3][4].estaActivo = false : listaBloques[3][4].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(44) != listaBonus.end()){ 
			listaBonus[44].lanzado = true;
			idBonus = 45;
			generarBonus();
		}
	}

	if (listaBloques[3][5].estaActivo && (listaBloques[3][5].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[3][5].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[3][5].puntos[0][1] - 5.2 > centroYPelota) && (listaBloques[3][5].puntos[2][1] - 5.2 < centroYPelota))
	{
		listaBloques[3][5].vida -= 1;
		listaBloques[3][5].vida == 0 ? listaBloques[3][5].estaActivo = false : listaBloques[3][5].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(45) != listaBonus.end()){ 
			listaBonus[45].lanzado = true;
			idBonus = 45;
			generarBonus();
		}
	}

	if (listaBloques[3][6].estaActivo && (listaBloques[3][6].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[3][6].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[3][6].puntos[0][1] - 5.2 > centroYPelota) && (listaBloques[3][6].puntos[2][1] - 5.2 < centroYPelota))
	{
		listaBloques[3][6].vida -= 1;
		listaBloques[3][6].vida == 0 ? listaBloques[3][6].estaActivo = false : listaBloques[3][6].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(46) != listaBonus.end()){ 
			listaBonus[46].lanzado = true;
			idBonus = 46;
			generarBonus();
		}
	}
	*/
	/*------ FILA 5 ------ */
	/*if (listaBloques[4][0].estaActivo && (listaBloques[4][0].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[4][0].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[4][0].puntos[0][1] - 4.6 > centroYPelota) && (listaBloques[4][0].puntos[2][1] - 4.6 < centroYPelota))
	{
		listaBloques[4][0].vida -= 1;
		listaBloques[4][0].vida == 0 ? listaBloques[4][0].estaActivo = false : listaBloques[4][0].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(50) != listaBonus.end()){ 
			listaBonus[50].lanzado = true;
			idBonus = 50;
			generarBonus();
		}
	}

	if (listaBloques[4][1].estaActivo && (listaBloques[4][1].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[4][1].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[4][1].puntos[0][1] - 4.6 > centroYPelota) && (listaBloques[4][1].puntos[2][1] - 4.6 < centroYPelota))
	{
		listaBloques[4][1].vida -= 1;
		listaBloques[4][1].vida == 0 ? listaBloques[4][1].estaActivo = false : listaBloques[4][1].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(51) != listaBonus.end()){ 
			listaBonus[51].lanzado = true;
			idBonus = 51;
			generarBonus();
		}
	}

	if (listaBloques[4][2].estaActivo && (listaBloques[4][2].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[4][2].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[4][2].puntos[0][1] - 4.6 > centroYPelota) && (listaBloques[4][2].puntos[2][1] - 4.6 < centroYPelota))
	{
		listaBloques[4][2].vida -= 1;
		listaBloques[4][2].vida == 0 ? listaBloques[4][2].estaActivo = false : listaBloques[4][2].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(52) != listaBonus.end()){ 
			listaBonus[52].lanzado = true;
			idBonus = 52;
			generarBonus();
		}
	}

	if ((listaBloques[4][3].estaActivo) && (listaBloques[4][3].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[4][3].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[4][3].puntos[0][1] - 4.6 > centroYPelota) && (listaBloques[4][3].puntos[2][1] - 4.6 < centroYPelota))
	{
		listaBloques[4][3].vida -= 1;
		listaBloques[4][3].vida == 0 ? listaBloques[4][3].estaActivo = false : listaBloques[4][3].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(53) != listaBonus.end()){ 
			listaBonus[53].lanzado = true;
			idBonus = 53;
			generarBonus();
		}
	}

	if (listaBloques[4][4].estaActivo && (listaBloques[4][4].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[4][4].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[4][4].puntos[0][1] - 4.6 > centroYPelota) && (listaBloques[4][4].puntos[2][1] - 4.6 < centroYPelota))
	{
		listaBloques[4][4].vida -= 1;
		listaBloques[4][4].vida == 0 ? listaBloques[4][4].estaActivo = false : listaBloques[4][4].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(54) != listaBonus.end()){ 
			listaBonus[54].lanzado = true;
			idBonus = 54;
			generarBonus();
		}
	}

	if (listaBloques[4][5].estaActivo && (listaBloques[4][5].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[4][5].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[4][5].puntos[0][1] - 4.6 > centroYPelota) && (listaBloques[4][5].puntos[2][1] - 4.6 < centroYPelota))
	{
		listaBloques[4][5].vida -= 1;
		listaBloques[4][5].vida == 0 ? listaBloques[4][5].estaActivo = false : listaBloques[4][5].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(55) != listaBonus.end()){ 
			listaBonus[55].lanzado = true;
			idBonus = 55;
			generarBonus();
		}
	}

	if (listaBloques[4][6].estaActivo && (listaBloques[4][6].puntos[0][0] - 0.1 < centroXPelota) && (listaBloques[4][6].puntos[3][0] + 0.2 > centroXPelota)
		&& (listaBloques[4][6].puntos[0][1] - 4.6 > centroYPelota) && (listaBloques[4][6].puntos[2][1] - 4.6 < centroYPelota))
	{
		listaBloques[4][6].vida -= 1;
		listaBloques[4][6].vida == 0 ? listaBloques[4][6].estaActivo = false : listaBloques[4][6].estaActivo = true;

		velXPelota = -velXPelota;
		velYPelota = -velYPelota;

		if (listaBonus.find(56) != listaBonus.end()){ 
			listaBonus[56].lanzado = true;
			idBonus = 56;
			generarBonus();
		}
	}
	*/

	/*if (listaBonus[idBonus].centroYBonus == posYIniPlat)
	{
		if (true) // en centro en x esta en la plataforma
		{
			listaBonus[idBonus].agarrado = true;
			if (listaBonus[idBonus].tipo == 0)
			{
				printf("entre");
				velXPelota = velXPelota*aumentVel;
				velYPelota = velYPelota*aumentVel;	
			}
			else if (listaBonus[idBonus].tipo == 1)
			{
				largoXPlat = largoXPlat*disminTam;	
			}
		}
	}*/

}

/*
	Descripción:
		Permite verificar si la pelota está chocando con los diferentes objetos del juego.
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

	// Colisión techo.
	if (centroYPelota + radPelota > posYIniTecho - 8.15)
	{
		centroYPelota = posYIniTecho - radPelota - 8.15;
		velYPelota = -velYPelota;
	}

	// Colisión plataforma en X.
	if  ((centroYPelota < 0 && centroYPelota > - largoYPlat + 0.1) && (posXIniPlat + movPlat - 0.1 <= centroXPelota) && 
		 (- posXIniPlat + movPlat + 0.1 >= centroXPelota)){
			 velYPelota = -velYPelota;
		if (posXIniPlat + movPlat + (largoXPlat / 2) >= centroXPelota){ 
			if (direccion == -1) velXPelota = -velXPelota;
		}else{
			if (direccion == 1) velXPelota = -velXPelota;
		}
	}

	colisionPelotaBloques(); // Se verifican las colisiones entre la pelota y los bloques.
}

void actualizacionBloques(void){
	for(int i= 0; i < 5; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			bloqueIJ = 10 + 10*i + j;
			glNewList(bloqueIJ, GL_COMPILE);
				glBegin(GL_QUADS);
					glPushMatrix();
						if (listaBloques[i][j].estaActivo){
							if (listaBloques[i][j].esEspecial){
								if (listaBloques[i][j].vida == 2) glColor3f(1.0, 0.0, 0.0);
								else glColor3f(1.0, 1.0, 1.0); // Color bloque roto.
							}else glColor3f(0.96, 0.67, 0.2);
						}	
						else{
							glColor3f(0.0, 0.0, 0.0);
						}
						
						if (listaBloques[i][j].esEspecial && listaBloques[i][j].vida == 1){ // Código del bloque roto.
							// Mitad izquierda del bloque roto
							glVertex2f(listaBloques[i][j].puntos[0][0], listaBloques[i][j].puntos[0][1]);
							glVertex2f(listaBloques[i][j].puntos[1][0], listaBloques[i][j].puntos[1][1]);
							glVertex2f(listaBloques[i][j].puntos[1][0]+0.6, listaBloques[i][j].puntos[1][1]);
							glVertex2f(listaBloques[i][j].puntos[0][0]+0.9, listaBloques[i][j].puntos[0][1]);
							// Mitad derecha del bloque roto
							glVertex2f(listaBloques[i][j].puntos[3][0]-0.6, listaBloques[i][j].puntos[3][1]);
							glVertex2f(listaBloques[i][j].puntos[2][0]-0.9, listaBloques[i][j].puntos[2][1]);
							glVertex2f(listaBloques[i][j].puntos[2][0], listaBloques[i][j].puntos[2][1]);
							glVertex2f(listaBloques[i][j].puntos[3][0], listaBloques[i][j].puntos[3][1]);
						}else{
							glVertex2f(listaBloques[i][j].puntos[0][0], listaBloques[i][j].puntos[0][1]);
							glVertex2f(listaBloques[i][j].puntos[1][0], listaBloques[i][j].puntos[1][1]);
							glVertex2f(listaBloques[i][j].puntos[2][0], listaBloques[i][j].puntos[2][1]);
							glVertex2f(listaBloques[i][j].puntos[3][0], listaBloques[i][j].puntos[3][1]);
						}
					glPopMatrix();
				glEnd();
			glEndList();
		}
	}
}

/*
	Descripción:
		permite crear el bonus
*/
void dibujarBonus(int id, float tam)
{
	// Se definen los puntos que generan al bonus.
	// ( Cada vez que le hace push a un vector se le agrega una nueva coordenada.)
	if (listaBonus[id].tipo == 0)
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
				glVertex2d(x + listaBonus[id].centroXBonus, y + listaBonus[id].centroYBonus);

				temp = x;
				x = coseno*x - seno*y;
				y = seno*temp + coseno*y;
			}
		glEnd();
	}
	else if (listaBonus[id].tipo == 1)
	{
		glBegin(GL_QUADS);
			glVertex2f(bonus[id].puntos[0][0], bonus[id].puntos[0][1]);
			glVertex2f(bonus[id].puntos[1][0], bonus[id].puntos[1][1]);
			glVertex2f(bonus[id].puntos[2][0], bonus[id].puntos[2][1]);
			glVertex2f(bonus[id].puntos[3][0], bonus[id].puntos[3][1]);
		glEnd();
	}
}

void generarBonus(void)
{
	glNewList(listaBonus[idBonus].id + 60, GL_COMPILE);
		glPushMatrix();
			glColor3f(0.93, 0.0, 0.93);
			dibujarBonus(idBonus, tamBonus);
		glPopMatrix();
	glEndList();
}

/*
	Descripción:
		Permite compilar los comandos que se utilizarán para crear el 
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
	actualizacionBloques();
}

/*
	Descripción:
		Permite ejecutar los comandos que se utilizarán para crear el 
		juego.
*/
void ejecutarJuego(void)
{
	// Transformación para que se pueda ver mejor en la pantalla.
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

		// Si se comenzó el juego.
		if (comenzoJuego) {
			centroXPelota += velXPelota;
			centroYPelota += velYPelota;

			revisarColisionPelota();
		}
	glPopMatrix();

	glPushMatrix();
		if (listaBonus.find(10) != listaBonus.end() && listaBonus[10].lanzado){ 
			glCallList(60 + listaBonus[10].id);
			listaBonus[10].centroYBonus += velBonus;
		};
		if (listaBonus.find(11) != listaBonus.end() && listaBonus[11].lanzado){ 
			glCallList(60 + listaBonus[11].id);
			listaBonus[11].centroYBonus += velBonus;
		};
		if (listaBonus.find(12) != listaBonus.end() && listaBonus[12].lanzado){ 
			glCallList(60 + listaBonus[12].id);
			listaBonus[12].centroYBonus += velBonus;
		};
		if (listaBonus.find(13) != listaBonus.end() && listaBonus[13].lanzado){ 
			glCallList(60 + listaBonus[13].id);
			listaBonus[13].centroYBonus += velBonus;
		};
		if (listaBonus.find(14) != listaBonus.end() && listaBonus[14].lanzado){ 
			glCallList(60 + listaBonus[14].id);
			listaBonus[14].centroYBonus += velBonus;
		};
		if (listaBonus.find(15) != listaBonus.end() && listaBonus[15].lanzado){ 
			glCallList(60 + listaBonus[15].id);
			listaBonus[15].centroYBonus += velBonus;
		};
		if (listaBonus.find(16) != listaBonus.end() && listaBonus[16].lanzado){ 
			glCallList(60 + listaBonus[16].id);
			listaBonus[16].centroYBonus += velBonus;
		};
		/*
		if (listaBonus.find(20) != listaBonus.end() && listaBonus[20].lanzado) glCallList(60 + listaBonus[20].id);
		if (listaBonus.find(21) != listaBonus.end() && listaBonus[21].lanzado) glCallList(60 + listaBonus[21].id);
		if (listaBonus.find(22) != listaBonus.end() && listaBonus[22].lanzado) glCallList(60 + listaBonus[22].id);
		if (listaBonus.find(23) != listaBonus.end() && listaBonus[23].lanzado) glCallList(60 + listaBonus[23].id);
		if (listaBonus.find(24) != listaBonus.end() && listaBonus[24].lanzado) glCallList(60 + listaBonus[24].id);
		if (listaBonus.find(25) != listaBonus.end() && listaBonus[25].lanzado) glCallList(60 + listaBonus[25].id);
		if (listaBonus.find(26) != listaBonus.end() && listaBonus[26].lanzado) glCallList(60 + listaBonus[26].id);

		if (listaBonus.find(30) != listaBonus.end() && listaBonus[30].lanzado) glCallList(60 + listaBonus[30].id);
		if (listaBonus.find(31) != listaBonus.end() && listaBonus[31].lanzado) glCallList(60 + listaBonus[31].id);
		if (listaBonus.find(32) != listaBonus.end() && listaBonus[32].lanzado) glCallList(60 + listaBonus[32].id);
		if (listaBonus.find(33) != listaBonus.end() && listaBonus[33].lanzado) glCallList(60 + listaBonus[33].id);
		if (listaBonus.find(34) != listaBonus.end() && listaBonus[34].lanzado) glCallList(60 + listaBonus[34].id);
		if (listaBonus.find(35) != listaBonus.end() && listaBonus[35].lanzado) glCallList(60 + listaBonus[35].id);
		if (listaBonus.find(36) != listaBonus.end() && listaBonus[36].lanzado) glCallList(60 + listaBonus[36].id);

		if (listaBonus.find(40) != listaBonus.end() && listaBonus[40].lanzado) glCallList(60 + listaBonus[40].id);
		if (listaBonus.find(41) != listaBonus.end() && listaBonus[41].lanzado) glCallList(60 + listaBonus[41].id);
		if (listaBonus.find(42) != listaBonus.end() && listaBonus[42].lanzado) glCallList(60 + listaBonus[42].id);
		if (listaBonus.find(43) != listaBonus.end() && listaBonus[43].lanzado) glCallList(60 + listaBonus[43].id);
		if (listaBonus.find(44) != listaBonus.end() && listaBonus[44].lanzado) glCallList(60 + listaBonus[44].id);
		if (listaBonus.find(45) != listaBonus.end() && listaBonus[45].lanzado) glCallList(60 + listaBonus[45].id);
		if (listaBonus.find(46) != listaBonus.end() && listaBonus[46].lanzado) glCallList(60 + listaBonus[46].id);

		if (listaBonus.find(50) != listaBonus.end() && listaBonus[50].lanzado) glCallList(60 + listaBonus[50].id);
		if (listaBonus.find(51) != listaBonus.end() && listaBonus[51].lanzado) glCallList(60 + listaBonus[51].id);
		if (listaBonus.find(52) != listaBonus.end() && listaBonus[52].lanzado) glCallList(60 + listaBonus[52].id);
		if (listaBonus.find(53) != listaBonus.end() && listaBonus[53].lanzado) glCallList(60 + listaBonus[53].id);
		if (listaBonus.find(54) != listaBonus.end() && listaBonus[54].lanzado) glCallList(60 + listaBonus[54].id);
		if (listaBonus.find(55) != listaBonus.end() && listaBonus[55].lanzado) glCallList(60 + listaBonus[55].id);
		if (listaBonus.find(56) != listaBonus.end() && listaBonus[56].lanzado) glCallList(60 + listaBonus[56].id);
		*/
	glPopMatrix();
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
	
	// Permite saber que tecla es seleccionada.
	glutKeyboardFunc(teclaPresionada);
	glutSpecialFunc(flechaPresionada);

	//ejesCoordenada(2.0);
	if (!cargoBloques) generarListaBloques();
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