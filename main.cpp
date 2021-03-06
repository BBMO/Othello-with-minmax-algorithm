#include<iostream>
#include<sstream>
#include<ctime>
#include<string>
#include <conio.h>

#include <allegro.h>
BITMAP *buffer, *aside, *cuadro, *fichaB, *fichaN, *cursor, *hover, *exitButton, *exitButtonHover, *turno;
BITMAP *menuBase, *menu2Base, *menuPVIA, *menuPVP, *menuSalir, *menuNegro, *menuBlanco, *youlose, *youwin, *p1win, *p2win, *empate;
FONT *font1, *font2, *font3;

using namespace std;
int contBlancas, contNegras;
#include "Mesa.h"
pair<int, int> minimaxDecision(Mesa *b, int cpuval);
int valorMax(Mesa *b, int cpuval, int alpha, int beta, int depth, int maxdepth, time_t start);
int valorMin(Mesa *b, int cpuval, int alpha, int beta, int depth, int maxdepth, time_t start);

void pantalla(){
    blit(buffer, screen, 0, 0, 0,0, 780,480);
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

    // adicionalmente, si se alcanza el l�mite de tiempo (5 s), simplemente vuelva a anotar

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
	if (profundidad == max_profundidad || difftime(now, start) >= 5)
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

void imprimirAside(bool primero, bool noPermitido=false, bool pvp=false, bool turnoS=false){
    draw_sprite(buffer, aside, 480, 0);
    if(contBlancas<10)textprintf_ex(buffer, font1, 670, 205, makecol(0,0,0), -1, "0%d", contBlancas);
    else textprintf_ex(buffer, font1, 670, 205, makecol(0,0,0), -1, "%d", contBlancas);
    if(contNegras<10)textprintf_ex(buffer, font1, 553, 205, makecol(0,0,0), -1, "0%d", contNegras);
    else textprintf_ex(buffer, font1, 553, 205, makecol(0,0,0), -1, "%d", contNegras);

    if(!pvp){
        if(primero){
           textout_ex(buffer, font1, "IA",  670, 55, makecol(0,0,0), -1);
           textout_ex(buffer, font1, "Tu",553, 55, makecol(0,0,0), -1);
        } else {
           textout_ex(buffer, font1, "Tu", 670, 55, makecol(0,0,0), -1);
           textout_ex(buffer, font1, "IA", 553, 55, makecol(0,0,0), -1);
        }
    }
    else{
        textout_ex(buffer, font1, "P2",  670, 55, makecol(0,0,0), -1);
        textout_ex(buffer, font1, "P1",553, 55, makecol(0,0,0), -1);
    }

    if(noPermitido)  textout_ex(buffer, font2, "�NO PERMITIDO!", 542, 360, makecol(255,0,0), -1);

    if(turnoS) textout_ex(buffer, font3, "+", 667, 250, makecol(0,0,0), -1);//draw_sprite(buffer, turno, 660, 250);
    else textout_ex(buffer, font3, "+", 550, 250, makecol(0,0,0), -1);//draw_sprite(buffer, turno, 543, 250);


    if(mouse_x>633 && mouse_x<753 && mouse_y>415 && mouse_y<455) draw_sprite(buffer, exitButtonHover, 633, 415);
    else draw_sprite(buffer, exitButton, 633, 415);
}

//Primero=True -> Enpieza Jugador
//Primero=false -> Empieza IA
//pvp=true -> jugador vs jugador
void play(int cpuval, bool primero=true, bool pvp=false) {
	Mesa *b = new Mesa();
	int humanPlayer = -1*cpuval;
	int cpuPlayer = cpuval;

	b->imprimir(); pantalla();

	int pases = 0;

	int row, col, auxrow, auxcol;
	bool click, auxPrimero=primero, noPermitido=false,noPermitido2=false, turno=false;

		while(!b->tableroLleno() && pases<2) {

			if(primero && !noPermitido2){
                //Comprueba si el jugador pasa
                if(!b->tieneMovimientoValido(humanPlayer)) {
                    cout << "Tu debes pasar." << endl;
                    pases++;
                }
                else {
                    pases = 0;
                    //Click
                    click=false;
                    mouse_b=0;
                    while(!click){
                        if(mouse_b & 1){
                            click=true;
                            row=mouse_y/60;
                            col=mouse_x/60;
                            if(mouse_x>633 && mouse_x<753 && mouse_y>415 && mouse_y<455) return;
                        }
                        auxrow=mouse_y/60;
                        auxcol=mouse_x/60;

                        b->imprimir(false);
                        if(mouse_x < 460)draw_sprite(buffer, hover, auxcol*60, auxrow*60);
                        if(pvp) imprimirAside(auxPrimero, false, pvp, false);
                        else imprimirAside(auxPrimero, noPermitido, pvp, turno);
                        show_mouse(buffer);
                        pantalla();
                    }
                    pantalla(); mouse_b=0;
                    if(!b->jugarTablero(row+1, col+1, humanPlayer)) {
                        //cout << "Movimiento no permitido." << endl;
                        noPermitido=true;
                        continue;
                    }
                    noPermitido=false;
                    system("cls");
                }
                if(turno==false) turno=true;
                else turno=false;
			}

			//Compruebo pvp
			if(!pvp){///IA
                //Mueve Computadora
                if(b->tableroLleno())
                    break;
                else {
                    show_mouse(buffer);
                    b->imprimir();
                    imprimirAside(auxPrimero, false, false, turno);
                    pantalla();cout<< "..." << endl;

                    /*if(hacerMovimientoSimpleCPU(b, cpuPlayer))
                        pases=0;*/
                    if(hacerMovimientoInteligenteCPU(b, cpuPlayer))
                        pases=0;
                    else
                        pases++;
                    show_mouse(buffer);
                    imprimirAside(auxPrimero, false, false, turno);
                    b->imprimir();
                    pantalla();
                    primero=true;
                    position_mouse(-1,-1);
                }
                if(turno==false) turno=true;
                else turno=false;
			}
			else{///OTRO JUGADOR

                //Comprueba si el jugador pasa

                if(!b->tieneMovimientoValido(cpuPlayer)) {
                    cout << "Tu debes pasar." << endl;
                    pases++;
                }
                else {
                    pases = 0;

                    //Click
                    click=false;
                    while(true){
                        if(mouse_b & 1){
                            row=mouse_y/60;
                            col=mouse_x/60;
                            if(mouse_x>633 && mouse_x<753 && mouse_y>415 && mouse_y<455) return;
                            break;
                        }
                        auxrow=mouse_y/60;
                        auxcol=mouse_x/60;

                        b->imprimir(false);
                        if(mouse_x < 460)draw_sprite(buffer, hover, auxcol*60, auxrow*60);
                        imprimirAside(auxPrimero, false, pvp, turno);
                        show_mouse(buffer);
                        pantalla();
                    }
                    pantalla(); mouse_b=0;

                    if(!b->jugarTablero(row+1, col+1, cpuPlayer)) {
                        //cout << "Movimiento no permitido." << endl;
                        noPermitido2=true;
                        continue;
                    }
                    noPermitido2=false;
                    system("cls");
                }
                if(turno==false) turno=true;
                else turno=false;
			}
		}
	/*** NO BORRAR, EN CONSTRUCCION ***/

	position_mouse(-1,-1);
	while(true){
        if(mouse_b & 1){
            if(mouse_x>633 && mouse_x<753 && mouse_y>415 && mouse_y<455)break;
        }
        b->imprimir(false);
        imprimirAside(auxPrimero, false, pvp, turno);
        if(pvp){
           if(contBlancas<contNegras) draw_sprite(buffer, p1win, 215,140);
           else if(contBlancas>contNegras) draw_sprite(buffer, p2win, 215, 140);
           else  draw_sprite(buffer, empate, 215,140);
        }
        else{
            if(auxPrimero){
               if(contBlancas<contNegras) draw_sprite(buffer, youwin, 215,140);
               else if(contBlancas>contNegras) draw_sprite(buffer, youlose, 215,140);
               else draw_sprite(buffer, empate, 215,140);
            }else{
               if(contBlancas<contNegras) draw_sprite(buffer, youlose, 215,140);
               else if(contBlancas>contNegras) draw_sprite(buffer, youwin, 215,140);
               else draw_sprite(buffer, empate, 215,140);
            }
        }
        show_mouse(buffer);
        pantalla();
    }
	/**********************************/
}

void menu(){
    int op=-2;
    while(true){
        while(op==-2){
            if(mouse_x>232 && mouse_x<567 && mouse_y>193 && mouse_y<243) draw_sprite(buffer, menuPVP, 0,0);
            else if(mouse_x>232 && mouse_x<567 && mouse_y>253 && mouse_y<300) draw_sprite(buffer, menuPVIA, 0,0);
            else if(mouse_x>232 && mouse_x<567 && mouse_y>312 && mouse_y<357) draw_sprite(buffer, menuSalir, 0,0);
            else draw_sprite(buffer, menuBase, 0,0);
            if(mouse_b & 1){
                if(mouse_x>232 && mouse_x<567 && mouse_y>193 && mouse_y<243) op=1;
                else if(mouse_x>232 && mouse_x<567 && mouse_y>253 && mouse_y<300) op=2;
                else if(mouse_x>232 && mouse_x<567 && mouse_y>312 && mouse_y<357) op=0;
            }
            show_mouse(buffer);
            pantalla();
        }

        position_mouse(-1,-1);
        mouse_b = 0;
        switch(op){
            case 1:{ ///PVP
                clear(buffer);
                play(-1, true, true);
                break;
            }
            case 2:{
                clear(buffer);
                int ficha=0;
                mouse_b = 0;
                while(true){
                    if(mouse_x>210 && mouse_x<298 && mouse_y>260 && mouse_y<348) draw_sprite(buffer, menuBlanco, 0,0);
                    else if(mouse_x>505 && mouse_x<593 && mouse_y>260 && mouse_y<348) draw_sprite(buffer, menuNegro, 0,0);
                    else draw_sprite(buffer, menu2Base, 0,0);
                    if(mouse_b & 1){
                        if(mouse_x>210 && mouse_x<298 && mouse_y>260 && mouse_y<348) ficha=1;
                        else if(mouse_x>505 && mouse_x<593 && mouse_y>260 && mouse_y<348) ficha=-1;
                        if(ficha==0) continue;
                        break;
                    }
                    show_mouse(buffer);
                    pantalla();
                }
                mouse_b=0;
                position_mouse(-1,-1);
                if(ficha==-1)play(-1, true); // inicia jugador
                else play(1, false); // inicia IA

                break;
            }
            case 0:{
                exit(1);
                break;
            }
        }
        op=-2;
        position_mouse(-1,-1);
        clear(buffer);
    }
}

int main(){
    system("mode con: cols=15 lines=15");
    allegro_init();
    install_keyboard();
    install_mouse();

    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 780,480,0,0);

    buffer = create_bitmap(780, 480);
    menuBase = load_bitmap("Images/Menu-Base.bmp", NULL);
    menuPVIA = load_bitmap("Images/Menu-PVIA.bmp", NULL);
    menuPVP = load_bitmap("Images/Menu-PVP.bmp", NULL);
    menuSalir = load_bitmap("Images/Menu-salir.bmp", NULL);
    menu2Base = load_bitmap("Images/Menu2-Base.bmp", NULL);
    menuBlanco = load_bitmap("Images/Menu2-Blanco.bmp", NULL);
    menuNegro = load_bitmap("Images/Menu2-Negro.bmp", NULL);
    youwin = load_bitmap("Images/YouWins.bmp", NULL);
    youlose = load_bitmap("Images/YouLose.bmp", NULL);
    p1win = load_bitmap("Images/Player1.bmp", NULL);
    p2win = load_bitmap("Images/Player2.bmp", NULL);
    empate = load_bitmap("Images/Draw.bmp", NULL);
    aside = load_bitmap("Images/Aside.bmp", NULL);
    fichaN = load_bitmap("Images/Negro.bmp", NULL);
    fichaB = load_bitmap("Images/Blanco.bmp", NULL);
    cuadro = load_bitmap("Images/Tablero2.bmp", NULL);
    cursor = load_bitmap("Images/cursor.bmp", NULL);
    hover = load_bitmap("Images/Hover32Bits.bmp", NULL);
    turno = load_bitmap("Images/Flecha.bmp", NULL);
    exitButton = load_bitmap("Images/Exit.bmp", NULL);
    exitButtonHover = load_bitmap("Images/ExitHover.bmp", NULL);

    font1 = load_font("Fonts/lastninja.pcx", NULL, NULL);
    font2 = load_font("Fonts/lastninja12.pcx", NULL, NULL);
    font3 = load_font("Fonts/lastninja36.pcx", NULL, NULL);

    set_mouse_sprite(cursor);

	//cout << "Presiona Y si quieres empezar primero." << endl;

	char a='y';
	//cin >> a;
    menu();
	if (a == 'y' || a == 'Y') {
        play(-1, true, true); //pvp
        //play(-1, true); // inicia jugador
        //play(1, false); // inicia IA
	}
	else
		play(1);
	return 0;
}

END_OF_MAIN();
