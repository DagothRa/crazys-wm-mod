/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <locale>
#include <sstream>
#include "cBrothel.h"
#include "cWindowManager.h"
#include "cGold.h"
#include "sFacilityList.h"
#include "cGetStringScreenManager.h"
#include "InterfaceProcesses.h"
#include "cScriptManager.h"
#include "cGangs.h"
#include "FileList.h"
#include "cClinic.h"
#include "cClinicScreen.h"

extern	string					ReadTextFile(DirPath path, string file);
extern	int						g_CurrentScreen;
extern	int						g_CurrBrothel;
extern	int						g_Building;
extern	cWindowManager			g_WinManager;
extern	cInterfaceEventManager	g_InterfaceEvents;
extern	cGold					g_Gold;
extern	cGangManager			g_Gangs;
extern	cBrothelManager			g_Brothels;
extern	bool					g_InitWin;
extern	bool					g_Cheats;
extern	bool					g_CTRLDown;
extern	bool					g_AllTogle;
extern	bool					eventrunning;
extern	cClinicManager			g_Clinic;
extern	bool					g_TryEr;

bool cClinicScreen::ids_set = false;

void cClinicScreen::set_ids()
{
	ids_set = true;
	back_id = get_id("BackButton");
	curbrothel_id = get_id("CurrentBrothel");
	dungeon_id = get_id("Dungeon");
	girlimage_id = get_id("GirlImage");
	girls_id = get_id("Girls");
	info_id = get_id("Info");
	nextbrothel_id = get_id("Next");
	prevbrothel_id = get_id("Prev");
	setup_id = get_id("SetUp");
	staff_id = get_id("Staff");
	turns_id = get_id("Turn");
	walk_id = get_id("WalkButton");
	weeks_id = get_id("Weeks");
	clinic_id = get_id("Clinic");
	clinicdetails_id = get_id("ClinicDetails");
}

void cClinicScreen::init()
{
	g_CurrentScreen = SCREEN_CLINIC;
	g_Building = BUILDING_CLINIC;
	// buttons enable/disable
	DisableButton(walk_id, g_TryEr);
}

void cClinicScreen::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (girlimage_id != -1 && !eventrunning)	HideImage(girlimage_id, true);
	init();

	if (g_InitWin)
	{
		EditTextItem(g_Clinic.GetBrothelString(0), clinicdetails_id);
		g_InitWin = false;
	}
	if (g_InterfaceEvents.GetNumEvents() == 0)	return;	// no events means we can go home

	// otherwise, compare event IDs
	if (g_InterfaceEvents.CheckButton(back_id))			// if it's the back button, pop the window off the stack and we're done
	{
		g_CurrentScreen = SCREEN_TOWN;
		g_InitWin = true;
		g_WinManager.Pop();
		return;
	}
	else if (g_InterfaceEvents.CheckButton(walk_id))
	{
		do_walk();
		if (!g_Cheats) g_TryEr = true;
		g_InitWin = true;
		return;
	}
	else if (g_InterfaceEvents.CheckButton(girls_id))
	{
		g_InitWin = true;
		g_WinManager.push("Clinic");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(staff_id))
	{
		g_InitWin = true;
		g_WinManager.push("Gangs");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(turns_id))
	{
		g_InitWin = true;
		g_WinManager.push("Turn Summary");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(setup_id))
	{
		g_Building = BUILDING_CLINIC;
		g_InitWin = true;
		g_WinManager.push("Building Setup");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(dungeon_id))
	{
		g_InitWin = true;
		g_WinManager.push("Dungeon");
		return;
	}
	else if (g_InterfaceEvents.CheckButton(weeks_id))
	{
		g_InitWin = true;
		if (!g_CTRLDown) { g_CTRLDown = false; AutoSaveGame(); }
		NextWeek();
		g_WinManager.push("Turn Summary");
		return;
	}
}

