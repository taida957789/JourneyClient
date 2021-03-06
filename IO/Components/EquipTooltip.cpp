/////////////////////////////////////////////////////////////////////////////
// This file is part of the Journey MMORPG client                           //
// Copyright � 2015 Daniel Allendorf                                               //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////
#include "EquipTooltip.h"

#include "..\..\Gameplay\Stage.h"

#include "nlnx\nx.hpp"
#include "nlnx\node.hpp"

namespace jrc
{
	EquipTooltip::EquipTooltip() 
	{
		nl::node itemtt = nl::nx::ui["UIToolTip.img"]["Item"];

		top = itemtt["Frame"]["top"];
		mid = itemtt["Frame"]["line"];
		line = itemtt["Frame"]["dotline"];
		bot = itemtt["Frame"]["bottom"];
		base = itemtt["ItemIcon"]["base"];
		cover = itemtt["ItemIcon"]["cover"];
		shade = itemtt["ItemIcon"]["shade"];

		potential[Equip::POT_NONE] = {};
		potential[Equip::POT_HIDDEN] = itemtt["ItemIcon"]["0"];
		potential[Equip::POT_RARE] = itemtt["ItemIcon"]["1"];
		potential[Equip::POT_EPIC] = itemtt["ItemIcon"]["2"];
		potential[Equip::POT_UNIQUE] = itemtt["ItemIcon"]["3"];
		potential[Equip::POT_LEGENDARY] = itemtt["ItemIcon"]["4"];

		requirements.push_back(Maplestat::LEVEL);
		requirements.push_back(Maplestat::STR);
		requirements.push_back(Maplestat::DEX);
		requirements.push_back(Maplestat::INT);
		requirements.push_back(Maplestat::LUK);

		reqstattextures[Maplestat::LEVEL][false] = itemtt["Equip"]["Cannot"]["reqLEV"];
		reqstattextures[Maplestat::LEVEL][true] = itemtt["Equip"]["Can"]["reqLEV"];
		reqstattextures[Maplestat::FAME][false] = itemtt["Equip"]["Cannot"]["reqPOP"];
		reqstattextures[Maplestat::FAME][true] = itemtt["Equip"]["Can"]["reqPOP"];
		reqstattextures[Maplestat::STR][false] = itemtt["Equip"]["Cannot"]["reqSTR"];
		reqstattextures[Maplestat::STR][true] = itemtt["Equip"]["Can"]["reqSTR"];
		reqstattextures[Maplestat::DEX][false] = itemtt["Equip"]["Cannot"]["reqDEX"];
		reqstattextures[Maplestat::DEX][true] = itemtt["Equip"]["Can"]["reqDEX"];
		reqstattextures[Maplestat::INT][false] = itemtt["Equip"]["Cannot"]["reqINT"];
		reqstattextures[Maplestat::INT][true] = itemtt["Equip"]["Can"]["reqINT"];
		reqstattextures[Maplestat::LUK][false] = itemtt["Equip"]["Cannot"]["reqLUK"];
		reqstattextures[Maplestat::LUK][true] = itemtt["Equip"]["Can"]["reqLUK"];

		reqstatpositions[Maplestat::LEVEL] = { 98, 48 };
		reqstatpositions[Maplestat::STR] = { 98, 64 };
		reqstatpositions[Maplestat::DEX] = { 98, 72 };
		reqstatpositions[Maplestat::INT] = { 173, 64 };
		reqstatpositions[Maplestat::LUK] = { 173, 72 };

		reqset[false] = { itemtt["Equip"]["Cannot"], Charset::LEFT };
		reqset[true] = { itemtt["Equip"]["Can"], Charset::LEFT };

		jobsback = itemtt["Equip"]["Job"]["normal"];
		jobs[false][0] = itemtt["Equip"]["Job"]["disable"]["0"];
		jobs[false][1] = itemtt["Equip"]["Job"]["disable"]["1"];
		jobs[false][2] = itemtt["Equip"]["Job"]["disable"]["2"];
		jobs[false][3] = itemtt["Equip"]["Job"]["disable"]["3"];
		jobs[false][4] = itemtt["Equip"]["Job"]["disable"]["4"];
		jobs[false][5] = itemtt["Equip"]["Job"]["disable"]["5"];
		jobs[true][0] = itemtt["Equip"]["Job"]["enable"]["0"];
		jobs[true][1] = itemtt["Equip"]["Job"]["enable"]["1"];
		jobs[true][2] = itemtt["Equip"]["Job"]["enable"]["2"];
		jobs[true][3] = itemtt["Equip"]["Job"]["enable"]["3"];
		jobs[true][4] = itemtt["Equip"]["Job"]["enable"]["4"];
		jobs[true][5] = itemtt["Equip"]["Job"]["enable"]["5"];

		invpos = 0;
	}

