/*
- hoodhokage by ibanned -
*/

#include "MetaInfo.h"
#include "Utilities.h"

void PrintMetaHeader()
{
	printf("                                  hood");
	Utilities::SetConsoleColor(FOREGROUND_INTENSE_GREEN);
	printf("hokage\n");
	Utilities::SetConsoleColor(FOREGROUND_WHITE);
	Utilities::Log("Build %s", __DATE__);
	Utilities::Log("Setting Up hoodhokage for %s", hoodhokage_META_GAME);
}