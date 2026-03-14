#ifndef __MATRIX_KEYBOARD_H__
#define __MATRIX_KEYBOARD_H__

#include "main.h"

#define KEY_NONE 0

void Matrix_Keyboard_Init(void);
uint8_t Matrix_Keyboard_Scan(void);

#endif /* __MATRIX_KEYBOARD_H__ */