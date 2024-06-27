#ifndef _JOYSTICK_H
#define _JOYSTICK_H

struct joystick_t {
    unsigned char right;
    unsigned char left;
    unsigned char up;
    unsigned char down;
    unsigned char push;
    unsigned char kick;
    unsigned char def;
};

/* Cria a estrutura responsavel por receber os comandos do personagem */
struct joystick_t *joystick_create ();

void joystick_destroy (struct joystick_t *element);					//Implementação da função "joystick_destroy"; libera a memória do elemento na heap (!)]

/* Sequencia de funcoes para detectar caso uma tecla seja pressionada/solta */
void joystick_left (struct joystick_t *element);

void joystick_right (struct joystick_t *element);

void joystick_up (struct joystick_t *element);

void joystick_down (struct joystick_t *element);

void joystick_push (struct joystick_t *element);

void joystick_kick (struct joystick_t *element);

void joystick_def (struct joystick_t *element);

#endif



