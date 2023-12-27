#include "config.h"

#include <SDL.h>
#include <io.h>

#include "engine/global.h"

#include "config.h"


#define TEMP_FILE_NAME "./temp.ini"

// ------------------------------------------------------------------------------------------ static vaiables -----------------------------------------------------------------------------------------

// Static variable to store the file name
static const char* configFileName = "./config.ini";

static const char* CONFIG_DEFAULT =
	"[controls]\n"
	"left = A\n"
	"right = D\n"
	"up = W\n"
	"down = S\n"
	"escape = Escape\n"
	"\n";
static char tmp_buffer[20] = { 0 };

// ------------------------------------------------------------------------------------------ privete funcs  ------------------------------------------------------------------------------------------

error_code __overrideLine(int lineToOverride, const char* newLine);

// ========================================================================================== main functions ==========================================================================================

//
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
	
	/*	OLD VERSION
	config_key_bind(IK_left, "A");
	config_key_bind(IK_left, config_get_value(file_config.data, "left"));
	config_key_bind(IK_right, config_get_value(file_config.data, "right"));
	config_key_bind(IK_down, config_get_value(file_config.data, "down"));
	config_key_bind(IK_escape, config_get_value(file_config.data, "escape"));*/
	
	free(file_config.data);

	LOG_FUNC_END("")
	return 0;
}

//
void config_init(void) {

    LOG_FUNC_START("");
    
    // make shure file exists
    FILE* config_file;
    if (fopen_s(&config_file, configFileName, "r") != 0) {

        // File does not exist, create a new file
        CL_VALIDATE(fopen_s(&config_file, configFileName, "w") == 0, return, "Created new config file [%s]", "Failed to create a new config file [%s]", configFileName)
        fprintf(config_file, "# config file for PFF-Engine");
    }
    fclose(config_file);





	LOG_FUNC_END("")
}

/*
//
void config_key_bind(input_key key, const char* key_name) {
	
	SDL_Scancode scan_code = SDL_GetScancodeFromName(key_name);
	CL_VALIDATE(scan_code != SDL_SCANCODE_UNKNOWN, return, "", "Invalid scan code when binding key: %s\n", key_name);

	global.config.keybinds[key] = scan_code;
}
*/




bool override_input_action_key_bind_bool(const char* action_name, const char* key) {


}

bool override_input_action_key_bind_float(const char* action_name, const char* plus, const char* minus) {


}






// Function to read from the config file
error_code config_try_to_find(const char* key, char* value) {

    FILE* configFile;
    if (fopen_s(&configFile, configFileName, "r") != 0) {
        // 
        return EC_error_opening_file;
    }

    char line[MAX_CONFIG_FIELD_LENGTH * 2];
    while (fgets(line, sizeof(line), configFile) != NULL) {

        if (line[0] == '#')
            continue;

        char currentKey[MAX_CONFIG_FIELD_LENGTH];
        char currentValue[MAX_CONFIG_FIELD_LENGTH];
        if (sscanf_s(line, "%49[^=]=%49[^\n]", currentKey, (unsigned int)sizeof(currentKey), currentValue, (unsigned int)sizeof(currentValue)) == 2) {
            if (strncmp(key, currentKey, (MAX_CONFIG_FIELD_LENGTH - 1)) == 0) {

                // Found the key, copy the corresponding value
                strncpy_s(value, MAX_CONFIG_FIELD_LENGTH, currentValue, MAX_CONFIG_FIELD_LENGTH - 1);

                fclose(configFile);
                return EC_success;
            }
        }
    }

    fclose(configFile);
    return EC_generic_not_found; // Key not found
}


// Function to write to the config file
error_code config_save_a_configuration(const char* key, const char* value, const bool override) {

    // make shure file exists
    FILE* configFile;
    if (fopen_s(&configFile, configFileName, "r") != 0) {

        // File does not exist, create a new file
        CL_VALIDATE(fopen_s(&configFile, configFileName, "w") == 0, return EC_error_opening_file, "Created new config file [%s]", "Failed to create a new config file [%s]", configFileName)
            fprintf(configFile, "# config file for PFF-Engine");
    }

    // check if the key is already present
    char line[MAX_CONFIG_FIELD_LENGTH *2];
    int currentLine = 0;
    while (fgets(line, sizeof(line), configFile) != NULL) {

        currentLine++;
        char currentKey[MAX_CONFIG_FIELD_LENGTH];
        if (sscanf_s(line, "%49[^=]", currentKey, (unsigned int)sizeof(currentKey)) == 1) {
            if (strncmp(key, currentKey, MAX_CONFIG_FIELD_LENGTH - 1) == 0) {
                
                // Key is found
                if (override) {

                    fclose(configFile);
                    char newLine[MAX_CONFIG_FIELD_LENGTH * 2];
                    sprintf_s(newLine, sizeof(newLine), "%s=%s", key, value);
                    __overrideLine(currentLine, newLine);
                }
                fclose(configFile);
                return EC_success;
            }
        }
    }

    // Key is not present, add a new key-value pair
    fclose(configFile);
    CL_VALIDATE(fopen_s(&configFile, configFileName, "a") == 0, return EC_error_opening_file, "", "Failed to re-open for appending");

    fprintf(configFile, "%s=%s\n", key, value);
    fclose(configFile);
    return EC_success;
}



















// ------------------------------------------------------------------------------------------ privete funcs  ------------------------------------------------------------------------------------------

//
error_code __overrideLine(int lineToOverride, const char* newLine) {

    FILE* oldFile, * newFile;
    char buffer[1024];
    int currentLine = 0;

    fopen_s(&oldFile, configFileName, "r");
    CL_VALIDATE(oldFile != NULL, return EC_error_opening_file, "", "Failed to open old file");

    fopen_s(&newFile, TEMP_FILE_NAME, "w");
    CL_VALIDATE(newFile != NULL, fclose(oldFile); return EC_error_opening_file, "", "Failed to open new file");

    // replace line with the new content OR copy line as is
    while (fgets(buffer, sizeof(buffer), oldFile) != NULL) {

        currentLine++;
        if (currentLine == lineToOverride)
            fprintf(newFile, "%s\n", newLine);
        else
            fprintf(newFile, "%s", buffer);
    }

    fclose(oldFile);
    fclose(newFile);

    // Rename the new file to the original filename
    CL_VALIDATE(!remove(configFileName), , "", "Failed to remove file: %s", ERROR_STR);
    CL_VALIDATE(!rename(TEMP_FILE_NAME, configFileName), return EC_error_opening_file, "", "Failed to rename file [%s] %s", TEMP_FILE_NAME, ERROR_STR);
    return EC_success;
}