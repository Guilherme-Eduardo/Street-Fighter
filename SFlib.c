#include "SFlib.h"
#include "joystick.h"

#define STEP 12
#define X_SCREEN 800
#define Y_SCREEN 600    
#define MAX_FRAME 4
#define ROUNDS 3
#define GRAVITY 7.5
#define JUMP_STRENGTH -250
#define GROUND_LEVEL 420
#define CLOCK 90
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define PUSH 4
#define KICK 5
#define DEF 6
#define DOWN_DEF 7
#define DOWN_ATK 8

/* Funcao responsavel pela criacao do personagem e com as suas caracteristicas */
struct character_t* create_character (ALLEGRO_BITMAP *nome, unsigned int xS, unsigned int yS, unsigned int xD,
                                    unsigned int yD, unsigned int maxX, unsigned int maxY, unsigned int currentFrame, int direction) {

    struct character_t *player = (struct character_t*) malloc (sizeof(struct character_t));
    if (!player) exit(1);
    player->x_sprite = xS;
    player->y_sprite = yS;
    player->x_display = xD;
    player->y_display = yD;
    player->maxX = maxX;
    player->maxY = maxY;
    player->currentFrame = currentFrame;
    player->fighter = nome;    
    player->maxFrame = 4;
    player->side = 100;
    player->life = 360;
    player->rounds_won = 0;
    player->vel_y = 0;
    player->jump = 0;
    player->kick = 0; 
    player->push = 0;   
    player->frame = 0.f;
    player->stamina = 100;
    player->joystick = joystick_create();
    player->direction = direction;
    return player;
}

void destroy_character (struct character_t *player) {
    if (!player) return;

    al_destroy_bitmap (player->fighter);
    joystick_destroy (player->joystick);
    free (player);
}

/* Verifica se houve colisao entre personagens (sem ataque e no solo) */
int collision (struct character_t *p1, struct character_t *p2) {
	if (!p1 || !p2) return 0;
	if ((p1->x_display + p1->side/2 > p2->x_display && 
        p1->x_display < p2->x_display + p2->side/2) &&
        (p1->y_display >= 300 && p2->y_display >= 300)) {            
        return 1;
        }
	else return 0;
}

/* Verifica se houve colisao entre os personagem quando ocorre um ataque */
int collision_hit (struct character_t *p1, struct character_t *p2) {
	if (!p1 || !p2) return 0;
	if (p1->x_display + p1->side > p2->x_display && 
        p1->x_display < p2->x_display +  p2->side) 
        return 1;	
	else return 0;
}

/* Verifica se p1 está no nível do chão e se p1 passou por p2*/
void rotate_position(struct character_t *p1, struct character_t *p2) {
    if (((p1->x_display > p2->x_display) && (p2->x_display < p1->x_display)) ||
        ((p2->x_display < p1->x_display ) && (p1->x_display > p2->x_display ))) {                               // Alterna a direção de ambos os personagens       
      
        p1->direction = 1;
        p2->direction = 0;
    } 
    else {
        p1->direction = 0;
        p2->direction = 1;
    }
}

/* zera as variaveis do joystick*/
void default_joystick (struct character_t *player) {
    if (!player) return;

    player->joystick->down = 0;
    player->joystick->right = 0;
    player->joystick->left= 0;
    player->joystick->up = 0;
    player->joystick->kick = 0;
    player->joystick->push = 0;
}

/* Reseta as configuracoes para o inicio de um novo round*/
void reset_character (struct character_t *player1, struct character_t *player2) {

    //Redefini a vida do jogador
	player1->life = 360;
	player2->life = 360;

    //Posicionamento no cenario para a proxima rodada
	player1->x_display = 200;
	player2->x_display = 400;

    default_joystick (player1);
    default_joystick (player2);
}

/* Apos realizar movimentacoes, o personagem retorna para a posicao padrao (posição standard/default) */
void default_position (struct character_t *player1, struct character_t *player2) {
    if (!player1 || !player2) return;   

    player1->maxFrame = 4;
    player1->currentFrame = 80;
    player1->y_display = 420;    

    player2->maxFrame = 4;
    player2->currentFrame = 80;
    player2->y_display = 420;
     
     if (collision(player1, player2))
        player1->x_display -= 5;
}

