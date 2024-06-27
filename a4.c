#include <stdio.h>
#include <stdlib.h>
#include "joystick.h"
#include "SFlib.h"

#define STEP 12
#define X_SCREEN 800
#define Y_SCREEN 600   
#define MAX_FRAME 4
#define ROUNDS 3
#define GRAVITY 5.0
#define JUMP_STRENGTH -250
#define GROUND_LEVEL 420
#define CLOCK 90
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define PUSH 4
#define KICK 5

int main () {

    al_init();                                                                                      //Faz a preparação de requisitos da biblioteca Allegro
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard();                                                                          //Habilita a entrada via teclado (eventos de teclado), no programa
	al_init_primitives_addon();	                                                                    //Faz a inicialização dos addons das imagens básicas

	char fighter;	
	int hp, game_on = 0, timming = CLOCK, fps = 0;
		
	struct character_t* player1;
    struct character_t* player2;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);                                             //Cria o relógio do jogo; isso indica quantas atualizações serão realizadas por segundo (30, neste caso)
    ALLEGRO_DISPLAY *display = al_create_display(X_SCREEN,Y_SCREEN);                                //Cria uma janela para o programa, define a largura (x) e a altura (y) da tela em píxeis (320x320, neste caso)
    ALLEGRO_FONT* font = al_load_font("./font/Act_Of_Rejection.ttf", 20, 0);
    ALLEGRO_FONT* time = al_load_font("./font/DS-DIGIT.TTF", 45, 0);

    /* Carrega as imagens dos personagens e do cenario*/
    ALLEGRO_BITMAP * ken = al_load_bitmap("./images/Ken.png");
    ALLEGRO_BITMAP * ryu = al_load_bitmap ("./images/Ryu.png");

    ALLEGRO_BITMAP * scene = al_load_bitmap("./images/cenario.jpg");
    ALLEGRO_BITMAP * scene2 = al_load_bitmap("./images/cenario2.png");
    ALLEGRO_BITMAP * logo = al_load_bitmap("./images/StreetFighterArcTitle2.png");
    ALLEGRO_BITMAP * menuBitmap = al_load_bitmap("./images/menuNovo.jpg");
	ALLEGRO_BITMAP * rounds[ROUNDS];

    /* Imagem para definir qual eh o round atual que esta sendo jogado */
	rounds[0] = al_load_bitmap("./images/rounds1.png");
	rounds[1] = al_load_bitmap("./images/rounds2.png");
	rounds[2] = al_load_bitmap("./images/rounds3.png");

	ALLEGRO_KEYBOARD_STATE keystate;
    ALLEGRO_EVENT_QUEUE * event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));					//Indica que eventos de tela serão inseridos na nossa fila de eventos
    al_register_event_source(event_queue, al_get_timer_event_source(timer));						//Indica que eventos de relógio serão inseridos na nossa fila de eventos
    al_register_event_source(event_queue, al_get_keyboard_event_source() ); 						//Indica que eventos de teclado serão inseridos na nossa fila de eventos
	al_get_keyboard_state(&keystate);
    al_start_timer(timer);
	al_set_window_position(display, 500, 200);                                                      // Define a coordenada na qual a janela será iniciada
    al_set_window_title(display, "Street Fighter!");                                                //Altera o titulo da janela

    ALLEGRO_EVENT event;		                                                                    //Variável que guarda um evento capturado, sua estrutura é definida em: https:		//www.allegro.cc/manual/5/ALLEGRO_EVENT
	al_start_timer(timer);		    																//Função que inicializa o relógio do programa
while (1) {
    al_wait_for_event(event_queue, &event);    														//Função que captura eventos da fila, inserindo os mesmos na variável de eventos

    if (!game_on) {
        menu(font, menuBitmap, logo, display);
        fighter = choose_character(font, display); 
        if (fighter == 'R' || fighter == 'r') {
            player1 = create_character(ryu, 70, 80, 200, 300, 1000, 1000, 80, 0);
            player2 = create_character(ken, 70, 80, 600, 300, 1000, 1000, 80, 1);
        } else {
            player1 = create_character(ken, 70, 80, 200, 420, 1000, 1000, 80, 0);
            player2 = create_character(ryu, 70, 80, 600, 420, 1000, 1000, 80, 1);
        }
        game_on = 1;
        reset_character(player1, player2);
        al_flush_event_queue(event_queue);
    }

    if (event.type == ALLEGRO_EVENT_TIMER) {
        fps++;                                                                                      // Var responsavel por contar os frames por segundos (0 até 30)
        hp = update_position(player1, player2);                                                     // Atualiza a movimentacao dos personagens
        print_scene(player1, player2, scene, scene2,font, rounds);                                         //Remove vida caso um ataque tenha acertado o oponente
        remove_life(player1, player2, hp);
        print_character(player1);                                                                   //Imprime os personagens
        print_character(player2);
        print_time(time, &timming, &fps);                                                           //Imprime o timmer
        default_position(player1, player2);                                                         // Mantem a posicao padrao do personagem
        rotate_position (player1, player2);                                                         // Verifica a rotacao
        al_flip_display();        
    }    

    /*Verifica quais teclas foram apertadas*/
    if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP) {
        if (event.keyboard.keycode == ALLEGRO_KEY_A) joystick_left(player1->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_D) joystick_right(player1->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_W) joystick_up(player1->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_S) joystick_down(player1->joystick);            
        else if (event.keyboard.keycode == ALLEGRO_KEY_X) joystick_push(player1->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_C) joystick_kick(player1->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_Z) joystick_def(player1->joystick);  
        else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) joystick_left(player2->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) joystick_right(player2->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_UP) joystick_up(player2->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) joystick_down(player2->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_PAD_DELETE) joystick_push(player2->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_PAD_0) joystick_kick(player2->joystick);            
    }

    /* Verifica se a janela foi fechada*/
    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        break;
    }                                                                                                                               

    /*verifica se a tecla P foi pressionada (pausa o jogo)*/
    if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_P) {
        game_paused(font);
        clear_event_queue(event_queue);
    }

    /* Verifica se o round acabou*/
    if (end_round(player1, player2, &timming)) {
        al_rest(1.5);   
        check_winner(player1, player2);
        clear_event_queue(event_queue);
        reset_character(player1, player2);
        timming = CLOCK;        
    }

    /* Verifica se a MD3 acabou*/
    if (has_winner_match(font, player1, player2)) {
        game_on = 0; 
        timming = CLOCK;          
        print_winner(font, player1, player2);
        al_flip_display();
        al_flush_event_queue(event_queue);
        al_rest(4.0);        
    }
}
    /* Destroi os bitmaps e afins*/
    clear_event_queue (event_queue);
    al_destroy_event_queue(event_queue);    
    al_destroy_display (display);
    al_destroy_timer(timer);
    al_uninstall_keyboard();
    al_destroy_font(font);	
    al_destroy_font(time);	
    destroy_character (player1);
    destroy_character (player2);
    al_destroy_bitmap (scene);
    al_destroy_bitmap (logo);
    al_destroy_bitmap (menuBitmap);    
    for (int i = 0; i < ROUNDS; i++)
        al_destroy_bitmap (rounds[i]);
    al_shutdown_primitives_addon();
	return 0;
}
