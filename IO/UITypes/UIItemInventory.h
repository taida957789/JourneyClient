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
#include "..\Element.h"
#include "..\UIDragElement.h"
#include "..\Components\Icon.h"
#include "..\Components\EquipTooltip.h"
#include "..\Components\ItemTooltip.h"
#include "..\Components\Slider.h"

#include "..\..\Character\Inventory\Inventory.h"

namespace jrc
{
	// The Item inventory.
	class UIItemInventory : public UIDragElement<PosINV>
	{
	public:
		static constexpr Type TYPE = ITEMINVENTORY;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		enum Buttons
		{
			BT_TAB_EQUIP,
			BT_TAB_USE,
			BT_TAB_ETC,
			BT_TAB_SETUP,
			BT_TAB_CASH,
			BT_DROPMESO,
			BT_POINTS,
			BT_GATHER,
			BT_SORT,
			BT_EXPAND,
			BT_ITEMPOT,
			BT_UPGRADE,
			BT_MAGNIFY,
			BT_BITCASE
		};

		UIItemInventory();

		void draw(float inter) const override;
		void update() override;
		void buttonpressed(uint16_t buttonid) override;
		void doubleclick(Point<int16_t> position) override;
		void sendicon(const Icon& icon, Point<int16_t> position) override;
		void togglehide() override;
		Cursor::State sendmouse(bool pressed, Point<int16_t> position) override;

		void modify(Inventory::Type type, int16_t pos, int8_t mode, int16_t arg);
		void enablesort();
		void enablegather();

	private:
		class ItemIcon : public Icon::Type
		{
		public:
			ItemIcon(Inventory::Type sourcetab, Equipslot::Value eqsource, int16_t source);

			void ondrop() const override;
			void ondropequips(Equipslot::Value eqslot) const override;
			void ondropitems(Inventory::Type tab, Equipslot::Value, int16_t slot, bool) const override;

		private:
			Inventory::Type sourcetab;
			Equipslot::Value eqsource;
			int16_t source;
		};

		UIItemInventory& operator = (const UIItemInventory&) = delete;

		void showitem(int16_t slot);
		void clear_tooltip();
		void loadicons();
		void updateslot(int16_t slot);
		bool isvisible(int16_t slot) const;
		bool isnotvisible(int16_t slot) const;
		int16_t slotbypos(Point<int16_t> position) const;
		uint16_t buttonbytab(Inventory::Type tab) const;
		Point<int16_t> getslotpos(int16_t slot) const;
		Point<int16_t> gettabpos(Inventory::Type tab) const;
		Optional<Icon> geticon(int16_t slot) const;

		const Inventory& inventory;

		Animation newitemslot;
		Animation newitemtab;
		Texture projectile;
		Text mesolabel;
		Slider slider;

		std::map<int16_t, std::unique_ptr<Icon>> icons;

		Inventory::Type tab;
		std::pair<int16_t, int16_t> slotrange;
		Inventory::Type newtab;
		int16_t newslot;
	};
}