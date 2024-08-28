#pragma once

#include "Yuki/Core/PlatformDetection.h"

#ifdef YUKI_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Yuki/Core/Log.h"
#include "Yuki/Debug/Instrumentor.h"

#ifdef YUKI_PLATFORM_WINDOWS
	#include <Windows.h>
#endif