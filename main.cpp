#include<iostream>
#include<sstream>
#include<ctime>
#include<string>
#include <conio.h>

#include <allegro.h>
///ALLEGROBITMAP *buffer, *cuadro, *fichaB, *fichaN;

using namespace std;
#include "Mesa.h"
pair<int, int> minimaxDecision(Mesa *b, int cpuval);
int valorMax(Mesa *b, int cpuval, int alpha, int beta, int depth, int maxdepth, time_t start);
int valorMin(Mesa *b, int cpuval, int alpha, int beta, int depth, int maxdepth, time_t start);

void pantalla(){
    ///ALLEGROblit(buffer, screen, 0, 0, 0,0, 640,480);
}

bool hacerMovimientoSimpleCPU(Mesa *b, int cpuval) {
	for(int i=1; i<9;i++)
		for(int j=1; j<9; j++)
			if(b->getValorPosicion(i, j)==0)
				if(b->jugarTablero(i, j, cpuval))
					return true;
	cout << "Computadora pasa." << endl;
	return false;
}

bool hacerMovimientoInteligenteCPU(Mesa *b, int cpuval) {
	pair<int,int> temp = minimaxDecision(b, cpuval);
	if (b->getValorPosicion(temp.first, temp.second) == 0) {
		if (b->jugarTablero(temp.first, temp.second, cpuval))
			return true;
	}
	cout << "Computer passes." <<  endl;
	return false;
}

pair<int, int> minimaxDecision(Mesa *b, int cpuval) {
	// devuelve un par <int, int> <i, j> para la fila, columna del mejor movimiento
	Mesa *bt = new Mesa();
	bt->setTablero(b);

	int tempval;

	// computadora siempre tratando de maximizar la funci�n evaluar
    // necesita un n�mero m�s alto que el que evaluar, este puede ser tratado como el valor inicial m�ximo
	int maxval = 9000;
	int maxi;
	int maxj;

	bool nomove = true;

	int profundidad = 0; // profundizaci�n iterativa
	// iniciar reloj
	time_t start; time_t now;
	time(&start);

	//int temp;

	while (true) {
		profundidad++; //aumentar nuestro l�mite de profundidad

		for (int i = 1; i < 9; i++) {
			for (int j = 1; j < 9; j++) {
				if (bt->getValorPosicion(i, j)==0) {
					if (bt->jugarTablero(i, j, cpuval)) {

						tempval = valorMax(bt, cpuval, 9000, -9000, 1, profundidad, start); // inicia alpha en 9000, beta en -9000
						if (tempval <= maxval) { // Encontr� un nuevo valor maximo minimo
							nomove = false;
							maxi = i;
							maxj = j;
							maxval = tempval;
						}
						bt->setTablero(b); // De cualquier forma, borra la jugada y prueba la siguiente.
					}
				}
			}
		}
		time(&now);
		if (difftime(now, start) >= 5)
			break;

	}

	pair<int, int> ret;


	if (nomove) {
		nomove = false;
		maxi = 1; // solo devolver algo para que la computadora pueda pasar
		maxj = 1;
	}

	ret.first = maxi;
	ret.second = maxj;

	return ret;
}

int valorMax(Mesa *b, int cpuval, int alpha, int beta, int profundidad, int max_profundidad, time_t start) {
	// puntaje y heur�stica del tablero actual si es terminal
    // 2 formas de ser terminal: el juego ha terminado (generalmente no es el caso hasta el final del juego)
    // o alcanzado el l�mite de profundidad

    // adicionalmente, si se alcanza el l�mite de tiempo de 5 segundos, devuelva la heur�stica

    // si el juego termina y la computadora gana, devuelve el n�mero m�ximo posible (9000)
    // si el juego termina y el jugador gana, -9000
    // si empate, 0
	if (b->tableroLleno() || (!(b->tieneMovimientoValido(cpuval)) && !(b->tieneMovimientoValido(-1*cpuval)))) {
		int score = b->score();
		if(score==0)
			return 0;
		else if((score>0 && (cpuval == 1)) || (score<0 && (cpuval == -1)))
			return 9000;
		else
			return -9000;
	}

	// alcanzado el l�mite de profundidad o el l�mite de tiempo, calificar el tablero de acuerdo con la funci�n heur�stica
	time_t now;
	time(&now);
	if (profundidad == max_profundidad || difftime(now, start) >= 5)
		return b->evaluar(cpuval, profundidad);

	// maximizar el valor m�nimo de los sucesores
	int minval = beta;
	int tempval;

	Mesa *bt = new Mesa();
	bt->setTablero(b);

	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			if (b->getValorPosicion(i, j)==0) {
				if (b->jugarTablero(i, j, -1*cpuval)) { // ya que este es el turno del jugador, cambia el valor

					tempval = valorMin(b, cpuval, alpha, minval, profundidad+1, max_profundidad, start); // nuevo alpha/beta correspondiente, nuestro minval siempre ser�> = beta
					if (tempval >= minval) { //encontr� un nuevo valor m�ximo m�nimo
						minval = tempval;
					}

					b->setTablero(bt); // De cualquier forma, borra la jugada y prueba la siguiente.

					if (minval > alpha) {
						return alpha;
					}
				}
			}
		}
	}
	return minval;
}

