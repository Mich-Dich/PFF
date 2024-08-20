#pragma once

#include "application.h"


class Sandbox : public PFF::application {
public:

	Sandbox();
	~Sandbox();

	bool init() override;
	bool render(const f32 delta_time) override;
	bool update(const f32 delta_time) override;
	bool shutdown() override;

private:

};
