#pragma once

#include <SDKDDKVer.h>

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN         // Exclude rarely-used stuff from Windows headers
#endif

#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>

#include <assert.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wrl.h>
#include <process.h>
#include <shellapi.h>

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <numbers>
#include <string>
#include <thread>
#include <vector>


#if defined(_DEBUG)
    #define ENABLE_D3D12_DEBUG_INTERFACE    (1)
#else
    #define ENABLE_D3D12_DEBUG_INTERFACE    (0)
#endif
