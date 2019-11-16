/*
- hoodhokage by ibanned -
*/

#include "Menu.h"
#include "Controls.h"
#include "Hooks.h" // for the unload meme
#include "Interfaces.h"
#include "CRC32.h"
#include <fstream>

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 470

hoodhokageWindow Menu::Window;
struct Config_t {
	int id;
	std::string name;
};
std::vector<Config_t> configs;
typedef void(__cdecl* MsgFn)(const char* msg, va_list);
void MsgX(const char* msg, ...)
{
	if (msg == nullptr)
		return; //If no string was passed, or it was null then don't do anything
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Msg"); 	char buffer[989];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	fn(buffer, list); //Calls the function, we got the address above.
}
void save_callback()
{
	int should_save = Menu::Window.MiscTab.ConfigListBox.GetIndex();
	std::string config_directory = "hoodhokage\\cfg\\";
	config_directory += configs[should_save].name; config_directory += ".xml";
	GUI.SaveWindowState(&Menu::Window, XorStr(config_directory.c_str()));
	Interfaces::CVar->ConsoleColorPrintf(Color(30, 255, 30, 255), "[hoodhokage] ");
	std::string uremam = "saved configuration.     \n";
	MsgX(uremam.c_str());
}
void load_callback()
{
	int should_load = Menu::Window.MiscTab.ConfigListBox.GetIndex();
	std::string config_directory = "hoodhokage\\cfg\\";
	config_directory += configs[should_load].name; config_directory += ".xml";
	GUI.LoadWindowState(&Menu::Window, XorStr(config_directory.c_str()));
	Interfaces::CVar->ConsoleColorPrintf(Color(30, 255, 30, 255), "[hoodhokage] ");
	std::string uremam = "loaded configuration.     \n";
	MsgX(uremam.c_str());
}
void list_configs() {
	configs.clear();
	Menu::Window.MiscTab.ConfigListBox.ClearItems();
	std::ifstream file_in;
	file_in.open("hoodhokage\\cfg\\hoodhokage.txt");
	if (file_in.fail()) {
		std::ofstream("hoodhokage\\cfg\\hoodhokage.txt");
		file_in.open("hoodhokage\\cfg\\hoodhokage.txt");
	}
	int line_count;
	while (!file_in.eof()) {
		Config_t config;
		file_in >> config.name;
		config.id = line_count;
		configs.push_back(config);
		line_count++;
		Menu::Window.MiscTab.ConfigListBox.AddItem(config.name);
	}
	file_in.close();
	if (configs.size() > 7) Menu::Window.MiscTab.ConfigListBox.AddItem(" ");
}
void add_config() {
	std::fstream file;
	file.open("hoodhokage\\cfg\\hoodhokage.txt", std::fstream::app);
	if (file.fail()) {
		std::fstream("hoodhokage\\cfg\\hoodhokage.txt");
		file.open("hoodhokage\\cfg\\hoodhokage.txt", std::fstream::app);
	}
	file << std::endl << Menu::Window.MiscTab.NewConfigName.getText();
	file.close();
	list_configs();
	int should_add = Menu::Window.MiscTab.ConfigListBox.GetIndex();
	std::string config_directory = "hoodhokage\\cfg\\";
	config_directory += Menu::Window.MiscTab.NewConfigName.getText(); config_directory += ".xml";
	GUI.SaveWindowState(&Menu::Window, XorStr(config_directory.c_str()));
	Menu::Window.MiscTab.NewConfigName.SetText("");
}
void remove_config() {
	int should_remove = Menu::Window.MiscTab.ConfigListBox.GetIndex();
	std::string config_directory = "hoodhokage\\cfg\\";
	config_directory += configs[should_remove].name; config_directory += ".xml";
	std::remove(config_directory.c_str());
	std::ofstream ofs("hoodhokage\\cfg\\hoodhokage.txt", std::ios::out | std::ios::trunc);
	ofs.close();
	std::fstream file;
	file.open("hoodhokage\\cfg\\hoodhokage.txt", std::fstream::app);
	if (file.fail()) {
		std::fstream("hoodhokage\\cfg\\hoodhokage.txt");
		file.open("hoodhokage\\cfg\\hoodhokage.txt", std::fstream::app);
	}
	for (int i = 0; i < configs.size(); i++) {
		if (i == should_remove) continue;
		Config_t config = configs[i];
		file << std::endl << config.name;
	}
	file.close();
	list_configs();
}

void UnLoadCallbk()
{
	DoUnload = true;
}

void KnifeApplyCallbk()
{
	static ConVar* Meme = Interfaces::CVar->FindVar("cl_fullupdate");
	Meme->nFlags &= ~FCVAR_CHEAT;
	Interfaces::Engine->ClientCmd_Unrestricted("cl_fullupdate");
}

void hoodhokageWindow::Setup()
{
	SetPosition(50, 50);
	SetSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	SetTitle("hoodhokage");

	RegisterTab(&RageBotTab);
	RegisterTab(&LegitBotTab);
	RegisterTab(&VisualsTab);
	RegisterTab(&MiscTab);
	//RegisterTab(&AntiAimTab);
    //RegisterTab(&ColorTab);
	RegisterTab(&SkinTab);
	//RegisterTab(&Playerlist);

	RECT Client = GetClientArea();
	Client.bottom -= 29;

	RageBotTab.Setup();
	LegitBotTab.Setup();
	VisualsTab.Setup();
	MiscTab.Setup();
	//AntiAimTab.Setup();
    //ColorTab.Setup();
	SkinTab.Setup();
	//Playerlist.Setup();

}

void CRageBotTab::Setup()
{
	SetTitle("rage");
	SetTitle2("C");

#pragma region Aimbot

	AimbotGroup.SetPosition(5, 23);
	AimbotGroup.SetText("aimbot");
	AimbotGroup.AddTab(CGroupTab("aimbot", 1));
	AimbotGroup.AddTab(CGroupTab("backtrack", 2));
	AimbotGroup.AddTab(CGroupTab("accuracy", 3));
	AimbotGroup.SetSize(225, 375);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl(1, "aimbot", this, &AimbotEnable);

	AimbotAutoFire.SetFileId("aim_autofire");
	AimbotGroup.PlaceLabledControl(1, "automatic shoot", this, &AimbotAutoFire);

	AimbotFov.SetFileId("aim_fov");
	AimbotFov.SetBoundaries(0.f, 180.f);
	AimbotFov.SetValue(180.f);
	AimbotGroup.PlaceLabledControl(1, "maximum fov", this, &AimbotFov);

	AimbotSilentAim.SetFileId("aim_silentmethod");
	AimbotSilentAim.AddItem("aim lock");
	AimbotSilentAim.AddItem("silent");
	AimbotSilentAim.AddItem("anti aim");
	AimbotGroup.PlaceLabledControl(1, "silent aim", this, &AimbotSilentAim);

	AimbotAutoPistol.SetFileId("aim_autopistol");
	AimbotGroup.PlaceLabledControl(1, "auto pistol", this, &AimbotAutoPistol);

	Autorevolver.SetFileId("acc_autorevolver");
	AimbotGroup.PlaceLabledControl(1, "auto revolver", this, &Autorevolver);

	AimbotAutoRevolver.SetFileId("aim_acock");
	AimbotGroup.PlaceLabledControl(1, "automatic revolver", this, &AimbotAutoRevolver);
	
	AimbotAimStep.SetFileId("aim_aimstep");
	AimbotGroup.PlaceLabledControl(1, "speed limit", this, &AimbotAimStep);

	AccuracyResolverenable.SetFileId("acc_reslvrenbl");
	AimbotGroup.PlaceLabledControl(1, "resolver correct", this, &AccuracyResolverenable);

	//

	AimbotBacktrack2.SetFileId("legit_backtrack");
	AimbotGroup.PlaceLabledControl(2, "backtrack", this, &AimbotBacktrack2); 

	TickModulation2.SetFileId("tick_modulate");
	TickModulation2.SetBoundaries(0.1f, 13.f);
	TickModulation2.SetValue(13.f);
	AimbotGroup.PlaceLabledControl(2, "tick", this, &TickModulation2);

	AccuracyPrediction.SetFileId("acc_engnprd");
	AimbotGroup.PlaceLabledControl(2, "prediction", this, &AccuracyPrediction);

	AccuracyPositionAdjustment.SetFileId("acc_posadj");
	AimbotGroup.PlaceLabledControl(2, "disabled interpolation", this, &AccuracyPositionAdjustment);

	//

	AccuracyRecoil.SetFileId("acc_norecoil");
	AimbotGroup.PlaceLabledControl(3, "remove recoil", this, &AccuracyRecoil);

	AccuracyAutoScope.SetFileId("acc_scope");
	AimbotGroup.PlaceLabledControl(3, "automatic scope", this, &AccuracyAutoScope);

	AccuracyDrop.SetFileId("acc_accuracydrop");
	AccuracyDrop.AddItem("hit chance");
	AccuracyDrop.AddItem("spread limit");
	AimbotGroup.PlaceLabledControl(3, "accuracy", this, &AccuracyDrop);

	AccuracyHitchanceVal.SetFileId("acc_chance");
	AccuracyHitchanceVal.SetBoundaries(0, 100);
	AccuracyHitchanceVal.SetValue(0);
	AimbotGroup.PlaceLabledControl(3, "", this, &AccuracyHitchanceVal);

	AccuracyMinimumDamage.SetFileId("acc_mindmg");
	AccuracyMinimumDamage.SetBoundaries(1.f, 99.f);
	AccuracyMinimumDamage.SetValue(1.f);
	AimbotGroup.PlaceLabledControl(3, "minimum damage", this, &AccuracyMinimumDamage);

	AutoMinimumDamage.SetFileId("acc_AutoMinimumDamage");
	AimbotGroup.PlaceLabledControl(3, "dynamic damage", this, &AutoMinimumDamage);

	AccuracyAutoStop.SetFileId("acc_stop");
	AimbotGroup.PlaceLabledControl(3, "quick stop", this, &AccuracyAutoStop);

	//end accuracy

#pragma endregion Aimbot Controls Get Setup in here

#pragma region Target
	TargetGroup.SetPosition(235, 23); //344
	TargetGroup.SetText("target");
	TargetGroup.AddTab(CGroupTab("target", 1));
	TargetGroup.AddTab(CGroupTab("scale", 2));
	TargetGroup.SetSize(225, 375);
	RegisterControl(&TargetGroup);

	TargetSelection.SetFileId("tgt_selection");
	TargetSelection.AddItem("cross");
	TargetGroup.PlaceLabledControl(1, "detection", this, &TargetSelection);

	TargetHitbox.SetFileId("tgt_hitbox");
	TargetHitbox.AddItem("head");
	TargetHitbox.AddItem("neck");
	TargetHitbox.AddItem("chest");
	TargetHitbox.AddItem("pelvis");
	TargetHitbox.AddItem("shin");
	TargetGroup.PlaceLabledControl(1, "hitbox", this, &TargetHitbox);

	TargetHitscan.SetFileId("tgt_hitscan");
	TargetHitscan.AddItem("none"); //0
	TargetHitscan.AddItem("low"); // 1
	TargetHitscan.AddItem("medium"); // 2
	TargetHitscan.AddItem("high"); // 3
	TargetGroup.PlaceLabledControl(1, "hitscan", this, &TargetHitscan);

	//

	TargetMultipoint.SetFileId("tgt_multipoint");
	TargetGroup.PlaceLabledControl(2, "multipoint", this, &TargetMultipoint);

	TargetPointscale.SetFileId("tgt_pointscale");
	TargetPointscale.SetBoundaries(0.f, 10.f);
	TargetPointscale.SetValue(5.f);
	TargetGroup.PlaceLabledControl(2, "point scale", this, &TargetPointscale);

	BaimIfUnderXHealth.SetFileId("acc_BaimIfUnderXHealth");
	BaimIfUnderXHealth.SetBoundaries(0, 100);
	BaimIfUnderXHealth.SetValue(0);
	TargetGroup.PlaceLabledControl(2, "body x health", this, &BaimIfUnderXHealth);

#pragma endregion Targetting controls 

#pragma region Accuracy
	AccuracyGroup.SetPosition(465, 23); //344
	AccuracyGroup.SetText("anti aim");
	AccuracyGroup.AddTab(CGroupTab("anti aim", 1));
	AccuracyGroup.AddTab(CGroupTab("misc", 2));
	AccuracyGroup.AddTab(CGroupTab("bind", 3));
	AccuracyGroup.SetSize(235, 375);
	RegisterControl(&AccuracyGroup);

	//tabs 1

	AntiAimEnable.SetFileId("aa_enable");
	AccuracyGroup.PlaceLabledControl(1, "enable", this, &AntiAimEnable);

	AntiAimPitch.SetFileId("aa_x");
	AntiAimPitch.AddItem("none");
	AntiAimPitch.AddItem("emotion");
	AntiAimPitch.AddItem("up*");
	AntiAimPitch.AddItem("minimal*");
	AntiAimPitch.AddItem("confuser r*");
	AccuracyGroup.PlaceLabledControl(1, "pitch", this, &AntiAimPitch);

	staticyaw.SetFileId("aa_y");
	staticyaw.AddItem("none");//0
	staticyaw.AddItem("backwards");//1
	staticyaw.AddItem("automatic mouse");//2
	staticyaw.AddItem("backwards jitter");//4
	staticyaw.AddItem("lowerbody");//5
	staticyaw.AddItem("random");//6
	staticyaw.AddItem("manual");//6
	staticyaw.AddItem("spinbot");//7
	AccuracyGroup.PlaceLabledControl(1, "real", this, &staticyaw);

	fakeyaw.SetFileId("aa_idley");//0
	fakeyaw.AddItem("none");
	fakeyaw.AddItem("backwards");//1
	fakeyaw.AddItem("automatic mouse");//2
	fakeyaw.AddItem("backwards jitter");//4
	fakeyaw.AddItem("lowerbody");//5
	fakeyaw.AddItem("random");//6
	fakeyaw.AddItem("manual");//6
	fakeyaw.AddItem("spinbot");//7
	AccuracyGroup.PlaceLabledControl(1, "fake", this, &fakeyaw);

	//tabs 2
	BreakLBY.SetFileId("aa_aaresolver2");
	AccuracyGroup.PlaceLabledControl(2, "force 180 lby", this, &BreakLBY);

	AntiAimKnife.SetFileId("aa_knife");
	AccuracyGroup.PlaceLabledControl(2, "on knife", this, &AntiAimKnife);

	AntiAimTarget.SetFileId("aa_target");
	AccuracyGroup.PlaceLabledControl(2, "at target", this, &AntiAimTarget);

	AntiAimOffset.SetFileId("aa_offset");
	AntiAimOffset.SetBoundaries(0.f, 360.f);
	AntiAimOffset.SetValue(0.f);
	AccuracyGroup.PlaceLabledControl(2, "jitter modifier", this, &AntiAimOffset);

	SpinSpeed.SetFileId("aa_AntiAimSpinspeed");
	SpinSpeed.SetBoundaries(0.f, 100.f);
	SpinSpeed.SetValue(0.f);
	AccuracyGroup.PlaceLabledControl(2, "spin speed", this, &SpinSpeed);

	//tabs 3
	keybasedaaright2.SetFileId("otr_keybasedrightx");
	AccuracyGroup.PlaceLabledControl(3, "left", this, &keybasedaaright2);

	keybasedaaright.SetFileId("otr_keybasedright");
	AccuracyGroup.PlaceLabledControl(3, "", this, &keybasedaaright);

	keybasedaaleft2.SetFileId("otr_keybasedleftx");
	AccuracyGroup.PlaceLabledControl(3, "right", this, &keybasedaaleft2);

	keybasedaaleft.SetFileId("otr_keybasedleft");
	AccuracyGroup.PlaceLabledControl(3, "", this, &keybasedaaleft);

	keybasedaaback2.SetFileId("otr_keybasedbackx");
	AccuracyGroup.PlaceLabledControl(3, "backwards", this, &keybasedaaback2);

	keybasedaaback.SetFileId("otr_keybasedback");
	AccuracyGroup.PlaceLabledControl(3, "", this, &keybasedaaback);

	AnimFixz.SetFileId("otr_animfixz");
	AccuracyGroup.PlaceLabledControl(3, "anim fix", this, &AnimFixz);

#pragma endregion  Accuracy controls get Setup in here
}

