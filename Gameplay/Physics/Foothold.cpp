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
#include "Foothold.h"

#include "..\..\Util\Misc.h"

namespace jrc
{
	Foothold::Foothold(nl::node src, uint8_t ly)
	{
		id = string_conversion::or_zero<uint16_t>(src.name());
		prev = src["prev"];
		next = src["next"];
		layer = ly;

		horizontal = Range<int16_t>(src["x1"], src["x2"]);
		vertical = Range<int16_t>(src["y1"], src["y2"]);
	}

	Foothold::Foothold()
	{
		id = 0;
		prev = 0;
		next = 0;
		layer = 0;
	}

	double Foothold::getslope() const
	{ 
		return iswall() ? 0.0f : static_cast<double>(getvdelta()) / gethdelta();
	}

	double Foothold::resolvex(double x) const
	{ 
		return isfloor() ? vertical.first() : getslope() * (x - horizontal.first()) + vertical.first(); 
	}
}
