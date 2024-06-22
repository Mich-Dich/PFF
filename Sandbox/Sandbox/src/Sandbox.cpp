
#include "Sandbox.h"

Sandbox::Sandbox() {

	LOG(Trace, "register player controller");
}

Sandbox::~Sandbox() { }

bool Sandbox::init() {

	LOG(Trace, "init editor logic");
	return true;
}

bool Sandbox::shutdown() { return true; }

bool Sandbox::render(const f32 delta_time) { return true; }

bool Sandbox::update(const f32 delta_time) { return true; }
