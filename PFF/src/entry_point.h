#pragma once

#include "application.h"
#include "game_types.h"

extern bool create_game(game* out_game);


int main(void) {
	
	log_init("main", "logs", "[$B$L$X$E] [$B$F: $G$E] - $B$C$E$Z", CL_THREAD_ID, CL_FALSE);

	game game_inst = { 0 };

	// validate user defines creation
	CL_ASSERT(create_game(&game_inst), "called create_game()", "Failed to create game");
	CL_ASSERT(game_inst.initalize && game_inst.update && game_inst.render && game_inst.on_resize, "All function Pointers are set", "The games function Pointers musst be assigned");

	// create application
	CL_ASSERT(application_create(&game_inst), "Creating application", "Failed to create application");

	// begin Game-Loop
	CL_ASSERT(application_run(), "Application finished successfully", "application did not shutdown gracefully");

	return 0;
}