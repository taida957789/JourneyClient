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
#include "Texture.h"

#include "..\Util\Interpolated.h"
#include "..\Util\Rectangle.h"

#include <vector>
#include <map>

namespace jrc
{
	// Class which consists of multiple textures to make an Animation.
	class Animation
	{
	public:
		Animation(nl::node source);
		Animation();

		bool update();
		bool update(uint16_t timestep);
		void reset();
		
		void draw(const DrawArgument& arguments, float inter) const;

		uint16_t getdelay(int16_t frame) const;
		uint16_t getdelayuntil(int16_t frame) const;
		Point<int16_t> getorigin() const;
		Point<int16_t> getdimensions() const;
		Point<int16_t> gethead() const;
		Rectangle<int16_t> getbounds() const;

	private:
		struct Frame
		{
			Frame(nl::node src);
			Frame();

			float opcstep(uint16_t timestep) const;
			float scalestep(uint16_t timestep) const;

			Texture texture;
			uint16_t delay;
			std::pair<uint8_t, uint8_t> opacities;
			std::pair<int16_t, int16_t> scales;
			Rectangle<int16_t> bounds;
			Point<int16_t> head;
		};
		const Frame& getframe() const;

		std::vector<Frame> frames;
		bool animated;
		bool zigzag;

		Nominal<int16_t> frame;
		Linear<float> opacity;
		Linear<float> xyscale;

		uint16_t delay;
		int16_t framestep;
		float opcstep;
	};
}

