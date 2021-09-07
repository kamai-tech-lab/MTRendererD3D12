/// @file Main.cpp
/// @author Masayoshi Kamai

#include "stdafx.h"
#include "MTRendererD3D12.h"

/// @brief Win32エントリポイント
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

    MTRenderer renderer(hInstance, nCmdShow);
    if (!renderer.Init()) {
        return 0;
    }
    int ret = renderer.Run();
    renderer.Deinit();

    return ret;
}