/* Função responsavel por aplicar o efeito de gravidade caso o personagem esteja pulando */
void apply_gravity (struct character_t *player) {
    if (player->jump) {
        if (player->y_display <= 100 && player->vel_y < 0) {
            player->vel_y = 0;                                                                                   // Inverte a direção do movimento caso ele alcance a altura limite
        } 
		else {
            player->vel_y += GRAVITY;                                                                           // Aplica a gravidade na variavel "vel" que será usada para somar com o eixo y do personagem (Diminui a altura do salto até encostar no chao)
            player->maxFrame = 7;                                                                               // Informações sobre o frame correto da spritesheet 
            player->currentFrame = player->currentFrame * 8;	
        }
    }
}

/* Atualiza a posicao do jogador, caso ele esteja pulando*/
void update_position_jump (struct character_t *player) {
    player->y_display += player->vel_y;                                                                         //gravidade aplicada no personagem
    
    if (player->y_display >= GROUND_LEVEL) {                                                                    // Caso o jogador encoste no chão, ele atualiza os parametros para o pulo ser falso
        player->y_display = GROUND_LEVEL;
        player->vel_y = 0;
        player->jump = 0;
    }
}

/* Funcao responsavel por implementar o pulo do personagem*/
void character_jump (struct character_t *player) {
    if (!player->jump) {
        player->vel_y = JUMP_STRENGTH; // Vel_y recebe a altura do salto 
        player->jump = 1;
    }
}

void character_move (struct character_t *element, char steps, unsigned char trajectory, 
                    unsigned short max_x, unsigned short max_y) {

	if (trajectory == LEFT){ 
		if ((element->x_display - steps * STEP) + element->side / 2 >= 0) {
			element->x_display = element->x_display - steps*STEP;  				//Verifica se a movimentação para a esquerda é desejada e possível; se sim, efetiva a mesma
			if (steps > 0 && element->jump == 0) {
				element->currentFrame = element->currentFrame * 3;
				element->maxFrame = 5;                
			}			
		}
	}
	else if (trajectory == RIGHT){ 
		if ((element->x_display + steps * STEP) + element->side <= max_x) {
			element->x_display = element->x_display + steps * STEP;             //Verifica se a movimentação para a direita é desejada e possível; se sim, efetiva a mesma
			if (steps > 0 && element->jump == 0) {
				element->currentFrame = element->currentFrame * 3;
				element->maxFrame = 5;                
			}			
		}
	}	
	else if (trajectory == UP){ 
		if ((element->y_display - steps * STEP) - element->side >= 0) {           //Verifica se a movimentação para cima é desejada e possível; se sim, efetiva a mesma						
			element->maxFrame = 7;
			element->currentFrame = element->currentFrame * 8;						
            element->frame = 0;			
		}			
	}
	else if (trajectory == DOWN){ 
		if ((element->y_display + steps * STEP) + element->side/2 <= max_y ) {			
			if (element->jump == 0 ) {                                           //Verifica se a movimentação para baixo é desejada e possível; se sim, efetiva a mesma
				element->currentFrame = element->currentFrame * 9;
				element->maxFrame = 1;	
                element->frame = 0;		
			}			
		}			
	}	
	else if (trajectory == PUSH){                                                // Verifica se a movimentação do soco
        if (element->jump == 0 && !element->push) {
            element->push = 1;
            element->frame = 0;
        }					
	}	
	else if (trajectory == KICK && element->jump == 0 && !element->kick) {	    //verifica a movimentação do chute		
        element->kick = 1;		
        element->frame = 1;		
    }
    else if (trajectory == DEF) {	    //verifica a movimentação do chute		
        element->currentFrame = element->currentFrame * 11;
        element->maxFrame = 1;	
        element->frame = 0;	
	}
    else if (trajectory == DOWN_DEF) {	    //verifica a movimentação do chute		
        element->currentFrame = 80;
        element->currentFrame = element->currentFrame * 12;
        element->maxFrame = 1;	
        element->frame = 0;	
	}
    else if (trajectory == DOWN_ATK) {	    //verifica a movimentação do chute		
        element->currentFrame = 80;
        element->currentFrame = element->currentFrame * 9;
        element->maxFrame = 3;	
        element->frame = 1;        
	}
    
}

