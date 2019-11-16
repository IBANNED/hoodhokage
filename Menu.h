/*
- hoodhokage by ibanned -
*/

#pragma once

#include "GUI.h"
#include "Controls.h"

class CRageBotTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;

	// Aimbot Settings
	CGroupBox AimbotGroup;
	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CSlider	  AimbotFov;
	CComboBox AimbotSilentAim;
	CCheckBox AimbotPerfectSilentAim;
	CCheckBox AimbotAutoPistol;
	CCheckBox AimbotAutoRevolver;
	CCheckBox AimbotAimStep;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CKeyBind  AimbotStopKey;
	CCheckBox  baimall;
	CSlider   AimbotMultiPointIndexes;
	CCheckBox AwpAtBody;
	CKeyBind SomeShit;
	CKeyBind bigbaim;
	CSlider   bruteX;
	CSlider   baim;
	CSlider BaimIfUnderXHealth;

	// Target Selection Settings
	CGroupBox TargetGroup;
	CComboBox TargetSelection;
	CCheckBox TargetFriendlyFire;
	CComboBox TargetHitbox;
	CComboBox TargetHitscan;
	CCheckBox TargetMultipoint;
	CComboBox AccuracyDrop;
	CSlider   TargetSmartScan;
	CSlider   TargetPointscale; 

	// Accuracy Settings
	CGroupBox AccuracyGroup;
	CCheckBox AccuracyRecoil;
	CCheckBox AccuracyAutoWall;
	CSlider	  AccuracyMinimumDamage;
	CCheckBox AutoMinimumDamage;
	CCheckBox AccuracyAutoStop;
	CCheckBox AccuracyAutoCrouch;
	CCheckBox Autorevolver;
	CCheckBox AccuracyAutoScope;
	CSlider   AccuracyHitchanceVal;
	CCheckBox AutoHitChance;
	CCheckBox AccuracyPositionAdjustment;
	CCheckBox AccuracyPrediction;
	CCheckBox AccuracyBacktracking;
	CCheckBox AimbotBacktrack2;
	CCheckBox InfoResolver;
	CSlider   TickModulation2;
	CCheckBox FakePingExploit;
	CSlider FakePingValue;

	//resolver
	CCheckBox LBYCorrection;
	CCheckBox Override;
	CSlider OverrideCount;
	CKeyBind OverrideKey;
	CComboBox OverrideMode;
	CCheckBox AccuracyResolver;
	CCheckBox AccuracyResolverPitch;
	CComboBox AccuracyResolverYaw;
	CCheckBox AccuracyResolverenable;
	CSlider forceback;
	CKeyBind enemyflip;
	CCheckBox Resolver3;
	CCheckBox AnimFixz;
	CComboBox AimbotResolver;
	CCheckBox AdvancedResolver;
	//end resolver

    //other gg
	// Anti-Aim Settings
	CGroupBox AntiAimGroup;
	CCheckBox AntiAimEnable;
	CComboBox AntiAimPitch;
	CComboBox staticyaw;
	CComboBox movingyaw;
	CComboBox fakeyaw;
	CComboBox edgeyaw;
	CComboBox AntiAimEdge;
	CSlider	  AntiAimOffset;
	CComboBox AntiResolver2;
	CSlider	  SpinSpeed;
	CSlider	  SpinSpeed2;
	CSlider	  AntiAimOffsetReal;
	CSlider	  AntiAimOffsetFake;
	CCheckBox AntiAimKnife;
	CCheckBox Edgewall;
	CCheckBox AntiAimTarget;
	CCheckBox BreakLBY;
	CCheckBox AntiAimJitterLBY;
	CSlider	  AntiAimSpinspeed;
	CComboBox AntiResolver;
	CKeyBind  FlipKey;

	CKeyBind  keybasedaaleft;
	CKeyBind  keybasedaaright;
	CKeyBind  keybasedaaback;
	CCheckBox FakeLagEnable2;
	CSlider   FakeLagChoke2;

	CSeperate  keybasedaaleft2;
	CSeperate  keybasedaaright2;
	CSeperate  keybasedaaback2;
	//end

};

class CLegitBotTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;

	// Aimbot Settings
	CGroupBox AimbotGroup;
	CSlider   TickModulation;
	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CCheckBox AimbotFriendlyFire;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CSlider   TickDistance;
	CCheckBox OtherRadar;
	CCheckBox AimbotAutoPistol;
	CCheckBox AimbotBacktrack;
	CCheckBox AimbotChicken;
	CCheckBox AimbotSmokeCheck;
	CCheckBox OtherMemewalk;
	CCheckBox AimIndicator;

	CLabel pistol;
	CLabel rifle;
	CLabel sniper;
	// Main
	CGroupBox TriggerGroup;

	CCheckBox TriggerEnable;
	CCheckBox TriggerKeyPress;
	CKeyBind  TriggerKeyBind;
	CCheckBox TriggerHitChance;
	CSlider2   TriggerHitChanceAmmount;
	CSeperate TriggerDelay1;
	CSlider2   TriggerDelay;
	CSlider2   TriggerBurst;
	CSlider2   TriggerBreak;
	CCheckBox TriggerRecoil;

	// Trigger Filter
	CGroupBox TriggerFilterGroup;

	CCheckBox TriggerHead;
	CCheckBox TriggerChest;
	CCheckBox TriggerStomach;
	CCheckBox TriggerArms;
	CCheckBox TriggerLegs;
	CCheckBox TriggerTeammates;

	// Rifles
	CGroupBox2 WeaponMainGroup;

	CSlider2   WeaponMainSpeed;
	CSlider2   WeaponMainFoV;
	CCheckBox WeaponMainRecoil;
	CCheckBox WeaponMainPSilent;
	CSlider2   WeaponMainInacc;
	CComboBox WeaponMainHitbox;
	CSlider2 WeaponMainAimtime;
	CSlider WeaoponMainStartAimtime;

	// Rifle m4
	CSlider2   WeaponMainSpeedM4;
	CSlider2   WeaponMainFoVM4;
	CCheckBox WeaponMainRecoilM4;
	CCheckBox WeaponMainPSilentM4;
	CSlider2   WeaponMainInaccM4;
	CComboBox WeaponMainHitboxM4;
	CSlider2 WeaponMainAimtimeM4;
	CSlider WeaoponMainStartAimtimeM4;

	//aug 

	CSlider2   WeaponMainSpeedaug;
	CSlider2   WeaponMainFoVaug;
	CCheckBox WeaponMainRecoilaug;
	CCheckBox WeaponMainPSilentaug;
	CSlider2   WeaponMainInaccaug;
	CComboBox WeaponMainHitboxaug;
	CSlider2 WeaponMainAimtimeaug;
	CSlider WeaoponMainStartAimtimeaug;

	//famas 

	CSlider2   WeaponMainSpeedfamas;
	CSlider2   WeaponMainFoVfamas;
	CCheckBox WeaponMainRecoilfamas;
	CCheckBox WeaponMainPSilentfamas;
	CSlider2   WeaponMainInaccfamas;
	CComboBox WeaponMainHitboxfamas;
	CSlider2 WeaponMainAimtimefamas;
	CSlider WeaoponMainStartAimtimefamas;

	//MP9 

	CSlider2   WeaponMainSpeedMP9;
	CSlider2   WeaponMainFoVMP9;
	CCheckBox WeaponMainRecoilMP9;
	CCheckBox WeaponMainPSilentMP9;
	CSlider2   WeaponMainInaccMP9;
	CComboBox WeaponMainHitboxMP9;
	CSlider2 WeaponMainAimtimeMP9;
	CSlider WeaoponMainStartAimtimeMP9;

	//P90 

	CSlider2   WeaponMainSpeedP90;
	CSlider2   WeaponMainFoVP90;
	CCheckBox WeaponMainRecoilP90;
	CCheckBox WeaponMainPSilentP90;
	CSlider2   WeaponMainInaccP90;
	CComboBox WeaponMainHitboxP90;
	CSlider2 WeaponMainAimtimeP90;
	CSlider WeaoponMainStartAimtimeP90;

