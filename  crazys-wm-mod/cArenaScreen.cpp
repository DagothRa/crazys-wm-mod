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
#include "cArena.h"
#include "cArenaScreen.h"

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
extern	cArenaManager			g_Arena;
extern	bool					g_TryOuts;

bool cArenaScreen::ids_set = false;

void cArenaScreen::set_ids()
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
	arena_id = get_id("Arena");
	arenadetails_id = get_id("ArenaDetails");
}

void cArenaScreen::init()
{
	g_CurrentScreen = SCREEN_ARENA;
	g_Building = BUILDING_ARENA;
	// buttons enable/disable
	DisableButton(walk_id, g_TryOuts);
}

void cArenaScreen::process()
{
	if (!ids_set) set_ids();	// we need to make sure the ID variables are set
	if (girlimage_id != -1 && !eventrunning)	HideImage(girlimage_id, true);
	init();

	if (g_InitWin)
	{
		EditTextItem(g_Arena.GetBrothelString(0), arenadetails_id);
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
		if (!g_Cheats) g_TryOuts = true;
		g_InitWin = true;
		return;
	}
	else if (g_InterfaceEvents.CheckButton(girls_id))
	{
		g_InitWin = true;
		g_WinManager.push("Arena");
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
		g_Building = BUILDING_ARENA;
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

string cArenaScreen::walk_no_luck()
{
	if (m_first_walk)
	{
		m_first_walk = false;
		return	"Your father once called this 'talent spotting' - and looking these girls over you see no talent for anything.";
	}
	switch (g_Dice % 8)
	{
	case 0: return "Married. Married. Bodyguard. Already works for you. Married. Hideous. Not a woman. Married. Escorted. Married...\nMight as well go home, there's nothing happening out here.";
	case 1: return "It's not a bad life, if you can get paid for hanging around on street corners and eyeing up the pretty girls. Not a single decent prospect in the bunch of them, mind. Every silver lining has a cloud...";
	case 2: return "You've walked and walked and walked, and the prettiest thing you've seen all day turned out not to be female. It's time to go home...";
	case 3: return "When the weather is bad, the hunting is good. Get them cold and wet enough and girls too proud to spread their legs suddenly can't get their knickers off fast enough, if the job only comes with room and board. The down side is that you spend far too much time walking in the rain when everyone sane is warm inside. Time to head home for a mug of cocoa and a nice hot trollop.";
	case 4: return "There's a bit of skirt over there with a lovely figure, and had a face that was pretty, ninety years ago. Over yonder, a sweet young thing frolicking through the marketplace. She's being ever so daring, spending her daddy's gold, and hasn't yet realised that there's a dozen of her daddy's goons keeping a discreet eye on her. It's like that everywhere today. Maybe tomorrow will be better.";
	default:return "The city is quiet and no one shows up.";
	}
	// I don't think this should happen, hence the overly dramatic prose
	return "The sky is filled with thunder, and portals are opening all over Crossgate. You've seen five rains of frogs so far and three madmen speaking in tongues. In the marketplace a mechanical contraption materialised which walked like a man and declaimed alien poetry for half an hour before bursting into flames and collapsing. And the only thing all day that looked female and and attractive turned out to be some otherwordly species of carnivorous plant, and had to be destroyed by the town guard. The only good thing about this day is that it's over. It's time to go home.";
}

void cArenaScreen::do_walk()
{
	if (g_TryOuts)
	{
		g_MessageQue.AddToQue("You can only do this once per week.", COLOR_RED);
		return;
	}
	sGirl *girl = g_Girls.GetRandomGirl(false, false, true);	// let's get a girl for the player to meet was to get arena.. dont think this should happen this is tryouts arena girl should be ready to fight. CRAZY
	//sGirl *girl = g_Girls.GetRandomGirl();						// let's get a girl for the player to meet
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

	g_Building = BUILDING_ARENA;
	int v[2] = { 3, -1 };
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
		introfile = "MeetArenaTry.script.intro";
		intro = DirPath() << "Resources" << "Scripts";
		dp = DirPath() << "Resources" << "Scripts" << "MeetArenaTry.script";
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
		message = "You hold open try outs to all girls willing to step into the arena and fight for their life.";
	}
	else
	{
		message = ReadTextFile(intro, introfile);
	}
	if (message.size() > 0) g_MessageQue.AddToQue(message, COLOR_BLUE);

	eventrunning = true;
	sm.Load(dp, girl);
}