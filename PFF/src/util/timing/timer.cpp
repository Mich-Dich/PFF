
#include "util/pffpch.h"

#include "timer.h"

namespace PFF::util {

    void cancel_timer(timer& timer) {

        timer.shared_state->first = true;
        timer.shared_state->second.notify_one();
        if (timer.future.valid()) {
            timer.future.wait();
        }
    }

}
