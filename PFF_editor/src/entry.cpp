
#include <PFF.h>

#include "PFF_editor.h"

namespace PFF {

	// ============================================================================= implementstion =============================================================================

	application* create_application() {

		return new PFF_editor();
	}

}
