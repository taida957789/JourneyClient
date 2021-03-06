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
#include "CharStats.h"

#include "StatCaps.h"

namespace jrc
{
	CharStats::CharStats(const StatsEntry& s)
		: name(s.name), petids(s.petids), job(s.job),
		exp(s.exp), mapid(s.mapid), portal(s.portal),
		rank(s.rank), jobrank(s.jobrank), basestats(s.stats) {

		init_totalstats();
	}

	CharStats::CharStats() {}

	void CharStats::init_totalstats()
	{
		totalstats.clear();
		buffdeltas.clear();
		percentages.clear();

		totalstats[Equipstat::HP] = getstat(Maplestat::MAXHP);
		totalstats[Equipstat::MP] = getstat(Maplestat::MAXMP);
		totalstats[Equipstat::STR] = getstat(Maplestat::STR);
		totalstats[Equipstat::DEX] = getstat(Maplestat::DEX);
		totalstats[Equipstat::INT] = getstat(Maplestat::INT);
		totalstats[Equipstat::LUK] = getstat(Maplestat::LUK);
		totalstats[Equipstat::SPEED] = 100;
		totalstats[Equipstat::JUMP] = 100;

		maxdamage = 0;
		mindamage = 0;
		honor = 0;
		attackspeed = 0;
		projectilerange = 400;
		mastery = 0.0f;
		critical = 0.05f;
		mincrit = 0.5f;
		maxcrit = 0.75f;
		damagepercent = 0.0f;
		bossdmg = 0.0f;
		ignoredef = 0.0f;
		stance = 0.0f;
		resiststatus = 0.0f;
	}

	void CharStats::close_totalstats()
	{
		totalstats[Equipstat::ACC] += calculateaccuracy();

		for (auto iter : percentages)
		{
			Equipstat::Value stat = iter.first;
			int32_t total = totalstats[stat];
			total += static_cast<int32_t>(total * iter.second);
			set_total(stat, total);
		}

		int32_t primary = getprimary();
		int32_t secondary = getsecondary();
		int32_t attack = gettotal(Equipstat::WATK);
		float multiplier = damagepercent + static_cast<float>(attack) / 100;
		maxdamage = static_cast<int32_t>((primary + secondary) * multiplier);
		mindamage = static_cast<int32_t>(((primary * 0.9f * mastery) + secondary) * multiplier);
	}

	int32_t CharStats::calculateaccuracy() const
	{
		int32_t totaldex = gettotal(Equipstat::DEX);
		int32_t totalluk = gettotal(Equipstat::LUK);
		return static_cast<int32_t>(totaldex * 0.8f + totalluk * 0.5f);
	}

	int32_t CharStats::getprimary() const
	{
		Equipstat::Value primary = job.primarystat(weapontype);
		return static_cast<int32_t>(getmultiplier() * gettotal(primary));
	}

	int32_t CharStats::getsecondary() const
	{
		Equipstat::Value secondary = job.secondarystat(weapontype);
		return gettotal(secondary);
	}

	float CharStats::getmultiplier() const
	{
		switch (weapontype)
		{
		case Weapon::SWORD_1H:
			return 4.0f;
		case Weapon::AXE_1H:
		case Weapon::MACE_1H:
		case Weapon::WAND:
		case Weapon::STAFF:
			return 4.4f;
		case Weapon::DAGGER:
		case Weapon::CROSSBOW:
		case Weapon::CLAW:
		case Weapon::GUN:
			return 3.6f;
		case Weapon::SWORD_2H:
			return 4.6f;
		case Weapon::AXE_2H:
		case Weapon::MACE_2H:
		case Weapon::KNUCKLE:
			return 4.8f;
		case Weapon::SPEAR:
		case Weapon::POLEARM:
			return 5.0f;
		case Weapon::BOW:
			return 3.4f;
		default:
			return 0.0f;
		}
	}

	void CharStats::set_stat(Maplestat::Value stat, uint16_t value)
	{
		basestats[stat] = value;
	}

	void CharStats::set_total(Equipstat::Value stat, int32_t value)
	{
		auto iter = EQSTAT_CAPS.find(stat);
		if (iter != EQSTAT_CAPS.end())
		{
			int32_t cap_value = iter->second;

			if (value > cap_value)
				value = cap_value;
		}

		totalstats[stat] = value;
	}

	void CharStats::add_buff(Equipstat::Value stat, int32_t value)
	{
		int32_t current = gettotal(stat);
		set_total(stat, current + value);
		buffdeltas[stat] += value;
	}

	void CharStats::add_value(Equipstat::Value stat, int32_t value)
	{
		int32_t current = gettotal(stat);
		set_total(stat, current + value);
	}

	void CharStats::add_percent(Equipstat::Value stat, float percent)
	{
		percentages[stat] += percent;
	}

