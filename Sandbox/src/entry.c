
#include <PFF.h>
#include <entry_point.h>

#include "game.h"


// creating game
bool create_game(game* out_game) {

	out_game->app_config.start_width = 800;
	out_game->app_config.start_hight = 600;
	out_game->app_config.name = "TestApp";
	out_game->app_config.target_fps = 120;
	out_game->initalize = game_initalize;
	out_game->update = game_update;
	out_game->render = game_render;
	out_game->on_resize = game_on_resize;
	out_game->state = malloc(sizeof(game_state));

	return true;
}
