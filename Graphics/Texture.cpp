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
#include "Texture.h"
#include "GraphicsGL.h"

#include "..\Configuration.h"

#include "nlnx\nx.hpp"

namespace jrc
{
	Texture::Texture(nl::node src)
	{
		if (src.data_type() == nl::node::type::bitmap)
		{
			std::string link = src["source"];
			if (link != "")
			{
				nl::node srcfile = src;
				while (srcfile != srcfile.root())
				{
					srcfile = srcfile.root();
				}
				src = srcfile.resolve(link.substr(link.find('/') + 1));
			}

			bitmap = src;
			origin = src["origin"];
			dimensions = Point<int16_t>(bitmap.width(),  bitmap.height());

			GraphicsGL::get().addbitmap(bitmap);
		}
	}

	Texture::Texture() {}

	Texture::~Texture() {}

	void Texture::draw(const DrawArgument& args) const
	{
		size_t id = bitmap.id();
		if (id == 0)
			return;

		Point<int16_t> stretch = args.getstretch();
		int16_t w = stretch.x();
		if (w == 0)
			w = dimensions.x();
		int16_t h = stretch.y();
		if (h == 0)
			h = dimensions.y();

		Point<int16_t> absp = args.getpos() - origin;

		GraphicsGL::get().draw(bitmap, absp.x(), absp.y(), w, h, args.getopacity(), args.getxscale(),
			args.getyscale(), args.getcenter().x(), args.getcenter().y(), args.getangle());
	}

	void Texture::shift(Point<int16_t> amount)
	{
		origin -= amount;
	}

	bool Texture::isloaded() const
	{
		return bitmap.id() > 0;
	}

	int16_t Texture::width() const
	{
		return dimensions.x();
	}

	int16_t Texture::height() const
	{
		return dimensions.y();
	}

	Point<int16_t> Texture::getorigin() const
	{
		return origin;
	}

	Point<int16_t> Texture::getdimensions() const
	{
		return dimensions;
	}
}
