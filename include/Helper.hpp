#ifndef HELPER_HPP
#define HELPER_HPP

#include <SFML/Graphics.hpp>
#include <random>
#include <chrono>

const sf::Vector2u windowSize = sf::Vector2u(1920, 1200);

#define ll long long
#define ull unsigned long long
#define Point2 sf::Vector2f
#define Point3 sf::Vector3f

float dotProduct(Point2 a, Point2 b);
float dotProduct(Point3 a, Point3 b);

std::ostream& operator << (std::ostream& os, Point2 x);
std::ostream& operator << (std::ostream& os, Point3 x); 

ll rngesus(ll l, ll r);


#endif