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
#include "AttackHandlers.h"

#include "..\..\Gameplay\Combat\Skill.h"
#include "..\..\Gameplay\Stage.h"

#include <unordered_map>
#include <vector>

namespace jrc
{
	AttackHandler::AttackHandler(Attack::Type t)
	{
		type = t;
	}

	void AttackHandler::handle(InPacket& recv) const
	{
		int32_t cid = recv.read_int();
		uint8_t count = recv.read_byte();

		recv.skip(1);

		AttackResult attack;
		attack.type = type;

		attack.level = recv.read_byte();
		attack.skill = (attack.level > 0) ? recv.read_int() : 0;

		attack.display = recv.read_byte();
		attack.toleft = recv.read_bool();
		attack.stance = recv.read_byte();
		attack.speed = recv.read_byte();

		recv.skip(1);

		attack.bullet = recv.read_int();

		attack.mobcount = (count >> 4) & 0xF;
		attack.hitcount = count & 0xF;
		for (uint8_t i = 0; i < attack.mobcount; i++)
		{
			int32_t oid = recv.read_int();

			recv.skip(1);

			uint8_t length = (attack.skill == Skill::MESO_EXPLOSION) ? recv.read_byte() : attack.hitcount;
			for (uint8_t j = 0; j < length; j++)
			{
				int32_t damage = recv.read_int();
				bool critical = false; // todo
				auto singledamage = std::make_pair(damage, critical);
				attack.damagelines[oid].push_back(singledamage);
			}
		}

		Stage::get().showattack(cid, attack);
	}
}