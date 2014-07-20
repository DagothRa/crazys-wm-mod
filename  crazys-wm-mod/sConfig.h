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
#pragma once

#include <iostream>
#include <string>
#include <vector>

class TiXmlElement;

using namespace std;

typedef unsigned int u_int;

struct SDL_Color;

struct sConfigData
{
/*
 *	initialisation
 */
 	struct {
		int	gold;
		int girl_meet;
		int slave_house_perc;
		bool auto_use_items;
		bool auto_combat_equip;
		int torture_mod;
	} initial;

/*
 *	resolution
 */
 	struct Resolution {
		string	resolution;
		int width;
		int height;
		bool fullscreen;
		bool configXML;
	} resolution;
/*
 *	income factors
 */

	struct InFactors {
		double extortion;
		double brothel_work;
		double street_work;
		double movie_income;
		double stripper_work;
		double barmaid_work;
		double slave_sales;
		double item_sales;
	} in_fact;
/*
 *	outgoings factors
 */
	struct OutFactors {
		double training;
		double actress_wages;
		double movie_cost;
		double goon_wages;
		double matron_wages;
		double staff_wages;		// `J` ?not used?
		double girl_support;
		double consumables;
		double item_cost;
		double slave_cost;
		double brothel_cost;
		double brothel_support;	// `J` ?not used?
		double bar_cost;		// `J` ?not used?
		double casino_cost;		// `J` ?not used?
		double bribes;
		double fines;
		double advertising;
	} out_fact;

	struct GambleData {
		int	odds;
		int	base;
		int	spread;
		double	house_factor;
		double	customer_factor;
	} gamble;

	struct TaxData {
		double	rate;
		double	minimum;
		double	laundry;
	} tax;

	struct PregnancyData {
		int	player_chance;
		int	customer_chance;
		int	monster_chance;
		double	good_sex_factor;
		int	chance_of_girl;
		int	weeks_pregnant;
		int	weeks_till_grown;
		int	cool_down;
	} pregnancy;

	struct GangData {
		int	max_recruit_list;
		int	start_random;
		int	start_boosted;
		int	init_member_min;
		int	init_member_max;
		int	chance_remove_unwanted;
		int	add_new_weekly_min;
		int	add_new_weekly_max;
	} gangs;

	struct ProstitutionData {
		double	rape_streets;
		double	rape_brothel;
	} prostitution;

	struct item_data {
		SDL_Color*	rarity_color[9];
	} items;

	struct font_data {
		string	normal;
		string	fixed;
		bool	antialias;
		font_data()
		: normal(""), fixed(""), antialias("")
		{
		}
	} fonts;

	struct DebugData {
		bool	log_items;
		bool	log_girls;
		bool	log_rgirls;
		bool	log_fonts;
		bool	log_torture;
	} debug;

	string override_dir;

	sConfigData(const char *filename = "config.xml");

	void	set_defaults();;
	void	get_income_factors(TiXmlElement *el);
	void	get_expense_factors(TiXmlElement *el);
	void	get_tax_factors(TiXmlElement *el);
	void	get_gambling_factors(TiXmlElement *el);
	void	get_preg_factors(TiXmlElement *el);
	void	get_gang_factors(TiXmlElement *el);
	void	get_pros_factors(TiXmlElement *el);
	void	get_item_data(TiXmlElement *el);
	void	get_font_data(TiXmlElement *el);
	void	get_initial_values(TiXmlElement *el);
	void	get_resolution_data(TiXmlElement *el);
	void	get_att(TiXmlElement *el, const char *name, int *data);
	void	get_att(TiXmlElement *el, const char *name, double *data);
	void	get_att(TiXmlElement *el, const char *name, string &s);
	void	get_att(TiXmlElement *el, const char *name, bool &bval);
	void	get_debug_flags(TiXmlElement *el);
};

class cConfig 
{
static	sConfigData	*data;
public:
	cConfig() {
		if(!data) {
			data = new sConfigData();
		}
	}
	void	reload(const char *filename) {
		sConfigData *newd = new sConfigData(filename);
		*data = *newd;
	}

