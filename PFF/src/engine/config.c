#include "config.h"

#include <SDL.h>

#include "engine/global.h"

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
	CL_VALIDATE(line, return "Not found", "", "Could not find config value [%s]", value);

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


// CHANGE: load func should create the file if not found
static int config_load(void) {

	LOG_FUNC_START("")
	File file_config = read_from_file("./config.ini");
	CL_VALIDATE(file_config.is_valid, return 1, "Loaded file [./config.ini]", "Could not read file [./config.ini]");

	config_key_bind(IK_left, "A");
	config_key_bind(IK_left, config_get_value(file_config.data, "left"));
	config_key_bind(IK_right, config_get_value(file_config.data, "right"));
	config_key_bind(IK_down, config_get_value(file_config.data, "down"));
	config_key_bind(IK_escape, config_get_value(file_config.data, "escape"));
	free(file_config.data);

	LOG_FUNC_END("")
	return 0;
}

//
void config_init(void) {

	LOG_FUNC_START("")
	if (config_load() == 0) {
		return;
	}
	LOG(Trace, "  After config_load()")
	write_to_file((void*)CONFIG_DEFAULT, strlen(CONFIG_DEFAULT), "./config.ini");

	CL_ASSERT(config_load() == 0, "", "Could not create or load config file");
	LOG_FUNC_END("")
}

//
void config_key_bind(input_key key, const char* key_name) {

	SDL_Scancode scan_code = SDL_GetScancodeFromName(key_name);
	CL_VALIDATE(scan_code != SDL_SCANCODE_UNKNOWN, return, "", "Invalid scan code when binding key: %s\n", key_name);

	global.config.keybinds[key] = scan_code;
}