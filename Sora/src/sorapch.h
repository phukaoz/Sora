#pragma once

#include "Sora/Core/PlatformDetection.h"

#ifdef SORA_PLATFORM_WINDOWS
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

#include "Sora/Core/Log.h"
#include "Sora/Debug/Instrumentor.h"

#ifdef SORA_PLATFORM_WINDOWS
	#include <Windows.h>
#endif