string cClinicScreen::walk_no_luck()
{
	if (m_first_walk)
	{
		m_first_walk = false;
		return	"Your father once called this 'talent spotting' - and looking these girls over you see no talent for anything.";
	}
	switch (g_Dice % 8)
	{
	case 0:	return "Married. Married. Bodyguard. Already works for you. Married. Hideous. Not a woman. Married. Escorted. Married...\nMight as well go home, there's nothing happening here.";
	case 1:	return "It's not a bad life, if you can get paid for hanging in the clinic eyeing up the pretty girls that might be brought in. Not a single decent prospect in the bunch of them. ";
	case 2:	return "You've walked and walked and walked, and the prettiest thing you've seen all day turned out not to be female. It's time to go home...";
	case 3:	return "When the weather is bad, the hunting is good. The cold brings in the sick. But nothing of note today. ";
	case 4:	return "There's a bit of skirt over there with a lovely figure, and had a face that was pretty, ninety years ago. Over yonder, a sweet young thing but she's got daddy's gold. Looks like nothing to gain here today. ";
	default:return "The Clinic is quite not much going on here.";
	}
	// I don't think this should happen, hence the overly dramatic prose
	return "The sky is filled with thunder, and portals are opening all over Crossgate. You've seen five rains of frogs so far and three madmen speaking in tongues. In the marketplace a mechanical contraption materialised which walked like a man and declaimed alien poetry for half an hour before bursting into flames and collapsing. And the only thing all day that looked female and and attractive turned out to be some otherwordly species of carnivorous plant, and had to be destroyed by the town guard. The only good thing about this day is that it's over. It's time to go home.";
}

void cClinicScreen::do_walk()
{
	if (g_TryEr)
	{
		g_MessageQue.AddToQue("You can only do this once per week.", COLOR_RED);
		return;
	}
	sGirl *girl = g_Girls.GetRandomGirl();						// let's get a girl for the player to meet
	if (girl == 0)												// if there's no girl, no meeting
	{
		g_MessageQue.AddToQue(walk_no_luck(), COLOR_RED);
		return;
	}
	// most of the time, you're not going to find anyone unless you're cheating, of course.
	if (!g_Dice.percent(cfg.initial.girl_meet()) && !g_Cheats)
	{
		g_MessageQue.AddToQue(walk_no_luck(), COLOR_BLUE);
		return;
	}
	if (girlimage_id != -1)
	{
		if (girl)
		{
			PrepareImage(girlimage_id, girl, IMGTYPE_PROFILE, true, -1);
			HideImage(girlimage_id, false);
		}
		else HideImage(girlimage_id, true);
	}

	g_Building = BUILDING_CLINIC;
	int v[2] = { 0, -1 };
	cTrigger* trig = 0;
	DirPath dp;
	DirPath intro;
	string introfile = "";
	string message = "";
	cScriptManager sm;

	// is there a girl specific talk script?
	if (!(trig = girl->m_Triggers.CheckForScript(TRIGGER_MEET, false, v)))
	{
		// no, so trigger the default one
		introfile = "MeetClinicTry.script.intro";
		intro = DirPath() << "Resources" << "Scripts";
		dp = DirPath() << "Resources" << "Scripts" << "MeetClinicTry.script";
	}
	else
	{
		// trigger the girl-specific one
		introfile = trig->m_Script + ".intro";
		intro = DirPath(cfg.folders.characters().c_str()) << girl->m_Name;
		dp = DirPath(cfg.folders.characters().c_str()) << girl->m_Name << trig->m_Script;
	}

	FileList abstest(intro, introfile.c_str());
	if (abstest.size() == 0)
	{
		message = "You go to your clinic in the hopes that your men have brought in a potential new girl as per your orders.";
	}
	else
	{
		message = ReadTextFile(intro, introfile);
	}
	if (message.size() > 0) g_MessageQue.AddToQue(message, COLOR_BLUE);

	eventrunning = true;
	sm.Load(dp, girl);
}