#include <stdlib.h>
#include "joystick.h"

/* Cria a estrutura responsavel por receber os comandos do personagem */
struct joystick_t *joystick_create () {
    struct joystick_t *element = (struct joystick_t*) malloc (sizeof(struct joystick_t));
    if (!element) exit(1);
    element->right = 0;
    element->left = 0;
    element->up = 0;
    element->down = 0;
    element->push = 0;
    element->kick = 0;
    element->def = 0;
    return element;
}

void joystick_destroy (struct joystick_t *element){ free(element);}							//Implementação da função "joystick_destroy"; libera a memória do elemento na heap (!)]

/* Sequencia de funcoes para detectar caso uma tecla seja pressionada/solta */
void joystick_left (struct joystick_t *element){ element->left = element->left ^ 1;}			//Implementação da função "joystick_left"; muda o estado do botão (!)

void joystick_right (struct joystick_t *element){ element->right = element->right ^ 1;}		//Implementação da função "joystick_right"; muda o estado do botão (!)

void joystick_up (struct joystick_t *element){ element->up = element->up ^ 1;}				//Implementação da função "joystick_up"; muda o estado do botão (!)

void joystick_down (struct joystick_t *element){ element->down = element->down ^ 1;}			//Implementação da função "joystick_down"; muda o estado do botão (!)

void joystick_push (struct joystick_t *element){ element->push = element->push ^ 1;}         //Implementação da função "joystick_push"; muda o estado do botão (!)

void joystick_kick (struct joystick_t *element){ element->kick = element->kick ^ 1;}         //Implementação da função "joystick_kick"; muda o estado do botão (!)

void joystick_def (struct joystick_t *element){ element->def = element->def ^ 1;}         //Implementação da função "joystick_def"; muda o estado do botão (!)

