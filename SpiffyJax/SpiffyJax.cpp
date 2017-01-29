// SpiffyJax.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PluginSDK.h"

PluginSetup("SpiffyJax - xSlapppz");

IMenu* MainMenu;
IMenu* ComboMenu;
IMenu* FarmMenu;
IMenu* HarassMenu;
IMenu* Drawings;
IMenu* SkinMenu;

IMenuOption* ComboQ;
IMenuOption* ComboW;
IMenuOption* ComboE;
IMenuOption* ComboR;
IMenuOption* AutoR;
IMenuOption* AutoRHealth;
IMenuOption* AutoStun;
IMenuOption* AutoStunEnemies;
IMenuOption* HarassQ;
IMenuOption* HarassW;
IMenuOption* FarmQ;
IMenuOption* FarmW;
IMenuOption* DrawQ;
IMenuOption* DrawE;
IMenuOption* WHop;
IMenuOption* SkinSelector;
IMenuOption* ChangeSkin;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;
ISpell2* Ward;

bool eCounterStrike = false;

void DrawMenu()
{
	MainMenu = GPluginSDK->AddMenu("SpiffyJax");
	ComboMenu = MainMenu->AddMenu("Combo Menu");
	HarassMenu = MainMenu->AddMenu("Harass Menu");
	FarmMenu = MainMenu->AddMenu("Farm Menu");
	SkinMenu = MainMenu->AddMenu("Skin Selector");
	Drawings = MainMenu->AddMenu("Drawings");

	ComboQ = ComboMenu->CheckBox("Use Q", true);
	ComboW = ComboMenu->CheckBox("Use W", true);
	ComboE = ComboMenu->CheckBox("Use E", true);
	AutoStun = ComboMenu->CheckBox("Auto Stun", true);
	ComboR = ComboMenu->CheckBox("Use R", true);
	AutoR = ComboMenu->CheckBox("Auto R when health <= x", false);
	AutoRHealth = ComboMenu->AddInteger("HP%", 10, 100, 50);

	HarassQ = HarassMenu->CheckBox("Use Q", true);
	HarassW = HarassMenu->CheckBox("Use W", true);

	FarmQ = FarmMenu->CheckBox("Use Q", true);
	FarmW = FarmMenu->CheckBox("Use W", true);

	ChangeSkin = SkinMenu->CheckBox("Enable Skin Selector", true);
	SkinSelector = SkinMenu->AddInteger("SkinID", 1, 12, 1);

	DrawQ = Drawings->CheckBox("Draw Q", true);
	DrawE = Drawings->CheckBox("Draw E", true);
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ,kTargetCast,false,false,kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, kCollidesWithNothing);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, true, kCollidesWithNothing);
	Ward = GPluginSDK->CreateSpell2(kSlotTrinket, kCircleCast, false, false, kCollidesWithWalls);

	Q->SetOverrideRange(700);
	W->SetOverrideRange(GEntityList->Player()->AttackRange());
	E->SetOverrideRange(187.5);
	Ward->SetOverrideRange(600);
}

void Combo()
{
	if (ComboE->Enabled())
	{
		E->CastOnPlayer();
	}
	if (ComboQ->Enabled()) 
	{
		Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range()));
	}
	if (AutoStun->Enabled() && GEntityList->Player()->GetSpellState(kSlotE) != DisabledOne)
	{
		E->CastOnPlayer();
	}
	if (ComboW->Enabled())
	{
		W->CastOnPlayer();
	}
	if (ComboR->Enabled())
	{
		if (AutoR->Enabled() && GEntityList->Player()->HealthPercent() <= AutoRHealth->GetFloat())
		{
			R->CastOnPlayer();
		}
		else if(!AutoR->Enabled())
		{
			R->CastOnPlayer();
		}
	}
}

void Harass()
{
	if (HarassW->Enabled())
	{
		W->CastOnPlayer();
	}
	if (HarassQ->Enabled())
	{
		Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range()));
	}
}

void LaneClear()
{
	if (FarmQ->Enabled()) {
		Q->LastHitMinion();
	}
	if (FarmW->Enabled())
	{
		W->LastHitMinion();
	}
}

PLUGIN_EVENT(void) OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		Combo();
	}
	else if (GOrbwalking->GetOrbwalkingMode() == kModeMixed)
	{
		Harass();
	}
	else if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		LaneClear();
	}
	if (ChangeSkin->Enabled())
	{
		GEntityList->Player()->SetSkinId(SkinSelector->GetInteger());
	}
}

PLUGIN_EVENT(void) OnRender()
{
	if (DrawQ->Enabled())
	{
		GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 0, 255, 64), Q->Range());
	}
	if (DrawE->Enabled())
	{
		GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 0, 183, 255), E->Range());
	}
}

PLUGIN_EVENT(void) OnOrbwalkAfterAttack(IUnit* Source, IUnit* Target)
{
	if (W->IsReady() && GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		W->CastOnPlayer();
}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	// Initializes global interfaces for core access
	PluginSDKSetup(PluginSDK);
	DrawMenu();
	LoadSpells();

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAfterAttack);
}

// Called when plugin is unloaded
PLUGIN_API void OnUnload()
{
	MainMenu->Remove();

	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAfterAttack);
}