//--------------------------------------------

	// USP
	CGroupBox2 WeaponPistGroup;

	CSlider2   WeaponPistSpeed;
	CSlider2   WeaponPistFoV;
	CCheckBox WeaponPistRecoil;
	CCheckBox WeaponPistPSilent;
	CSlider2   WeaponPistInacc;
	CComboBox WeaponPistHitbox;
	CSlider2 WeaponPistAimtime;
	CSlider2 WeaoponPistStartAimtime;

	// DESERT

	CSlider2   WeaponPistSpeedD;
	CSlider2   WeaponPistFoVD;
	CCheckBox WeaponPistRecoilD;
	CCheckBox WeaponPistPSilentD;
	CSlider2   WeaponPistInaccD;
	CComboBox WeaponPistHitboxD;
	CSlider2 WeaponPistAimtimeD;
	CSlider2 WeaoponPistStartAimtimeD;

	// CZ

	CSlider2   WeaponPistSpeedCZ;
	CSlider2   WeaponPistFoVCZ;
	CCheckBox WeaponPistRecoilCZ;
	CCheckBox WeaponPistPSilentCZ;
	CSlider2   WeaponPistInaccCZ;
	CComboBox WeaponPistHitboxCZ;
	CSlider2 WeaponPistAimtimeCZ;
	CSlider2 WeaoponPistStartAimtimeCZ;

	// TEC-9

	CSlider2   WeaponPistSpeedtec;
	CSlider2   WeaponPistFoVtec;
	CCheckBox WeaponPistRecoiltec;
	CCheckBox WeaponPistPSilenttec;
	CSlider2   WeaponPistInacctec;
	CComboBox WeaponPistHitboxtec;
	CSlider2 WeaponPistAimtimetec;
	CSlider2 WeaoponPistStartAimtimetec;

	// P250

	CSlider2   WeaponPistSpeedp250;
	CSlider2   WeaponPistFoVp250;
	CCheckBox WeaponPistRecoilp250;
	CCheckBox WeaponPistPSilentp250;
	CSlider2   WeaponPistInaccp250;
	CComboBox WeaponPistHitboxp250;
	CSlider2 WeaponPistAimtimep250;
	CSlider2 WeaoponPistStartAimtimep250;

	// GLOCK

	CSlider2   WeaponPistSpeedGLOCK;
	CSlider2   WeaponPistFoVGLOCK;
	CCheckBox WeaponPistRecoilGLOCK;
	CCheckBox WeaponPistPSilentGLOCK;
	CSlider2   WeaponPistInaccGLOCK;
	CComboBox WeaponPistHitboxGLOCK;
	CSlider2 WeaponPistAimtimeGLOCK;
	CSlider2 WeaoponPistStartAimtimeGLOCK;

	// FIVEN7

	CSlider2   WeaponPistSpeedFIVEN7;
	CSlider2   WeaponPistFoVFIVEN7;
	CCheckBox WeaponPistRecoilFIVEN7;
	CCheckBox WeaponPistPSilentFIVEN7;
	CSlider2   WeaponPistInaccFIVEN7;
	CComboBox WeaponPistHitboxFIVEN7;
	CSlider2 WeaponPistAimtimeFIVEN7;
	CSlider2 WeaoponPistStartAimtimeFIVEN7;

//--------------------------------------------

	// SCAR20
	CGroupBox2 WeaponSnipGroup;

	CSlider2   WeaponSnipSpeed;
	CSlider2   WeaponSnipFoV;
	CCheckBox WeaponSnipRecoil;
	CCheckBox WeaponSnipPSilent;
	CSlider2   WeaponSnipInacc;
	CComboBox WeaponSnipHitbox;
	CSlider2 WeaponSnipAimtime;
	CSlider2 WeaoponSnipStartAimtime;

	// G3SG1

	CSlider2   WeaponSnipSpeedG3SG1;
	CSlider2   WeaponSnipFoVG3SG1;
	CCheckBox WeaponSnipRecoilG3SG1;
	CCheckBox WeaponSnipPSilentG3SG1;
	CSlider2   WeaponSnipInaccG3SG1;
	CComboBox WeaponSnipHitboxG3SG1;
	CSlider2 WeaponSnipAimtimeG3SG1;
	CSlider2 WeaoponSnipStartAimtimeG3SG1;

	// SSG08Z

	CSlider2   WeaponSnipSpeedSSG08Z;
	CSlider2   WeaponSnipFoVSSG08Z;
	CCheckBox WeaponSnipRecoilSSG08Z;
	CCheckBox WeaponSnipPSilentSSG08Z;
	CSlider2   WeaponSnipInaccSSG08Z;
	CComboBox WeaponSnipHitboxSSG08Z;
	CSlider2 WeaponSnipAimtimeSSG08Z;
	CSlider2 WeaoponSnipStartAimtimeSSG08Z;

	// AWP

	CSlider2   WeaponSnipSpeedAWP;
	CSlider2   WeaponSnipFoVAWP;
	CCheckBox WeaponSnipRecoilAWP;
	CCheckBox WeaponSnipPSilentAWP;
	CSlider2   WeaponSnipInaccAWP;
	CComboBox WeaponSnipHitboxAWP;
	CSlider2 WeaponSnipAimtimeAWP;
	CSlider2 WeaoponSnipStartAimtimeAWP;

	CGroupBox Triggerbot;
