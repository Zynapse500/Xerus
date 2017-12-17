#pragma once


namespace xr {

	template <class T>
	bool rangeIntersects(T aMin, T aMax, T bMin, T bMax) {
		return aMin < bMax && aMax > bMin;
	}


	template <class T> 
	struct Range {
		T lower, upper;

		Range() {}

		Range(T lower, T upper)
			: lower(lower), upper(upper) {}
	};



	template <class T>
	struct Rectangle {
		T x, y;
		T width, height;

		Rectangle(T x, T y, T width, T height)
			: x(x), y(y), width(width), height(height) {}

		bool intersects(T x, T y, T w, T h)  {
			return (this->x < x + w && this->x + this->width > x &&
					this->y < y + h && this->y + this->height > y);
		}
	};
}
