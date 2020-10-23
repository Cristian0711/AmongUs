// dllmain.cpp : Defines the entry point for the DLL application.

#include "pch.h"
#include "il2cpp.h"
#include <Windows.h>
#include <process.h>
#include <iostream>
#include <Psapi.h>
#include <stdint.h>
#include <vector>

DWORD g_dwGameAssembly = NULL;
HMODULE g_dllModule;

struct AmongUsName
{
    char name[7];
};


std::string GetPlayerName(GameData_PlayerInfo_o *player)
{
    std::string name;
    for (int i = 0; i < player->fields.PlayerName->fields.m_stringLength; i++)
        name += *(uint16_t*)(&player->fields.PlayerName->fields.m_firstChar + i);
    return name;
}

template<typename N>
void GetName(N n, GameData_PlayerInfo_o* player)
{
    char name[n] = *reinterpret_cast<AmongUsName**>(g_dwGameAssembly + 0xDA5ACC);
}

void exitCheat()
{
    FreeConsole();
    FreeLibraryAndExitThread(g_dllModule, 0);
    system("pause");
}

uint32_t FindPtrOffset(uint32_t addr, std::vector<uint32_t> offsets)
{
    uint32_t addrToPtr = *(uint32_t*)addr;
    for (uint32_t i : offsets)
        addrToPtr = *(uint32_t*)(addrToPtr + i);
    return addrToPtr;
}

void mainThread()
{
    while (g_dwGameAssembly == NULL)
    {
        g_dwGameAssembly = (DWORD)LoadLibraryA("GameAssembly.dll");
        Sleep(100);
    }

    std::cout << "Found the library of GameAssembly.dll, base: 0x" << std::hex << g_dwGameAssembly << std::endl;

    auto pGame = *reinterpret_cast<AmongUsClient_c**>(g_dwGameAssembly + 0xDA5ACC);
    if (pGame == NULL)
    {
        std::cout << "Failed to initialise pGame!" << std::endl;
        exitCheat();
        return;
    }

    std::cout << "pGame initialised, base: 0x" << std::hex << pGame << std::endl;

    while (true)
    {
        if (GetAsyncKeyState(VK_END))
        {
            exitCheat();
            return;
        }

        if (GetAsyncKeyState(VK_HOME))
        {
            if (pGame->static_fields->Instance->fields.GameState == 0)
                continue;

            system("cls");

            uint32_t ptrToRVA = FindPtrOffset(g_dwGameAssembly + 0x00DB9140, { 0xC, 0x28, 0x100, 0x160 });

            for (int i = 0; i < 10; i++)
            {
                auto pPlayer = reinterpret_cast<GameData_PlayerInfo_o * (__cdecl*)(void*, BYTE)>(ptrToRVA)(pGame->static_fields->Instance->fields.GameDataPrefab->klass->static_fields->Instance, i);
                if (!pPlayer)
                {
                    break;
                }

                std::string name = GetPlayerName(pPlayer);
                
                if (pPlayer->fields.IsImpostor)
                {
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                    std::cout << name << " is an impostor!" << std::endl;
                }
                else
                {
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
                    std::cout << name << " is not an impostor!" << std::endl;
                }
            }
        }
        Sleep(10);
    }
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        AllocConsole();
        SetConsoleTitleA("Make#1197");
        g_dllModule = hModule;
        freopen("CONOUT$", "w", stdout);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)mainThread, NULL, NULL, NULL);
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