/* Funcao principal por atualizar o posicionamento de cada personagem e selecionar*/
int update_position (struct character_t *player1, struct character_t *player2) {
    apply_gravity (player1);
    apply_gravity (player2);

    update_position_jump (player1);
    update_position_jump (player2);

    if (player1->joystick->left && !player1->joystick->down && !player1->joystick->right && player1->push == 0 && player1->kick == 0) {
        character_move (player1, 1, 0, X_SCREEN, Y_SCREEN);
        if (collision (player1, player2)) character_move (player1, -1, 0, X_SCREEN, Y_SCREEN);
    }
    if (player1->joystick->right && !player1->joystick->down && !player1->joystick->left && player1->push == 0 && player1->kick == 0) {
        character_move (player1, 1, 1, X_SCREEN, Y_SCREEN);
        if (collision (player1, player2)) character_move (player1, -1, 1, X_SCREEN, Y_SCREEN);
    }
    if (player1->joystick->up) {
        character_jump (player1);		
    }
    if (player1->joystick->down && player1->push == 0 && player1->kick == 0) {
        character_move (player1, 1, 3, X_SCREEN, Y_SCREEN);        
    }
    if (player1->joystick->push && !player1->jump && !player1->joystick->down && !player1->joystick->left && !player1->joystick->right) {
        character_move (player1, 1, 4, X_SCREEN, Y_SCREEN);
        if (collision_hit (player1, player2)) return 2;        
    }
    if (player1->joystick->kick && !player1->jump && !player1->joystick->down && !player1->joystick->left && !player1->joystick->right && !player1->joystick->push) {
        character_move (player1, 1, 5, X_SCREEN, Y_SCREEN);
        if (collision_hit(player1, player2)) return 2;        
    }
    if (player1->joystick->def && player1->jump == 0 && player1->joystick->down == 0 && player1->joystick->right == 0 && player1->joystick->left == 0) {
        character_move (player1, 1, 6, X_SCREEN, Y_SCREEN);
    }
    if (player1->joystick->down && player1->joystick->def) {
        character_move (player1, 1, 7, X_SCREEN, Y_SCREEN);
    }
    if (player1->joystick->down && player1->joystick->push) {
        character_move (player1, 1, 8, X_SCREEN, Y_SCREEN);
        if (collision_hit(player1, player2)) return 2;        
    }
    if (player2->joystick->left && !player2->joystick->down && !player2->joystick->right && player2->push == 0 && player2->kick == 0) {
        character_move (player2, 1, 0, X_SCREEN, Y_SCREEN);
        if (collision (player1, player2)) character_move (player2, -1, 0, X_SCREEN, Y_SCREEN);
    }
    if (player2->joystick->right && !player2->joystick->down && !player2->joystick->left && player2->push == 0 && player2->kick == 0) {
        character_move (player2, 1, 1, X_SCREEN, Y_SCREEN);
        if (collision (player1, player2)) character_move (player2, -1, 1, X_SCREEN, Y_SCREEN);
    }
    if (player2->joystick->up) {
        character_jump (player2);
    }
    if (player2->joystick->down && player2->push == 0 && player2->kick == 0) {
        character_move (player2, 1, 3, X_SCREEN, Y_SCREEN);        
    }
    if (player2->joystick->push && !player2->jump && !player2->joystick->down && !player2->joystick->left && !player2->joystick->right) {
        character_move (player2, 1, 4, X_SCREEN, Y_SCREEN);
        if (collision_hit (player1, player2)) return 1;        
    }
    if (player2->joystick->kick && !player2->jump && !player2->joystick->down && !player2->joystick->left && !player2->joystick->right && !player2->joystick->push) {
        character_move (player2, 1, 5, X_SCREEN, Y_SCREEN);
        if (collision_hit (player1, player2)) return 1;       
    }
    if (player2->joystick->def && player2->jump == 0 && player2->joystick->down == 0 && player2->joystick->right == 0 && player2->joystick->left == 0) {
        character_move (player2, 1, 6, X_SCREEN, Y_SCREEN);
    }
    if (player2->joystick->down && player2->joystick->def) {
        character_move (player2, 1, 7, X_SCREEN, Y_SCREEN);
    }
    if (player2->joystick->down && player2->joystick->push) {
        character_move (player2, 1, 8, X_SCREEN, Y_SCREEN);
        if (collision_hit(player2, player2)) return 2;        
    }

    return 0;
}

