#pragma once

// -------------------------------------------------------------------------------------------------------------------------------------------
// Defines that have influence of system behavior
// -------------------------------------------------------------------------------------------------------------------------------------------

// collect timing-data from evey magor function?
#define PFF_PROFILE								1	// general
#define PFF_PROFILE_RENDERER					0	// renderer

// log assert and validation behaviour?
// NOTE - expr in assert/validation will still be executed
#define ENABLE_LOGGING_OF_VALIDATION			1
#define ENABLED_LOGGING_OF_ASSERTS				1

// log client assert and validation behaviour?
// NOTE - expr in assert/validation will still be executed
#define ENABLE_LOGGING_OF_CLIENT_VALIDATION		1
#define ENABLED_LOGGING_OF_CLIENT_ASSERTS		1