void CLegitBotTab::Setup()
{
	SetTitle("legit");
	SetTitle2("D");

#pragma region Aimbot
	AimbotGroup.SetPosition(5, 13);
	AimbotGroup.SetText("aimbot");
	AimbotGroup.AddTab(CGroupTab("aimbot", 1));
	AimbotGroup.AddTab(CGroupTab("other", 2));
	AimbotGroup.SetSize(256, 180);
	RegisterControl(&AimbotGroup);

	AimbotEnable.SetFileId("aim_enable");
	AimbotGroup.PlaceLabledControl(1, "legitbot", this, &AimbotEnable);

//	AimbotAutoFire.SetFileId("aim_autofire");
//	AimbotGroup.PlaceLabledControl(1, "Automatic Shoot", this, &AimbotAutoFire);

	AimbotAutoPistol.SetFileId("aim_apistol");
	AimbotGroup.PlaceLabledControl(1, "auto pistol", this, &AimbotAutoPistol);

	AimbotKeyPress.SetFileId("aim_usekey");
	AimbotGroup.PlaceLabledControl(1, "on key press", this, &AimbotKeyPress);

	AimbotKeyBind.SetFileId("aim_key");
	AimbotGroup.PlaceLabledControl(1, "", this, &AimbotKeyBind);

	OtherRadar.SetFileId("legit_radar");
	AimbotGroup.PlaceLabledControl(2, "radar", this, &OtherRadar);

	AimbotBacktrack.SetFileId("legit_backtrack");
	AimbotGroup.PlaceLabledControl(2, "backtrack", this, &AimbotBacktrack);

	TickModulation.SetFileId("tick_modulate");
	TickModulation.SetBoundaries(0.1f, 13.f);
	TickModulation.SetValue(13.f);
	AimbotGroup.PlaceLabledControl(2, "tick backtrack", this, &TickModulation);

	AimIndicator.SetFileId("FovIndicator");
	AimbotGroup.PlaceLabledControl(2, "Fov Indicator", this, &AimIndicator);

	//LegitChoice.SetFileId("legit_aa");
	//LegitChoice.AddItem("none");
	//LegitChoice.AddItem("Backwards");
	//LegitChoice.AddItem("Sideways");
	//LegitChoice.AddItem("Manual");
	//AimbotGroup.PlaceLabledControl(2, "Legit AA", this, &LegitChoice);
//	AimbotFriendlyFire.SetFileId("aim_friendfire");
//	AimbotGroup.PlaceLabledControl(0, "Friendly Fire", this, &AimbotFriendlyFire);

#pragma endregion Aimbot shit

#pragma region Main Weapon
	WeaponMainGroup.SetPosition(5, 205);
	WeaponMainGroup.SetText2("W");
	WeaponMainGroup.AddTab2(CGroupTab("W", 1));
	WeaponMainGroup.AddTab2(CGroupTab("T", 2));
	WeaponMainGroup.AddTab2(CGroupTab("U", 3));
	WeaponMainGroup.AddTab2(CGroupTab("R", 4));
	WeaponMainGroup.AddTab2(CGroupTab("O", 5));
	WeaponMainGroup.AddTab2(CGroupTab("P", 6));
	WeaponMainGroup.SetSize(256, 195);
	RegisterControl(&WeaponMainGroup);

	WeaponMainSpeed.SetFileId("main_speedS");
	WeaponMainSpeed.SetBoundaries(0.1f, 100.f);
	WeaponMainSpeed.SetValue(1.0f);
	WeaponMainGroup.PlaceLabledControl2(1, "smooth", this, &WeaponMainSpeed);

	WeaponMainFoV.SetFileId("main_fovS");
	WeaponMainFoV.SetBoundaries(0.1f, 30.f);
	WeaponMainFoV.SetValue(5.f);
	WeaponMainGroup.PlaceLabledControl2(1, "maximum fov", this, &WeaponMainFoV);

	WeaponMainRecoil.SetFileId("main_recoilS");
	WeaponMainGroup.PlaceLabledControl2(1, "ajust recoil", this, &WeaponMainRecoil);

	WeaponMainHitbox.SetFileId("main_hitboxS");
	WeaponMainHitbox.AddItem("head");
	WeaponMainHitbox.AddItem("neck");
	WeaponMainHitbox.AddItem("chest");
	WeaponMainHitbox.AddItem("stomach");
	WeaponMainGroup.PlaceLabledControl2(1, "hitbox", this, &WeaponMainHitbox);

	//WeaponMainInacc.SetFileId("main_inacc");
	//WeaponMainInacc.SetBoundaries(0.f, 15.f);
	//WeaponMainGroup.PlaceLabledControl2(0, "miss chance", this, &WeaponMainInacc);

	//M4A4 -------------------------------------------------------------------------

	WeaponMainSpeedM4.SetFileId("main_speedy");
	WeaponMainSpeedM4.SetBoundaries(0.1f, 100.f);
	WeaponMainSpeedM4.SetValue(1.0f);
	WeaponMainGroup.PlaceLabledControl2(2, "smooth", this, &WeaponMainSpeedM4);

	WeaponMainFoVM4.SetFileId("main_fovy");
	WeaponMainFoVM4.SetBoundaries(0.1f, 30.f);
	WeaponMainFoVM4.SetValue(5.f);
	WeaponMainGroup.PlaceLabledControl2(2, "maximum fov", this, &WeaponMainFoVM4);

	WeaponMainRecoilM4.SetFileId("main_recoily");
	WeaponMainGroup.PlaceLabledControl2(2, "ajust recoil", this, &WeaponMainRecoilM4);

	WeaponMainHitboxM4.SetFileId("main_hitboxy");
	WeaponMainHitboxM4.AddItem("head");
	WeaponMainHitboxM4.AddItem("neck");
	WeaponMainHitboxM4.AddItem("chest");
	WeaponMainHitboxM4.AddItem("stomach");
	WeaponMainGroup.PlaceLabledControl2(2, "hitbox", this, &WeaponMainHitboxM4);

	//WeaponMainInaccM4.SetFileId("main_inacc");
	//WeaponMainInaccM4.SetBoundaries(0.f, 15.f);
	//WeaponMainGroup.PlaceLabledControl2(2, "Miss Chance", this, &WeaponMainInacc);

	//AUG -------------------------------------------------------------------------

	WeaponMainSpeedaug.SetFileId("main_speedD");
	WeaponMainSpeedaug.SetBoundaries(0.1f, 100.f);
	WeaponMainSpeedaug.SetValue(1.0f);
	WeaponMainGroup.PlaceLabledControl2(3, "smooth", this, &WeaponMainSpeedaug);

	WeaponMainFoVaug.SetFileId("main_fovD");
	WeaponMainFoVaug.SetBoundaries(0.1f, 30.f);
	WeaponMainFoVaug.SetValue(5.f);
	WeaponMainGroup.PlaceLabledControl2(3, "maximum fov", this, &WeaponMainFoVaug);

	WeaponMainRecoilaug.SetFileId("main_recoilD");
	WeaponMainGroup.PlaceLabledControl2(3, "ajust recoil", this, &WeaponMainRecoilaug);

	WeaponMainHitboxaug.SetFileId("main_hitboxD");
	WeaponMainHitboxaug.AddItem("head");
	WeaponMainHitboxaug.AddItem("neck");
	WeaponMainHitboxaug.AddItem("chest");
	WeaponMainHitboxaug.AddItem("stomach");
	WeaponMainGroup.PlaceLabledControl2(3, "hitbox", this, &WeaponMainHitboxaug);

	//WeaponMainInaccM4.SetFileId("main_inacc");
	//WeaponMainInaccM4.SetBoundaries(0.f, 15.f);
	//WeaponMainGroup.PlaceLabledControl2(3, "Miss Chance", this, &WeaponMainInacc);

	//FAMAS -------------------------------------------------------------------------

	WeaponMainSpeedfamas.SetFileId("main_speedJ");
	WeaponMainSpeedfamas.SetBoundaries(0.1f, 100.f);
	WeaponMainSpeedfamas.SetValue(1.0f);
	WeaponMainGroup.PlaceLabledControl2(4, "smooth", this, &WeaponMainSpeedfamas);

	WeaponMainFoVfamas.SetFileId("main_fovJ");
	WeaponMainFoVfamas.SetBoundaries(0.1f, 30.f);
	WeaponMainFoVfamas.SetValue(5.f);
	WeaponMainGroup.PlaceLabledControl2(4, "maximum fov", this, &WeaponMainFoVfamas);

	WeaponMainRecoilfamas.SetFileId("main_recoilJ");
	WeaponMainGroup.PlaceLabledControl2(4, "ajust recoil", this, &WeaponMainRecoilfamas);

	WeaponMainHitboxfamas.SetFileId("main_hitboxJ");
	WeaponMainHitboxfamas.AddItem("head");
	WeaponMainHitboxfamas.AddItem("neck");
	WeaponMainHitboxfamas.AddItem("chest");
	WeaponMainHitboxfamas.AddItem("stomach");
	WeaponMainGroup.PlaceLabledControl2(4, "hitbox", this, &WeaponMainHitboxfamas);

	//WeaponMainInaccM4.SetFileId("main_inacc");
	//WeaponMainInaccM4.SetBoundaries(0.f, 15.f);
	//WeaponMainGroup.PlaceLabledControl2(3, "Miss Chance", this, &WeaponMainInacc);

	//MP9 -------------------------------------------------------------------------

	WeaponMainSpeedMP9.SetFileId("main_speedK");
	WeaponMainSpeedMP9.SetBoundaries(0.1f, 100.f);
	WeaponMainSpeedMP9.SetValue(1.0f);
	WeaponMainGroup.PlaceLabledControl2(5, "smooth", this, &WeaponMainSpeedMP9);

	WeaponMainFoVMP9.SetFileId("main_fovK");
	WeaponMainFoVMP9.SetBoundaries(0.1f, 30.f);
	WeaponMainFoVMP9.SetValue(5.f);
	WeaponMainGroup.PlaceLabledControl2(5, "maximum fov", this, &WeaponMainFoVMP9);

	WeaponMainRecoilMP9.SetFileId("main_recoilK");
	WeaponMainGroup.PlaceLabledControl2(5, "ajust recoil", this, &WeaponMainRecoilMP9);

	WeaponMainHitboxMP9.SetFileId("main_hitboxK");
	WeaponMainHitboxMP9.AddItem("head");
	WeaponMainHitboxMP9.AddItem("neck");
	WeaponMainHitboxMP9.AddItem("chest");
	WeaponMainHitboxMP9.AddItem("stomach");
	WeaponMainGroup.PlaceLabledControl2(5, "hitbox", this, &WeaponMainHitboxMP9);

	//WeaponMainInaccM4.SetFileId("main_inacc");
	//WeaponMainInaccM4.SetBoundaries(0.f, 15.f);
	//WeaponMainGroup.PlaceLabledControl2(6, "Miss Chance", this, &WeaponMainInacc);

	//P90 -------------------------------------------------------------------------

	WeaponMainSpeedP90.SetFileId("main_speedL");
	WeaponMainSpeedP90.SetBoundaries(0.1f, 100.f);
	WeaponMainSpeedP90.SetValue(1.0f);
	WeaponMainGroup.PlaceLabledControl2(6, "smooth", this, &WeaponMainSpeedP90);

	WeaponMainFoVP90.SetFileId("main_fovL");
	WeaponMainFoVP90.SetBoundaries(0.1f, 30.f);
	WeaponMainFoVP90.SetValue(5.f);
	WeaponMainGroup.PlaceLabledControl2(6, "maximum fov", this, &WeaponMainFoVP90);

	WeaponMainRecoilP90.SetFileId("main_recoilL");
	WeaponMainGroup.PlaceLabledControl2(6, "ajust recoil", this, &WeaponMainRecoilP90);

	WeaponMainHitboxP90.SetFileId("main_hitboxL");
	WeaponMainHitboxP90.AddItem("head");
	WeaponMainHitboxP90.AddItem("neck");
	WeaponMainHitboxP90.AddItem("chest");
	WeaponMainHitboxP90.AddItem("stomach");
	WeaponMainGroup.PlaceLabledControl2(6, "hitbox", this, &WeaponMainHitboxP90);

	//WeaponMainInaccM4.SetFileId("main_inacc");
	//WeaponMainInaccM4.SetBoundaries(0.f, 15.f);
	//WeaponMainGroup.PlaceLabledControl2(7, "Miss Chance", this, &WeaponMainInacc);
#pragma endregion

#pragma region Pistols
	WeaponPistGroup.SetPosition(440, 205);
	WeaponPistGroup.SetText2("A");
	WeaponPistGroup.AddTab2(CGroupTab("G", 1)); //USP
	WeaponPistGroup.AddTab2(CGroupTab("A", 2)); //DESERT //REVOLVER
	WeaponPistGroup.AddTab2(CGroupTab("I", 3)); //CZ //BERRETAS
	WeaponPistGroup.AddTab2(CGroupTab("H", 4)); //TEC-9
	WeaponPistGroup.AddTab2(CGroupTab("F", 5)); //P250
	WeaponPistGroup.AddTab2(CGroupTab("D", 6)); //GLOCK
	WeaponPistGroup.AddTab2(CGroupTab("C", 7)); //FIVEN-7
	WeaponPistGroup.SetSize(256, 195);
	RegisterControl(&WeaponPistGroup);

	//USP ------------------------------------------------------------------------------

	WeaponPistSpeed.SetFileId("pist_speed1");
	WeaponPistSpeed.SetBoundaries(0.1f, 100.f);
	WeaponPistSpeed.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl2(1, "smooth", this, &WeaponPistSpeed);

	WeaponPistFoV.SetFileId("pist_fov1");
	WeaponPistFoV.SetBoundaries(0.1f, 30.f);
	WeaponPistFoV.SetValue(5.f);
	WeaponPistGroup.PlaceLabledControl2(1, "maximum fov", this, &WeaponPistFoV);

	WeaponPistRecoil.SetFileId("pist_recoil1");
	WeaponPistGroup.PlaceLabledControl2(1, "ajust recoil", this, &WeaponPistRecoil);

	WeaponPistHitbox.SetFileId("pist_hitbox1");
	WeaponPistHitbox.AddItem("head");
	WeaponPistHitbox.AddItem("neck");
	WeaponPistHitbox.AddItem("chest");
	WeaponPistHitbox.AddItem("stomach");
	WeaponPistGroup.PlaceLabledControl2(1, "hitbox", this, &WeaponPistHitbox);

	//DESERT ------------------------------------------------------------------------------

	WeaponPistSpeedD.SetFileId("pist_speed2");
	WeaponPistSpeedD.SetBoundaries(0.1f, 100.f);
	WeaponPistSpeedD.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl2(2, "smooth", this, &WeaponPistSpeedD);

	WeaponPistFoVD.SetFileId("pist_fov2");
	WeaponPistFoVD.SetBoundaries(0.1f, 30.f);
	WeaponPistFoVD.SetValue(5.f);
	WeaponPistGroup.PlaceLabledControl2(2, "maximum fov", this, &WeaponPistFoVD);

	WeaponPistRecoilD.SetFileId("pist_recoil2");
	WeaponPistGroup.PlaceLabledControl2(2, "ajust recoil", this, &WeaponPistRecoilD);

	WeaponPistHitboxD.SetFileId("pist_hitbox2");
	WeaponPistHitboxD.AddItem("head");
	WeaponPistHitboxD.AddItem("neck");
	WeaponPistHitboxD.AddItem("chest");
	WeaponPistHitboxD.AddItem("stomach");
	WeaponPistGroup.PlaceLabledControl2(2, "hitbox", this, &WeaponPistHitboxD);

	//CZ ------------------------------------------------------------------------------

	WeaponPistSpeedCZ.SetFileId("pist_speed3");
	WeaponPistSpeedCZ.SetBoundaries(0.1f, 100.f);
	WeaponPistSpeedCZ.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl2(3, "smooth", this, &WeaponPistSpeedCZ);

	WeaponPistFoVCZ.SetFileId("pist_fov3");
	WeaponPistFoVCZ.SetBoundaries(0.1f, 30.f);
	WeaponPistFoVCZ.SetValue(5.f);
	WeaponPistGroup.PlaceLabledControl2(3, "maximum fov", this, &WeaponPistFoVCZ);

	WeaponPistRecoilCZ.SetFileId("pist_recoil3");
	WeaponPistGroup.PlaceLabledControl2(3, "ajust recoil", this, &WeaponPistRecoilCZ);

	WeaponPistHitboxCZ.SetFileId("pist_hitbox3");
	WeaponPistHitboxCZ.AddItem("head");
	WeaponPistHitboxCZ.AddItem("neck");
	WeaponPistHitboxCZ.AddItem("chest");
	WeaponPistHitboxCZ.AddItem("stomach");
	WeaponPistGroup.PlaceLabledControl2(3, "hitbox", this, &WeaponPistHitboxCZ);

	//TEC-9 ------------------------------------------------------------------------------

	WeaponPistSpeedtec.SetFileId("pist_speed4");
	WeaponPistSpeedtec.SetBoundaries(0.1f, 100.f);
	WeaponPistSpeedtec.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl2(4, "smooth", this, &WeaponPistSpeedtec);

	WeaponPistFoVtec.SetFileId("pist_fov4");
	WeaponPistFoVtec.SetBoundaries(0.1f, 30.f);
	WeaponPistFoVtec.SetValue(5.f);
	WeaponPistGroup.PlaceLabledControl2(4, "maximum fov", this, &WeaponPistFoVtec);

	WeaponPistRecoiltec.SetFileId("pist_recoil4");
	WeaponPistGroup.PlaceLabledControl2(4, "ajust recoil", this, &WeaponPistRecoiltec);

	WeaponPistHitboxtec.SetFileId("pist_hitbox4");
	WeaponPistHitboxtec.AddItem("head");
	WeaponPistHitboxtec.AddItem("neck");
	WeaponPistHitboxtec.AddItem("chest");
	WeaponPistHitboxtec.AddItem("stomach");
	WeaponPistGroup.PlaceLabledControl2(4, "hitbox", this, &WeaponPistHitboxtec);

	//P250 ------------------------------------------------------------------------------

	WeaponPistSpeedp250.SetFileId("pist_speed5");
	WeaponPistSpeedp250.SetBoundaries(0.1f, 100.f);
	WeaponPistSpeedp250.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl2(5, "smooth", this, &WeaponPistSpeedp250);

	WeaponPistFoVp250.SetFileId("pist_fov5");
	WeaponPistFoVp250.SetBoundaries(0.1f, 30.f);
	WeaponPistFoVp250.SetValue(5.f);
	WeaponPistGroup.PlaceLabledControl2(5, "maximum fov", this, &WeaponPistFoVp250);

	WeaponPistRecoilp250.SetFileId("pist_recoil5");
	WeaponPistGroup.PlaceLabledControl2(5, "ajust recoil", this, &WeaponPistRecoilp250);

	WeaponPistHitboxp250.SetFileId("pist_hitbox5");
	WeaponPistHitboxp250.AddItem("head");
	WeaponPistHitboxp250.AddItem("neck");
	WeaponPistHitboxp250.AddItem("chest");
	WeaponPistHitboxp250.AddItem("stomach");
	WeaponPistGroup.PlaceLabledControl2(5, "hitbox", this, &WeaponPistHitboxp250);

	//GLOCK ------------------------------------------------------------------------------

	WeaponPistSpeedGLOCK.SetFileId("pist_speed6");
	WeaponPistSpeedGLOCK.SetBoundaries(0.1f, 100.f);
	WeaponPistSpeedGLOCK.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl2(6, "smooth", this, &WeaponPistSpeedGLOCK);

	WeaponPistFoVGLOCK.SetFileId("pist_fov6");
	WeaponPistFoVGLOCK.SetBoundaries(0.1f, 30.f);
	WeaponPistFoVGLOCK.SetValue(5.f);
	WeaponPistGroup.PlaceLabledControl2(6, "maximum fov", this, &WeaponPistFoVGLOCK);

	WeaponPistRecoilGLOCK.SetFileId("pist_recoil6");
	WeaponPistGroup.PlaceLabledControl2(6, "ajust recoil", this, &WeaponPistRecoilGLOCK);

	WeaponPistHitboxGLOCK.SetFileId("pist_hitbox6");
	WeaponPistHitboxGLOCK.AddItem("head");
	WeaponPistHitboxGLOCK.AddItem("neck");
	WeaponPistHitboxGLOCK.AddItem("chest");
	WeaponPistHitboxGLOCK.AddItem("stomach");
	WeaponPistGroup.PlaceLabledControl2(6, "hitbox", this, &WeaponPistHitboxGLOCK);

	//FIVEN-7 ------------------------------------------------------------------------------

	WeaponPistSpeedFIVEN7.SetFileId("pist_speed7");
	WeaponPistSpeedFIVEN7.SetBoundaries(0.1f, 100.f);
	WeaponPistSpeedFIVEN7.SetValue(1.0f);
	WeaponPistGroup.PlaceLabledControl2(7, "smooth", this, &WeaponPistSpeedFIVEN7);

	WeaponPistFoVFIVEN7.SetFileId("pist_fov7");
	WeaponPistFoVFIVEN7.SetBoundaries(0.1f, 30.f);
	WeaponPistFoVFIVEN7.SetValue(5.f);
	WeaponPistGroup.PlaceLabledControl2(7, "maximum fov", this, &WeaponPistFoVFIVEN7);

	WeaponPistRecoilFIVEN7.SetFileId("pist_recoil7");
	WeaponPistGroup.PlaceLabledControl2(7, "ajust recoil", this, &WeaponPistRecoilFIVEN7);

	WeaponPistHitboxFIVEN7.SetFileId("pist_hitbox7");
	WeaponPistHitboxFIVEN7.AddItem("head");
	WeaponPistHitboxFIVEN7.AddItem("neck");
	WeaponPistHitboxFIVEN7.AddItem("chest");
	WeaponPistHitboxFIVEN7.AddItem("stomach");
	WeaponPistGroup.PlaceLabledControl2(7, "hitbox", this, &WeaponPistHitboxFIVEN7);

#pragma endregion

#pragma region Snipers
	WeaponSnipGroup.SetPosition(440, 13);
	WeaponSnipGroup.SetText2("Z");
	WeaponSnipGroup.AddTab2(CGroupTab("Y", 1)); //SCAR20
	WeaponSnipGroup.AddTab2(CGroupTab("X", 2)); //G3SG1
	WeaponSnipGroup.AddTab2(CGroupTab("a", 3)); //SSG08
	WeaponSnipGroup.AddTab2(CGroupTab("Z", 4)); //AWP
	WeaponSnipGroup.SetSize(256, 180);
	RegisterControl(&WeaponSnipGroup);

	//SCAR20

	WeaponSnipSpeed.SetFileId("snip_speed8");
	WeaponSnipSpeed.SetBoundaries(0.1f, 100.f);
	WeaponSnipSpeed.SetValue(1.0f);
	WeaponSnipGroup.PlaceLabledControl2(1, "smooth", this, &WeaponSnipSpeed);

	WeaponSnipFoV.SetFileId("snip_fov8");
	WeaponSnipFoV.SetBoundaries(0.1f, 30.f);
	WeaponSnipFoV.SetValue(5.f);
	WeaponSnipGroup.PlaceLabledControl2(1, "maximum fov", this, &WeaponSnipFoV);

	WeaponSnipRecoil.SetFileId("snip_recoil8");
	WeaponSnipGroup.PlaceLabledControl2(1, "ajust recoil", this, &WeaponSnipRecoil);

	WeaponSnipHitbox.SetFileId("snip_hitbox8");
	WeaponSnipHitbox.AddItem("head");
	WeaponSnipHitbox.AddItem("neck");
	WeaponSnipHitbox.AddItem("chest");
	WeaponSnipHitbox.AddItem("stomach");
	WeaponSnipGroup.PlaceLabledControl2(1, "hitbox", this, &WeaponSnipHitbox);

	//G3SG1

	WeaponSnipSpeedG3SG1.SetFileId("snip_speed9");
	WeaponSnipSpeedG3SG1.SetBoundaries(0.1f, 100.f);
	WeaponSnipSpeedG3SG1.SetValue(1.0f);
	WeaponSnipGroup.PlaceLabledControl2(2, "smooth", this, &WeaponSnipSpeedG3SG1);

	WeaponSnipFoVG3SG1.SetFileId("snip_fov9");
	WeaponSnipFoVG3SG1.SetBoundaries(0.1f, 30.f);
	WeaponSnipFoVG3SG1.SetValue(5.f);
	WeaponSnipGroup.PlaceLabledControl2(2, "maximum fov", this, &WeaponSnipFoVG3SG1);

	WeaponSnipRecoilG3SG1.SetFileId("snip_recoil9");
	WeaponSnipGroup.PlaceLabledControl2(2, "ajust recoil", this, &WeaponSnipRecoilG3SG1);

	WeaponSnipHitboxG3SG1.SetFileId("snip_hitbox9");
	WeaponSnipHitboxG3SG1.AddItem("head");
	WeaponSnipHitboxG3SG1.AddItem("neck");
	WeaponSnipHitboxG3SG1.AddItem("chest");
	WeaponSnipHitboxG3SG1.AddItem("stomach");
	WeaponSnipGroup.PlaceLabledControl2(2, "hitbox", this, &WeaponSnipHitboxG3SG1);

	//SSG08ZZ

	WeaponSnipSpeedSSG08Z.SetFileId("snip_speed0");
	WeaponSnipSpeedSSG08Z.SetBoundaries(0.1f, 100.f);
	WeaponSnipSpeedSSG08Z.SetValue(1.0f);
	WeaponSnipGroup.PlaceLabledControl2(3, "smooth", this, &WeaponSnipSpeedSSG08Z);

	WeaponSnipFoVSSG08Z.SetFileId("snip_fov0");
	WeaponSnipFoVSSG08Z.SetBoundaries(0.1f, 30.f);
	WeaponSnipFoVSSG08Z.SetValue(5.f);
	WeaponSnipGroup.PlaceLabledControl2(3, "maximum fov", this, &WeaponSnipFoVSSG08Z);

	WeaponSnipRecoilSSG08Z.SetFileId("snip_recoil0");
	WeaponSnipGroup.PlaceLabledControl2(3, "ajust recoil", this, &WeaponSnipRecoilSSG08Z);

	WeaponSnipHitboxSSG08Z.SetFileId("snip_hitbox0");
	WeaponSnipHitboxSSG08Z.AddItem("head");
	WeaponSnipHitboxSSG08Z.AddItem("neck");
	WeaponSnipHitboxSSG08Z.AddItem("chest");
	WeaponSnipHitboxSSG08Z.AddItem("stomach");
	WeaponSnipGroup.PlaceLabledControl2(3, "hitbox", this, &WeaponSnipHitboxSSG08Z);

	//AWP

	WeaponSnipSpeedAWP.SetFileId("snip_speedI");
	WeaponSnipSpeedAWP.SetBoundaries(0.1f, 100.f);
	WeaponSnipSpeedAWP.SetValue(1.0f);
	WeaponSnipGroup.PlaceLabledControl2(4, "smooth", this, &WeaponSnipSpeedAWP);

	WeaponSnipFoVAWP.SetFileId("snip_fovI");
	WeaponSnipFoVAWP.SetBoundaries(0.1f, 30.f);
	WeaponSnipFoVAWP.SetValue(5.f);
	WeaponSnipGroup.PlaceLabledControl2(4, "maximum fov", this, &WeaponSnipFoVAWP);

	WeaponSnipRecoilAWP.SetFileId("snip_recoilI");
	WeaponSnipGroup.PlaceLabledControl2(4, "ajust recoil", this, &WeaponSnipRecoilAWP);

	WeaponSnipHitboxAWP.SetFileId("snip_hitboxI");
	WeaponSnipHitboxAWP.AddItem("head");
	WeaponSnipHitboxAWP.AddItem("neck");
	WeaponSnipHitboxAWP.AddItem("chest");
	WeaponSnipHitboxAWP.AddItem("stomach");
	WeaponSnipGroup.PlaceLabledControl2(4, "hitbox", this, &WeaponSnipHitboxAWP);

	Triggerbot.SetText("Trigger");
	Triggerbot.SetPosition(265, 13);
	Triggerbot.SetSize(170, 387);
	RegisterControl(&Triggerbot);

	TriggerEnable.SetFileId("trig_enable");
	Triggerbot.PlaceLabledControl(0, "enable", this, &TriggerEnable);

	TriggerKeyPress.SetFileId("trig_onkey");
	Triggerbot.PlaceLabledControl(0, "on key press", this, &TriggerKeyPress);

	TriggerKeyBind.SetFileId("trig_key");
	Triggerbot.PlaceLabledControl(0, "", this, &TriggerKeyBind);

	TriggerDelay.SetFileId("trig_time");
	TriggerDelay.SetBoundaries(0.f, 1000.f);
	Triggerbot.PlaceLabledControl(0, "delay", this, &TriggerDelay);
#pragma endregion
}

