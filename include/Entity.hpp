#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>
#include <Helper.hpp>


struct Entity {              
	float m;
	Point2 u, v, r;
	float r_z;
	int t;
	Entity(float m = 0, int t = 0, Point2 u = Point2(0, 0), Point2 v = Point2(0, 0), Point2 r = Point2(0, 0), float r_z = 0);
	sf::Vector2f get_momentum();
	float get_kinetic_energy();
	void progress(float delta, bool has_friction = true);
};

#endif