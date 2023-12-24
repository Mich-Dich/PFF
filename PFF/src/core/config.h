#pragma once

#include "core/pch.h"
#include "core/input.h"
#include "core/types.h"
#include "core/container/darray.h"

typedef struct {
	DArray key_bindings;
	u8 keybinds[5];		//replace with dynamic Array
} Config_State;

void config_init(void);
void config_key_bind(input_key Key, const char* Key_Name);