void CVisualTab::Setup()
{
	SetTitle("visuals");
	SetTitle3("E");

#pragma region Options
	OptionsGroup.SetText("Options");
	OptionsGroup.AddTab(CGroupTab("player esp", 1));
	OptionsGroup.AddTab(CGroupTab("chams", 2));
	OptionsGroup.AddTab(CGroupTab("box", 3));
	OptionsGroup.SetPosition(5, 23);
	OptionsGroup.SetSize(225, 375);
	RegisterControl(&OptionsGroup);

	Active.SetFileId("ftr_active");
	OptionsGroup.PlaceLabledControl(1, "active", this, &Active);

	FiltersEnemiesOnly.SetFileId("ftr_enemyonly");
	OptionsGroup.PlaceLabledControl(1, "enemy only", this, &FiltersEnemiesOnly);

	OptionsBox.SetFileId("otr_recoilhair");
	OptionsGroup.PlaceLabledControl(1, "show box", this, &OptionsBox);

	OptionsName.SetFileId("opt_name");
	OptionsGroup.PlaceLabledControl(1, "show Name", this, &OptionsName);

	OptionsWeapon.SetFileId("opt_weapon");
	OptionsGroup.PlaceLabledControl(1, "show Weapon", this, &OptionsWeapon);

	OptionsVitals.SetFileId("opt_hp");
	OptionsVitals.AddItem("none");
	OptionsVitals.AddItem("normal");
	OptionsVitals.AddItem("styles");
	OptionsGroup.PlaceLabledControl(1, "health", this, &OptionsVitals);
	
	PostProcessing.SetFileId("opt_fpsboost");
	OptionsGroup.PlaceLabledControl(1, "post processing", this, &PostProcessing);

	BacktrackingLol.SetFileId("opt_backdot");
	OptionsGroup.PlaceLabledControl(1, "backtrack line", this, &BacktrackingLol);

	OptionsSkeleton.SetFileId("opt_bone");
	OptionsGroup.PlaceLabledControl(1, "show bones", this, &OptionsSkeleton);

	ShowImpacts.SetFileId("opt_impacts");
	OptionsGroup.PlaceLabledControl(1, "show impacts", this, &ShowImpacts);

	DamageIndicator.SetFileId("opt_damageindicator");
	OptionsGroup.PlaceLabledControl(1, "damage indicator", this, &DamageIndicator);

	Nades.SetFileId("aa_nadescreen");
	OptionsGroup.PlaceLabledControl(1, "wallbang indicator", this, &Nades);

	IsScoped.SetFileId("opt_scoped");
	OptionsGroup.PlaceLabledControl(1, "show scoped", this, &IsScoped);

	HasDefuser.SetFileId("opt_dontbealoserbuyadefuser");
	OptionsGroup.PlaceLabledControl(1, "show defuser", this, &HasDefuser);

	IsDefusing.SetFileId("opt_hedufusinglel");
	OptionsGroup.PlaceLabledControl(1, "show defusing", this, &IsDefusing);

	RemoveScope.SetFileId("opt_RemoveScope");
	OptionsGroup.PlaceLabledControl(1, "remove scope", this, &RemoveScope);

	//

	OptionsChams.SetFileId("opt_chams");
	OptionsChams.AddItem("none");
	OptionsChams.AddItem("Normal");
	OptionsChams.AddItem("Flat");
	OptionsChams.AddItem("Visible Only");
	OptionsGroup.PlaceLabledControl(2, "chams", this, &OptionsChams);

	Localplayerz.SetFileId("opt_localplayerchams");
	OptionsGroup.PlaceLabledControl(2, "team chams", this, &Localplayerz);

	OtherNoHands.SetFileId("otr_hands");
	OtherNoHands.AddItem("none");
	OtherNoHands.AddItem("remove");
	OtherNoHands.AddItem("Chams");
	OtherNoHands.AddItem("Rainbow");
	OptionsGroup.PlaceLabledControl(2, "hand", this, &OtherNoHands);

	//chams

	CTChamsR.SetFileId("ctchamsr");
	CTChamsR.SetBoundaries(0.f, 255.f);
	CTChamsR.SetValue(184.f);
	OptionsGroup.PlaceLabledControl(2, "chams red", this, &CTChamsR);

	CTChamsG.SetFileId("ctchamsg");
	CTChamsG.SetBoundaries(0.f, 255.f);
	CTChamsG.SetValue(180.f);
	OptionsGroup.PlaceLabledControl(2, "chams green", this, &CTChamsG);

	CTChamsB.SetFileId("ctchamsb");
	CTChamsB.SetBoundaries(0.f, 255.f);
	CTChamsB.SetValue(177.f);
	OptionsGroup.PlaceLabledControl(2, "chams blue", this, &CTChamsB);

	TChamsRVis.SetFileId("ctchamsrvis");
	TChamsRVis.SetBoundaries(0.f, 255.f);
	TChamsRVis.SetValue(255.f);
	OptionsGroup.PlaceLabledControl(2, "chams red vis", this, &TChamsRVis);

	TChamsGVis.SetFileId("ctchamsgvis");
	TChamsGVis.SetBoundaries(0.f, 255.f);
	TChamsGVis.SetValue(77.f);
	OptionsGroup.PlaceLabledControl(2, "chams green vis", this, &TChamsGVis);

	TChamsBVis.SetFileId("ctchamsbvis");
	TChamsBVis.SetBoundaries(0.f, 255.f);
	TChamsBVis.SetValue(77.f);
	OptionsGroup.PlaceLabledControl(2, "chams blue vis", this, &TChamsBVis);

	BoxR.SetFileId("boxr");
	BoxR.SetBoundaries(0.f, 255.f);
	BoxR.SetValue(238.f);
	OptionsGroup.PlaceLabledControl(3, "box red", this, &BoxR);

	BoxG.SetFileId("boxg");
	BoxG.SetBoundaries(0.f, 255.f);
	BoxG.SetValue(162.f);
	OptionsGroup.PlaceLabledControl(3, "box green", this, &BoxG);

	BoxB.SetFileId("boxb");
	BoxB.SetBoundaries(0.f, 255.f);
	BoxB.SetValue(147.f);
	OptionsGroup.PlaceLabledControl(3, "box blue", this, &BoxB);

	Opacidade.SetFileId("boxopacity");
	Opacidade.SetBoundaries(0.f, 255.f);
	Opacidade.SetValue(254.f);
	OptionsGroup.PlaceLabledControl(3, "opacity", this, &Opacidade);

	BoxRVis.SetFileId("boxrvis");
	BoxRVis.SetBoundaries(0.f, 255.f);
	BoxRVis.SetValue(0.f);
	OptionsGroup.PlaceLabledControl(3, "box red vis", this, &BoxRVis);

	BoxGVis.SetFileId("boxgvis");
	BoxGVis.SetBoundaries(0.f, 255.f);
	BoxGVis.SetValue(221.f);
	OptionsGroup.PlaceLabledControl(3, "box green vis", this, &BoxGVis);

	BoxBVis.SetFileId("boxbvis");
	BoxBVis.SetBoundaries(0.f, 255.f);
	BoxBVis.SetValue(225.f);
	OptionsGroup.PlaceLabledControl(3, "box Blue vis", this, &BoxBVis);

	OpacidadeVis.SetFileId("boxopacityvis");
	OpacidadeVis.SetBoundaries(0.f, 255.f);
	OpacidadeVis.SetValue(255.f);
	OptionsGroup.PlaceLabledControl(3, "opacity", this, &OpacidadeVis);
#pragma endregion Setting up the Options controls
#pragma region Other
	OtherGroup.SetText("other");
	OtherGroup.SetPosition(465, 23);
	OtherGroup.SetSize(235, 375);
	RegisterControl(&OtherGroup);

	OtherNoVisualRecoil.SetFileId("otr_visrecoil");
	OtherGroup.PlaceLabledControl(0, "adjust visual recoil", this, &OtherNoVisualRecoil);

	OtherNoFlash.SetFileId("otr_noflash");
	OtherGroup.PlaceLabledControl(0, "remove flash", this, &OtherNoFlash);

	OtherNoSmoke.SetFileId("otr_nosmoke");
	OtherGroup.PlaceLabledControl(0, "remove smoke", this, &OtherNoSmoke);

	FakeAngleChams.SetFileId("fakechams");
	OtherGroup.PlaceLabledControl(0, "angle ghost", this, &FakeAngleChams);

	Watermark.SetFileId("otr_watermark");
	Watermark.SetState(true);
	OtherGroup.PlaceLabledControl(0, "watermark", this, &Watermark);

	OtherFOV.SetFileId("otr_fov");
	OtherFOV.SetBoundaries(0.f, 30.f);
	OtherFOV.SetValue(0.f);
	OtherGroup.PlaceLabledControl(0, "override fov", this, &OtherFOV);

	OtherViewmodelFOV.SetFileId("otr_viewfov");
	OtherViewmodelFOV.SetBoundaries(0.f, 180.f);
	OtherViewmodelFOV.SetValue(0.f);
	OtherGroup.PlaceLabledControl(0, "viewmodel fov", this, &OtherViewmodelFOV);

#pragma endregion Setting up the Other controls

#pragma region WorldOptions
	CvarGroup.SetText("world");
	CvarGroup.AddTab(CGroupTab("ambient", 1));
	CvarGroup.AddTab(CGroupTab("other", 2));
	CvarGroup.SetPosition(235, 23);
	CvarGroup.SetSize(225, 375);
	RegisterControl(&CvarGroup);

	AmbientExposure.SetFileId("otr_ambientexposure");
	AmbientExposure.SetBoundaries(0.f, 2.f);
	AmbientExposure.SetValue(2.f);
	CvarGroup.PlaceLabledControl(1, "ambient exposure", this, &AmbientExposure);

	AmbientRed.SetFileId("otr_ambientred");
	AmbientRed.SetBoundaries(0.f, .5f);
	AmbientRed.SetValue(0.f);
	CvarGroup.PlaceLabledControl(1, "ambient red", this, &AmbientRed);

	AmbientGreen.SetFileId("otr_ambientgreen");
	AmbientGreen.SetBoundaries(0.f, .5f);
	AmbientGreen.SetValue(0.f);
	CvarGroup.PlaceLabledControl(1, "ambient green", this, &AmbientGreen);

	AmbientBlue.SetFileId("otr_ambientblue");
	AmbientBlue.SetBoundaries(0.f, .5f);
	AmbientBlue.SetValue(0.f);
	CvarGroup.PlaceLabledControl(1, "ambient blue", this, &AmbientBlue);

	angleindicator.SetFileId("otr_aaindicator");
	angleindicator.AddItem("none");
	angleindicator.AddItem("Arrow");
	CvarGroup.PlaceLabledControl(2, "indicator aa", this, &angleindicator);

	AmbientSkybox.SetFileId("otr_skybox");
	AmbientSkybox.AddItem("none");
	AmbientSkybox.AddItem("skybox 1");
	AmbientSkybox.AddItem("skybox 2");
	AmbientSkybox.AddItem("skybox 3");
	AmbientSkybox.AddItem("skybox 4");
	AmbientSkybox.AddItem("skybox 5");
	CvarGroup.PlaceLabledControl(2, "change skybox", this, &AmbientSkybox);

#pragma endregion Setting up the Other controls
}

