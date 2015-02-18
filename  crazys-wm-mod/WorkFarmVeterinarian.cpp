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
#include "cRng.h"
#include "CLog.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "cBrothel.h"
#include "cFarm.h"

extern CLog g_LogFile;
extern cMessageQue g_MessageQue;
extern cRng g_Dice;
extern cGold g_Gold;
extern cBrothelManager g_Brothels;
extern cFarmManager g_Farm;

// `J` Job Farm - Staff
bool cJobManager::WorkFarmVeterinarian(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary)
{
	int actiontype = ACTION_WORKFARM;
	stringstream ss; string girlName = girl->m_Realname; ss << girlName;

	if (g_Girls.DisobeyCheck(girl, ACTION_WORKFARM, brothel))			// they refuse to work 
	{
		ss << " refused to work during the " << (Day0Night1 ? "night" : "day") << " shift.";
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
		return true;
	}
	ss << " worked as a Veterinarian on the farm.";

	g_Girls.UnequipCombat(girl);	// put that shit away, you'll scare off the customers!

	int fame = 0;
	int wages = 25, work = 0;
	int roll = g_Dice.d100();

	double jobperformance = JP_FarmVeterinarian(girl, false);


	if (jobperformance >= 245)
	{
		wages += 155;	fame += 2;
		ss << " She must be the perfect at this.\n\n";
	}
	else if (jobperformance >= 185)
	{
		wages += 95;	fame += 1;
		ss << " She's unbelievable at this.\n\n";
	}
	else if (jobperformance >= 145)
	{
		wages += 55;
		ss << " She's good at this job.\n\n";
	}
	else if (jobperformance >= 100)
	{
		wages += 15;
		ss << " She made a few mistakes but overall she is okay at this.\n\n";
	}
	else if (jobperformance >= 70)
	{
		ss << " She was nervous and made a few mistakes. She isn't that good at this.\n\n";
		wages -= 5;
	}
	else
	{
		ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n\n";
		wages -= 15;
	}



	//enjoyed the work or not
	if (roll <= 5)
	{
		ss << "\nSome of the patrons abused her during the shift."; work -= 1;
	}
	else if (roll <= 25)
	{
		ss << "\nShe had a pleasant time working."; work += 3;
	}
	else
	{
		ss << "\nOtherwise, the shift passed uneventfully."; work += 1;
	}


#if 0

	// `J` Farm Bookmark - adding in items that can be created in the farm















#endif


	g_Girls.UpdateEnjoyment(girl, ACTION_WORKFARM, work, true);
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1);
	girl->m_Pay = max(wages, 0);

	// Improve stats
	int xp = 10, libido = 1, skill = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))		{ skill += 1; xp += 3; }
	else if (g_Girls.HasTrait(girl, "Slow Learner"))	{ skill -= 1; xp -= 3; }
	if (g_Girls.HasTrait(girl, "Nymphomaniac"))			{ libido += 2; }

	g_Girls.UpdateStat(girl, STAT_FAME, fame);
	
	g_Girls.UpdateStat(girl, STAT_EXP, (g_Dice % xp) + 1);
	g_Girls.UpdateStatTemp(girl, STAT_LIBIDO, libido);

	// primary (+2 for single or +1 for multiple)
	g_Girls.UpdateStat(girl, SKILL_MEDICINE, (g_Dice % skill) + 1);
	g_Girls.UpdateStat(girl, SKILL_ANIMALHANDLING, (g_Dice % skill) + 1);
	// secondary (-1 for one then -2 for others)
	g_Girls.UpdateStat(girl, STAT_INTELLIGENCE, max(0, (g_Dice % skill) - 1));
	g_Girls.UpdateStat(girl, STAT_CHARISMA, max(0, (g_Dice % skill) - 2));
	g_Girls.UpdateSkill(girl, SKILL_BEASTIALITY, max(0, (g_Dice % skill) - 2));

	return false;
}

double cJobManager::JP_FarmVeterinarian(sGirl* girl, bool estimate)// not used
{
	double jobperformance =
		// primary - first 100
		((girl->medicine() + girl->animalhandling()) / 2) +
		// secondary - second 100
		((girl->intelligence() + girl->charisma() + girl->beastiality()) / 3) +
		// level bonus
		girl->level();

	//good traits
	if (g_Girls.HasTrait(girl, "Quick Learner"))	jobperformance += 5;
	if (g_Girls.HasTrait(girl, "Psychic"))			jobperformance += 10;
	if (g_Girls.HasTrait(girl, "Optimist"))			jobperformance += 10;	//less willing to give up on the animal
	//bad traits
	if (g_Girls.HasTrait(girl, "Dependant"))		jobperformance -= 50;	//needs others to do the job
	if (g_Girls.HasTrait(girl, "Clumsy"))  			jobperformance -= 20;	//spills food and breaks things often
	if (g_Girls.HasTrait(girl, "Aggressive"))  		jobperformance -= 20;	//gets mad easy
	if (g_Girls.HasTrait(girl, "Nervous"))  		jobperformance -= 30;	//don't like to be around people
	if (g_Girls.HasTrait(girl, "Meek"))				jobperformance -= 20;
	if (g_Girls.HasTrait(girl, "Pessimist"))		jobperformance -= 10;	//more likely to give up on the animal

	return jobperformance;
}