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
void config_init(void) {
        
    // make shure file exists
    FILE* config_file;
    if (fopen_s(&config_file, configFileName, "r") != 0) {

        // File does not exist, create a new file
        CORE_VALIDATE(fopen_s(&config_file, configFileName, "w") == 0, return, "Created new config file [%s]", "Failed to create a new config file [%s]", configFileName)
        fprintf(config_file, "# config file for PFF-Engine\n");
    }
    fclose(config_file);





    CORE_LOG_INIT_SUBSYS("config")
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
        CORE_VALIDATE(fopen_s(&configFile, configFileName, "w") == 0, return EC_error_opening_file, "Created new config file [%s]", "Failed to create a new config file [%s]", configFileName)
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
    CORE_VALIDATE(fopen_s(&configFile, configFileName, "a") == 0, return EC_error_opening_file, "", "Failed to re-open for appending");

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
    CORE_VALIDATE(oldFile != NULL, return EC_error_opening_file, "", "Failed to open old file");

    fopen_s(&newFile, TEMP_FILE_NAME, "w");
    CORE_VALIDATE(newFile != NULL, fclose(oldFile); return EC_error_opening_file, "", "Failed to open new file");

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
    CORE_VALIDATE(!remove(configFileName), , "", "Failed to remove file: %s", ERROR_STR);
    CORE_VALIDATE(!rename(TEMP_FILE_NAME, configFileName), return EC_error_opening_file, "", "Failed to rename file [%s] %s", TEMP_FILE_NAME, ERROR_STR);
    return EC_success;
}