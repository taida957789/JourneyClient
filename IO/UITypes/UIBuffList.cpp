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
#include "UIBuffList.h"

#include "..\..\Data\DataFactory.h"
#include "..\..\Util\Misc.h"

#include "nlnx\nx.hpp"

namespace jrc
{
	BuffIcon::BuffIcon(int32_t buff, int32_t dur)
		: cover(IconCover::BUFF, dur - FLASH_TIME) {

		buffid = buff;
		duration = dur;
		opacity.set(1.0f);
		opcstep = -0.05f;

		if (buffid >= 0)
		{
			std::string strid = string_format::extend_id(buffid, 7);
			nl::node src = nl::nx::skill[strid.substr(0, 3) + ".img"]["skill"][strid];
			icon = src["icon"];
		}
		else
		{
			icon = DataFactory::get().getitemdata(-buffid).geticon(true);
		}
	}

	BuffIcon::BuffIcon()
		: cover(IconCover::BUFF, 0) {}

	BuffIcon::~BuffIcon() {}

	void BuffIcon::draw(Point<int16_t> position, float alpha) const
	{
		icon.draw(DrawArgument(position, opacity.get(alpha)));
		cover.draw(position + Point<int16_t>(1, -31), alpha);
	}

	bool BuffIcon::update()
	{
		if (duration <= FLASH_TIME)
		{
			opacity += opcstep;

			bool fadedout = opcstep < 0.0f && opacity.last() <= 0.0f;
			bool fadedin = opcstep > 0.0f && opacity.last() >= 1.0f;
			if (fadedout || fadedin)
				opcstep = -opcstep;
		}
		else
		{
			cover.update();
		}

		duration -= Constants::TIMESTEP;
		return duration < Constants::TIMESTEP;
	}


	UIBuffList::UIBuffList() 
	{
		position = Point<int16_t>(750, 40);
		active = true;
	}

	void UIBuffList::draw(float alpha) const
	{
		Point<int16_t> icpos = position;
		for (auto& icon : icons)
		{
			icon.second.draw(icpos, alpha);
			icpos.shiftx(-32);
		}
	}

	void UIBuffList::update()
	{
		std::vector<int32_t> expiredbuffs;
		for (auto& icon : icons)
		{
			bool expired = icon.second.update();
			if (expired)
				expiredbuffs.push_back(icon.first);
		}

		for (auto& exp : expiredbuffs)
		{
			icons.erase(exp);
		}
	}

	Cursor::State UIBuffList::sendmouse(bool pressed, Point<int16_t> cursorposition)
	{
		return UIElement::sendmouse(pressed, cursorposition);
	}

	void UIBuffList::addbuff(int32_t buffid, int32_t duration)
	{
		icons[buffid] = BuffIcon(buffid, duration);
	}
}