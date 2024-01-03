#include <stdio.h>

#include "main.h"
#include "mp_session.h"

gSScriptInit& GetScriptInit()
{
    static gSScriptInit s_ScriptInit;
    return s_ScriptInit;
}

extern "C" __declspec(dllexport)
gSScriptInit const * GE_STDCALL ScriptInit()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    MPSession::GetInstance();
    return &GetScriptInit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
    switch(dwReason)
    { 
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hModule);
            break;
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}