//--------------------------------------------

	// MPs
	CGroupBox2 WeaponMpGroup;

	CSlider2   WeaponMpSpeed;
	CSlider2   WeaponMpFoV;
	CSlider2 WeaponMpRecoil;
	CCheckBox WeaponMpPSilent;
	CComboBox WeaponMpHitbox;
	CSlider2 WeaponMpAimtime;
	CSlider2 WeaoponMpStartAimtime;

	// Shotguns
	CGroupBox2 WeaponShotgunGroup;

	CSlider2   WeaponShotgunSpeed;
	CSlider2   WeaponShotgunFoV;
	CSlider2 WeaponShotgunRecoil;
	CCheckBox WeaponShotgunPSilent;
	CComboBox WeaponShotgunHitbox;
	CSlider WeaponShotgunAimtime;
	CSlider WeaoponShotgunStartAimtime;
	CComboBox LegitChoice;

	// Machineguns
	CGroupBox2 WeaponMGGroup;

	CSlider2   WeaponMGSpeed;
	CSlider2   WeaponMGFoV;
	CSlider2 WeaponMGRecoil;
	CCheckBox WeaponMGPSilent;
	CComboBox WeaponMGHitbox;
	CSlider2 WeaponMGAimtime;
	CSlider2 WeaoponMGStartAimtime;
};

class CVisualTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;

	// Options Settings
	CGroupBox OptionsGroup;
	CCheckBox OptionsBox;
	CCheckBox OptionsFillBox;
	CCheckBox OptionsName;
	CComboBox OptionsVitals;
	CCheckBox FakeAngleChams;
	CCheckBox OptionsBarrel;
	CCheckBox OptionsWeapon;
	CCheckBox OptionsInfo;
	CCheckBox OptionsHelmet;
	CCheckBox OtherSniperCrosshair;
	CCheckBox OtherSpreadCrosshair;
	CCheckBox OptionsKit;
	CCheckBox BulletTracers;
	CCheckBox OptionsDefuse;
	CCheckBox OptionsGlow;
	CComboBox OptionsChams;
	CCheckBox OptionsSkeleton;
	CCheckBox OptionsAimSpot;
	CCheckBox ChamsXQZ;
	CCheckBox GrenadeTracer;
	CCheckBox ShowImpacts;
	CCheckBox IsScoped;
	CCheckBox HasDefuser;
	CCheckBox IsDefusing;
	CCheckBox Nades;
	CCheckBox BacktrackingLol;
	CCheckBox Localplayerz;
	CCheckBox RemoveScope;
	CCheckBox Logs;
	CComboBox angleindicator;
	CCheckBox OptionsLBY;
	CCheckBox OtherGlow;
	CCheckBox Grenades;
	CSlider   BulletTraceLength;

	//color chams
	CSlider TChamsR;
	CSlider TChamsG;
	CSlider TChamsB;
	CSlider CTChamsR;
	CSlider CTChamsG;
	CSlider CTChamsB;

	//Chams Vis
	CSlider TChamsRVis;
	CSlider TChamsGVis;
	CSlider TChamsBVis;
	CSlider Opacityx;
	CSlider CTChamsRVis;
	CSlider CTChamsGVis;
	CSlider CTChamsBVis;

	//color box
	CSlider BoxR;
	CSlider BoxG;
	CSlider BoxB;
	CSlider Opacidade;

	CSlider BoxRVis;
	CSlider BoxGVis;
	CSlider BoxBVis;
	CSlider OpacidadeVis;

	// Filters Settings
	CGroupBox FiltersGroup;
	CCheckBox FiltersAll;
	CCheckBox FiltersPlayers;
	CCheckBox FiltersEnemiesOnly;
	CCheckBox FiltersWeapons;
	CCheckBox FiltersChickens;
	CCheckBox FiltersC4;

	// Other Settings
	CCheckBox Watermark;
	CGroupBox OtherGroup;
	CCheckBox OtherCrosshair;
	CComboBox OtherRecoilCrosshair;
	CComboBox OtherHitmarkerType;
	CCheckBox OtherHitmarker;
	CCheckBox OtherNoVisualRecoil;
	CCheckBox OtherNoSky; 
	CCheckBox OtherNoFlash; 
	CCheckBox OtherPlayerLine;
	CCheckBox OtherEnemyCircle;
	CCheckBox PostProcessing;
	CCheckBox OtherNoSmoke;
	CCheckBox OtherAsusWalls;
	CComboBox OtherNoHands;
	CComboBox HandSelfVisual;
	CCheckBox HandXQZ;
	CSlider OtherViewmodelFOV;
	CSlider OtherFOV;
	CCheckBox NightSky;
	CSlider Test;
	CCheckBox DamageIndicator;

	// Cvar Options
	CGroupBox CvarGroup;
	CSlider2 AmbientExposure;
	CSlider2 AmbientRed;
	CSlider2 AmbientGreen;
	CSlider2 AmbientBlue;
	CComboBox AmbientSkybox;
	CCheckBox NightMode;
};