	struct InFactors {
		double extortion()	    { return data->in_fact.extortion; }
		double brothel_work()	{ return data->in_fact.brothel_work; }
		double street_work()	{ return data->in_fact.street_work; }
		double movie_income()	{ return data->in_fact.movie_income; }
		double stripper_work()	{ return data->in_fact.stripper_work; }
		double barmaid_work()	{ return data->in_fact.barmaid_work; }
		double slave_sales()	{ return data->in_fact.slave_sales; }
		double item_sales()	    { return data->in_fact.item_sales; }
	} in_fact;
/*
 *	outgoings factors
 */
	struct OutFactors {
		double training()	    { return data->out_fact.training; }
		double actress_wages()	{ return data->out_fact.actress_wages; }
		double movie_cost()	    { return data->out_fact.movie_cost; }
		double goon_wages()	    { return data->out_fact.goon_wages; }
		double matron_wages()	{ return data->out_fact.matron_wages; }
		double staff_wages()	{ return data->out_fact.staff_wages; }
		double girl_support()	{ return data->out_fact.girl_support; }
		double consumables()	{ return data->out_fact.consumables; }
		double item_cost()	    { return data->out_fact.item_cost; }
		double slave_cost()	    { return data->out_fact.slave_cost; }
		double brothel_cost()	{ return data->out_fact.brothel_cost; }
		double brothel_support(){ return data->out_fact.brothel_support; }
		double bar_cost()	    { return data->out_fact.bar_cost; }
		double casino_cost()	{ return data->out_fact.casino_cost; }
		double bribes()		    { return data->out_fact.bribes; }
		double fines()		    { return data->out_fact.fines; }
		double advertising()	{ return data->out_fact.advertising; }
	} out_fact;

	struct ProstitutionData {
		double	rape_streets()  { return data->prostitution.rape_streets; }
		double	rape_brothel()  { return data->prostitution.rape_brothel; }
	} prostitution;

	struct font_data {
		string&	normal()	    { return data->fonts.normal; }
		string&	fixed()		    { return data->fonts.normal; }
		bool&	antialias()	    { return data->fonts.antialias; }
	} fonts;

	struct item_data {
		SDL_Color* rarity_color(int num)	{ return data->items.rarity_color[num]; }
	} items;

	struct TaxData {
		double	rate()		    { return data->tax.rate; }
		double	minimum()	    { return data->tax.minimum; }
		double	laundry()	    { return data->tax.laundry; }
	} tax;

	struct GambleData {
		int	odds()		            { return data->gamble.odds; }
		int	base()		            { return data->gamble.base; }
		int	spread()	            { return data->gamble.spread; }
		double	house_factor()	    { return data->gamble.house_factor; }
		double	customer_factor( )  { return data->gamble.customer_factor; }
	} gamble;

	struct PregnancyData {
		int	player_chance()	        { return data->pregnancy.player_chance; }
		int	customer_chance()       { return data->pregnancy.customer_chance; }
		int	monster_chance()        { return data->pregnancy.monster_chance; }
		double	good_sex_factor()   { return data->pregnancy.good_sex_factor; }
		int	chance_of_girl()        { return data->pregnancy.chance_of_girl; }
		int	weeks_pregnant()        { return data->pregnancy.weeks_pregnant; }
		int	weeks_till_grown()      { return data->pregnancy.weeks_till_grown; }
		int	cool_down()             { return data->pregnancy.cool_down; }
	} pregnancy;

	struct GangData {
		int	max_recruit_list()      { return data->gangs.max_recruit_list; }
		int	start_random()          { return data->gangs.start_random; }
		int	start_boosted()         { return data->gangs.start_boosted; }
		int	init_member_min()       { return data->gangs.init_member_min; }
		int	init_member_max()       { return data->gangs.init_member_max; }
		int	chance_remove_unwanted(){ return data->gangs.chance_remove_unwanted; }
		int	add_new_weekly_min()    { return data->gangs.add_new_weekly_min; }
		int	add_new_weekly_max()    { return data->gangs.add_new_weekly_max; }
	} gangs;

 	struct {
		int	gold()					{ return data->initial.gold; }
		int	girl_meet()				{ return data->initial.girl_meet; }
		int slave_house_perc()		{ return data->initial.slave_house_perc; }
		bool auto_use_items()		{ return data->initial.auto_use_items; }
		bool auto_combat_equip()	{ return data->initial.auto_combat_equip; }
		int	torture_mod()			{ return data->initial.torture_mod; }
	} initial;

	struct Resolution{
		string& resolution()	{ return data->resolution.resolution; }
		int width()				{ return data->resolution.width; }
		int height()			{ return data->resolution.height; }
		bool fullscreen()		{ return data->resolution.fullscreen; }
		bool configXML()		{ return data->resolution.configXML; }
	} resolution;

 	struct {
		bool	log_items()	        { return data->debug.log_items; }
		bool	log_rgirls()	    { return data->debug.log_rgirls; }
		bool	log_girls()     	{ return data->debug.log_girls; }
		bool	log_fonts()	        { return data->debug.log_fonts; }
		bool	log_torture()	    { return data->debug.log_torture; }
	} debug;

	string override_dir() {
		return data->override_dir;
	}

};


