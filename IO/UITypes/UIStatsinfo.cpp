/////////////////////////////////////////////////////////////////////////////
// This file is part of the Journey MMORPG client                           //
// Copyright � 2015 Daniel Allendorf                                        //
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
#include "UIStatsinfo.h"

#include "..\UI.h"
#include "..\Components\MapleButton.h"

#include "..\..\Gameplay\Stage.h"
#include "..\..\Net\Packets\PlayerPackets.h"

#include "nlnx\nx.hpp"

namespace jrc
{
	UIStatsinfo::UIStatsinfo() :
		UIDragElement<PosSTATS>(Point<int16_t>(212, 20)), stats(Stage::get().getplayer().getstats()) {

		nl::node src = nl::nx::ui["UIWindow4.img"]["Stat"]["main"];
		nl::node detail = nl::nx::ui["UIWindow4.img"]["Stat"]["detail"];

		sprites.emplace_back(src["backgrnd"]);
		sprites.emplace_back(src["backgrnd2"]);
		sprites.emplace_back(src["backgrnd3"]);

		detailtextures.emplace_back(detail["backgrnd"]);
		detailtextures.emplace_back(detail["backgrnd2"]);
		detailtextures.emplace_back(detail["backgrnd3"]);

		abilities["rare"] = detail["abilityTitle"]["rare"]["0"];
		abilities["epic"] = detail["abilityTitle"]["epic"]["0"];
		abilities["unique"] = detail["abilityTitle"]["unique"]["0"];
		abilities["legendary"] = detail["abilityTitle"]["legendary"]["0"];
		abilities["none"] = detail["abilityTitle"]["normal"]["0"];

		buttons[BT_HP] = std::make_unique<MapleButton>(src["BtHpUp"]);
		buttons[BT_MP] = std::make_unique<MapleButton>(src["BtMpUp"]);
		buttons[BT_STR] = std::make_unique<MapleButton>(src["BtStrUp"]);
		buttons[BT_DEX] = std::make_unique<MapleButton>(src["BtDexUp"]);
		buttons[BT_LUK] = std::make_unique<MapleButton>(src["BtLukUp"]);
		buttons[BT_INT] = std::make_unique<MapleButton>(src["BtIntUp"]);

		buttons[BT_DETAILOPEN] = std::make_unique<MapleButton>(src["BtDetailOpen"]);
		buttons[BT_DETAILCLOSE] = std::make_unique<MapleButton>(src["BtDetailClose"]);
		buttons[BT_DETAILCLOSE]->setactive(false);

		updateap();

		for (size_t i = 0; i < NUMLABELS; i++)
		{
			statlabels[i] = Text(Text::A11M, Text::LEFT, Text::LIGHTGREY);
		}
		statoffsets[NAME] = Point<int16_t>(73, 27);
		statoffsets[JOB] = Point<int16_t>(73, 45);
		statoffsets[GUILD] = Point<int16_t>(73, 63);
		statoffsets[FAME] = Point<int16_t>(73, 81);
		statoffsets[DAMAGE] = Point<int16_t>(73, 99);
		statoffsets[HP] = Point<int16_t>(73, 117);
		statoffsets[MP] = Point<int16_t>(73, 135);
		statoffsets[AP] = Point<int16_t>(70, 177);
		statoffsets[STR] = Point<int16_t>(73, 204);
		statoffsets[DEX] = Point<int16_t>(73, 222);
		statoffsets[INT] = Point<int16_t>(73, 240);
		statoffsets[LUK] = Point<int16_t>(73, 258);
		statoffsets[ATTACK] = Point<int16_t>(73, 37);
		statoffsets[CRIT] = Point<int16_t>(73, 55);
		statoffsets[MINCRIT] = Point<int16_t>(73, 73);
		statoffsets[MAXCRIT] = Point<int16_t>(168, 73);
		statoffsets[BDM] = Point<int16_t>(73, 91);
		statoffsets[IGNOREDEF] = Point<int16_t>(168, 91);
		statoffsets[RESIST] = Point<int16_t>(73, 109);
		statoffsets[STANCE] = Point<int16_t>(168, 109);
		statoffsets[WDEF] = Point<int16_t>(73, 127);
		statoffsets[MDEF] = Point<int16_t>(73, 145);
		statoffsets[ACCURACY] = Point<int16_t>(73, 163);
		statoffsets[AVOID] = Point<int16_t>(73, 199);
		statoffsets[SPEED] = Point<int16_t>(73, 235);
		statoffsets[JUMP] = Point<int16_t>(168, 235);
		statoffsets[HONOR] = Point<int16_t>(73, 353);

		updateall();
		updatestat(Maplestat::JOB);
		updatestat(Maplestat::FAME);

		dimension = Point<int16_t>(212, 318);
		showdetail = false;
	}

