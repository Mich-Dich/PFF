
#include <PFF.h>

class Sandbox : public PFF::application {

public:

	Sandbox() {
		LOG(Debug, "Sandbox Constructor");
	}
	~Sandbox() {}

};

PFF::application* PFF::create_application() {

	return new Sandbox();
}
