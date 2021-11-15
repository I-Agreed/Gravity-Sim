#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include "Planet.hpp"
#include <iostream>

#define PI 3.14159265
using namespace std;

const float G = 1;
const int FPS = 60;
const float PHYS_SCALE = 0.4; // physics time scale
const int WIDTH = 800; // screen width
const int HEIGHT = 600; // screen height
const int TRAIL_LENGTH = 90; // number of points in trail
const int TRAIL_WIDTH = 5; // maximum trail width

sf::Vector2f cameraPos(WIDTH/2, HEIGHT/2);
float zoom = 2;

vector<Planet> planets;
sf::Clock frameClock;
bool lastMouseDown = false;
sf::Vector2i lastMousePos;

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

void add_trail(Planet* p) {
	float trailSize = (-1/(length(p->vel)/4+1) + 1) * TRAIL_WIDTH; // when moving faster, the trail is thicker
	sf::Vector2f normal = normalize(sf::Vector2f(-p->vel.y, p->vel.x));
	p->trail.push_back(p->pos + normal*trailSize);
	p->trail.push_back(p->pos - normal*trailSize);
	if (p->trail.size() > TRAIL_LENGTH * 2) {
		p->trail.pop_front();
		p->trail.pop_front(); // each trail 'point' is actually two points
	}
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
			add_trail(&planets[i]);
		}
		planets[i].pos += planets[i].vel * PHYS_SCALE;
	} 
}

void draw_trail(Planet p, sf::RenderWindow* win) {
	if (p.createTrail) {
		sf::VertexArray line(sf::TriangleStrip, p.trail.size());
		for (int i = 0; i < p.trail.size(); i++) {
			line[i].position = p.trail[i]/zoom + cameraPos;
			float colourScale = (i/(p.trail.size()-1.f));
			line[i].color = sf::Color(p.colour.r, p.colour.g, p.colour.b, p.colour.a *  colourScale);
		}
		win->draw(line);
	}
}

void draw_planet(Planet p, sf::RenderWindow* win) {
	sf::CircleShape circle;
	circle.setRadius(p.size/zoom);
	circle.setFillColor(p.colour);
	circle.setPosition((p.pos - sf::Vector2f(p.size, p.size))/zoom + cameraPos);

	win->draw(circle);
}

void draw_all_planets(sf::RenderWindow* win) {
	for (Planet p : planets) {
		draw_trail(p, win);
	}
	for (Planet p : planets) {
		draw_planet(p, win);
	}
}

void handle_mouse_move(sf::Event e, sf::RenderWindow* win) {
	sf::Vector2i pos = sf::Mouse::getPosition(*win);
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2i change = pos - lastMousePos;
			cameraPos += sf::Vector2f(change.x, change.y);
	}
	lastMousePos = pos;
}

void handle_mouse_scroll(sf::Event e, sf::RenderWindow* win) {
	zoom *= pow(2, -e.mouseWheelScroll.delta);
}

int main() {
	sf::RenderWindow window(sf::VideoMode(800, 600), "Gravity Sim");
	
	planets.push_back(Planet(0, 0, 0, 0, 10000, true));
	planets.push_back(Planet(100, 0, 0, -8, 100, false, true, sf::Color(255, 0, 0)));
	planets.push_back(Planet(-50, 50*sqrt(3), 4*sqrt(3), 4, 100, false, true, sf::Color(0, 255, 0)));
	planets.push_back(Planet(-50, -50*sqrt(3), -4*sqrt(3), 4, 100, false, true, sf::Color(0, 0, 255)));

	while (window.isOpen()) {
		frameClock.getElapsedTime();
		sf::sleep(sf::seconds(1.0 / FPS) - frameClock.getElapsedTime());
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::MouseMoved) handle_mouse_move(event, &window);
			if (event.type == sf::Event::MouseWheelScrolled) handle_mouse_scroll(event, &window);
		}
		window.clear();
		draw_all_planets(&window);
		window.display();
		update();
		frameClock.restart();
	}
}