class CMiscTab : public CTab
{
public:
	void Setup();

	// Knife Changer
	CGroupBox KnifeGroup;
	CCheckBox KnifeEnable;
	CComboBox KnifeModel;
	CComboBox KnifeSkin;
	CButton   KnifeApply;

	// Other Settings
	CGroupBox OtherGroup;
	CCheckBox OtherAutoJump;
	CCheckBox OtherEdgeJump;
	CComboBox OtherAutoStrafe;
	CCheckBox OtherSafeMode;
	CComboBox OtherHitmarkerType;
	CComboBox OtherChatSpam;
	CCheckBox OtherTeamChat;
	CSlider	  OtherChatDelay;
	CKeyBind  OtherAirStuck;
	CKeyBind  OtherLagSwitch;
	CCheckBox OtherSpectators;
	CCheckBox OtherCrasher;
	CCheckBox OtherHitmarker;
	CCheckBox OptionsCompRank;
	CComboBox FakeLagTyp;
	CGroupBox grpPList;
	CListBox lstPlayers;

	CGroupBox ConfigGroup;
	CListBox2 ConfigListBox;
	CButton SaveConfig;
	CButton LoadConfig;
	CButton RemoveConfig;
	CTextField2 NewConfigName;
	CButton AddConfig;

	CSeperate sas;
	CSeperate eas;
	CSeperate gassz;
	CSeperate szas;
	CSeperate zdas;
	CSeperate zdasz;

	// Fake Lag Settings
	CGroupBox FakeLagGroup;
	CCheckBox FakeLagEnable;
	CSlider   FakeLagChoke;
	//CCheckBox FakeLagWhileShooting;

	CCheckBox OtherThirdperson;
	CComboBox OtherThirdpersonType;
	CSlider OtherThirdpersonRange;
	CKeyBind OtherThirdpersonKey;
	CKeyBind bindkey2;
	CKeyBind bindkey3;
	//CCheckBox OtherAutoAccept;
	CCheckBox OtherWalkbot;
	CCheckBox OtherClantag; 
	CGroupBox OtherConfig;
	CComboBox configurationbox;
	CKeyBind configuration2;
	CComboBox ConfigBox;


	CCheckBox OtherAutoAccept;

	// Fake Lag Settings
	CCheckBox lbyidicador;

	//CCheckBox FakeLagWhileShooting;

	// Teleport shit cause we're cool
	CGroupBox TeleportGroup;
	CCheckBox TeleportEnable;
	CKeyBind  TeleportKey;



	//MASTER SWITCH
	CLabel AntiAimActiveLabel;
	CCheckBox AntiAimActive;


};

class CAntiAimTab : public CTab
{
	public:
		void Setup();

		
};

class CColorTab : public CTab
{
	public:
		void Setup();

