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
#include <string>

#include <nlnx\node.hpp>

namespace jrc
{
	template <class T>
	class Point
	{
	public:
		// Construct a point from a vector property.
		Point(nl::node src)
		{
			a = static_cast<T>(src.x());
			b = static_cast<T>(src.y());
		}

		// Construct a point from the specified coordinates.
		constexpr Point(T first, T second)
			: a(first), b(second) {}

		// Construct a point with coordinates (0, 0).
		constexpr Point()
			: Point(0, 0) {}

		// Return the x-coordinate.
		constexpr T x() const
		{
			return a;
		}

		// Return the y-coordinate.
		constexpr T y() const
		{
			return b;
		}

		// Return the inner product.
		constexpr T length() const
		{
			return static_cast<T>(
				std::sqrt(a * a + b * b)
				);
		}

		// Check wether the coordinates are equal.
		constexpr bool straight() const
		{
			return a == b;
		}

		// Return a string representation of the point.
		std::string tostring() const
		{
			return "(" + std::to_string(a) + "," + std::to_string(b) + ")";
		}

		// Return the distance to another point.
		constexpr T distance(Point<T> v) const
		{
			return Point<T>(a - v.a, b - v.b)
				.length();
		}

		// Set the x-coordinate.
		void setx(T v)
		{
			a = v;
		}

		// Set the y-coordinate.
		void sety(T v)
		{
			b = v;
		}

		// Shift the x-coordinate by the specified amount.
		void shiftx(T v)
		{
			a += v;
		}

		// Shift the y-coordinate by the specified amount.
		void shifty(T v)
		{
			b += v;
		}

		// Shift the coordinates by the specified amounts.
		void shift(T x, T y)
		{
			a += x;
			b += y;
		}

		// Shift the this point by the amounts defined by another point. Equivalent to operator +=.
		void shift(Point<T> v)
		{
			a += v.a;
			b += v.b;
		}

		// Check wether point is equivalent to the specified point.
		constexpr bool operator == (const Point<T>& v) const
		{
			return a == v.a && b == v.b;
		}

		// Check wether point is not equivalent to the specified point.
		constexpr bool operator != (const Point<T>& v) const
		{
			return !(*this == v);
		}

		// Shift the this point by the amounts defined by another point.
		void operator += (Point<T> v)
		{
			a += v.a;
			b += v.b;
		}

		// Shift the this point in reverse direction by the amounts defined by another point.
		void operator -= (Point<T> v)
		{
			a -= v.a;
			b -= v.b;
		}

		// Return a point whose coordaintes are the negation of this point's coordinates.
		Point<T> operator - ()
		{
			return Point<T>(-a, -b);
		}

		// Return a point whose coordinates have been added the specified amount.
		Point<T> operator + (T v) const
		{
			return Point<T>(a + v, b + v);
		}

		// Return a point whose coordinates have been substracted the specified amount.
		Point<T> operator - (T v) const
		{
			return Point<T>(a - v, b - v);
		}

		// Return a point whose coordinates have been multiplied by the specified amount.
		Point<T> operator * (T v) const
		{
			return Point<T>(a * v, b * v);
		}

		// Return a point whose coordinates have been divided by the specified amount.
		Point<T> operator / (T v) const
		{
			return Point<T>(a / v, b / v);
		}

		// Return a point whose coordinates are the sum of this and another points coordinates.
		Point<T> operator + (Point<T> v) const
		{
			return Point<T>(a + v.a, b + v.b);
		}

		// Return a point whose coordinates are the difference of this and another points coordinates.
		Point<T> operator - (Point<T> v) const
		{
			return Point<T>(a - v.a, b - v.b);
		}

		// Return a point whose coordinates are the product of this and another points coordinates.
		Point<T> operator * (Point<T> v) const
		{
			return Point<T>(a / v.a, b / v.b);
		}

		// Return a point whose coordinates are the division of this and another points coordinates.
		Point<T> operator / (Point<T> v) const
		{
			T va = v.a;
			if (va == 0)
			{
				va = 1;
			}
			T vb = v.b;
			if (vb == 0)
			{
				vb = 1;
			}
			return Point<T>(a / va, b / vb);
		}

	private:
		T a;
		T b;
	};
}