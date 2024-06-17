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

    al_init(); //Faz a preparação de requisitos da biblioteca Allegro
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard(); //Habilita a entrada via teclado (eventos de teclado), no programa
	al_init_primitives_addon();	//Faz a inicialização dos addons das imagens básicas

	char fighter;	
	int hp, game_on = 0, timming = CLOCK, fps = 0;
		
	struct character_t* player1;
    struct character_t* player2;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0); //Cria o relógio do jogo; isso indica quantas atualizações serão realizadas por segundo (30, neste caso)
    ALLEGRO_DISPLAY *display = al_create_display(X_SCREEN,Y_SCREEN); //Cria uma janela para o programa, define a largura (x) e a altura (y) da tela em píxeis (320x320, neste caso)
    ALLEGRO_FONT* font = al_load_font("./font/Act_Of_Rejection.ttf", 20, 0);
    ALLEGRO_FONT* time = al_load_font("./font/DS-DIGIT.TTF", 45, 0);

    /* Carrega as imagens dos personagens e do cenario*/
    ALLEGRO_BITMAP * ken = al_load_bitmap("./images/Ken.png");
    ALLEGRO_BITMAP * ryu = al_load_bitmap ("./images/Ryu.png");

    ALLEGRO_BITMAP * scene = al_load_bitmap("./images/cenario.jpg");
    ALLEGRO_BITMAP * logo = al_load_bitmap("./images/StreetFighterArcTitle2.png");
    ALLEGRO_BITMAP * menuBitmap = al_load_bitmap("./images/menuNovo.jpg");
	ALLEGRO_BITMAP * rounds[ROUNDS];

    /* Imagem para definir qual eh o round atual que esta sendo jogado */
	rounds[0] = al_load_bitmap("./images/rounds1.png");
	rounds[1] = al_load_bitmap("./images/rounds2.png");
	rounds[2] = al_load_bitmap("./images/rounds3.png");

	ALLEGRO_KEYBOARD_STATE keystate;
    ALLEGRO_EVENT_QUEUE * event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source() );
	al_get_keyboard_state(&keystate);
    al_start_timer(timer);
	al_set_window_position(display, 500, 200);
    al_set_window_title(display, "Street Fighter!");

    ALLEGRO_EVENT event;
	al_start_timer(timer);	
while (1) {
    al_wait_for_event(event_queue, &event);    

    if (!game_on) {
        menu(font, menuBitmap, logo);
        fighter = choose_character(font); 
        if (fighter == 'R' || fighter == 'r') {
            player1 = create_character(ryu, 70, 80, 200, 300, 1000, 1000, 80, 0);
            player2 = create_character(ken, 70, 80, 600, 300, 1000, 1000, 80, 1);
        } else {
            player1 = create_character(ken, 70, 80, 200, 420, 1000, 1000, 80, 0);
            player2 = create_character(ryu, 70, 80, 600, 420, 1000, 1000, 80, 1);
        }
        game_on = 1;
        clear_event_queue(event_queue);
    }

    if (event.type == ALLEGRO_EVENT_TIMER) {
        fps++;
        hp = update_position(player1, player2);
        print_scene(player1, player2, scene, font, rounds);
        remove_life(player1, player2, hp);
        print_character(player1);
        print_character(player2);
        print_time(time, &timming, &fps);
        default_position(player1, player2);
        rotate_position (player1, player2);
        al_flip_display();        
    }    

    if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_UP) {
        if (event.keyboard.keycode == ALLEGRO_KEY_A) joystick_left(player1->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_D) joystick_right(player1->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_W) joystick_up(player1->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_S) joystick_down(player1->joystick);            
        else if (event.keyboard.keycode == ALLEGRO_KEY_X) joystick_push(player1->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_C) joystick_kick(player1->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) joystick_left(player2->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) joystick_right(player2->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_UP) joystick_up(player2->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) joystick_down(player2->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_PAD_DELETE) joystick_push(player2->joystick);
        else if (event.keyboard.keycode == ALLEGRO_KEY_PAD_0) joystick_kick(player2->joystick);            
    }


    if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        break;
    }                                                                                                                               

    if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_P) {
        game_paused(font);
        clear_event_queue(event_queue);
    }

    if (end_round(player1, player2, &timming)) {
        check_winner(player1, player2);
        reset_character(player1, player2);
        clear_event_queue(event_queue);
        al_rest(1.5);   
        timming = CLOCK;         
    }

    if (has_winner_match(font, player1, player2)) {
        game_on = 0; 
        timming = CLOCK;          
        print_winner(font, player1, player2);
        al_flip_display();
        al_rest(4.0);
    }
}


    /* Destruir tudo*/
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
    al_shutdown_primitives_addon(); // Finaliza o addon de primitivas
    /*al_shutdown_image_addon();
    al_shutdown_font_addon();*/
	return 0;
}