	void EquipTooltip::setequip(const Equip* equip, int16_t ivp)
	{
		if (ivp == invpos)
			return;

		invpos = ivp;

		if (equip == nullptr)
			return;

		const Clothing& cloth = equip->getcloth();
		const CharStats& stats = Stage::get().getplayer().getstats();
		
		filllength = 500;

		itemicon = cloth.geticon(true);

		for (auto& ms : requirements)
		{
			canequip[ms] = stats.getstat(ms) >= cloth.getreqstat(ms);
			std::string reqstr = std::to_string(cloth.getreqstat(ms));
			reqstr.insert(0, 3 - reqstr.size(), '0');
			reqstatstrings[ms] = reqstr;
		}

		okjobs.clear();
		switch (cloth.getreqstat(Maplestat::JOB))
		{
		case 0:
			okjobs.push_back(0);
			okjobs.push_back(1);
			okjobs.push_back(2);
			okjobs.push_back(3);
			okjobs.push_back(4);
			okjobs.push_back(5);
			canequip[Maplestat::JOB] = true;
			break;
		case 1:
			okjobs.push_back(1);
			canequip[Maplestat::JOB] = (stats.getstat(Maplestat::JOB) / 100 == 1) || (stats.getstat(Maplestat::JOB) / 100 >= 20);
			break;
		case 2:
			okjobs.push_back(2);
			canequip[Maplestat::JOB] = stats.getstat(Maplestat::JOB) / 100 == 2;
			break;
		case 4:
			okjobs.push_back(3);
			canequip[Maplestat::JOB] = stats.getstat(Maplestat::JOB) / 100 == 3;
			break;
		case 8:
			okjobs.push_back(4);
			canequip[Maplestat::JOB] = stats.getstat(Maplestat::JOB) / 100 == 4;
			break;
		case 16:
			okjobs.push_back(5);
			canequip[Maplestat::JOB] = stats.getstat(Maplestat::JOB) / 100 == 5;
			break;
		default:
			canequip[Maplestat::JOB] = false;
		}

		prank = equip->getpotrank();
		switch (prank)
		{
		case Equip::POT_HIDDEN:
			potflag = Text(Text::A11M, Text::CENTER, Text::RED);
			potflag.settext("(Hidden Potential)");
			break;
		case Equip::POT_RARE:
			potflag = Text(Text::A11M, Text::CENTER, Text::WHITE);
			potflag.settext("(Rare Item)");
			break;
		case Equip::POT_EPIC:
			potflag = Text(Text::A11M, Text::CENTER, Text::WHITE);
			potflag.settext("(Epic Item)");
			break;
		case Equip::POT_UNIQUE:
			potflag = Text(Text::A11M, Text::CENTER, Text::WHITE);
			potflag.settext("(Unique Item)");
			break;
		case Equip::POT_LEGENDARY:
			potflag = Text(Text::A11M, Text::CENTER, Text::WHITE);
			potflag.settext("(Legendary Item)");
			break;
		default:
			filllength -= 16;
		}

		Text::Color namecolor;
		switch (equip->getquality())
		{
		case Equip::EQQ_GREY:
			namecolor = Text::LIGHTGREY;
			break;
		case Equip::EQQ_ORANGE:
			namecolor = Text::ORANGE;
			break;
		case Equip::EQQ_BLUE:
			namecolor = Text::MEDIUMBLUE;
			break;
		case Equip::EQQ_VIOLET:
			namecolor = Text::VIOLET;
			break;
		case Equip::EQQ_GOLD:
			namecolor = Text::YELLOW;
			break;
		default:
			namecolor = Text::WHITE;
		}

		std::string namestr = cloth.getname();
		if (equip->getlevel() > 0)
		{
			namestr.append(" (+");
			namestr.append(std::to_string(equip->getlevel()));
			namestr.append(")");
		}
		name = { Text::A12B, Text::CENTER, namecolor };
		name.settext(namestr, 400);

		std::string desctext = cloth.getdesc();
		hasdesc = desctext.size() > 0;
		if (hasdesc)
		{
			desc = Itemtext(desctext, 250);
			filllength += desc.getheight() + 10;
		}

		category = { Text::A11L, Text::LEFT, Text::WHITE, "CATEGORY: " + cloth.gettype() };

		isweapon = cloth.isweapon();
		if (isweapon)
		{
			const Weapon& weapon = reinterpret_cast<const Weapon&>(cloth);
			wepspeed = { Text::A11L, Text::LEFT, Text::WHITE, "ATTACK SPEED: " + weapon.getspeedstring() };
		}
		else
		{
			filllength -= 18;
		}

		hasslots = (equip->getslots() > 0) || (equip->getlevel() > 0);
		if (hasslots)
		{
			slots = { Text::A11L, Text::LEFT, Text::WHITE, "UPGRADES AVAILABLE: " + std::to_string(equip->getslots()) };

			std::string vicious = std::to_string(equip->getvicious());
			if (equip->getvicious() > 1)
				vicious.append(" (MAX) ");
			hammers = { Text::A11L, Text::LEFT, Text::WHITE, "VICIOUS HAMMERS USED: " + vicious };
		}
		else
		{
			filllength -= 36;
		}

		statlabels.clear();
		for (Equipstat::Value es = Equipstat::STR; es <= Equipstat::JUMP; es = static_cast<Equipstat::Value>(es + 1))
		{
			if (equip->getstat(es) > 0)
			{
				int16_t delta = equip->getstat(es) - cloth.getdefstat(es);
				std::string statstr = std::to_string(equip->getstat(es));
				if (delta != 0)
				{
					statstr.append(" (");
					statstr.append((delta < 0) ? "-" : "+");
					statstr.append(std::to_string(abs(delta)) + ")");
				}
				statlabels[es] = { Text::A11L, Text::LEFT, Text::WHITE, Equipstat::names[es] + std::string(": ") + statstr };
			}
			else
			{
				filllength -= 18;
			}
		}
	}

