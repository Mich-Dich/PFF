#include "config.h"

static const char* CONFIG_DEFAULT =
	"[controls]\n"
	"left = A\n"
	"right = D\n"
	"up = W\n"
	"down = S\n"
	"escape = Escape\n"
	"\n";
static char tmp_buffer[20] = { 0 };

static char* config_get_value(const char* config_bufer, const char* value) {

	char* line = strstr(config_bufer, value);
	CL_VALIDATE(line, return "Not found", "", "Could not find config value [%s]");

	size_t len = strlen(line);
	char* end = line + len;

	char* curr = line;
	char* tmp_ptr = &tmp_buffer[0];
	// Move pointer to '='.
	while (*curr != '=' && curr != end)
		++curr;
	// Consume '='.
	++curr;
	// Consume any spaces.
	while (*curr == ' ')
		++curr;
	// Get characters until end of line.
	while (*curr != '\n' && *curr != 0 && curr != end)
		*tmp_ptr++ = *curr++;

	*(tmp_ptr + 1) = 0;

	return tmp_buffer;
}

static void load_controlls(const char* config_buffer) {

	config_key_bind(INPUT_KEY_LEFT, config_get_value(config_buffer, "left"));
	config_key_bind(INPUT_KEY_RIGHT, config_get_value(config_buffer, "right"));
	config_key_bind(INPUT_KEY_UP, config_get_value(config_buffer, "up"));
	config_key_bind(INPUT_KEY_DOWN, config_get_value(config_buffer, "down"));
	config_key_bind(INPUT_KEY_ESCAPE, config_get_value(config_buffer, "escape"));
}


//
static int confic_load(void) {

	File file_config = read_from_file("./config.ini");
	CL_VALIDATE(file_config.is_valid, "", "Could not read file [%s]", "./config.ini");

	load_controlls(file_config.data);
	free(file_config.data);
	
	return 0;
}

//
void config_init(void) {

	if (config_load() == 0) {
		return;
	}

	write_to_file((void*)CONFIG_DEFAULT, sizeof(CONFIG_DEFAULT), "./config.ini");

	CL_ASSERT(config_load() == 0, "", "Could not create or load config file");
}

//
void config_key_bind(Input_key Key, const char* Key_Name) {


}

//
void config_key_bind(Input_key key, const char* key_name) {

	SDL_Scancode scan_code = SDL_GetScancodeFromName(key_name);
	CL_VALIDATE(scan_code != SDL_SCANCODE_UNKNOWN, return, "", "Invalid scan code when binding key: %s\n", key_name);

	global.config.keybinds[key] = scan_code;
}