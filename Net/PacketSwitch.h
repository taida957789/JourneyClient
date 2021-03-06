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
#include "PacketHandler.h"

#include <memory>

namespace jrc
{
	// Class which contains the array of handler classes to use.
	class PacketSwitch
	{
	public:
		// Register all handlers.
		PacketSwitch();

		// Forward a packet to the correct handler.
		void forward(int8_t* buffer, size_t length) const;

	private:
		// Maximum number of handler classes needed for now.
		static const uint16_t NUM_HANDLERS = 500;

		std::unique_ptr<PacketHandler> handlers[NUM_HANDLERS];
	};
}