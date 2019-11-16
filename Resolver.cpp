#include "Resolver.h"
#include "Ragebot.h"
#include "Hooks.h"

float YawDelta[64];
float reset[64];
float Delta[64];
float OldLowerBodyYaw[64];
float Resolved_angles[64];
int iSmart;
static int jitter = -1;


float RandomFloat(float min, float max)
{
	typedef float(*RandomFloat_t)(float, float);
	return ((RandomFloat_t)GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat")) (min, max);
}