	void UIStatsinfo::draw(float inter) const
	{
		UIElement::draw(inter);

		if (showdetail)
		{
			for (auto& dtit : detailtextures)
			{
				dtit.draw(position + Point<int16_t>(213, 0));
			}
			abilities.at("none").draw(position + Point<int16_t>(213, 0));
		}

		size_t last = showdetail ? NUMLABELS : NUMNORMAL;
		for (size_t i = 0; i < last; i++)
		{
			Point<int16_t> labelpos = position + statoffsets[i];
			if (i >= NUMNORMAL)
				labelpos.shiftx(213);

			statlabels[i].draw(labelpos);
		}
	}

	void UIStatsinfo::updateall()
	{
		updatesimple(AP, Maplestat::AP);
		if (hasap ^ (stats.getstat(Maplestat::AP) > 0))
		{
			updateap();
		}

		statlabels[NAME].settext(stats.getname());
		statlabels[GUILD].settext("");

		statlabels[HP].settext(std::to_string(stats.getstat(Maplestat::HP)) + " / " + std::to_string(stats.gettotal(Equipstat::HP)));
		statlabels[MP].settext(std::to_string(stats.getstat(Maplestat::MP)) + " / " + std::to_string(stats.gettotal(Equipstat::MP)));

		updatebasevstotal(STR, Maplestat::STR, Equipstat::STR);
		updatebasevstotal(DEX, Maplestat::DEX, Equipstat::DEX);
		updatebasevstotal(INT, Maplestat::INT, Equipstat::INT);
		updatebasevstotal(LUK, Maplestat::LUK, Equipstat::LUK);

		statlabels[DAMAGE].settext(std::to_string(stats.getmindamage()) + " ~ " + std::to_string(stats.getmaxdamage()));
		if (stats.isdamagebuffed())
		{
			statlabels[DAMAGE].setcolor(Text::RED);
		}
		else
		{
			statlabels[DAMAGE].setcolor(Text::LIGHTGREY);
		}

		updatebuffed(ATTACK, Equipstat::WATK);
		updatebuffed(WDEF, Equipstat::WDEF);
		updatebuffed(MDEF, Equipstat::MDEF);
		updatebuffed(ACCURACY, Equipstat::ACC);
		updatebuffed(AVOID, Equipstat::AVOID);

		statlabels[CRIT].settext(std::to_string(static_cast<int32_t>(stats.getcritical() * 100)) + "%");
		statlabels[MINCRIT].settext(std::to_string(static_cast<int32_t>(stats.getmincrit() * 100)) + "%");
		statlabels[MAXCRIT].settext(std::to_string(static_cast<int32_t>(stats.getmaxcrit() * 100)) + "%");
		statlabels[BDM].settext(std::to_string(static_cast<int32_t>(stats.getbossdmg() * 100)) + "%");
		statlabels[IGNOREDEF].settext(std::to_string(static_cast<int32_t>(stats.getignoredef() * 100)) + "%");
		statlabels[RESIST].settext(std::to_string(static_cast<int32_t>(stats.getresist() * 100)) + "%");
		statlabels[STANCE].settext(std::to_string(static_cast<int32_t>(stats.getstance() * 100)) + "%");
		statlabels[SPEED].settext(std::to_string(stats.gettotal(Equipstat::SPEED)) + "%");
		statlabels[JUMP].settext(std::to_string(stats.gettotal(Equipstat::JUMP)) + "%");
		statlabels[HONOR].settext(std::to_string(stats.gethonor()));
	}

	void UIStatsinfo::updatestat(Maplestat::Value stat)
	{
		switch (stat)
		{
		case Maplestat::JOB:
			statlabels[JOB].settext(stats.getjobname());
			break;
		case Maplestat::FAME:
			updatesimple(FAME, Maplestat::FAME);
			break;
		}
	}

