
#include <PFF.h>

#include "Sandbox.h"

namespace PFF {

	// ============================================================================= implementstion =============================================================================

	application* create_application() {

		return new Sandbox();
	}

}