int valorMin(Mesa *b, int cpuval, int alpha, int beta, int profundidad, int max_profundidad, time_t start) {
	// puntaje y heur�stica del tablero actual si es terminal

    // 2 formas de ser terminal: el juego ha terminado o ya no hay movimientos v�lidos (generalmente no es el caso hasta el final del juego)
    // o alcanzado el l�mite de profundidad

    // adicionalmente, si se alcanza el l�mite de tiempo (20 s), simplemente vuelva a anotar

    // si el juego termina y la computadora gana, devuelve el n�mero m�ximo posible (9000)
    // si el juego termina y el jugador gana, -9000
    // si el juego termina y nadie gana, punt�a 0
	if (b->tableroLleno() || (!(b->tieneMovimientoValido(cpuval)) && !(b->tieneMovimientoValido(-1*cpuval)))) {
		int score = b->score();
		if(score==0)
			return 0;
		else if((score>0 && (cpuval == 1)) || (score<0 && (cpuval == -1)))
			return 9000;
		else
			return -9000;
	}

	time_t now;
	time(&now);
	// Alcanz� el l�mite de profundidad, calificar el tablero seg�n la funci�n heur�stica.
	if (profundidad == max_profundidad || difftime(now, start) >= 20)
		return b->evaluar(cpuval, profundidad);

	// Minimizar el valor m�ximo de los sucesores.
	int maxval = alpha;
	int tempval;

	Mesa *bt = new Mesa();
	bt->setTablero(b);

	for (int i = 1; i < 9; i++) {
		for (int j = 1; j < 9; j++) {
			if (b->getValorPosicion(i, j)==0) {
				if (b->jugarTablero(i, j, cpuval)) {

					tempval = valorMax(b, cpuval, maxval, beta, profundidad+1, max_profundidad, start); // nuestro maxval siempre <= alpha
					if (tempval <= maxval) { // encontr� un nuevo valor m�ximo m�nimo
						maxval = tempval;
					}

					b->setTablero(bt); // De cualquier forma, borra la jugada y prueba la siguiente.

					if (maxval < beta) {
						return beta;
					}
				}
			}
		}
	}
	return maxval;
}

void play(int cpuval) {
	Mesa *b = new Mesa();
	int humanPlayer = -1*cpuval;
	int cpuPlayer = cpuval;

	b->imprimir();
	///ALLEGRO blit(buffer, screen, 0, 0, 0,0, 640,480);
	int pases = 0;

	int row, col;

	if (cpuPlayer == -1) { // cpu plays second
		while(!b->tableroLleno() && pases<2) {
			//check if player must pass:
			if(!b->tieneMovimientoValido(humanPlayer)) {
				cout << "Tu debes pasar." << endl;
				pases++;
			}
			else {
				pases = 0;
				cout << "Fila (1-8): ";
				cin >> row;
				cout << "Columna (1-8): ";
				cin >> col;
				if(!b->jugarTablero(row, col, humanPlayer)) {
					cout << "Movimiento no permitido." << endl;
					continue;
				}
				system("cls");
			}
			//move for computer:
			if(b->tableroLleno())
				break;
			else {
				b->imprimir(); cout<< "..." << endl;
				///ALLEGRO blit(buffer, screen, 0, 0, 0,0, 640,480);

				/*if(hacerMovimientoSimpleCPU(b, cpuPlayer))
					pases=0;*/
				if(hacerMovimientoInteligenteCPU(b, cpuPlayer))
					pases=0;
				else
					pases++;
                b->imprimir();
                ///ALLEGRO blit(buffer, screen, 0, 0, 0,0, 640,480);
			}
		}
	}
	/*** NO BORRAR, EN CONSTRUCCION ***/
	/*else { // cpu juega primero
		while(!b->tableroLleno() && pases<2) {
			//mover para cpu:
			if(b->tableroLleno())
				break;
			else {
				cout << "..." << endl;
				//if(hacerMovimientoSimpleCPU(b, cpuPlayer))
				//	pases=0;
				if(hacerMovimientoInteligenteCPU(b, cpuPlayer))
					pases=0;
				else
					pases++;
				b->imprimir();
			}

			//comprobar si el jugador pasa:
			if(!b->tieneMovimientoValido(humanPlayer)) {
				cout << "Tu debes pasar." << endl;
				pases++;
			}
			else {
				pases = 0;
				while (true) {
					cout << "Fila (1-8): ";
					cin >> row;
					cout << "Columna (1-8): ";
					cin >> col;
					if(!b->jugarTablero(row, col, humanPlayer)) {
						cout << "Movimiento no permitido." << endl;
					}
					else
						break;
				}
				b->imprimir();
			}
		}
	}*/

	/*int score = b->score();
	if(score==0)
		cout << "Tie game." << endl;
	else if((score>0 && (cpuval == 1)) || (score<0 && (cpuval == -1)))
		cout << "Computadora gana by "  << endl;
	else
		cout << "Player gana by "  << endl;
	char a;
	cin >> a;*/
	getch();
	/**********************************/
}

int main(){
    ///ALLEGROallegro_init();
    ///ALLEGROinstall_keyboard();
    ///ALLEGROset_color_depth(32);
    ///ALLEGROset_gfx_mode(GFX_AUTODETECT_WINDOWED, 640,480,0,0);

    ///ALLEGRObuffer = create_bitmap(640, 480);
    ///ALLEGROfichaN = load_bitmap("Comida.bmp", NULL);
    ///ALLEGROfichaB = load_bitmap("enemigo.bmp", NULL);
    ///ALLEGROcuadro = load_bitmap("roca.bmp", NULL);

	//cout << "Presiona Y si quieres empezar primero." << endl;

	char a='y';
	//cin >> a;

	if (a == 'y' || a == 'Y') {
		play(-1); // valor del cpu = -1
	}
	else
		play(1);
	return 0;
}

END_OF_MAIN();
