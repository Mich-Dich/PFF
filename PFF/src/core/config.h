#pragma once

#include "core/pch.h"
#include "core/input.h"
#include "core/types.h"
#include "core/container/darray.h"


typedef struct {
	c_vector key_bindings;
	input_mapping old_style[10];
	u8 keybinds[5];		//replace with dynamic Array
} Config_State;

void config_init(void);
void config_key_bind(input_key Key, const char* Key_Name);
