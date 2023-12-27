#pragma once

#include "pch.h"
#include "engine/input.h"

#define MAX_CONFIG_FIELD_LENGTH 100


typedef struct {
	u32 test;
	//c_vector bound_input_actions;		//replace with dynamic Array
} Config_State;


void config_init(void);
error_code config_try_to_find(const char* key, char* value);
error_code config_save_a_configuration(const char* key, const char* value, const bool override);