/* Limpa a fila de eventos do teclado. Usado quando o jogo/rodada são iniciados */
void clear_event_queue(ALLEGRO_EVENT_QUEUE *queue) {
    al_flush_event_queue(queue);
}

/* Usuario decide entre qual personagem ele irá escolher para jogar*/
char choose_character (ALLEGRO_FONT* font, ALLEGRO_DISPLAY *display) {
    char letter = '\0';
    al_clear_to_color(al_map_rgb(0, 0, 0));
    ALLEGRO_BITMAP *fundoMenu = al_load_bitmap("./images/fundoMenuPrincipal.jpg");
    ALLEGRO_BITMAP *ryu = al_load_bitmap("./images/SelecionaRyu.png");
    ALLEGRO_BITMAP *ken = al_load_bitmap("./images/SelecionaKen.png");
    al_draw_bitmap (fundoMenu, 0, 0, 0);
    al_draw_text (font, al_map_rgb(255,255,255), 300, 50, 0, "Player 1: SELECT THE CHARACTER");
    al_draw_text (font, al_map_rgb(255,255,255), 300, 80, 0, "Press K for Ken or R for Ryu");
    
    al_draw_scaled_bitmap(ryu, 0, 0, 736 ,1233, 100, 180, 300, 300, 0);                     
    al_draw_scaled_bitmap(ken, 0, 0, 736 ,1233, 500, 180, 300, 300, 0);                     
    al_flip_display ();
    
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	ALLEGRO_EVENT event;
    al_register_event_source (event_queue, al_get_keyboard_event_source());
     al_register_event_source(event_queue, al_get_display_event_source(display));
    do {        
        al_wait_for_event (event_queue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_R) {
                letter = 'R';
            } else if (event.keyboard.keycode == ALLEGRO_KEY_K) {
                letter = 'K';
            } 
        }
        if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ) exit(1);
    } while (letter != 'R' && letter != 'K');
    
    clear_event_queue (event_queue);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap (fundoMenu);
    al_destroy_bitmap (ryu);
    al_destroy_bitmap (ken);
    
    al_clear_to_color(al_map_rgb(0, 0, 0));
	return letter;
}

/* Funcao que pausa o jogo durante o round */
void game_paused (ALLEGRO_FONT *font) {    
    if (!font) return;
    
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();    // Cria uma fila de eventos para capturar eventos do teclado
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    ALLEGRO_EVENT event;
    int pause = 1;

    while (pause) {

        al_wait_for_event(event_queue, &event); // Aguarda um evento        
        if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_P) // Verifica se a tecla P foi pressionada para retomar o jogo
            pause = 0; // Define que o jogo não está mais pausado
                
        al_draw_text(font, al_map_rgb(0, 0, 0), 350, 330, 0, "PAUSE");
        al_draw_text(font, al_map_rgb(255, 255, 255), 350, 325, 0, "PAUSE");

        al_draw_text(font, al_map_rgb(0, 0, 0), 100, 200, 0, "Player 1:");
        al_draw_text(font, al_map_rgb(255, 255, 255), 100, 195, 0, "player 1");

        al_draw_text(font, al_map_rgb(0, 0, 0), 80, 250, 0, "Movement: A S D W");
        al_draw_text(font, al_map_rgb(255, 255, 255), 80, 245, 0, "Movement: A S D W");

        al_draw_text(font, al_map_rgb(0, 0, 0), 90, 295, 0, "Attack: X - C");
        al_draw_text(font, al_map_rgb(255, 255, 255), 90, 290, 0, "Attack: X - C");

        al_draw_text(font, al_map_rgb(0, 0, 0), 600, 200, 0, "Player 2:");
        al_draw_text(font, al_map_rgb(255, 255, 255), 600, 195, 0, "player 2");

        al_draw_text(font, al_map_rgb(0, 0, 0), 580, 250, 0, "Movement: arrows");
        al_draw_text(font, al_map_rgb(255, 255, 255), 580, 245, 0,"Movement: arrows");

        al_draw_text(font, al_map_rgb(0, 0, 0), 570, 295, 0, "Attack: PAD Zero - PAD DEL");
        al_draw_text(font, al_map_rgb(255, 255, 255), 570, 290, 0, "Attack: PAD Zero - PAD DEL");

        al_flip_display();
    } 
    clear_event_queue (event_queue);
    al_destroy_event_queue(event_queue);
}


