//////////////////////////////////////////////////////////////////////////////
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
#include "..\Util\Point.h"

#include <cstdint>
#include <string>
#include <vector>

namespace jrc
{
	// A packet to be sent to the server. Used as a base class to create specific packets.
	class OutPacket
	{
	public:
		// Construct a packet by writing its opcode.
		OutPacket(int16_t opcode);

		// Send the packet to the server.
		void dispatch();

	protected:
		// Skip a number of bytes (filled with zeroes).
		void skip(size_t count);
		// Write a byte.
		void writech(int8_t ch);
		// Write a short.
		void writesh(int16_t sh);
		// Write an int.
		void writeint(int32_t in);
		// Write a long.
		void writelg(int64_t lg);

		// Write a point, one short for x and one for y.
		void writepoint(Point<int16_t> point);
		// Write a timestamp as an integer.
		void writetime();
		// Write a string. Writes the length as a short
		// and then each individual character as a byte.
		void writestr(const std::string& str);

	private:
		std::vector<int8_t> bytes;
	};
}

