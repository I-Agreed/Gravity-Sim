#pragma once
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <deque>
#include "Vector2.hpp"
using namespace std;

struct Planet {
	IA::Vector2f pos;
	IA::Vector2f vel;

	float mass = 0; // physics mass
	float size = 0; // visual size (circle radius)
	bool fixed = false;
	deque<IA::Vector2f> trail;
	bool createTrail = true;
	sf::Color colour;

	Planet(float x = 0, float y = 0, float vx = 0, float vy = 0, float m = 0, bool f = false, bool t = true, sf::Color c = sf::Color(255, 255, 255)) {
		pos = IA::Vector2f(x, y);
		vel = IA::Vector2f(vx, vy);
		mass = m;
		size = pow(m, 1.0/3.0);
		colour = c;
		fixed = f;
		createTrail = t;
	}

	Planet copy() {
		Planet newPlanet(pos.x, pos.y, vel.x, vel.y, mass, fixed, createTrail, colour);
		for (sf::Vector2f pos: trail) {
			newPlanet.trail.push_back(IA::Vector2f(pos.x, pos.y));
		}
		return newPlanet;
	}
};