	void EquipTooltip::draw(Point<int16_t> pos) const
	{
		if (invpos == 0)
			return;

		top.draw({ pos });
		mid.draw({ pos + Point<int16_t>(0, 13), Point<int16_t>(0, filllength) });
		bot.draw({ pos + Point<int16_t>(0, filllength + 13) });

		name.draw(pos + Point<int16_t>(130, 3));
		if (prank != Equip::POT_NONE)
		{
			potflag.draw(pos + Point<int16_t>(130, 20));
			pos.shifty(16);
		}
		pos.shifty(26);

		line.draw({ pos });

		base.draw(pos + Point<int16_t>(10, 10));
		shade.draw(pos + Point<int16_t>(10, 10));
		itemicon.draw({ pos + Point<int16_t>(20, 82), 2.0f, 2.0f });
		potential.at(prank).draw(pos + Point<int16_t>(10, 10));
		cover.draw(pos + Point<int16_t>(10, 10));

		pos.shifty(12);

		for (auto& ms : requirements)
		{
			Point<int16_t> reqpos = reqstatpositions.at(ms);
			bool reqok = canequip.at(ms);
			reqstattextures.at(ms).at(reqok).draw({ pos + reqpos });
			reqset.at(reqok).draw(reqstatstrings.at(ms), 6, { pos + reqpos + Point<int16_t>(54, 0) });
		}

		pos.shifty(88);

		DrawArgument jobargs = { pos + Point<int16_t>(8, 0) };
		jobsback.draw(jobargs);
		for (auto& jbit : okjobs)
		{
			jobs.at(canequip.at(Maplestat::JOB)).at(jbit).draw(jobargs);
		}

		line.draw({ pos + Point<int16_t>(0, 30) });

		pos.shifty(32);

		category.draw(pos + Point<int16_t>(10, 0));

		pos.shifty(18);

		if (isweapon)
		{
			wepspeed.draw(pos + Point<int16_t>(10, 0));
			pos.shifty(18);
		}

		for (auto& stit : statlabels)
		{
			stit.second.draw(pos + Point<int16_t>(10, 0));
			pos.shifty(18);
		}

		if (hasslots)
		{
			slots.draw(pos + Point<int16_t>(10, 0));
			pos.shifty(18);
			hammers.draw(pos + Point<int16_t>(10, 0));
			pos.shifty(18);
		}

		if (hasdesc)
		{
			line.draw({ pos + Point<int16_t>(0, 5) });
			desc.draw({ pos + Point<int16_t>(10, 6) });
		}
	}
}