void CMiscTab::Setup()
{
	SetTitle("misc");
	SetTitle4("F");
#pragma region Other
	OtherGroup.SetPosition(465, 23);
	OtherGroup.SetSize(235, 375);
	OtherGroup.SetText("other");
	RegisterControl(&OtherGroup);

	OtherAutoJump.SetFileId("otr_autojump");
	OtherGroup.PlaceLabledControl(0, "bunny hop", this, &OtherAutoJump);

	OtherAutoStrafe.SetFileId("otr_strafe");
	OtherAutoStrafe.AddItem("none");
	OtherAutoStrafe.AddItem("silent");
	OtherAutoStrafe.AddItem("legit");
	OtherGroup.PlaceLabledControl(0, "auto strafer", this, &OtherAutoStrafe);

	OtherSafeMode.SetFileId("otr_safemode");
	OtherSafeMode.SetState(true);
	OtherGroup.PlaceLabledControl(0, "safe mode", this, &OtherSafeMode);

	OtherClantag.SetFileId("otr_clantag");
	OtherGroup.PlaceLabledControl(0, "clantag", this, &OtherClantag);

	OtherHitmarker.SetFileId("otr_hitmarker");
	OtherGroup.PlaceLabledControl(0, "hitmarkers", this, &OtherHitmarker);

	OtherHitmarkerType.SetFileId("otr_hitsound");
	OtherHitmarkerType.AddItem("call of duty");
	OtherHitmarkerType.AddItem("metal");
	OtherHitmarkerType.AddItem("bameware");
	OtherGroup.PlaceLabledControl(0, "hitmarker type", this, &OtherHitmarkerType);

	ConfigGroup.SetText("configurations");
	ConfigGroup.SetPosition(235, 23);
	ConfigGroup.SetSize(225, 375);
	ConfigListBox.SetHeightInItems(7);
	RegisterControl(&ConfigGroup);

	list_configs();
	ConfigGroup.PlaceLabledControl(0, "", this, &ConfigListBox);

	sas.SetFileId("separetezzzz");
	ConfigGroup.PlaceLabledControl(0, "", this, &sas);


	LoadConfig.SetText("Load");
	LoadConfig.SetCallback(&load_callback);
	ConfigGroup.PlaceLabledControl(0, "", this, &LoadConfig);

	gassz.SetFileId("separetezz");
	ConfigGroup.PlaceLabledControl(0, "", this, &gassz);

	SaveConfig.SetText("Save");
	SaveConfig.SetCallback(&save_callback);
	ConfigGroup.PlaceLabledControl(0, "", this, &SaveConfig);

	szas.SetFileId("separetezz");
	ConfigGroup.PlaceLabledControl(0, "", this, &szas);

	RemoveConfig.SetText("Remove");
	RemoveConfig.SetCallback(&remove_config);
	ConfigGroup.PlaceLabledControl(0, "", this, &RemoveConfig);

	zdasz.SetFileId("separete");
	ConfigGroup.PlaceLabledControl(0, "", this, &zdasz);

	ConfigGroup.PlaceLabledControl(0, "", this, &NewConfigName);

	zdas.SetFileId("separetez");
	ConfigGroup.PlaceLabledControl(0, "", this, &zdas);

	AddConfig.SetText("Add");
	AddConfig.SetCallback(&add_config);
	ConfigGroup.PlaceLabledControl(0, "", this, &AddConfig);
#pragma endregion other random options

#pragma region FakeLag
	FakeLagGroup.SetPosition(5, 23);
	FakeLagGroup.SetSize(225, 375);
	FakeLagGroup.AddTab(CGroupTab("misc", 1));
	FakeLagGroup.AddTab(CGroupTab("competitive", 2));
	FakeLagGroup.SetText("misc");
	RegisterControl(&FakeLagGroup);

	OtherThirdpersonType.SetFileId("otr_tptypelul");
	OtherThirdpersonType.AddItem("real angle");
	OtherThirdpersonType.AddItem("fake angle");
	FakeLagGroup.PlaceLabledControl(1, "thirdperson type", this, &OtherThirdpersonType);

	OtherThirdpersonKey.SetFileId("otr_tpkeylul");
	FakeLagGroup.PlaceLabledControl(1, "", this, &OtherThirdpersonKey);
	
	//

	OtherSpectators.SetFileId("otr_speclist");
	FakeLagGroup.PlaceLabledControl(2, "spectators list", this, &OtherSpectators);

	OptionsCompRank.SetFileId("otr_rankreveal");
	FakeLagGroup.PlaceLabledControl(2, "rank reveal", this, &OptionsCompRank);

	OtherAutoAccept.SetFileId("otr_autoaccept");
	FakeLagGroup.PlaceLabledControl(2, "auto accept", this, &OtherAutoAccept);

#pragma endregion fakelag shit
}

