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
#include "UIStatusbar.h"
#include "UIStatsinfo.h"
#include "UIItemInventory.h"
#include "UIEquipInventory.h"

#include "..\UI.h"
#include "..\Components\MapleButton.h"

#include "..\..\Gameplay\Stage.h"

#include "nlnx\nx.hpp"

namespace jrc
{
	UIStatusbar::UIStatusbar() 
		: stats(Stage::get().getplayer().getstats()), chatbar(POSITION) {

		nl::node mainbar = nl::nx::ui["StatusBar2.img"]["mainBar"];

		sprites.emplace_back(mainbar["backgrnd"]);
		sprites.emplace_back(mainbar["gaugeBackgrd"]);
		sprites.emplace_back(mainbar["notice"]);
		sprites.emplace_back(mainbar["lvBacktrnd"]);
		sprites.emplace_back(mainbar["lvCover"]);

		expbar = { 
			mainbar.resolve("gauge/exp/0"), 
			mainbar.resolve("gauge/exp/1"),
			mainbar.resolve("gauge/exp/2"), 
			308, getexppercent() 
		};
		hpbar = {
			mainbar.resolve("gauge/hp/0"),
			mainbar.resolve("gauge/hp/1"),
			mainbar.resolve("gauge/hp/2"),
			137, gethppercent()
		};
		mpbar = {
			mainbar.resolve("gauge/mp/0"),
			mainbar.resolve("gauge/mp/1"),
			mainbar.resolve("gauge/mp/2"),
			137, getmppercent()
		};

		statset = { mainbar["gauge"]["number"], Charset::RIGHT };
		levelset = { mainbar["lvNumber"], Charset::LEFT };

		joblabel = { Text::A11M, Text::LEFT, Text::YELLOW, stats.getjobname() };
		namelabel = { Text::A13M, Text::LEFT, Text::WHITE, stats.getname() };

		buttons[BT_WHISPER] = std::make_unique<MapleButton>(mainbar["BtChat"]);
		buttons[BT_CALLGM] = std::make_unique<MapleButton>(mainbar["BtClaim"]);

		buttons[BT_CASHSHOP] = std::make_unique<MapleButton>(mainbar["BtCashShop"]);
		buttons[BT_TRADE] = std::make_unique<MapleButton>(mainbar["BtMTS"]);
		buttons[BT_MENU] = std::make_unique<MapleButton>(mainbar["BtMenu"]);
		buttons[BT_OPTIONS] = std::make_unique<MapleButton>(mainbar["BtSystem"]);

		buttons[BT_CHARACTER] = std::make_unique<MapleButton>(mainbar["BtCharacter"]);
		buttons[BT_STATS] = std::make_unique<MapleButton>(mainbar["BtStat"]);
		buttons[BT_QUEST] = std::make_unique<MapleButton>(mainbar["BtQuest"]);
		buttons[BT_INVENTORY] = std::make_unique<MapleButton>(mainbar["BtInven"]);
		buttons[BT_EQUIPS] = std::make_unique<MapleButton>(mainbar["BtEquip"]);
		buttons[BT_SKILL] = std::make_unique<MapleButton>(mainbar["BtSkill"]);

		position = POSITION;
		dimension = DIMENSION;
	}

