// dllmain.cpp : Defines the entry point for the DLL application..
#include "Index.hpp"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    int32_t tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpFlag |= _CRTDBG_ALLOC_MEM_DF;
    tmpFlag = (tmpFlag & 0x0000FFFF) | _CRTDBG_CHECK_EVERY_128_DF;
    _CrtSetDbgFlag(tmpFlag);
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH: {
        break;
    }
    case DLL_THREAD_ATTACH: {
        break;
    }
    case DLL_THREAD_DETACH: {
        break;
    }
    case DLL_PROCESS_DETACH: {
        _CrtDumpMemoryLeaks();
        break;
    }
    }
    return TRUE;
}