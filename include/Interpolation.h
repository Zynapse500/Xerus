#pragma once


namespace xr {

	// Return the smaller of the two
	template <class T>
	T min(const T& a, const T& b) {
		return (a < b) ? a : b;
	}

	// Return the larger of the two
	template <class T>
	T max(const T& a, const T& b) {
		return (a > b) ? a : b;
	}


	// Clamp a value into a range
	template <class T>
	T clamp(const T& t, const T& minOut, const T& maxOut) {
		return min(max(t, minOut), maxOut);
	}


	// Linearly interpolate between two points
	template <class T, class P>
	P lerp(const T& t, const P& p1, const P& p2) {
		T x = clamp(t, T(0), T(1));

		return x * (p2 - p1) + p1;
	}


	// Cubically interpolate between two points
	template <class T, class P>
	P smoothLerp(const T& t, const P& p1, const P& p2) {
		T x = clamp(t, T(0), T(1));

		return lerp(x*x * (3. - 2.*x), p1, p2);
	}

	// Cubically interpolate between two points
	template <class T, class P>
	P smootherLerp(const T& t, const P& p1, const P& p2) {
		T x = clamp(t, T(0), T(1));

		return lerp(x*x*x * (x * (6.*x - 15.) + 10.), p1, p2);
	}


	// Normalizes into range
	template <class T>
	T normalize(const T& t, const T& minIn, const T& maxIn) {
		return (t - minIn) / (maxIn - minIn);
	}


	// Map one range of values onto another (normalized lerp)
	template <class T, class P>
	P map(const T& t, const T& minIn, const T& maxIn, const P& minOut, const P& maxOut) {
		T x = normalize(t, minIn, maxIn);

		return lerp(x, minOut, maxOut);
	}
}
