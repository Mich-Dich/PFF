
#include <util/pch_editor.h>
#include <entry_point.h>

#include "PFF_editor.h"

namespace PFF {

	// --------------------- entry-point into application ---------------------

	application* create_application() { return new PFF_editor(); }

}
