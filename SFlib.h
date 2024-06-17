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

struct character_t* create_character (ALLEGRO_BITMAP *nome, unsigned int xS, unsigned int yS, unsigned int xD,
                                    unsigned int yD, unsigned int maxX, unsigned int maxY, unsigned int currentFrame, int direction);

void destroy_character (struct character_t *player);

void apply_gravity (struct character_t *player);

int collision (struct character_t *p1, struct character_t *p2);

int collision_hit (struct character_t *p1, struct character_t *p2);

void update_position_jump (struct character_t *player);

void default_position (struct character_t *player1, struct character_t *player2);

void default_joystick (struct character_t *player) ;

void character_jump (struct character_t *player);

void character_move (struct character_t *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y);

int update_position (struct character_t *player1, struct character_t *player2);

void clear_event_queue(ALLEGRO_EVENT_QUEUE *queue);

char choose_character (ALLEGRO_FONT* font);

void game_paused (ALLEGRO_FONT *font);

void menu (ALLEGRO_FONT* font, ALLEGRO_BITMAP* menuBitmap, ALLEGRO_BITMAP* logo);

void reset_character (struct character_t *player1, struct character_t *player2) ;

int has_winner_match (ALLEGRO_FONT *font, struct character_t *player1, struct character_t *player2);

void print_winner (ALLEGRO_FONT *font, struct character_t *player1, struct character_t *player2);

int end_round (struct character_t *player1, struct character_t *player2, int *timming);

void check_winner (struct character_t *player1, struct character_t *player2);

void remove_life (struct character_t *player1, struct character_t *player2, int jogador);

void print_scene (struct character_t *player1, struct character_t *player2, ALLEGRO_BITMAP *scene, ALLEGRO_FONT* font, ALLEGRO_BITMAP * rounds[]) ;

void print_time (ALLEGRO_FONT*font, int *num, int *fps);

void print_character(struct character_t *player);

void rotate_position (struct character_t *player1, struct character_t *player2);


#endif