		CGroupBox ColorGroup;
		CGroupBox CTColorGroup;
		CGroupBox TerroristColorGroup;
		CLabel  PlayerEsp;
		

		//FillBox
		CSlider	  FillBoxRed;
		CSlider	  FillBoxGreen;
		CSlider	  FillBoxBlue;
		CSlider	  FillBoxAlpha;
	
		//Box esp visble
		CSlider	  VisBoxRed;
		CSlider	  VisBoxGreen;
		CSlider	  VisBoxBlue;
		CSlider	  VisBoxAlpha;

		CSlider	  VisBoxRed2;
		CSlider	  VisBoxGreen2;
		CSlider	  VisBoxBlue2;
		CSlider	  VisBoxAlpha2;

		//Box esp 
		CSlider	  BoxRed;
		CSlider	  BoxGreen;
		CSlider	  BoxBlue;
		CSlider	  BoxAlpha;

		CSlider	  BoxRed2;
		CSlider	  BoxGreen2;
		CSlider	  BoxBlue2;
		CSlider	  BoxAlpha2;

		//Chams 
		CSlider	  CTChamsRed;
		CSlider	  CTChamsGreen;
		CSlider	  CTChamsBlue;

		CSlider	  CTVisChamsRed;
		CSlider	  CTVisChamsGreen;
		CSlider	  CTVisChamsBlue;

		CSlider	 TerroristChamsRed;
		CSlider	 TerroristChamsGreen;
		CSlider	TerroristChamsBlue;

		CSlider	 TerroristVisChamsRed;
		CSlider	 TerroristVisChamsGreen;
		CSlider	TerroristVisChamsBlue;



};



class SkinTab : public CTab
{
public:
	void Setup();

	// Knife Changer/Skin Changer
	CLabel SkinActive;
	CCheckBox SkinEnable;
	CButton   SkinApply;
	CGroupBox2 SkinChanger2;

	CComboBox GloveSkin;
	CCheckBox GloveActive;
	CComboBox GloveModel;
	// Knife
	CGroupBox KnifeGroup;
	CComboBox KnifeModel;
	CComboBox KnifeSkin;

	// Pistols
	CGroupBox PistolGroup;
	CComboBox GLOCKSkin;
	CComboBox USPSSkin;
	CComboBox DEAGLESkin;
	CComboBox MAGNUMSkin;
	CComboBox DUALSSkin;
	CComboBox FIVESEVENSkin;
	CComboBox TECNINESkin;
	CComboBox P2000Skin;
	CComboBox P250Skin;

	// MPs
	CGroupBox MPGroup;
	CComboBox MAC10Skin;
	CComboBox P90Skin;
	CComboBox UMP45Skin;
	CComboBox BIZONSkin;
	CComboBox MP7Skin;
	CComboBox MP9Skin;

	// Rifles
	CGroupBox Riflegroup;
	CComboBox M41SSkin;
	CComboBox M4A4Skin;
	CComboBox AK47Skin;
	CComboBox AUGSkin;
	CComboBox FAMASSkin;
	CComboBox GALILSkin;
	CComboBox SG553Skin;


	// Machineguns
	CGroupBox MachinegunsGroup;
	CComboBox NEGEVSkin;
	CComboBox M249Skin;

	// Snipers
	CGroupBox Snipergroup;
	CComboBox SCAR20Skin;
	CComboBox G3SG1Skin;
	CComboBox SSG08Skin;
	CComboBox AWPSkin;

	// Shotguns
	CGroupBox Shotgungroup;
	CComboBox MAG7Skin;
	CComboBox XM1014Skin;
	CComboBox SAWEDOFFSkin;
	CComboBox NOVASkin;
};


class hoodhokageWindow : public CWindow
{
public:
	void Setup();

	CRageBotTab RageBotTab;
	CLegitBotTab LegitBotTab;
	CVisualTab VisualsTab;
	CMiscTab MiscTab;
	CAntiAimTab AntiAimTab;
	CColorTab ColorTab;
	SkinTab SkinTab;
	
	//CPlayersTab Playerlist;

	CButton SaveButton;
	CButton LoadButton;
	CButton UnloadButton;

	CComboBox ConfigBox;
};

namespace Menu
{
	void SetupMenu();
	void DoUIFrame();

	extern hoodhokageWindow Window;
};