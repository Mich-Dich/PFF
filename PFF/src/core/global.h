#pragma once

#include "core/pch.h"
#include "render/render.h"

typedef struct global {
    Render_State render;
} Global;

extern Global global;