	void UIStatsinfo::buttonpressed(uint16_t id)
	{
		switch (id)
		{
		case BT_DETAILOPEN:
			showdetail = true;
			buttons[BT_DETAILOPEN]->setactive(false);
			buttons[BT_DETAILCLOSE]->setactive(true);
			break;
		case BT_DETAILCLOSE:
			showdetail = false;
			buttons[BT_DETAILCLOSE]->setactive(false);
			buttons[BT_DETAILOPEN]->setactive(true);
			break;
		case BT_HP:
			sendappacket(Maplestat::HP);
			break;
		case BT_MP:
			sendappacket(Maplestat::MP);
			break;
		case BT_STR:
			sendappacket(Maplestat::STR);
			break;
		case BT_DEX:
			sendappacket(Maplestat::DEX);
			break;
		case BT_INT:
			sendappacket(Maplestat::INT);
			break;
		case BT_LUK:
			sendappacket(Maplestat::LUK);
			break;
		}

		buttons[id]->setstate(Button::NORMAL);
	}

	void UIStatsinfo::sendappacket(Maplestat::Value stat) const
	{
		SpendApPacket(stat).dispatch();
		UI::get().disable();
	}

	void UIStatsinfo::updateap()
	{
		Button::State newstate;
		bool nowap = stats.getstat(Maplestat::AP) > 0;
		if (nowap)
		{
			newstate = Button::NORMAL;

			buttons[BT_HP]->setposition(Point<int16_t>(20, -36));
			buttons[BT_MP]->setposition(Point<int16_t>(20, -18));
			buttons[BT_STR]->setposition(Point<int16_t>(20, 51));
			buttons[BT_DEX]->setposition(Point<int16_t>(20, 69));
			buttons[BT_INT]->setposition(Point<int16_t>(20, 87));
			buttons[BT_LUK]->setposition(Point<int16_t>(20, 105));
		}
		else
		{
			newstate = Button::DISABLED;

			buttons[BT_HP]->setposition(Point<int16_t>(-48, 14));
			buttons[BT_MP]->setposition(Point<int16_t>(-48, 32));
			buttons[BT_STR]->setposition(Point<int16_t>(-48, 101));
			buttons[BT_DEX]->setposition(Point<int16_t>(-48, 119));
			buttons[BT_INT]->setposition(Point<int16_t>(-48, 137));
			buttons[BT_LUK]->setposition(Point<int16_t>(-48, 155));
		}
		buttons[BT_HP]->setstate(newstate);
		buttons[BT_MP]->setstate(newstate);
		buttons[BT_STR]->setstate(newstate);
		buttons[BT_DEX]->setstate(newstate);
		buttons[BT_LUK]->setstate(newstate);
		buttons[BT_INT]->setstate(newstate);

		hasap = nowap;
	}

	void UIStatsinfo::updatesimple(StatLabel label, Maplestat::Value stat)
	{
		statlabels[label].settext(std::to_string(stats.getstat(stat)));
	}

	void UIStatsinfo::updatebasevstotal(StatLabel label, Maplestat::Value bstat, Equipstat::Value tstat)
	{
		int32_t base = stats.getstat(bstat);
		int32_t total = stats.gettotal(tstat);
		int32_t delta = total - base;

		std::string stattext = std::to_string(total);
		if (delta)
		{
			stattext += " (" + std::to_string(base);
			if (delta > 0)
			{
				stattext += " + " + std::to_string(delta);
			}
			else if (delta < 0)
			{
				stattext += " - " + std::to_string(-delta);
			}
			stattext += ")";
		}
		statlabels[label].settext(stattext);
	}

	void UIStatsinfo::updatebuffed(StatLabel label, Equipstat::Value stat)
	{
		int32_t total = stats.gettotal(stat);
		int32_t delta = stats.getbuffdelta(stat);

		std::string stattext = std::to_string(total);
		if (delta)
		{
			stattext += " (" + std::to_string(total - delta);
			if (delta > 0)
			{
				stattext += " + " + std::to_string(delta);

				statlabels[label].setcolor(Text::RED);
			}
			else if (delta < 0)
			{
				stattext += " - " + std::to_string(-delta);

				statlabels[label].setcolor(Text::BLUE);
			}
			stattext += ")";
		}
		statlabels[label].settext(stattext);
	}
}