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
#include "SkillAction.h"

namespace jrc
{
	void RegularAction::apply(Char& target, Attack::Type atype) const
	{
		Weapon::Type weapontype = target.getlook().getequips().getweapontype();
		bool degenerate;
		switch (weapontype)
		{
		case Weapon::BOW:
		case Weapon::CROSSBOW:
		case Weapon::CLAW:
		case Weapon::GUN:
			degenerate = atype != Attack::RANGED;
			break;
		default:
			degenerate = false;
		}

		target.attack(degenerate);
	}


	SingleAction::SingleAction(nl::node src)
	{
		action = src["action"]["0"];
	}

	void SingleAction::apply(Char& target, Attack::Type) const
	{
		target.attack(action);
	}


	TwoHAction::TwoHAction(nl::node src)
	{
		actions[false] = src["action"]["0"];
		actions[true] = src["action"]["1"];
	}

	void TwoHAction::apply(Char& target, Attack::Type) const
	{
		bool twohanded = target.istwohanded();
		std::string action = actions[twohanded];

		target.attack(action);
	}


	ByLevelAction::ByLevelAction(nl::node src, int32_t id)
	{
		for (auto sub : src["level"])
		{
			int32_t level = string_conversion::or_zero<int32_t>(sub.name());
			actions[level] = sub["action"];
		}

		skillid = id;
	}

	void ByLevelAction::apply(Char& target, Attack::Type) const
	{
		int32_t level = target.getskilllevel(skillid);
		auto iter = actions.find(level);
		if (iter != actions.end())
		{
			target.attack(iter->second);
		}
	}
}