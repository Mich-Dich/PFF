#include <PFF.h>


class Sandbox : public PFF::application {

public:
	Sandbox() {

		LOG(Trace, "Start Custom Application");
		m_Window = new PFF::Window("Custom", 900, 600);
	}
	~Sandbox() {}

private:

};

PFF::application* PFF::Create_application() {

	return new Sandbox();
}
