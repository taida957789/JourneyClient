//////////////////////////////////////////////////////////////////////////////
// This file is part of the Journey MMORPG client                           //
// Copyright � 2016 Daniel Allendorf                                        //
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
#include "SkillBullet.h"

#include "..\..\Data\DataFactory.h"
#include "..\..\Util\Misc.h"

namespace jrc
{
	Animation RegularBullet::get(const Char&, int32_t bulletid) const
	{
		return DataFactory::get()
			.getbulletdata(bulletid)
			.getbullet();
	}


	SingleBullet::SingleBullet(nl::node src)
	{
		ball = src["ball"];
	}

	Animation SingleBullet::get(const Char&, int32_t) const
	{
		return ball.animation;
	}


	BySkillLevelBullet::BySkillLevelBullet(nl::node src, int32_t id)
	{
		skillid = id;

		for (auto sub : src["level"])
		{
			auto level = string_conversion::or_zero<int32_t>(sub.name());
			bullets[level] = sub["ball"];
		}
	}

	Animation BySkillLevelBullet::get(const Char& user, int32_t) const
	{
		int32_t level = user.getskilllevel(skillid);
		auto iter = bullets.find(level);
		if (iter != bullets.end())
		{
			return iter->second.animation;
		}
		else
		{
			return Animation();
		}
	}
}