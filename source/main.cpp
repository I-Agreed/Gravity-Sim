#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include "Planet.hpp"
using namespace std;

const float G = 1;
const int FPS = 60;
const float PHYS_SCALE = 0.4; // physics time scale
const int WIDTH = 800; // screen width
const int HEIGHT = 600; // screen height
const int TRAIL_LENGTH = 120; // number of points in trail
const int TRAIL_WIDTH = 5; // maximum trail width

sf::Vector2f cameraPos(WIDTH/2, HEIGHT/2);

vector<Planet> planets;
sf::Clock frameClock;

float length(sf::Vector2f p) {
	return sqrtf(pow(p.x, 2) + pow(p.y, 2));
}

float distance(sf::Vector2f p1, sf::Vector2f p2) {
	sf::Vector2f p3 = p2 - p1;
	return sqrtf(pow(p3.x, 2) + pow(p3.y, 2));
}

sf::Vector2f normalize(sf::Vector2f p) { // returns direction vector from p1 to p2
	return p / length(p);
}

sf::Vector2f direction(sf::Vector2f p1, sf::Vector2f p2) { // returns direction vector from p1 to p2
	sf::Vector2f p3 = p2 - p1;
	return p3 / length(p3);
}

void update() {
	for (int i = 0; i < planets.size(); i++) {
		if (!planets[i].fixed) {
			sf::Vector2f force = sf::Vector2f(0,0);
			for (int j = 0; j < planets.size(); j++) {
				if (j != i) {
					force += planets[j].mass * direction(planets[i].pos, planets[j].pos) / powf(distance(planets[i].pos, planets[j].pos), 2);
				}
			}
			planets[i].vel += G * force * PHYS_SCALE;
		}
	}

	for (int i = 0; i < planets.size(); i++) {
		if (planets[i].createTrail) {
			float trailSize = (-1/(length(planets[i].vel)/2+1) + 1) * TRAIL_WIDTH; // when moving faster, the trail is thicker
			sf::Vector2f normal = normalize(sf::Vector2f(-planets[i].vel.y, planets[i].vel.x));
			planets[i].trail.push_back(planets[i].pos + normal*trailSize);
			planets[i].trail.push_back(planets[i].pos - normal*trailSize);
			if (planets[i].trail.size() > TRAIL_LENGTH * 2) {
				planets[i].trail.pop_front();
				planets[i].trail.pop_front(); // each trail 'point' is actually two points
			}
		}
		planets[i].pos += planets[i].vel * PHYS_SCALE;
	} 
}

void draw_planet(Planet p, sf::RenderWindow* win) {
	if (p.createTrail) {
		sf::VertexArray line(sf::TriangleStrip, p.trail.size());
		for (int i = 0; i < p.trail.size(); i++) {
			line[i].position = p.trail[i] + cameraPos;
			float colourScale = (i/(p.trail.size()-1.f));
			line[i].color = sf::Color(p.colour.r * colourScale, p.colour.g *  colourScale,
									  p.colour.b *  colourScale, p.colour.a *  colourScale);
		}
		win->draw(line);
	}
	sf::CircleShape circle;
	circle.setRadius(p.size);
	circle.setFillColor(p.colour);
	circle.setPosition(p.pos - sf::Vector2f(p.size, p.size) + cameraPos);

	win->draw(circle);
}

void draw_all_planets(sf::RenderWindow* win) {
	for (Planet p : planets) {
		draw_planet(p, win);
	}
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
	
	planets.push_back(Planet(0, 0, 0, 0, 10000, true));
	planets.push_back(Planet(200, 0, 0, 4, 100));

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