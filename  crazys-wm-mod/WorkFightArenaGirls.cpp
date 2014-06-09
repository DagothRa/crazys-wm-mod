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
#include "cJobManager.h"
#include "cBrothel.h"
#include "cArena.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cArenaManager g_Arena;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;
extern cGold g_Gold;

bool cJobManager::WorkFightArenaGirls(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	int type_arena_girls = 0;
	int type_unique_arena_girls = 0;
	int wages = 175;

	string message = "";
	if(Preprocessing(ACTION_COMBAT, girl, brothel, DayNight, summary, message))
		return true;
	stringstream ss;
	ss.str(message);

	// ready armor and weapons!
	g_Girls.EquipCombat(girl);

	sGirl* tempgirl = g_Girls.CreateRandomGirl(18, false, false, false, false, false, true);
	Uint8 fight_outcome = 0;
	if (tempgirl)		// `J` reworked incase there are no Non-Human Random Girls
	{
		fight_outcome = g_Girls.girl_fights_girl(girl, tempgirl);
	}
	else
	{
		g_LogFile.write("Error: You have no Arena Girls for your girls to fight\n");
		g_LogFile.write("Error: You need an Arena Girl to allow WorkFightArenaGirls randomness");
		fight_outcome = 7;
	}
	if (fight_outcome == 7)
	{
		message = "There were no Arena Girls for her to fight.\n\n";
		message += "(Error: You need an Arena Girl to allow WorkFightArenaGirls randomness)";
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, DayNight);
	}
	else if (fight_outcome == 1)	// she won
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +3, true);
		ss << gettext ("She won the fight.");
		girl->m_Events.AddMessage(ss.str(),IMGTYPE_COMBAT,DayNight);
		int roll_max = girl->fame() + girl->charisma();
		roll_max /= 4;
		wages += 10 + g_Dice%roll_max;
		girl->m_Pay = wages;
		g_Girls.UpdateStat(girl, STAT_FAME, 2);

		{
				sGirl* ugirl = 0;
				bool unique = false;
				if((g_Dice%100)+1 < 25)	// chance of getting unique girl
					unique = true;
				if(unique)  // Unique arena girl type
				{
					ugirl = g_Girls.GetRandomGirl(false, false, true);
					if(ugirl == 0)
						unique = false;
				}
				if(unique)
				{
					ugirl->m_States &= ~(1 << STATUS_ARENA);
					g_Brothels.GetDungeon()->AddGirl(ugirl, DUNGEON_NEWGIRL);
					type_unique_arena_girls++;
				}
			}
		}
	else if (fight_outcome == 2) // she lost or it was a draw
	{
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, -2, true);
		ss << gettext ("She lost the fight.");
		girl->m_Events.AddMessage(ss.str(),IMGTYPE_COMBAT,DayNight);
		g_Girls.UpdateStat(girl, STAT_FAME, -2);

	}
	else if (fight_outcome == 0)  // it was a draw
	{
		// hmm, guess we'll just ignore draws for now
	}

		 // Cleanup
	if(tempgirl)
		delete tempgirl;
	tempgirl = 0;


	// Improve girl
	int fightxp = 1;	if (fight_outcome == 1)	fightxp = 3;
	int xp = 5 * fightxp, libido = 5, skill = 1;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
	{
		skill += 1;
		xp += 5;
	}
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
	{
		skill -= 1;
		xp -= 5;
	}

	if (g_Girls.HasTrait(girl, "Nymphomaniac"))
		libido += 2;

	//TODO make this actually work so people know that they won a girl. crazy
	ss << girl->m_Realname << type_unique_arena_girls << (" put up a good fight so you let them live to come work for you\n\n");
		ss << ".";
	

	g_Girls.UpdateStat(girl, STAT_EXP, xp);
	g_Girls.UpdateSkill(girl, SKILL_COMBAT, g_Dice%fightxp + skill);
	g_Girls.UpdateSkill(girl, SKILL_MAGIC, g_Dice%fightxp + skill);
	g_Girls.UpdateStat(girl, STAT_AGILITY, g_Dice%fightxp + skill);
	g_Girls.UpdateStat(girl, STAT_CONSTITUTION, g_Dice%fightxp + skill);
	g_Girls.UpdateTempStat(girl, STAT_LIBIDO, libido);
	g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +8, true);

	
	//gain traits
	g_Girls.PossiblyGainNewTrait(girl, "Tough", 35, ACTION_COMBAT, "She has become pretty Tough from all of the fights she's been in.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Fleet of Foot", 55, ACTION_COMBAT, "She is getting rather fast from all the fighting.", DayNight != 0);
	g_Girls.PossiblyGainNewTrait(girl, "Aggressive", 70, ACTION_COMBAT, "She is getting rather Aggressive from her enjoyment of combat.", DayNight != 0);

	//lose traits
	g_Girls.PossiblyLoseExistingTrait(girl, "Fragile", 75, ACTION_COMBAT, girl->m_Realname + " has had to heal from so many injuries you can't say she is fragile anymore.", DayNight != 0);

	return false;
}