void SkinTab::Setup()
{
	SetTitle("skinchanger");
	SetTitle5("G");

	SkinChanger2.SetText2("W");
	SkinChanger2.SetSize(500, 300);
	SkinChanger2.SetPosition(100, 23);
	SkinChanger2.AddTab2(CGroupTab("4", 1));
	SkinChanger2.AddTab2(CGroupTab("Y", 2));
	SkinChanger2.AddTab2(CGroupTab("S", 3));
	SkinChanger2.AddTab2(CGroupTab("D", 4));
	SkinChanger2.AddTab2(CGroupTab("L", 5));
	SkinChanger2.AddTab2(CGroupTab("d", 6));
	RegisterControl(&SkinChanger2);

	SkinEnable.SetFileId("otr_skinactive");
	SkinChanger2.PlaceLabledControl2(1, "active Skin", this, &SkinEnable);

	GloveActive.SetFileId("gloves_enable");
	SkinChanger2.PlaceLabledControl2(1, "active Gloves", this, &GloveActive);

	GloveModel.SetFileId("glove_model");
	GloveModel.AddItem("bloodhound");
	GloveModel.AddItem("sporty");
	GloveModel.AddItem("slick");
	GloveModel.AddItem("specialist");
	GloveModel.AddItem("moto");
	GloveModel.AddItem("handwrap");
	SkinChanger2.PlaceLabledControl2(1, "", this, &GloveModel);

	GloveSkin.SetFileId("glove_skin");
	GloveSkin.AddItem("bloodhound black"); //0
	GloveSkin.AddItem("bloodhound snakeskin"); //1
	GloveSkin.AddItem("bloodhound metallic"); //2
	GloveSkin.AddItem("sporty purple"); //3
	GloveSkin.AddItem("sporty green"); //4
	GloveSkin.AddItem("sporty light blue"); //5
	GloveSkin.AddItem("sporty military"); //6
	GloveSkin.AddItem("slick black"); //7
	GloveSkin.AddItem("slick military"); //8
	GloveSkin.AddItem("slick red"); //9
	GloveSkin.AddItem("specialist ddpat"); //10
	GloveSkin.AddItem("specialist kimono"); //11
	GloveSkin.AddItem("specialist emerald"); //12
	GloveSkin.AddItem("specialist orange"); //13
	GloveSkin.AddItem("moto mint"); //14
	GloveSkin.AddItem("Moto boom"); //15
	GloveSkin.AddItem("moto blue"); //16
	GloveSkin.AddItem("moto black"); //17
	GloveSkin.AddItem("moto black"); //17
	GloveSkin.AddItem("handwrap grey camo"); //18
	GloveSkin.AddItem("handwrap slaughter"); //19
	GloveSkin.AddItem("handwrap orange camo"); //20
	GloveSkin.AddItem("handwrap leathery"); //21
	SkinChanger2.PlaceLabledControl2(1, "", this, &GloveSkin);

	KnifeModel.SetFileId("knife_model");
	KnifeModel.AddItem("bayonet");
	KnifeModel.AddItem("bowie");
	KnifeModel.AddItem("butterfly");
	KnifeModel.AddItem("falchion");
	KnifeModel.AddItem("flip");
	KnifeModel.AddItem("gut");
	KnifeModel.AddItem("huntsman");
	KnifeModel.AddItem("karambit");
	KnifeModel.AddItem("m9 bayonet");
	KnifeModel.AddItem("shadow daggers");
	KnifeModel.AddItem("navaja");
	KnifeModel.AddItem("stiletto");
	KnifeModel.AddItem("ursus");
	KnifeModel.AddItem("talon");
	SkinChanger2.PlaceLabledControl2(1, "knife", this, &KnifeModel);

	KnifeSkin.SetFileId("knife_skin");
	KnifeSkin.AddItem("none");
	KnifeSkin.AddItem("crimson web");
	KnifeSkin.AddItem("bone mask");
	KnifeSkin.AddItem("fade");
	KnifeSkin.AddItem("night");
	KnifeSkin.AddItem("blue steel");
	KnifeSkin.AddItem("stained");
	KnifeSkin.AddItem("case hardened");
	KnifeSkin.AddItem("slaughter");
	KnifeSkin.AddItem("safari mesh");
	KnifeSkin.AddItem("boreal forest");
	KnifeSkin.AddItem("ultraviolet");
	KnifeSkin.AddItem("Urban Masked");
	KnifeSkin.AddItem("scorched");
	KnifeSkin.AddItem("rust coat");
	KnifeSkin.AddItem("tiger tooth");
	KnifeSkin.AddItem("damascus steel");
	KnifeSkin.AddItem("damascus steel");
	KnifeSkin.AddItem("marble fade");
	KnifeSkin.AddItem("rust coat");
	KnifeSkin.AddItem("doppler ruby");
	KnifeSkin.AddItem("doppler sapphire");
	KnifeSkin.AddItem("doppler blackpearl");
	KnifeSkin.AddItem("doppler phase 1");
	KnifeSkin.AddItem("doppler phase 2");
	KnifeSkin.AddItem("doppler phase 3");
	KnifeSkin.AddItem("doppler phase 4");
	KnifeSkin.AddItem("gamma doppler phase 1");
	KnifeSkin.AddItem("gamma doppler phase 2");
	KnifeSkin.AddItem("gamma doppler phase 3");
	KnifeSkin.AddItem("gamma doppler phase 4");
	KnifeSkin.AddItem("gamma doppler emerald");
	KnifeSkin.AddItem("lore");
	SkinChanger2.PlaceLabledControl2(1, "", this, &KnifeSkin);

	AWPSkin.SetFileId("awp_skin");
	AWPSkin.AddItem("boom");
	AWPSkin.AddItem("dragon lore");
	AWPSkin.AddItem("pink ddpat");
	AWPSkin.AddItem("fever dream");
	AWPSkin.AddItem("lightning strike");
	AWPSkin.AddItem("corticera");
	AWPSkin.AddItem("redline");
	AWPSkin.AddItem("man-o'-war");
	AWPSkin.AddItem("graphite");
	AWPSkin.AddItem("electric hive");
	AWPSkin.AddItem("pit viper");
	AWPSkin.AddItem("asiimov");
	AWPSkin.AddItem("oni taiji");
	AWPSkin.AddItem("mortis");
	AWPSkin.AddItem("medusa");
	AWPSkin.AddItem("sun in leo");
	AWPSkin.AddItem("hyper beast");
	AWPSkin.AddItem("elite build");
	SkinChanger2.PlaceLabledControl2(2, "awp", this, &AWPSkin);

	SSG08Skin.SetFileId("sgg08_skin");
	SSG08Skin.AddItem("lichen dashed");
	SSG08Skin.AddItem("dark water");
	SSG08Skin.AddItem("blue spruce");
	SSG08Skin.AddItem("sand dune");
	SSG08Skin.AddItem("palm");
	SSG08Skin.AddItem("mayan dreams");
	SSG08Skin.AddItem("blood in the water");
	SSG08Skin.AddItem("tropical storm");
	SSG08Skin.AddItem("acid fade");
	SSG08Skin.AddItem("slashed");
	SSG08Skin.AddItem("detour");
	SSG08Skin.AddItem("abyss");
	SSG08Skin.AddItem("big iron");
	SSG08Skin.AddItem("necropos");
	SSG08Skin.AddItem("ghost crusader");
	SSG08Skin.AddItem("dragonfire");
	SkinChanger2.PlaceLabledControl2(2, "sgg 08", this, &SSG08Skin);

	SCAR20Skin.SetFileId("scar20_skin");
	SCAR20Skin.AddItem("splash jam");
	SCAR20Skin.AddItem("storm");
	SCAR20Skin.AddItem("contractor");
	SCAR20Skin.AddItem("carbon Fiber");
	SCAR20Skin.AddItem("sand Mesh");
	SCAR20Skin.AddItem("palm");
	SCAR20Skin.AddItem("emerald");
	SCAR20Skin.AddItem("crimson web");
	SCAR20Skin.AddItem("cardiac");
	SCAR20Skin.AddItem("army sheen");
	SCAR20Skin.AddItem("cyrex");
	SCAR20Skin.AddItem("grotto");
	SCAR20Skin.AddItem("emerald");
	SCAR20Skin.AddItem("green marine");
	SCAR20Skin.AddItem("outbreak");
	SCAR20Skin.AddItem("bloodsport");
	SkinChanger2.PlaceLabledControl2(2, "scar20", this, &SCAR20Skin);

	G3SG1Skin.SetFileId("g3sg1_skin");
	G3SG1Skin.AddItem("desert storm");
	G3SG1Skin.AddItem("arctic camo");
	G3SG1Skin.AddItem("bone mask");
	G3SG1Skin.AddItem("contractor");
	G3SG1Skin.AddItem("safari mesh");
	G3SG1Skin.AddItem("polar camo");
	G3SG1Skin.AddItem("jungle dashed");
	G3SG1Skin.AddItem("varicamo");
	G3SG1Skin.AddItem("predator");
	G3SG1Skin.AddItem("demeter");
	G3SG1Skin.AddItem("azure zebra");
	G3SG1Skin.AddItem("green apple");
	G3SG1Skin.AddItem("orangekKimono");
	G3SG1Skin.AddItem("neon kimono");
	G3SG1Skin.AddItem("murky");
	G3SG1Skin.AddItem("chronos");
	G3SG1Skin.AddItem("flux");
	G3SG1Skin.AddItem("the executioner");
	G3SG1Skin.AddItem("orange crash");
	SkinChanger2.PlaceLabledControl2(2, "g3sg1", this, &G3SG1Skin);

	AK47Skin.SetFileId("ak47_skin");
	AK47Skin.AddItem("first class");
	AK47Skin.AddItem("red laminate");
	AK47Skin.AddItem("case hardened");
	AK47Skin.AddItem("black laminate");
	AK47Skin.AddItem("fire serpent");
	AK47Skin.AddItem("cartel");
	AK47Skin.AddItem("emerald pinstripe");
	AK47Skin.AddItem("blue laminate");
	AK47Skin.AddItem("redline");
	AK47Skin.AddItem("vulcan");
	AK47Skin.AddItem("jaguar");
	AK47Skin.AddItem("jet set");
	AK47Skin.AddItem("wasteland rebel");
	AK47Skin.AddItem("elite build");
	AK47Skin.AddItem("hydroponic");
	AK47Skin.AddItem("aquamarine revenge");
	AK47Skin.AddItem("frontside misty");
	AK47Skin.AddItem("point disarray");
	AK47Skin.AddItem("fuel injector");
	AK47Skin.AddItem("neon revolution");
	AK47Skin.AddItem("the empress");
	AK47Skin.AddItem("bloodsport");
	SkinChanger2.PlaceLabledControl2(3, "ak47", this, &AK47Skin);

	M41SSkin.SetFileId("m4a1s_skin");
	M41SSkin.AddItem("dark water");
	M41SSkin.AddItem("hyper beast");
	M41SSkin.AddItem("decimator");
	M41SSkin.AddItem("variCamo");
	M41SSkin.AddItem("nitro");
	M41SSkin.AddItem("bright water");
	M41SSkin.AddItem("atomic alloy");
	M41SSkin.AddItem("blood tiger");
	M41SSkin.AddItem("guardian");
	M41SSkin.AddItem("master piece");
	M41SSkin.AddItem("knight");
	M41SSkin.AddItem("cyrex");
	M41SSkin.AddItem("basilisk");
	M41SSkin.AddItem("icarus fell");
	M41SSkin.AddItem("hot rod");
	M41SSkin.AddItem("golden coi");
	M41SSkin.AddItem("chantico's fire");
	M41SSkin.AddItem("mecha industries");
	M41SSkin.AddItem("flashback");
	SkinChanger2.PlaceLabledControl2(3, "m4a1 silencer", this, &M41SSkin);

	M4A4Skin.SetFileId("m4a4_skin");
	M4A4Skin.AddItem("bullet rain");
	M4A4Skin.AddItem("zirka");
	M4A4Skin.AddItem("asiimov");
	M4A4Skin.AddItem("howl");
	M4A4Skin.AddItem("x-ray");
	M4A4Skin.AddItem("desert-strike");
	M4A4Skin.AddItem("griffin");
	M4A4Skin.AddItem("dragon king");
	M4A4Skin.AddItem("poseidon");
	M4A4Skin.AddItem("daybreak");
	M4A4Skin.AddItem("evil daimyo");
	M4A4Skin.AddItem("royal paladin");
	M4A4Skin.AddItem("the battleStar");
	M4A4Skin.AddItem("desolate space");
	M4A4Skin.AddItem("buzz kill");
	SkinChanger2.PlaceLabledControl2(3, "m4a4", this, &M4A4Skin);

	AUGSkin.SetFileId("aug_skin");
	AUGSkin.AddItem("bengal tiger");
	AUGSkin.AddItem("hot rod");
	AUGSkin.AddItem("chameleon");
	AUGSkin.AddItem("torque");
	AUGSkin.AddItem("radiation hazard");
	AUGSkin.AddItem("asterion");
	AUGSkin.AddItem("daedalus");
	AUGSkin.AddItem("akihabara accept");
	AUGSkin.AddItem("ricochet");
	AUGSkin.AddItem("fleet flock");
	AUGSkin.AddItem("syd mead");
	SkinChanger2.PlaceLabledControl2(3, "aug", this, &AUGSkin);

	FAMASSkin.SetFileId("famas_skin");
	FAMASSkin.AddItem("contrast spray");
	FAMASSkin.AddItem("colony");
	FAMASSkin.AddItem("cyanospatter");
	FAMASSkin.AddItem("djinn");
	FAMASSkin.AddItem("afterimage");
	FAMASSkin.AddItem("doomkitty");
	FAMASSkin.AddItem("spitfire");
	FAMASSkin.AddItem("teardown");
	FAMASSkin.AddItem("hexane");
	FAMASSkin.AddItem("pulse");
	FAMASSkin.AddItem("sergeant");
	FAMASSkin.AddItem("styx");
	FAMASSkin.AddItem("neural net");
	FAMASSkin.AddItem("survivor");
	FAMASSkin.AddItem("valence");
	FAMASSkin.AddItem("roll cage");
	FAMASSkin.AddItem("mecha industries");
	SkinChanger2.PlaceLabledControl2(3, "famas", this, &FAMASSkin);

	GLOCKSkin.SetFileId("glock_skin");
	GLOCKSkin.AddItem("groundwater");
	GLOCKSkin.AddItem("candy Apple");
	GLOCKSkin.AddItem("fade");
	GLOCKSkin.AddItem("night");
	GLOCKSkin.AddItem("dragon tattoo");
	GLOCKSkin.AddItem("twilight galaxy");
	GLOCKSkin.AddItem("sand dune");
	GLOCKSkin.AddItem("brass");
	GLOCKSkin.AddItem("catacombs");
	GLOCKSkin.AddItem("sand Dune");
	GLOCKSkin.AddItem("steel disruption");
	GLOCKSkin.AddItem("blue fissure");
	GLOCKSkin.AddItem("death rattle");
	GLOCKSkin.AddItem("water elemental");
	GLOCKSkin.AddItem("reactor");
	GLOCKSkin.AddItem("grinder");
	GLOCKSkin.AddItem("bunsen burner");
	GLOCKSkin.AddItem("wraith");
	GLOCKSkin.AddItem("royal legion");
	GLOCKSkin.AddItem("weasel");
	GLOCKSkin.AddItem("wsteland rebel");
	SkinChanger2.PlaceLabledControl2(4, "glock", this, &GLOCKSkin);

	USPSSkin.SetFileId("usps_skin");
	USPSSkin.AddItem("frest leaves");
	USPSSkin.AddItem("dark water");;
	USPSSkin.AddItem("varicamo");
	USPSSkin.AddItem("Overgrowth");
	USPSSkin.AddItem("caiman");
	USPSSkin.AddItem("blood tiger");
	USPSSkin.AddItem("serum");
	USPSSkin.AddItem("neo-noir");
	USPSSkin.AddItem("stainless");
	USPSSkin.AddItem("guardian");
	USPSSkin.AddItem("orion");
	USPSSkin.AddItem("road rash");
	USPSSkin.AddItem("royal blue");
	USPSSkin.AddItem("business class");
	USPSSkin.AddItem("para green");
	USPSSkin.AddItem("torque");
	USPSSkin.AddItem("kill confirmed");
	USPSSkin.AddItem("lead conduit");
	USPSSkin.AddItem("cyrex");
	SkinChanger2.PlaceLabledControl2(4, "usp silencer", this, &USPSSkin);

	P2000Skin.SetFileId("p2000_skin");
	P2000Skin.AddItem("Handgun");
	P2000Skin.AddItem("Corticera");
	P2000Skin.AddItem("Ocean Foam");
	P2000Skin.AddItem("Fire Elemental");
	P2000Skin.AddItem("Imperial dragon");
	P2000Skin.AddItem("Ocean Foam");
	P2000Skin.AddItem("Amber fade");
	SkinChanger2.PlaceLabledControl2(4, "p2000", this, &P2000Skin);

	P250Skin.SetFileId("p250_skin");
	P250Skin.AddItem("Whiteout");
	P250Skin.AddItem("nuclear threat");
	P250Skin.AddItem("splash");
	P250Skin.AddItem("mehndi");
	P250Skin.AddItem("asiimov");
	P250Skin.AddItem("undertow");
	P250Skin.AddItem("franklin");
	P250Skin.AddItem("supernova");
	P250Skin.AddItem("crtel");
	SkinChanger2.PlaceLabledControl2(4, "p250", this, &P250Skin);

	DEAGLESkin.SetFileId("deagle_skin");
	DEAGLESkin.AddItem("blaze");
	DEAGLESkin.AddItem("pilot");
	DEAGLESkin.AddItem("midnight storm");
	DEAGLESkin.AddItem("sunset storm");
	DEAGLESkin.AddItem("forest ddpat");
	DEAGLESkin.AddItem("crimson web");
	DEAGLESkin.AddItem("urban ddpat");
	DEAGLESkin.AddItem("Night");
	DEAGLESkin.AddItem("hypnotic");
	DEAGLESkin.AddItem("mudder");
	DEAGLESkin.AddItem("varicamo");
	DEAGLESkin.AddItem("golden koi");
	DEAGLESkin.AddItem("cobalt disruption");
	DEAGLESkin.AddItem("urban rubble");
	DEAGLESkin.AddItem("naga");
	DEAGLESkin.AddItem("hand cannon");
	DEAGLESkin.AddItem("hirloom");
	DEAGLESkin.AddItem("meteorite");
	DEAGLESkin.AddItem("conspiracy");
	DEAGLESkin.AddItem("bronze deco");
	DEAGLESkin.AddItem("sunset storm");
	DEAGLESkin.AddItem("corinthian");
	DEAGLESkin.AddItem("kumicho dragon");
	SkinChanger2.PlaceLabledControl2(4, "deagle", this, &DEAGLESkin);

	MAC10Skin.SetFileId("mac10_skin");
	MAC10Skin.AddItem("tornado");
	MAC10Skin.AddItem("candy apple");
	MAC10Skin.AddItem("silver");
	MAC10Skin.AddItem("forest ddpat");
	MAC10Skin.AddItem("urban ddpay");
	MAC10Skin.AddItem("fade");
	MAC10Skin.AddItem("neon rider");
	MAC10Skin.AddItem("ultraviolet");
	MAC10Skin.AddItem("palm");
	MAC10Skin.AddItem("graven");
	MAC10Skin.AddItem("tatter");
	MAC10Skin.AddItem("amber fade");
	MAC10Skin.AddItem("heat");
	MAC10Skin.AddItem("curse");
	MAC10Skin.AddItem("indigo");
	MAC10Skin.AddItem("commuter");
	MAC10Skin.AddItem("nuclear garden");
	MAC10Skin.AddItem("malachite");
	MAC10Skin.AddItem("rangeen");
	MAC10Skin.AddItem("lapis Gator");
	SkinChanger2.PlaceLabledControl2(5, "mac 10", this, &MAC10Skin);

	P90Skin.SetFileId("p90_skin");
	P90Skin.AddItem("leather");
	P90Skin.AddItem("virus");
	P90Skin.AddItem("contrast spray");
	P90Skin.AddItem("storm");
	P90Skin.AddItem("cold blooded");
	P90Skin.AddItem("glacier mesh");
	P90Skin.AddItem("sand spray");
	P90Skin.AddItem("death by kitty");
	P90Skin.AddItem("ash Wood");
	P90Skin.AddItem("fallout warning");
	P90Skin.AddItem("scorched");
	P90Skin.AddItem("emerald dragon");
	P90Skin.AddItem("teardown");
	P90Skin.AddItem("blind Spot");
	P90Skin.AddItem("trigon");
	P90Skin.AddItem("desert warfare");
	P90Skin.AddItem("nodule");
	P90Skin.AddItem("asiimov");
	P90Skin.AddItem("elite build");
	P90Skin.AddItem("shapewood");
	P90Skin.AddItem("shallow Grave");
	SkinChanger2.PlaceLabledControl2(5, "p90", this, &P90Skin);

	UMP45Skin.SetFileId("ump45_skin");
	UMP45Skin.AddItem("blaze");
	UMP45Skin.AddItem("forest ddpat");
	UMP45Skin.AddItem("gunsmoke");
	UMP45Skin.AddItem("urban ddpat");
	UMP45Skin.AddItem("grand prix");
	UMP45Skin.AddItem("carbon fiber");
	UMP45Skin.AddItem("caramel");
	UMP45Skin.AddItem("fallout warning");
	UMP45Skin.AddItem("scorched");
	UMP45Skin.AddItem("bone Pile");
	UMP45Skin.AddItem("delusion");
	UMP45Skin.AddItem("corporal");
	UMP45Skin.AddItem("indigo");
	UMP45Skin.AddItem("labyrinth");
	UMP45Skin.AddItem("minotaur's labyrinth");
	UMP45Skin.AddItem("riot");
	UMP45Skin.AddItem("primal saber");
	SkinChanger2.PlaceLabledControl2(5, "ump 45", this, &UMP45Skin);

	MP7Skin.SetFileId("mp7_skin");
	MP7Skin.AddItem("groundwater");
	MP7Skin.AddItem("whiteout");
	MP7Skin.AddItem("forest ddpat");
	MP7Skin.AddItem("anodized navy");
	MP7Skin.AddItem("skulls");
	MP7Skin.AddItem("gunsmoke");
	MP7Skin.AddItem("contrast spray");
	MP7Skin.AddItem("bone mask");
	MP7Skin.AddItem("ossified");
	MP7Skin.AddItem("orange peel");
	MP7Skin.AddItem("varicamo");
	MP7Skin.AddItem("army recon");
	MP7Skin.AddItem("groundwater");
	MP7Skin.AddItem("ocean foam");
	MP7Skin.AddItem("full stop");
	MP7Skin.AddItem("urban hazard");
	MP7Skin.AddItem("olive plaid");
	MP7Skin.AddItem("armor core");
	MP7Skin.AddItem("asterion");
	MP7Skin.AddItem("nemesis");
	MP7Skin.AddItem("special delivery");
	MP7Skin.AddItem("impire");
	SkinChanger2.PlaceLabledControl2(5, "mp7", this, &MP7Skin);

	MP9Skin.SetFileId("mp9_skin");
	MP9Skin.AddItem("ruby poison dart");
	MP9Skin.AddItem("hot rod");
	SkinChanger2.PlaceLabledControl2(5, "mp9", this, &MP9Skin);

	MAG7Skin.SetFileId("mag7_skin");
	MAG7Skin.AddItem("counter terrace");
	MAG7Skin.AddItem("bulldozer");
	MAG7Skin.AddItem("heat");
	SkinChanger2.PlaceLabledControl2(6, "mag 7", this, &MAG7Skin);

	XM1014Skin.SetFileId("xm1014_skin");
	XM1014Skin.AddItem("tranquality");
	XM1014Skin.AddItem("teclu burner");
	XM1014Skin.AddItem("scumbria");
	XM1014Skin.AddItem("heaven guard");
	XM1014Skin.AddItem("quicksilver");
	SkinChanger2.PlaceLabledControl2(6, "xm1014", this, &XM1014Skin);

	SAWEDOFFSkin.SetFileId("sawedoff_skin");
	SAWEDOFFSkin.AddItem("serenity");
	SAWEDOFFSkin.AddItem("orange ddpat");
	SAWEDOFFSkin.AddItem("fade");
	SAWEDOFFSkin.AddItem("the kraken");
	SAWEDOFFSkin.AddItem("wasteland princess");
	SAWEDOFFSkin.AddItem("highwayman");
	SkinChanger2.PlaceLabledControl2(6, "sawed off", this, &SAWEDOFFSkin);

	NOVASkin.SetFileId("nova_skin");
	NOVASkin.AddItem("candy Apple");
	NOVASkin.AddItem("blaze Orange");
	NOVASkin.AddItem("modern Hunter");
	NOVASkin.AddItem("forest leaves");
	NOVASkin.AddItem("bloomstick");
	NOVASkin.AddItem("sand dune");
	NOVASkin.AddItem("polar mesh");
	NOVASkin.AddItem("walnut");
	NOVASkin.AddItem("predator");
	NOVASkin.AddItem("tempest");
	NOVASkin.AddItem("graphite");
	NOVASkin.AddItem("ghost camo");
	NOVASkin.AddItem("rising skull");
	NOVASkin.AddItem("antique");
	NOVASkin.AddItem("green apple");
	NOVASkin.AddItem("caged steel");
	NOVASkin.AddItem("koi");
	NOVASkin.AddItem("moon in libra");
	NOVASkin.AddItem("ranger");
	NOVASkin.AddItem("hyper beast");
	SkinChanger2.PlaceLabledControl2(6, "nova", this, &NOVASkin);

	SkinApply.SetText("Apply");
	SkinApply.SetCallback(KnifeApplyCallbk);
	SkinApply.SetPosition(100, 330);
	SkinApply.SetSize(500, 95);
	RegisterControl(&SkinApply);


#pragma endregion Skinchanger

}
void Menu::SetupMenu()
{
	Window.Setup();

	GUI.RegisterWindow(&Window);
	GUI.BindWindow(VK_INSERT, &Window);
}

void Menu::DoUIFrame()
{
	// General Processing

	// If the "all filter is selected tick all the others
	if (Window.VisualsTab.FiltersAll.GetState())
	{
		Window.VisualsTab.FiltersC4.SetState(true);
		Window.VisualsTab.FiltersChickens.SetState(true);
		Window.VisualsTab.FiltersPlayers.SetState(true);
		Window.VisualsTab.FiltersWeapons.SetState(true);
	}

	GUI.Update();
	GUI.Draw();


}


