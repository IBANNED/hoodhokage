#include <fstream>
#include <ostream>


// General shit
#include "DLLMain.h"
#include "Utilities.h"
//#include "process.h"
// Injection stuff
#include "INJ/ReflectiveLoader.h"

// Stuff to initialise
#include "Offsets.h"
#include "Interfaces.h"
#include "Hooks.h"
#include "RenderManager.h"
#include "Hacks.h"
#include "Menu.h"
#include "CBulletListener.h"
#include "AntiAntiAim.h"
#include "MiscHacks.h"
#include "Dumping.h"

float lineLBY;
float lineRealAngle;
float lineFakeAngle;
float headPos;
float flipBool;
float lby;
int predicting;

// Used as part of the reflective DLL injection
extern HINSTANCE hAppInstance;

// Our DLL Instance
HINSTANCE HThisModule;
bool DoUnload;

//UCHAR
//szFileSys[255],
//szVolNameBuff[255];
float autowalldmgtest[65];
//
//HWND Findx = 0;
//bool Espere = true;
//DWORD processID = 0;
//DWORD Address, Valor = 0;
//DWORD WValor = 2;
//
//DWORD
//dwMFL,
//dwSysFlags,
//dwSerial;

// Our thread we use to setup everything we need
// Everything appart from code in hooks get's called from inside 
// here.

/*#define stacker (0)
#define binary (-2077646081)
#define kencastle (0)*/


bool is_file_exist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

void create_folder(const char * path) {
	if (!CreateDirectory(path, NULL)) {
		return;
	}
}

void initialize_configs_file() {

	create_folder("hoodhokage");
	create_folder("hoodhokage\\cfg");

	if (!is_file_exist("hoodhokage\\cfg\\hoodhokage.txt")) {
		std::ofstream("hoodhokage\\cfg\\hoodhokage.txt");
	}
}

int InitialThread()
{
	// Intro banner with info
	PrintMetaHeader();
	initialize_configs_file();

	// Initialise all our shit
	//CheckLoader();
	Offsets::Initialise(); // Set our VMT offsets and do any pattern scans
	Interfaces::Initialise(); // Get pointers to the valve classes
	NetVar.RetrieveClasses(); // Setup our NetVar manager (thanks shad0w bby)
	Render::Initialise();
	Hacks::SetupHacks();
	Menu::SetupMenu();
	Hooks::Initialise();
	ApplyAAAHooks();
	ApplyNetVarsHooks();
	// Dumping
	Dump::DumpClassIds();

	Utilities::Log("[hoodhokage] Systems Awake");

	return 0;
}

const char* checkcsgotype()
{
	if (strstr(GetCommandLineA(), "-scaleform"))
		return "client.dll";
	else
		return "client_panorama.dll";
}


// DllMain
// Entry point for our module
BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD     fdwReason,
	_In_ LPVOID    lpvReserved
	)
{
	switch (fdwReason)
	{
	case DLL_QUERY_HMODULE:
		if (lpvReserved != NULL)
			*(HMODULE *)lpvReserved = hAppInstance;
		break;
	case DLL_PROCESS_ATTACH:
		//CheckValidHardwareID();
		HThisModule = hinstDLL;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InitialThread, NULL, NULL, NULL);
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}