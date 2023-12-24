#include "config.h"

#include "core/config.h"
#include "core/input.h"
#include "core/types.h"
#include "core/logger.h"
#include "core/global.h"

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

	CL_LOG_FUNC_START("")
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

	CL_LOG_FUNC_END("")
	return tmp_buffer;
}

static void load_controlls(const char* config_buffer) {

	CL_LOG_FUNC_START("")
	config_key_bind(IK_left, config_get_value(config_buffer, "left"));
	config_key_bind(IK_right, config_get_value(config_buffer, "right"));
	config_key_bind(IK_up, config_get_value(config_buffer, "up"));
	config_key_bind(IK_down, config_get_value(config_buffer, "down"));
	config_key_bind(IK_escape, config_get_value(config_buffer, "escape"));
	CL_LOG_FUNC_END("")
}


// CHANGE: load func should create the file if not found
static int config_load(void) {

	CL_LOG_FUNC_START("")
	File file_config = read_from_file("./config.ini");
	CL_VALIDATE(file_config.is_valid, return 1, "Loaded file [./config.ini]", "Could not read file [./config.ini]");

	load_controlls(file_config.data);
	free(file_config.data);

	CL_LOG_FUNC_END("")
	return 0;
}

//
void config_init(void) {

	CL_LOG_FUNC_START("")
	if (config_load() == 0) {
		return;
	}
	CL_LOG(Trace, "  After config_load()")
	write_to_file((void*)CONFIG_DEFAULT, strlen(CONFIG_DEFAULT), "./config.ini");

	CL_ASSERT(config_load() == 0, "", "Could not create or load config file");
	CL_LOG_FUNC_END("")
}

//
void config_key_bind(Input_key key, const char* key_name) {

	SDL_Scancode scan_code = SDL_GetScancodeFromName(key_name);
	CL_VALIDATE(scan_code != SDL_SCANCODE_UNKNOWN, return, "", "Invalid scan code when binding key: %s\n", key_name);

	global.config.keybinds[key] = scan_code;
}
