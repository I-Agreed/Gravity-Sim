#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include "Planet.hpp"
using namespace std;

const float G = 1;
const int FPS = 60;

vector<Planet> planets;
sf::Clock frameClock;


float distance(sf::Vector2f p1, sf::Vector2f p2) {
	sf::Vector2f p3 = p2 - p1;
	return sqrtf(pow(p3.x, 2) + pow(p3.x, 2));
}

sf::Vector2f direction(sf::Vector2f p1, sf::Vector2f p2) { // returns direction vector from p1 to p2
	sf::Vector2f p3 = p2 - p1;
	return p3 * distance(p1, p2);
}

void update() {
	for (int i = 0; i < planets.size(); i++) {
		sf::Vector2f force = sf::Vector2f(0,0);
		for (int j = 0; j < planets.size(); j++) {
			if (j != i) {
				force += planets[j].mass * direction(planets[i].pos, planets[j].pos) / powf(distance(planets[i].pos, planets[j].pos), 2);
			}
		}
		planets[i].vel += G * force;
	}

	for (int i = 0; i < planets.size(); i++) {
		planets[i].pos += planets[i].vel;
	}
}

void draw_planet(Planet p, sf::RenderWindow* win) {
	sf::CircleShape circle;
	circle.setRadius(p.size);
	circle.setFillColor(p.colour);
	circle.setPosition(p.pos - sf::Vector2f(p.size, p.size));

	win->draw(circle);
}

void draw_all_planets(sf::RenderWindow* win) {
	for (Planet p : planets) {
		draw_planet(p, win);
	}
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
	
	while (window.isOpen()) {
		frameClock.getElapsedTime();
		sf::sleep(sf::seconds(1.0 / FPS) - frameClock.getElapsedTime());
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
		}

		window.clear();
		draw_all_planets(&window);
		window.display();
		update();
		frameClock.restart();
	}
}