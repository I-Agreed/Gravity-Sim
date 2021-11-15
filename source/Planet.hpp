#pragma once
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <deque>
using namespace std;

struct Planet {
	sf::Vector2f pos;
	sf::Vector2f vel;

	float mass = 0; // physics mass
	float size = 0; // visual size (circle radius)
	bool fixed = false;
	deque<sf::Vector2f> trail;
	bool createTrail = true;
	sf::Color colour;

	Planet(double x = 0, double y = 0, double vx = 0, double vy = 0, double m = 0, bool f = false, bool t = true) {
		pos = sf::Vector2f(x, y);
		vel = sf::Vector2f(vx, vy);
		mass = m;
		size = pow(m, 1.0/3.0);
		colour = sf::Color(255, 255, 255);
		fixed = f;
		createTrail = t;
	}
};

