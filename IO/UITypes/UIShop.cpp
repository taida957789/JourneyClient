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
#include "UIShop.h"
#include "UINotice.h"

#include "..\UI.h"
#include "..\Components\AreaButton.h"
#include "..\Components\MapleButton.h"
#include "..\Components\TwoSpriteButton.h"

#include "..\..\Character\Inventory\Inventory.h"
#include "..\..\Data\DataFactory.h"
#include "..\..\Gameplay\Stage.h"
#include "..\..\Net\Packets\NpcInteractionPackets.h"
#include "..\..\Util\Misc.h"

#include "nlnx\nx.hpp"
#include "nlnx\node.hpp"

namespace jrc
{
	UIShop::UIShop()
	{
		nl::node src = nl::nx::ui["UIWindow2.img"]["Shop"];
		nl::node src2 = nl::nx::ui["UIWindow2.img"]["Shop2"];

		sprites.emplace_back(src["backgrnd"]);
		sprites.emplace_back(src["backgrnd2"]);
		sprites.emplace_back(src["backgrnd3"]);

		buttons[BUY_ITEM] =std::make_unique<MapleButton>(src["BtBuy"]);
		buttons[SELL_ITEM] =std::make_unique<MapleButton>(src["BtSell"]);
		buttons[EXIT] =std::make_unique<MapleButton>(src["BtExit"]);

		nl::node sellen = src2["TabSell"]["enabled"];
		nl::node selldis = src2["TabSell"]["disabled"];

		constexpr Point<int16_t> stshift(-43, -9);
		for (uint16_t i = EQUIP; i <= CASH; i++)
		{
			std::string tabnum = std::to_string(i - EQUIP);
			buttons[i] = std::make_unique<TwoSpriteButton>(selldis[tabnum], sellen[tabnum], stshift);
		}

		for (uint16_t i = BUY0; i <= BUY4; i++)
		{
			Point<int16_t> pos(8, 116 + 42 * (i - BUY0));
			Point<int16_t> dim(200, 36);
			buttons[i] = std::make_unique<AreaButton>(pos, dim);
		}
		for (uint16_t i = SELL0; i <= SELL4; i++)
		{
			Point<int16_t> pos(242, 116 + 42 * (i - SELL0));
			Point<int16_t> dim(200, 36);
			buttons[i] = std::make_unique<AreaButton>(pos, dim);
		}

		selection = src["select"];
		impossible = src["ShopSpecial"]["impossible"];
		meso = src["meso"];

		mesolabel = { Text::A11M, Text::RIGHT, Text::LIGHTGREY };

		buyslider = {
			11, { 115, 308 }, 214, 5, 1, 
			[&](bool upwards) {
			int16_t shift = upwards ? -1 : 1;
			bool above = buystate.offset + shift >= 0;
			bool below = buystate.offset + shift <= buystate.lastslot - 5;
			if (above && below)
			{
				buystate.offset += shift;
			}
		} };
		sellslider = {
			11, { 115, 308 }, 445, 5, 1, 
			[&](bool upwards) {
			int16_t shift = upwards ? -1 : 1;
			bool above = sellstate.offset + shift >= 0;
			bool below = sellstate.offset + shift <= sellstate.lastslot - 5;
			if (above && below)
			{
				sellstate.offset += shift;
			}
		} };

		active = false;
		dimension = Texture(src["backgrnd"])
			.getdimensions();
		position = { 400 - dimension.x() / 2, 240 - dimension.y() / 2 };
	}

	void UIShop::draw(float alpha) const
	{
		UIElement::draw(alpha);

		npc.draw({ position + Point<int16_t>(64, 76), true });

		Stage::get().getplayer().getlook()
			.drawstanding(position + Point<int16_t>(294, 76), false);

		mesolabel.draw(position + Point<int16_t>(450, 62));

		buystate.draw(position, selection);
		sellstate.draw(position, selection);

		buyslider.draw(position);
		sellslider.draw(position);
	}

	void UIShop::update()
	{
		int64_t num_mesos = Stage::get().getplayer().getinvent().getmeso();
		std::string mesostr = std::to_string(num_mesos);
		string_format::split_number(mesostr);
		mesolabel.settext(mesostr);
	}

