#pragma once


namespace xr {
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
