#pragma once
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

struct Planet {
	sf::Vector2f pos;
	sf::Vector2f vel;

	float mass = 0; // physics mass
	float size = 0; // visual size (circle radius)
	sf::Color colour;

	Planet(double x = 0, double y = 0, double vx = 0, double vy = 0, double m = 0) {
		pos = sf::Vector2f(x, y);
		vel = sf::Vector2f(vx, vy);
		mass = m;
		size = 2;
	}
};