	void CharStats::set_weapontype(Weapon::Type w)
	{
		weapontype = w;
	}

	void CharStats::set_exp(int64_t e)
	{
		exp = e;
	}

	void CharStats::set_portal(uint8_t p)
	{
		portal = p;
	}

	void CharStats::set_mastery(float m)
	{
		mastery = 0.5f + m;
	}

	void CharStats::set_damagepercent(float d)
	{
		damagepercent = d;
	}

	void CharStats::set_reducedamage(float r)
	{
		reducedamage = r;
	}

	void CharStats::change_job(uint16_t id)
	{
		basestats[Maplestat::JOB] = id;
		job.changejob(id);
	}

	bool CharStats::isdamagebuffed() const
	{
		return getbuffdelta(Equipstat::WATK) > 0 || getbuffdelta(Equipstat::MAGIC) > 0;
	}

	uint16_t CharStats::getstat(Maplestat::Value stat) const
	{
		return basestats[stat];
	}

	int32_t CharStats::gettotal(Equipstat::Value stat) const
	{
		return totalstats[stat];
	}

	int32_t CharStats::getbuffdelta(Equipstat::Value stat) const
	{
		return buffdeltas[stat];
	}

	int64_t CharStats::getexpneeded() const
	{
		static const size_t LEVELCAP = 201;
		static const int64_t exptable[LEVELCAP] =
		{
			1, 15, 34, 57, 92, 135, 372, 560, 840, 1242, 1144, 1573, 2144, 2800, 3640, 4700, 5893, 7360, 9144,
			11120, 13477, 16268, 19320, 22880, 27008, 31477, 36600, 42444, 48720, 55813, 63800, 86784, 98208,
			110932, 124432, 139372, 155865, 173280, 192400, 213345, 235372, 259392, 285532, 312928, 342624,
			374760, 408336, 445544, 483532, 524160, 567772, 598886, 631704, 666321, 702836, 741351, 781976,
			824828, 870028, 917625, 967995, 1021041, 1076994, 1136013, 1198266, 1263930, 1333194, 1406252,
			1483314, 1564600, 1650340, 1740778, 1836173, 1936794, 2042930, 2154882, 2272970, 2397528, 2528912,
			2667496, 2813674, 2967863, 3130502, 3302053, 3483005, 3673873, 3875201, 4087562, 4311559, 4547832,
			4797053, 5059931, 5337215, 5629694, 5938202, 6263614, 6606860, 6968915, 7350811, 7753635, 8178534,
			8626718, 9099462, 9598112, 10124088, 10678888, 11264090, 11881362, 12532461, 13219239, 13943653,
			14707765, 15513750, 16363902, 17260644, 18206527, 19204245, 20256637, 21366700, 22537594, 23772654,
			25075395, 26449526, 27898960, 29427822, 31040466, 32741483, 34535716, 36428273, 38424542, 40530206,
			42751262, 45094030, 47565183, 50171755, 52921167, 55821246, 58880250, 62106888, 65510344, 69100311,
			72887008, 76881216, 81094306, 85594273, 90225770, 95170142, 100385466, 105886589, 111689174, 117809740,
			124265714, 131075474, 138258410, 145834970, 153826726, 162256430, 171148082, 180526997, 190419876,
			200854885, 211861732, 223471711, 223471711, 248635353, 262260570, 276632449, 291791906, 307782102,
			324648562, 342439302, 361204976, 380999008, 401877754, 423900654, 447130410, 471633156, 497478653,
			524740482, 553496261, 583827855, 615821622, 649568646, 685165008, 722712050, 762316670, 804091623,
			848155844, 894634784, 943660770, 995373379, 1049919840, 1107455447, 1168144006, 1232158297,
			1299680571, 1370903066, 1446028554, 1525246918, 1608855764, 1697021059
		};

		uint16_t level = getstat(Maplestat::LEVEL);
		return (level < LEVELCAP) ? exptable[level] : 0;
	}

	Rectangle<int16_t> CharStats::getrange() const
	{
		return Rectangle<int16_t>(-projectilerange, -5, -50, 50);
	}

	int32_t CharStats::getmapid() const
	{
		return mapid;
	}

	uint8_t CharStats::getportal() const
	{
		return portal;
	}

	int64_t CharStats::getexp() const
	{
		return exp;
	}

	std::string CharStats::getname() const
	{
		return name;
	}

	std::string CharStats::getjobname() const
	{
		return job.getname();
	}

	Weapon::Type CharStats::get_weapontype() const
	{
		return weapontype;
	}

	float CharStats::getmastery() const
	{
		return mastery;
	}

	float CharStats::getcritical() const
	{
		return critical;
	}

	const CharJob& CharStats::get_job() const
	{
		return job;
	}
}
