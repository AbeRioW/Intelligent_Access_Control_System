#ifndef __MATRIX_KEYBOARD_H__
#define __MATRIX_KEYBOARD_H__

#include "main.h"

#define KEY_NONE 0

uint8_t Matrix_Keyboard_Scan(void);
uint8_t KeyToDigit(uint8_t key);

#endif /* __MATRIX_KEYBOARD_H__ */