	void UIShop::buttonpressed(uint16_t buttonid)
	{
		constexpr Range<uint16_t> buy(BUY0, BUY4);
		constexpr Range<uint16_t> sell(SELL0, SELL4);
		if (buy.contains(buttonid))
		{
			int16_t selected = buttonid - BUY0;
			buystate.select(selected);

			buttons[buttonid]->setstate(Button::NORMAL);
		}
		else if(sell.contains(buttonid))
		{
			int16_t selected = buttonid - SELL0;
			sellstate.select(selected);

			buttons[buttonid]->setstate(Button::NORMAL);
		}
		else
		{
			switch (buttonid)
			{
			case BUY_ITEM:
				buystate.buy();
				buttons[buttonid]->setstate(Button::NORMAL);
				break;
			case SELL_ITEM:
				sellstate.sell();
				buttons[buttonid]->setstate(Button::NORMAL);
				break;
			case EXIT:
				NpcShopActionPacket().dispatch();
				active = false;
				break;
			case EQUIP:
				changeselltab(Inventory::EQUIP);
				break;
			case USE:
				changeselltab(Inventory::USE);
				break;
			case ETC:
				changeselltab(Inventory::ETC);
				break;
			case SETUP:
				changeselltab(Inventory::SETUP);
				break;
			case CASH:
				changeselltab(Inventory::CASH);
				break;
			}
		}

		clear_tooltip();
	}

	Cursor::State UIShop::sendmouse(bool clicked, Point<int16_t> cursorpos)
	{
		Point<int16_t> cursoroffset = cursorpos - position;
		if (buyslider.isenabled())
		{
			Cursor::State bstate = buyslider.sendcursor(cursoroffset, clicked);
			if (bstate != Cursor::IDLE)
			{
				clear_tooltip();
				return bstate;
			}
		}
		
		if (sellslider.isenabled())
		{
			Cursor::State sstate = sellslider.sendcursor(cursoroffset, clicked);
			if (sstate != Cursor::IDLE)
			{
				clear_tooltip();
				return sstate;
			}
		}

		int16_t xoff = cursoroffset.x();
		int16_t yoff = cursoroffset.y();
		int16_t slot = slotbypos(yoff);
		if (slot >= 0 && slot <= 4)
		{
			if (xoff > 7 && xoff < 209)
				showitem(slot, true);
			else if (xoff > 241 && xoff < 443)
				showitem(slot, false);
			else
				clear_tooltip();
		}
		else
		{
			clear_tooltip();
		}
		return UIElement::sendmouse(clicked, cursorpos);
	}

	void UIShop::clear_tooltip()
	{
		UI::get().clear_tooltip(SHOP);
	}

	void UIShop::showitem(int16_t slot, bool buy)
	{
		if (buy)
		{
			buystate.showitem(slot);
		}
		else
		{
			sellstate.showitem(slot);
		}
	}

	void UIShop::changeselltab(Inventory::Type type)
	{
		uint16_t oldtab = tabbyinventory(sellstate.tab);
		if (oldtab > 0)
		{
			buttons[oldtab]->setstate(Button::NORMAL);
		}
		uint16_t newtab = tabbyinventory(type);
		if (newtab > 0)
		{
			buttons[newtab]->setstate(Button::PRESSED);
		}

		sellstate.changetab(type, meso);

		sellslider.setrows(5, sellstate.lastslot);
	}

	void UIShop::reset(int32_t npcid)
	{
		std::string strid = string_format::extend_id(npcid, 7);
		npc = nl::nx::npc[strid + ".img"]["stand"]["0"];

		for (auto& button : buttons)
		{
			button.second->setstate(Button::NORMAL);
		}
		buttons[EQUIP]->setstate(Button::PRESSED);

		buystate.reset();
		sellstate.reset();

		changeselltab(Inventory::EQUIP);

		active = true;
	}

	void UIShop::modify(Inventory::Type type)
	{
		if (type == sellstate.tab)
			changeselltab(type);
	}

	void UIShop::additem(int32_t id, int32_t price, int32_t pitch, 
		int32_t time, int16_t buyable) {

		addrechargable(id, price, pitch, time, 0, buyable);
	}

	void UIShop::addrechargable(int32_t id, int32_t price, int32_t pitch, 
		int32_t time, int16_t chargeprice, int16_t buyable) {

		auto buyitem = BuyItem(meso, id, price, pitch, time, chargeprice, buyable);
		buystate.add(buyitem);

		buyslider.setrows(5, buystate.lastslot);
	}

	int16_t UIShop::slotbypos(int16_t y)
	{
		int16_t yoff = y - 115;
		if (yoff > 0 && yoff < 38)
			return 0;
		else if (yoff > 42 && yoff < 80)
			return 1;
		else if (yoff > 84 && yoff < 122)
			return 2;
		else if (yoff > 126 && yoff < 164)
			return 3;
		else if (yoff > 168 && yoff < 206)
			return 4;
		else
			return -1;
	}

