/*
- hoodhokage by ibanned -
*/

#pragma once

// It's actually in DLLMain but w/e
extern bool DoUnload;

#include "Utilities.h"

namespace resolvokek
{
	extern int Shots;
	extern int missedshots;
	extern float RealAngle;
	extern float FakeAngle;
	extern bool shouldflip;
	extern bool ySwitch;
	extern float NextTime;
	extern int reso;
	extern float fakeAngle;
	extern float OldSimulationTime[65];
	extern bool error;
}

#include "Interfaces.h"
namespace Hooks
{
	void Initialise();
	void DrawBeamd(Vector src, Vector end, Color color);
	void UndoHooks();

	// VMT Managers
	extern Utilities::Memory::VMTManager VMTPanel; // Hooking drawing functions
	extern Utilities::Memory::VMTManager VMTClient; // Maybe CreateMove
	extern Utilities::Memory::VMTManager VMTClientMode; // CreateMove for functionality
	extern Utilities::Memory::VMTManager VMTModelRender; // DrawModelEx for chams
	extern Utilities::Memory::VMTManager VMTPrediction; // InPrediction for no vis recoil
	extern Utilities::Memory::VMTManager VMTPlaySound; // Autoaccept shit
	extern Utilities::Memory::VMTManager VMTRenderView;
	extern Utilities::Memory::VMTManager VMTEventManager;
	extern Utilities::Memory::VMTManager VMTSurface;
};

namespace Resolver
{
	extern bool didhitHS;
}

extern bool flipAA;
extern float lineRealAngle;
extern float lineFakeAngle;
extern float RealAngle;
extern float FakeAngle;
extern float headPos;
extern float flipBool;
extern float lby;
extern int predicting;

