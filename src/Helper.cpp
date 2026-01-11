
#include <SFML/Graphics.hpp>
#include <Helper.hpp>


float dotProduct(Point2 a, Point2 b) {
	return a.x * b.x + a.y * b.y;
}

float dotProduct(Point3 a, Point3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

std::ostream& operator << (std::ostream& os, Point2 x) {
	return os << "(" << x.x << ", " << x.y << ")";
}

std::ostream& operator << (std::ostream& os, Point3 x) {
	return os << "(" << x.x << ", " << x.y << ", " << x.z << ")";
}

std::mt19937_64 rng(1);
ll rngesus(ll l, ll r) {
	return (ull)rng() % (r - l + 1) + l;
}