	uint16_t UIShop::tabbyinventory(Inventory::Type type)
	{
		switch (type)
		{
		case Inventory::EQUIP:
			return EQUIP;
		case Inventory::USE:
			return USE;
		case Inventory::ETC:
			return ETC;
		case Inventory::SETUP:
			return SETUP;
		case Inventory::CASH:
			return CASH;
		default:
			return 0;
		}
	}


	UIShop::BuyItem::BuyItem(Texture cur, int32_t i, int32_t p, int32_t pt, int32_t t, int16_t cp, int16_t b)
		: currency(cur), id(i), price(p), pitch(pt), time(t), chargeprice(cp), buyable(b) {

		namelabel = { Text::A11M, Text::LEFT, Text::DARKGREY };
		pricelabel = { Text::A11M, Text::LEFT, Text::DARKGREY };

		const ItemData& item = DataFactory::get().getitemdata(id);
		if (item.isloaded())
		{
			icon = item.geticon(false);
			namelabel.settext(item.getname());
		}

		std::string mesostr = std::to_string(price);
		string_format::split_number(mesostr);
		pricelabel.settext(mesostr + " Mesos");
	}

	void UIShop::BuyItem::draw(Point<int16_t> pos) const
	{
		icon.draw(pos + Point<int16_t>(0, 32));
		namelabel.draw(pos + Point<int16_t>(40, -1));
		currency.draw(pos + Point<int16_t>(38, 20));
		pricelabel.draw(pos + Point<int16_t>(53, 17));
	}

	int32_t UIShop::BuyItem::getid() const
	{
		return id;
	}

	int16_t UIShop::BuyItem::getbuyable() const
	{
		return buyable;
	}


	UIShop::SellItem::SellItem(const Item& item, int16_t s, bool sc, Texture cur)
	{
		icon = item.getidata().geticon(false);
		id = item.getid();
		sellable = item.getcount();
		slot = s;
		showcount = sc;
		currency = cur;

		namelabel = { Text::A11M, Text::LEFT, Text::DARKGREY };
		pricelabel = { Text::A11M, Text::LEFT, Text::DARKGREY };

		std::string name = item.getidata().getname();
		namelabel.settext(name);

		int32_t price = item.getidata().getprice();
		std::string mesostr = std::to_string(price);
		string_format::split_number(mesostr);
		pricelabel.settext(mesostr + " Mesos");
	}

	void UIShop::SellItem::draw(Point<int16_t> pos) const
	{
		icon.draw(pos + Point<int16_t>(0, 32));
		if (showcount)
		{
			static const Charset countset = { nl::nx::ui["Basic.img"]["ItemNo"], Charset::LEFT };
			countset.draw(std::to_string(sellable), pos + Point<int16_t>(0, 20));
		}
		namelabel.draw(pos + Point<int16_t>(40, -1));
		currency.draw(pos + Point<int16_t>(38, 20));
		pricelabel.draw(pos + Point<int16_t>(53, 17));
	}

	int32_t UIShop::SellItem::getid() const
	{
		return id;
	}

	int16_t UIShop::SellItem::getslot() const
	{
		return slot;
	}

	int16_t UIShop::SellItem::getsellable() const
	{
		return sellable;
	}


	void UIShop::BuyState::reset()
	{
		items.clear();

		offset = 0;
		lastslot = 0;
		selection = -1;
	}

	void UIShop::BuyState::draw(Point<int16_t> parentpos, const Texture& selected) const
	{
		for (int16_t i = 0; i < 5; i++)
		{
			int16_t slot = i + offset;
			if (slot >= lastslot)
				break;

			auto itempos = Point<int16_t>(12, 116 + 42 * i);
			if (slot == selection)
			{
				selected.draw(parentpos + itempos + Point<int16_t>(35, -1));
			}
			items[slot].draw(parentpos + itempos);
		}
	}

	void UIShop::BuyState::showitem(int16_t slot)
	{
		int16_t absslot = slot + offset;
		if (absslot < 0 || absslot >= lastslot)
			return;

		int32_t itemid = items[absslot].getid();
		UI::get().show_item(SHOP, itemid);
	}

	void UIShop::BuyState::add(BuyItem item)
	{
		items.push_back(item);

		lastslot++;
	}

