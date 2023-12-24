#pragma once

#include "pch.h"
#include "input.h"

typedef struct {
	u8 keybinds[5];		//replace with dynamic Array
} Config_State;

void config_init(void);
void config_key_bind(Input_key Key, const char* Key_Name);
