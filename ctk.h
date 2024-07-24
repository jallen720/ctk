#pragma once

#include <limits.h>
#include <math.h>
#include <xmmintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

namespace CTK
{

// Core
#include "ctk/common.h"
#include "ctk/io.h"
#include "ctk/debug.h"
#include "ctk/ascii_parsing.h"
#include "ctk/c_array.h"
#include "ctk/c_string.h"
#include "ctk/f_array.h"
#include "ctk/f_map.h"
#include "ctk/f_string.h"
#include "ctk/math.h"
#include "ctk/optional.h"
#include "ctk/pair.h"

// Allocators
#include "ctk/allocator.h"
#include "ctk/stack.h"
#include "ctk/free_list.h"
#include "ctk/free_list_debug.h"
#include "ctk/global_allocators.h"

// Collections
#include "ctk/array.h"
#include "ctk/string.h"
#include "ctk/pool.h"
#include "ctk/ring_buffer.h"

// System
#include "ctk/win32.h"
#include "ctk/file.h"
#include "ctk/json.h"
#include "ctk/window_keymap.h"
#include "ctk/window.h"
#include "ctk/thread_pool.h"

// Utils
#include "ctk/testing.h"
#include "ctk/profile.h"

}