	void UIStatusbar::draw(float inter) const
	{
		UIElement::draw(inter);

		int64_t currentexp = stats.getexp();
		uint16_t currenthp = stats.getstat(Maplestat::HP);
		uint16_t currentmp = stats.getstat(Maplestat::MP);
		uint32_t maxhp = stats.gettotal(Equipstat::HP);
		uint32_t maxmp = stats.gettotal(Equipstat::MP);

		expbar.draw(position + Point<int16_t>(-261, -15));
		hpbar.draw(position + Point<int16_t>(-261, -31));
		mpbar.draw(position + Point<int16_t>(-90, -31));

		std::string expstring = std::to_string(100 * getexppercent());
		statset.draw(
			std::to_string(currentexp) + "[" + expstring.substr(0, expstring.find('.') + 3) + "%]",
			position + Point<int16_t>(47, -13)
			);
		statset.draw(
			"[" + std::to_string(currenthp) + "/" + std::to_string(maxhp) + "]",
			position + Point<int16_t>(-124, -29)
			);
		statset.draw(
			"[" + std::to_string(currentmp) + "/" + std::to_string(maxmp) + "]",
			position + Point<int16_t>(47, -29)
			);
		levelset.draw(
			std::to_string(stats.getstat(Maplestat::LEVEL)),
			position + Point<int16_t>(-480, -24)
			);

		joblabel.draw(position + Point<int16_t>(-435, -21));
		namelabel.draw(position + Point<int16_t>(-435, -36));

		chatbar.draw(inter);
	}

	void UIStatusbar::update()
	{
		UIElement::update();

		chatbar.update();

		expbar.update(getexppercent());
		hpbar.update(gethppercent());
		mpbar.update(getmppercent());

		namelabel.settext(stats.getname());
		joblabel.settext(stats.getjobname());

		for (auto iter : message_cooldowns)
		{
			iter.second -= Constants::TIMESTEP;
		}
	}

	void UIStatusbar::buttonpressed(uint16_t id)
	{
		switch (id)
		{
		/*case BT_BAR_SYSOP:
			uinterface.add(UI_SYSTEM);
			break;*/
		case BT_STATS:
			UI::get().add(ElementTag<UIStatsinfo>());
			break;
		case BT_INVENTORY:
			UI::get().add(ElementTag<UIItemInventory>());
			break;
		case BT_EQUIPS:
			UI::get().add(ElementTag<UIEquipInventory>());
			break;
		}

		buttons[id].get()->setstate(Button::MOUSEOVER);
	}

	bool UIStatusbar::isinrange(Point<int16_t> cursorpos) const
	{
		Rectangle<int16_t> bounds(
			position - Point<int16_t>(512, 84),
			position - Point<int16_t>(512, 84) + dimension
			);
		return bounds.contains(cursorpos) || chatbar.isinrange(cursorpos);
	}

	Cursor::State UIStatusbar::sendmouse(bool pressed, Point<int16_t> cursorpos)
	{
		if (chatbar.isinrange(cursorpos))
		{
			UIElement::sendmouse(pressed, cursorpos);
			return chatbar.sendmouse(pressed, cursorpos);
		}
		else
		{
			chatbar.sendmouse(pressed, cursorpos);
			return UIElement::sendmouse(pressed, cursorpos);
		}
	}

	void UIStatusbar::sendchatline(const std::string& line, Chatbar::LineType type)
	{
		chatbar.sendline(line, type);
	}

	void UIStatusbar::displaymessage(Messages::Type line, Chatbar::LineType type)
	{
		if (message_cooldowns[line] > 0)
			return;

		std::string message = messages.stringfor(line);
	    chatbar.sendline(message, type);

		message_cooldowns[line] = MESSAGE_COOLDOWN;
	}

	float UIStatusbar::getexppercent() const
	{
		int64_t currentexp = stats.getexp();
		int64_t expneeded = stats.getexpneeded();

		return static_cast<float>(
			static_cast<double>(currentexp) / expneeded
			);
	}

	float UIStatusbar::gethppercent() const
	{
		uint16_t currenthp = stats.getstat(Maplestat::HP);
		uint32_t maxhp = stats.gettotal(Equipstat::HP);

		return static_cast<float>(currenthp) / maxhp;
	}

	float UIStatusbar::getmppercent() const
	{
		uint16_t currentmp = stats.getstat(Maplestat::MP);
		uint32_t maxmp = stats.gettotal(Equipstat::MP);

		return static_cast<float>(currentmp) / maxmp;
	}
}