#ifndef SFLIB_H
#define SFLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_primitives.h>


/*Definando a struct do personagem*/
struct character_t {
    float x_sprite;
    float y_sprite;
    float x_display;
    float y_display;
    float maxX;
    float maxY;
    float currentFrame;
    float maxFrame;    
    int side;
    int life;
    int rounds_won;
    float vel_y;
	float vel_x;
    int jump;
    int kick;
    int push;	
    float frame;
    int direction;
    struct joystick_t *joystick;
    ALLEGRO_BITMAP *fighter;
};

/*Cria o personagem*/
struct character_t* create_character (ALLEGRO_BITMAP *nome, unsigned int xS, unsigned int yS, unsigned int xD,
                                    unsigned int yD, unsigned int maxX, unsigned int maxY, unsigned int currentFrame, int direction);

/*Destroi o personagem*/
void destroy_character (struct character_t *player);

/*Mantem o personagem em sua posicao padrao /default */
void default_position (struct character_t *player1, struct character_t *player2);

/*Responsavel por direcionar os movimentacao dependendo da tecla*/
void character_move (struct character_t *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y);

/* Atualiza as posicao dos personagem conforme o joystick*/
int update_position (struct character_t *player1, struct character_t *player2);

/* Limpa a fila de eventos*/
void clear_event_queue(ALLEGRO_EVENT_QUEUE *queue);

/*Seleção de jogadores na tela de menu*/
char choose_character (ALLEGRO_FONT* font, ALLEGRO_DISPLAY *display);

/* Pausa no jogo*/
void game_paused (ALLEGRO_FONT *font);

/*Menu inicial do jogo*/
void menu (ALLEGRO_FONT* font, ALLEGRO_BITMAP* menuBitmap, ALLEGRO_BITMAP* logo, ALLEGRO_DISPLAY *display);

/* Reseta os jogadores apos o fim de uma rodada */
void reset_character (struct character_t *player1, struct character_t *player2) ;

/* verifica se ha vencedor da MD3 */
int has_winner_match (ALLEGRO_FONT *font, struct character_t *player1, struct character_t *player2);

/*Imprime o vencedor do confronto*/
void print_winner (ALLEGRO_FONT *font, struct character_t *player1, struct character_t *player2);

/* verifica se a rodada terminou*/
int end_round (struct character_t *player1, struct character_t *player2, int *timming);

/* Verifica se houve vencedor da rodada*/
void check_winner (struct character_t *player1, struct character_t *player2);

/* Funcao responsavel por diminuir o hp durante um ataque do oponente*/
void remove_life (struct character_t *player1, struct character_t *player2, int jogador);

/* imprime o cenario */
void print_scene (struct character_t *player1, struct character_t *player2, ALLEGRO_BITMAP *scene, ALLEGRO_FONT* font, ALLEGRO_BITMAP * rounds[]) ;

/* imprime o timer da rodada (90 segundos)*/
void print_time (ALLEGRO_FONT*font, int *num, int *fps);

/* imprime os personagens na tela*/
void print_character(struct character_t *player);

/* inverte a direcao do jogador (ex: esquerda para a direita...)*/
void rotate_position (struct character_t *player1, struct character_t *player2);

#endif