	void UIShop::BuyState::buy() const
	{
		if (selection < 0 || selection >= lastslot)
			return;

		const BuyItem& item = items[selection];
		int16_t buyable = item.getbuyable();
		int16_t slot = selection;
		int32_t itemid = item.getid();
		if (buyable > 1)
		{
			constexpr char* question = "How many would you like to buy?";
			auto onenter = [slot, itemid](int32_t qty){
				auto shortqty = static_cast<int16_t>(qty);

				NpcShopActionPacket(slot, itemid, shortqty, true).dispatch();
			};
			UI::get().add(
				ElementTag<UIEnterNumber, std::string, std::function<void(int32_t)>, int32_t, int32_t, int32_t>(
					question, onenter, 1, buyable, 1
					)
			);
		}
		else if (buyable > 0)
		{
			constexpr char* question = "Would you like to buy the item?";
			auto ondecide = [slot, itemid](bool yes){
				if (yes)
				{
					NpcShopActionPacket(slot, itemid, 1, true).dispatch();
				}
			};
			UI::get().add(
				ElementTag<UIYesNo, std::string, std::function<void(bool)>>(
					question, ondecide
					)
			);
		}
	}

	void UIShop::BuyState::select(int16_t selected)
	{
		int16_t slot = selected + offset;
		if (slot == selection)
		{
			buy();
		}
		else
		{
			selection = slot;
		}
	}


	void UIShop::SellState::reset()
	{
		items.clear();

		offset = 0;
		lastslot = 0;
		selection = -1;
		tab = Inventory::NONE;
	}

	void UIShop::SellState::changetab(Inventory::Type newtab, Texture meso)
	{
		tab = newtab;

		offset = 0;
		selection = -1;

		items.clear();

		int16_t slots = Stage::get()
			.getplayer()
			.getinvent()
			.getslots(tab);
		for (int16_t i = 1; i <= slots; i++)
		{
			Optional<Item> item = Stage::get().getplayer().getinvent().getitem(tab, i);
			if (item)
			{
				SellItem sellitem(*item, i, tab != Inventory::EQUIP, meso);
				items.push_back(sellitem);
			}
		}

		lastslot = static_cast<int16_t>(items.size());
	}

	void UIShop::SellState::draw(Point<int16_t> parentpos, const Texture& selected) const
	{
		for (int16_t i = 0; i < 5; i++)
		{
			int16_t slot = i + offset;
			if (slot >= lastslot)
				break;

			Point<int16_t> itempos(243, 116 + 42 * i);
			if (slot == selection)
			{
				selected.draw(parentpos + itempos + Point<int16_t>(35, -1));
			}
			items[slot].draw(parentpos + itempos);
		}
	}

	void UIShop::SellState::showitem(int16_t slot)
	{
		int16_t absslot = slot + offset;
		if (absslot < 0 || absslot >= lastslot)
			return;

		if (tab == Inventory::EQUIP)
		{
			int16_t realslot = items[absslot].getslot();
			Optional<Equip> equip = Stage::get().getplayer()
				.getinvent()
				.getequip(Inventory::EQUIP, realslot);
			UI::get().show_equip(SHOP, equip.get(), realslot);
		}
		else
		{
			int32_t itemid = items[absslot].getid();
			UI::get().show_item(SHOP, itemid);
		}
	}

	void UIShop::SellState::sell() const
	{
		if (selection < 0 || selection >= lastslot)
			return;

		const SellItem& item = items[selection];
		int32_t itemid = item.getid();
		int16_t sellable = item.getsellable();
		int16_t slot = item.getslot();
		if (sellable > 1)
		{
			constexpr char* question = "How many would you like to sell?";
			auto onenter = [itemid, slot](int32_t qty){
				auto shortqty = static_cast<int16_t>(qty);

				NpcShopActionPacket(slot, itemid, shortqty, false).dispatch();
			};
			UI::get().add(
				ElementTag<UIEnterNumber, std::string, std::function<void(int32_t)>, int32_t, int32_t, int32_t>(
					question, onenter, 1, sellable, 1
					)
			);
		}
		else if (sellable > 0)
		{
			constexpr char* question = "Would you like to sell the item?";
			auto ondecide = [itemid, slot](bool yes){
				if (yes)
				{
					NpcShopActionPacket(slot, itemid, 1, false).dispatch();
				}
			};
			UI::get().add(
				ElementTag<UIYesNo, std::string, std::function<void(bool)>>(
					question, ondecide
					)
			);
		}
	}

	void UIShop::SellState::select(int16_t selected)
	{
		int16_t slot = selected + offset;
		if (slot == selection)
		{
			sell();
		}
		else
		{
			selection = slot;
		}
	}
}