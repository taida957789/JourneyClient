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
#include "Char.h"
#include "CharStats.h"
#include "Skillbook.h"
#include "Questlog.h"
#include "Telerock.h"
#include "Monsterbook.h"
#include "Buff.h"
#include "ActiveBuffs.h"
#include "PassiveBuffs.h"

#include "Look\CharLook.h"
#include "Inventory\Inventory.h"

#include "..\Gameplay\Playable.h"
#include "..\Gameplay\Physics\Physics.h"
#include "..\Gameplay\Movement.h"
#include "..\Gameplay\Maplemap\MapInfo.h"
#include "..\Gameplay\Combat\Attack.h"
#include "..\Gameplay\Combat\Skill.h"

namespace jrc
{
	class Player : public Playable, public Char
	{
	public:
		// Construct a player object from the given character entry.
		Player(const CharEntry& entry);
		Player();

		// Draw the player.
		void draw(uint8_t layer, double viewx, double viewy, float alpha) const;
		// Update the player's animation, physics and states.
		int8_t update(const Physics& physics) override;
		// Set flipped ignore if attacking.
		void setflip(bool flipped) override;
		// Set state ignore if attacking.
		void setstate(State stance) override;

		// Respawn the player at the given position.
		void respawn(Point<int16_t> position, bool underwater);
		// Sends a Keyaction to the player's state, to apply forces, change the state and other behaviour.
		void sendaction(Keyboard::Action action, bool pressed);
		// Recalculates the total stats from base stats, inventories and skills.
		void recalcstats(bool equipchanged);
		// Change the equipment at the specified slot and recalculate stats.
		void changecloth(int16_t slot);
		// Use the item from the player's inventory with the given id.
		void useitem(int32_t itemid);

		// Return if the player is attacking.
		bool isattacking() const;
		// Return wether the player can attack or not.
		bool canattack() const;
		// Return wether the player can use a skill or not.
		SpecialMove::ForbidReason canuse(const SpecialMove& move) const;
		// Create an attack struct using the player's stats.
		Attack prepareattack(bool skill) const;

		// Execute a rush movement.
		void rush(double targetx);

		// Apply a buff to the player.
		void givebuff(Buff buff);
		// Cancel a buff.
		void cancelbuff(Buff::Stat stat);
		// Return wether the buff is active.
		bool hasbuff(Buff::Stat stat) const;

		// Change the player's level, display the levelup effect.
		void changelevel(uint16_t level);
		// Change the player's job, display the job change effect.
		void changejob(uint16_t jobid);

		// Return the character's level.
		uint16_t getlevel() const override;
		// Return the character's level of a skill.
		int32_t getskilllevel(int32_t skillid) const override;
		// Return the character's attacking speed.
		float getattackspeed() const override;

		// Returns the current walking force, calculated from the total ES_SPEED stat.
		float getwforce() const;
		// Returns the current jumping force, calculated from the total ES_JUMP stat.
		float getjforce() const;
		// Returns the climbing force, calculated from the total ES_SPEED stat.
		float getclimbforce() const;
		// Returns the flying force.
		float getflyforce() const;

		// Return wether the player is underwater.
		bool isunderwater() const;
		// Returns if a Keyaction is currently active. 
		bool keydown(Keyboard::Action action) const;
		// Return a pointer to the ladder the player is on.
		Optional<const Ladder> getladder() const;

		// Change players position to the seat's position and stance to Char::SIT.
		void setseat(Optional<const Seat> seat);
		// Change players xpos to the ladder x and change stance to Char::LADDER or Char::ROPE.
		void setladder(Optional<const Ladder> ladder);

		// Obtain a reference to the player's stats.
		CharStats& getstats();
		// Obtain a reference to the player's inventory.
		Inventory& getinvent();
		// Obtain a reference to the player's skills.
		Skillbook& getskills();
		// Obtain a reference to the player's questlog.
		Questlog& getquests();
		// Obtain a reference to the player's telerock locations.
		Telerock& gettrock();
		// Obtain a reference to the player's monsterbook.
		Monsterbook& getmonsterbook();

	private:
		CharStats stats;
		Inventory inventory;
		Skillbook skillbook;
		Questlog questlog;
		Telerock telerock;
		Monsterbook monsterbook;

		std::unordered_map<Buff::Stat, Buff> buffs;
		ActiveBuffs active_buffs;
		PassiveBuffs passive_buffs;

		std::map<Keyboard::Action, bool> keysdown;

		Movement lastmove;

		Optional<const Ladder> ladder;
		bool underwater;
	};
}