/* Menu principal do jogo. Usuario pode continuar a jogar ou quitar*/
void menu (ALLEGRO_FONT* font, ALLEGRO_BITMAP* menuBitmap, ALLEGRO_BITMAP* logo, ALLEGRO_DISPLAY *display) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_bitmap(menuBitmap, 0, 0, 0);
    al_draw_bitmap(logo, 250, 150, 0);   
    al_draw_text(font, al_map_rgb(0,0,0), 330, 330, 0, "Press ENTER to begin");
    al_draw_text(font, al_map_rgb(255,255,255), 330, 325, 0, "Press ENTER to begin");

    al_draw_text(font, al_map_rgb(0,0,0), 340, 380, 0, "Press ESC to quit");
    al_draw_text(font, al_map_rgb(255,255,255), 340, 375, 0, "Press ESC to quit");

    al_flip_display();

    ALLEGRO_EVENT_QUEUE * event_queue = al_create_event_queue();
	ALLEGRO_EVENT event;
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    while (1) {        

        al_wait_for_event (event_queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                break;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                exit(1);
            }             
        }
        if( event.type == 42 ){
            exit(1);
        }

    }
    clear_event_queue (event_queue);
    al_destroy_event_queue (event_queue);
	al_clear_to_color (al_map_rgb(255,255,255));
}

/* Verifica se ha um vencedor na partida. Nessario possuir 2 rounds vencidos */
int has_winner_match (ALLEGRO_FONT *font, struct character_t *player1, struct character_t *player2) {
	if (player1->rounds_won >=2 || player2->rounds_won >= 2) return 1; 
    else return 0;
}

/* Imprime a mensagem de qual jogador venceu o jogo */
void print_winner (ALLEGRO_FONT *font, struct character_t *player1, struct character_t *player2) {
	if (player1->rounds_won >=2) {
        al_draw_text(font, al_map_rgb(0,0,0), 330, 330, 0, "Player 1 venceu o jogo!");
        al_draw_text(font, al_map_rgb(255,255,255), 330, 325, 0, "Player 1 venceu o jogo!");
    }    
    else {
        al_draw_text(font, al_map_rgb(0,0,0), 330, 330, 0, "Player 2 venceu o jogo!");
        al_draw_text(font, al_map_rgb(255,255,255), 330, 325, 0, "Player 2 venceu o jogo!");
    }
}

/* Verifica se o round acabou por tempo ou por quantidade de vida/hp */
int end_round (struct character_t *player1, struct character_t *player2, int *timming) {
    if (player1->life <= 0 || player2->life <= 0 || *timming <= 0) return 1;
	else return 0;
}

/* Verifica se algum jogador venceu a rodada*/
void check_winner (struct character_t *player1, struct character_t *player2) {
    if (player1->life > player2->life) player1->rounds_won++;
    else if (player2->life > player1->life) player2->rounds_won++;
    else {
        player1->rounds_won++;
        player2->rounds_won++;
    }    
}

/* Remove os pontos de vida do jogador dependendo da variavel 'jogador' */
void remove_life (struct character_t *player1, struct character_t *player2, int jogador) {
	if (jogador == 1 && player1->joystick->down == 0 && player1->joystick->right == 0 && player1->joystick->left == 0 && player1->joystick->def == 0) {
        player1->life -= 5;
        player1->currentFrame = player1->currentFrame * 10;
        player1->maxFrame = 1;
    }
	else if (jogador == 2 && player2->joystick->down == 0 && player2->joystick->right == 0 && player2->joystick->left == 0 && player2->joystick->def == 0) {
        player2->life -= 5;
        player2->currentFrame = player2->currentFrame * 10;
        player2->maxFrame = 1;
    }
}

