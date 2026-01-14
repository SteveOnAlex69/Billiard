#include <Entity.hpp>
#include <algorithm>
#include <iostream>
#include <Helper.hpp>

Entity::Entity(float m, int t, Point2 u, Point2 v,
	Point2 r, float r_z) : m(m), t(t), u(u), v(v), r(r), r_z(r_z){}

const float G = 980;
const float MU = 300;
const float ROLL_MU = 500;
const float SPIN_MU = 100;

void disperse(Point2 &v, float F) {
	float len = v.length();
	if (len <= F) {
		v = Point2(0, 0);
	}
	else {
		v -= v.normalized() * F;
	}
}

void disperse_spin(float& r_z, float F) {
	float len = std::abs(r_z);
	if (len <= F) {
		r_z = 0;
	}
	else {
		r_z -= (r_z) / len * F;
	}
}

void Entity::progress(float delta, bool has_friction) {
	u += v * delta;
	if (!has_friction) return;

	// dispersing translational and rotational energy
	disperse(v, MU * delta);
	disperse(r, MU * delta);
	disperse_spin(r_z, SPIN_MU * delta);

	// correcting translational and rotational energy, so that they are getting closer to each other
	if ((v - r).length() > 0) {
		float v1 = (v - r).length() * 0.5f, v2 = ROLL_MU * delta;
		float len = std::min(v1, v2);
		Point2 diff = (v - r).normalized() * len;
		r += diff;
		v -= diff;
	}
}


float Entity::get_kinetic_energy() {
	return m * (v.lengthSquared() + r.lengthSquared());
}

sf::Vector2f Entity::get_momentum() {
	return v * m;
}