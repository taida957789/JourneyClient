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
#pragma once
#include "..\..\Util\Point.h"
#include "..\..\Util\Range.h"
#include "..\..\Util\Optional.h"

#include <nlnx\node.hpp>

#include <vector>

namespace jrc
{
	class Seat
	{
	public:
		Seat(nl::node source);

		bool inrange(Point<int16_t> position) const;
		Point<int16_t> getpos() const;

	private:
		Point<int16_t> pos;
	};


	class Ladder
	{
	public:
		Ladder(nl::node source);

		bool isladder() const;
		bool inrange(Point<int16_t> position, bool upwards) const;
		bool felloff(int16_t y, bool downwards) const;
		int16_t getx() const;

	private:
		int16_t x;
		int16_t y1;
		int16_t y2;
		bool ladder;
	};


	class MapInfo
	{
	public:
		MapInfo(nl::node src, Range<int16_t> walls, Range<int16_t> borders);
		MapInfo();

		bool isswimmap() const;
		std::string getbgm() const;
		Range<int16_t> getwalls() const;
		Range<int16_t> getborders() const;

		// Find a setat the player's position.
		Optional<const Seat> findseat(Point<int16_t> position) const;
		// Find a ladder at the player's position. upwards = false implies downwards.
		Optional<const Ladder> findladder(Point<int16_t> position, bool upwards) const;

	private:
		int32_t fieldlimit;
		bool cloud;
		std::string bgm;
		std::string mapdesc;
		std::string mapname;
		std::string streetname;
		std::string mapmark;
		bool swim;
		bool town;
		bool hideminimap;
		Range<int16_t> mapwalls;
		Range<int16_t> mapborders;
		std::vector<Seat> seats;
		std::vector<Ladder> ladders;
	};
}