/* Imprime o cenario / background */
void print_scene (struct character_t *player1, struct character_t *player2, ALLEGRO_BITMAP *scene, ALLEGRO_BITMAP *scene2, ALLEGRO_FONT* font, ALLEGRO_BITMAP * rounds[]) {
	al_clear_to_color(al_map_rgb(0,0,0));    

	if (player1->rounds_won + player2->rounds_won == 0) {
		al_draw_scaled_bitmap (rounds[0],0,0,372,118,350,100,100,50,0);
	    al_draw_bitmap(scene, 0, 0, 0);
	}
	else if (player1->rounds_won + player2->rounds_won == 1) {
		al_draw_scaled_bitmap (rounds[1],0,0,372,118,350,100,100,50,0);
        al_draw_bitmap (scene2, 0,0,0);
    }

	else if (player1->rounds_won + player2->rounds_won == 2) {
		al_draw_scaled_bitmap (rounds[2],0,0,372,118,350,100,100,50,0);	
        al_draw_bitmap(scene, 0, 0, 0);
    }

	ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);

    /*Imprime os retangulos de HP*/
	al_draw_filled_rectangle (5, 5, 375, 65, al_map_rgb(255, 218, 185));
	al_draw_filled_rectangle (10, 10, 10 + player1->life, 60, red);
	al_draw_filled_rectangle (425, 5, 795, 65, al_map_rgb(255, 218, 185));
	al_draw_filled_rectangle (430, 10, 430 + player2->life, 60, red);

    /*Imprime stamina*/
    al_draw_filled_rectangle (20, 70, 305, 90, al_map_rgb(255, 218, 185));
	al_draw_filled_rectangle (25, 75, 200 + player1->stamina, 85, red);

    al_draw_filled_rectangle (495, 70, 775, 90, al_map_rgb(255, 218, 185));
	al_draw_filled_rectangle (500, 75, 670 + player1->stamina, 85, red);

    al_draw_text(font, al_map_rgb(0,0,0), 15, 20, 0, "player1");
	al_draw_text(font, al_map_rgb(0,0,0), 720, 20, 0, "player2");

    al_draw_text(font, al_map_rgb(0,0,0), 600, 160, 0, "Press P for commands");
    al_draw_text(font, al_map_rgb(255,255,255), 600, 155, 0, "Press P for commands");

	/*Imprime o simbolo para identificar a quantidade de vitoria de round do player*/
	al_draw_filled_circle(340, 80, 10, al_map_rgb(0, 0, 0));
	al_draw_filled_circle(360, 80, 10, al_map_rgb(0, 0, 0));

	al_draw_filled_circle(440, 80, 10, al_map_rgb(0, 0, 0));
	al_draw_filled_circle(460, 80, 10, al_map_rgb(0, 0, 0));

	if (player1->rounds_won == 1) {
		al_draw_filled_circle(340, 80, 10, al_map_rgb(255, 255, 0));
	}
	if (player2->rounds_won == 1) {
		al_draw_filled_circle(440, 80, 10, al_map_rgb(255, 255, 0));
	}
}

/* Imprime o cronometro da rodada */
void print_time (ALLEGRO_FONT*font, int *num, int *fps) {
    if (!font) return;
    if (*fps >= 30) {
        (*num)--;
        *fps = 0;
    }
    char timming[2];
    sprintf (timming, "%d", *num);
    al_draw_text(font, al_map_rgb(255,255,255), 375, 5, 0, timming);
}

/* Imprime os personagem na tela conforme o frame atual */
void print_character(struct character_t *player) {

    player->frame += 0.3f;

    if (player->frame >= player->maxFrame) {
        player->frame -= player->maxFrame; 
        if (player->kick) player->kick = 0;
        if (player->push) player->push = 0;
    }   
    
    
    if (player->frame > 6 && player->jump) player->frame = 5;

    if (player->kick) {
        player->currentFrame = player->currentFrame * 6;
        player->maxFrame = 5;        
    }
    else if (player->push) {
        player->currentFrame = player->currentFrame * 2;
        player->maxFrame = 3;

    } 
    al_draw_scaled_bitmap(player->fighter, player->x_sprite * (int)(player->frame), player->currentFrame, player->x_sprite, player->y_sprite, player->x_display, player->y_display, 150, 150, player->direction);   
}