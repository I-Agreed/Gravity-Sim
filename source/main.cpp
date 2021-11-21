#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include "Planet.hpp"
#include "Vector2.hpp"

#define PI 3.14159265
using namespace std;

const float G = 1; // gravitational constant
const int FPS = 60;
const float PHYS_SCALE = 0.4; // physics time scale
const bool COLLISIONS = true; // enable collisions
const int START_WIDTH = 800; // starting screen width
const int START_HEIGHT = 600; // starting screen height
const int TRAIL_LENGTH = 120; // number of points in trail
const int TRAIL_WIDTH = 5; // maximum trail width
const float TRAIL_ALPHA = 0.5; // base trail alpha value
const float PLANET_CREATE_VEL_SCALE = 0.05;
const float ARROW_HEAD_SIZE = 20;
const int ARROW_LINE_WIDTH = 4;

IA::Vector2f cameraPos(START_WIDTH/2, START_HEIGHT/2);
float zoom = 1;
float zoomSensitivity = 0.3; // amount zoomed per scroll, 1 = 2x zoom
sf::Color arrowColor = sf::Color::White;

vector<Planet> planets;
sf::Clock frameClock;
bool creatingPlanet = false;
IA::Vector2f planetCreatePos;
bool mouseMoved = false;
IA::Vector2i lastMousePos;
bool paused = false;

vector<Planet> savedPlanets;
IA::Vector2f savedCameraPos;
float savedZoom;
int width = START_WIDTH;
int height = START_HEIGHT;

IA::Vector2f direction(IA::Vector2f p1, IA::Vector2f p2) { // returns direction vector from p1 to p2
	IA::Vector2f p3 = p2 - p1;
	return p3 / p3.magnitude();
}

IA::Vector2f to_screen(IA::Vector2f p) { // convert position to screen coord
	return p/zoom + cameraPos;
}

IA::Vector2f to_pos(IA::Vector2f p) { // convert screen coord to position
	return (p - cameraPos) * zoom;
}

sf::Color random_colour() {
	int colour = rand() % 3; // primary colour index
	int second = (rand() % 2) * 2 + 2; // Pick second colour index
	int rgb[3] = {0, 0, 0};
	rgb[colour] = 255; // ensures full brightness
	rgb[(colour + second) % 3] = rand() % 255;
	return sf::Color(rgb[0], rgb[1], rgb[2]);
}

void add_trail(Planet* p) {
	float trailSize = (-1/(p->vel.magnitude()/4+1) + 1) * TRAIL_WIDTH; // when moving faster, the trail is thicker
	IA::Vector2f normal = IA::Vector2f(-p->vel.y, p->vel.x).norm();
	p->trail.push_back(p->pos + normal*trailSize);
	p->trail.push_back(p->pos - normal*trailSize);
	if (p->trail.size() > TRAIL_LENGTH * 2) {
		p->trail.pop_front();
		p->trail.pop_front(); // each trail 'point' is actually two points
	}
}

void update() {
	if (!paused) {
		for (int i = 0; i < planets.size(); i++) {
			if (!planets[i].fixed) {
				IA::Vector2f force = IA::Vector2f(0,0);
				for (int j = 0; j < planets.size(); j++) {
					if (j != i) {
						force += planets[j].mass * direction(planets[i].pos, planets[j].pos) / powf(planets[i].pos.distance(planets[j].pos), 2);
					}
				}
				planets[i].vel += G * force * PHYS_SCALE;
			}
		}

		for (int i = 0; i < planets.size(); i++) {
			if (planets[i].createTrail && !planets[i].fixed) {
				add_trail(&planets[i]);
			}
			if (!planets[i].fixed) {
				planets[i].pos += planets[i].vel * PHYS_SCALE;
			}
		}
		if (COLLISIONS) {
			for (int i = 0; i < planets.size(); i++) {
				for (int j = 0; j < planets.size(); j++) {
					if (i != j) {
						// Check planets are in contact
						if (planets[i].pos.distance(planets[j].pos) < planets[i].size + planets[j].size) {

							// This math here calculates the number of physics steps since the two planets collided based on
							// their positions and velocities. I dont want to work this out again. This is probably not very efficient btw.
							// This would probably also get very messy on a three way collision.
							double px = planets[i].pos.x - planets[j].pos.x;
							double py = planets[i].pos.y - planets[j].pos.y;
							double vx = planets[i].vel.x - planets[j].vel.x;
							double vy = planets[i].vel.y - planets[j].vel.y;
							double r = planets[i].size + planets[j].size;
							double a = pow(vx, 2) + pow(vy, 2);
							double b = 2 * (px * vx + py * vy);
							double c = pow(px, 2) + pow(py, 2) - pow(r, 2);
							// Quadratic formula, gives two solutions but we want the larger one.
							double t = (-b + sqrt(pow(b, 2) - 4 * a * c)) / (2 * a);
							// Subtract velocity * t
							planets[i].pos -= planets[i].vel * t * PHYS_SCALE;
							planets[j].pos -= planets[j].vel * t * PHYS_SCALE;
							// Cancel out velocity along normal
							IA::Vector2f rel = (planets[j].pos - planets[i].pos).norm();
							IA::Vector2f normal = IA::Vector2f(-rel.y, rel.x);
							// Solve elastic collision
							double ui = planets[i].vel.dot(rel);
							double uj = planets[j].vel.dot(rel);
							double totalMass = planets[i].mass + planets[j].mass;
							double vi = (ui * (planets[i].mass - planets[j].mass) + uj * (2 * planets[j].mass)) / totalMass;
							double vj = (uj * (planets[j].mass - planets[i].mass) + ui * (2 * planets[i].mass)) / totalMass;
							planets[i].vel = normal * (normal.dot(planets[i].vel)) + rel * vi;
							planets[j].vel = normal * (normal.dot(planets[j].vel)) + rel * vj;
							// Add back corrected velocities
							planets[i].pos += planets[i].vel * t * PHYS_SCALE;
							planets[j].pos += planets[j].vel * t * PHYS_SCALE;
						}
					}
				}
			}
		}
	}
}

void draw_trail(Planet p, sf::RenderWindow* win) {
	if (p.createTrail && !p.fixed) {
		sf::VertexArray line(sf::TriangleStrip, p.trail.size() + 2); // add two points to connect trail to planet
		for (int i = 0; i < p.trail.size(); i++) {
			line[i].position = to_screen(p.trail[i]);
			float colourScale = (i/(p.trail.size()-1.f)) * TRAIL_ALPHA;
			line[i].color = sf::Color(p.colour.r, p.colour.g, p.colour.b, p.colour.a *  colourScale);
		}
		float trailSize = (-1/(p.vel.magnitude()/4+1) + 1) * p.size; // connect trail to planet with those 2 extra points
		IA::Vector2f normal = IA::Vector2f(-p.vel.y, p.vel.x).norm();
		line[p.trail.size()].position = to_screen(p.pos + normal*trailSize);
		line[p.trail.size()].color = sf::Color(p.colour.r, p.colour.g, p.colour.b, p.colour.a * TRAIL_ALPHA);
		line[p.trail.size() + 1].position = to_screen(p.pos - normal*trailSize);
		line[p.trail.size() + 1].color = sf::Color(p.colour.r, p.colour.g, p.colour.b, p.colour.a * TRAIL_ALPHA);
		win->draw(line);
	}
}

void draw_planet(Planet p, sf::RenderWindow* win) {
	sf::CircleShape circle;
	circle.setRadius(p.size/zoom);
	circle.setFillColor(p.colour);
	circle.setPosition(to_screen(p.pos - IA::Vector2f(p.size, p.size)));

	win->draw(circle);
}

void draw_arrow(IA::Vector2f start, IA::Vector2f end, sf::RenderWindow* win) {
	float angle = atan2f(end.y - start.y, end.x - start.x) * 180 / PI;
	float dist = start.distance(end);

	sf::RectangleShape line(IA::Vector2f(dist - ARROW_HEAD_SIZE, ARROW_LINE_WIDTH)); // Arrow Line
	line.setOrigin(IA::Vector2f(0, ARROW_LINE_WIDTH/2));
	line.setPosition(start);
	line.setRotation(angle);
	line.setFillColor(arrowColor);

	sf::ConvexShape head(3); // Arrowhead
	head.setPoint(0, IA::Vector2f(ARROW_HEAD_SIZE, ARROW_HEAD_SIZE/2));
	head.setPoint(1, IA::Vector2f(0, ARROW_HEAD_SIZE));
	head.setPoint(2, IA::Vector2f(0, 0));
	head.setOrigin(IA::Vector2f(-dist + ARROW_HEAD_SIZE, ARROW_HEAD_SIZE/2));
	head.setPosition(start);
	head.setRotation(angle);
	head.setFillColor(arrowColor);

	win->draw(line);
	win->draw(head);
}

void draw_all(sf::RenderWindow* win) {
	for (Planet p : planets) {
		draw_trail(p, win);
	}
	for (Planet p : planets) {
		draw_planet(p, win);
	}
	if (creatingPlanet) {
		IA::Vector2i mousePosi = sf::Mouse::getPosition(*win);
		IA::Vector2f mousePos(mousePosi.x, mousePosi.y);
		draw_arrow(planetCreatePos, mousePos, win);
	}
}

void save_state() {
	savedPlanets.clear();
	for (Planet p:planets) {
		savedPlanets.push_back(p.copy());
	}
	savedZoom = zoom;
	savedCameraPos = cameraPos;
}

void load_state() {
	planets.clear();
	for (Planet p:savedPlanets) {
		planets.push_back(p.copy());
	}
	zoom = savedZoom;
	cameraPos = savedCameraPos;
	creatingPlanet = false;
}

void handle_mouse_move(sf::Event e, sf::RenderWindow* win) {
	if (!creatingPlanet) {
		IA::Vector2i pos = sf::Mouse::getPosition(*win);
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				IA::Vector2i change = pos - lastMousePos;
				cameraPos += IA::Vector2f(change.x, change.y);
		}
		lastMousePos = pos;
		mouseMoved = true;
	}
}

void handle_mouse_scroll(sf::Event e, sf::RenderWindow* win) {
	if (!creatingPlanet) {
		zoom *= pow(2, -e.mouseWheelScroll.delta*zoomSensitivity);
		IA::Vector2f pos(e.mouseWheelScroll.x, e.mouseWheelScroll.y);
		cameraPos -= pos;
		cameraPos *= powf(2, e.mouseWheelScroll.delta*zoomSensitivity);
		cameraPos += pos;
	}
}

void handle_mouse_press(sf::Event e) {
	if (e.mouseButton.button == sf::Mouse::Button::Left) {
		mouseMoved = false;
	} else if (e.mouseButton.button == sf::Mouse::Button::Right) {
		creatingPlanet = false;
	}
}

void handle_mouse_release(sf::Event e) {
	if (e.mouseButton.button == sf::Mouse::Button::Left) {
		IA::Vector2f pos = IA::Vector2f(e.mouseButton.x, e.mouseButton.y);
		if (creatingPlanet) { // finish creating planet
			IA::Vector2f vel = PLANET_CREATE_VEL_SCALE * (pos - planetCreatePos) * zoom;
			IA::Vector2f createPos = to_pos(planetCreatePos);
			planets.push_back(Planet(createPos.x, createPos.y, vel.x, vel.y, 100, false, true, random_colour()));
			creatingPlanet = false;
		} else { // begin creating planet
			if (!mouseMoved) {
				creatingPlanet = true;
				planetCreatePos = pos;
			}
		}
	}
}

void handle_key_press(sf::Event e) {
	switch (e.key.code) {
		case (sf::Keyboard::S): save_state(); break;
		case (sf::Keyboard::L): load_state(); break;
		case (sf::Keyboard::Space): paused = !paused; break;
	}
}

void handle_resize(sf::Event e, sf::RenderWindow* win) {
	IA::Vector2u prevSize = win->getSize();
	sf::FloatRect area(0, 0, e.size.width, e.size.height);
    win->setView(sf::View(area));
	cout << e.size.width - area.width << "\n";
	cameraPos -= IA::Vector2f(width - area.width, height - area.height) / 2.f; // keep camera centred
	width = area.width;
	height = area.height;
}

string debug_info() {
	string out = "";
	IA::Vector2f momentum(0, 0);
	for (int i = 0; i < planets.size(); i++) {
		momentum += planets[i].vel * planets[i].mass;
	}
	out += "Momentum: ";
	out += momentum;
	out += "\n";

	double Ek = 0;
	for (int i = 0; i < planets.size(); i++) {
		Ek += pow(planets[i].vel.magnitude(), 2) * planets[i].mass / 2;
	}
	out += "Ek: ";
	out += to_string(Ek);
	out += "\n";

	double U = 0;
	for (int i = 0; i < planets.size(); i++) {
		for (int j = 0; j < planets.size(); j++) {
			if (i != j) {
				U += -G * planets[i].mass * planets[j].mass / planets[i].pos.distance(planets[j].pos);
			}
		}
	}
	out += "U: ";
	out += to_string(U);
	out += "\n";

	out += "Net E: ";
	out += to_string(U + Ek);
	out += "\n";

	return out;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(START_WIDTH, START_HEIGHT), "Gravity Sim");
	planets.push_back(Planet(0, 0, 0, 0, 10000, true));
	planets.push_back(Planet(0, 70, 11, 0, 30, false, true, sf::Color(204, 185, 182)));
	planets.push_back(Planet(150, 0, 0, -8, 100, false, true, sf::Color(0, 170, 255)));
	planets.push_back(Planet(170, 0, 0, -5.8, 10, false, true, sf::Color(146, 152, 156)));
	planets.push_back(Planet(-200, 0, 0, 7.4, 60, false, true, sf::Color(250, 96, 0)));


	save_state(); // Save default state
	while (window.isOpen()) {
		frameClock.getElapsedTime();
		sf::sleep(sf::seconds(1.0 / FPS) - frameClock.getElapsedTime());
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::MouseMoved) handle_mouse_move(event, &window);
			if (event.type == sf::Event::MouseWheelScrolled) handle_mouse_scroll(event, &window);
			if (event.type == sf::Event::MouseButtonPressed) handle_mouse_press(event);
			if (event.type == sf::Event::MouseButtonReleased) handle_mouse_release(event);
			if (event.type == sf::Event::KeyPressed) handle_key_press(event);
			if (event.type == sf::Event::Resized) handle_resize(event, &window);
		}
		window.clear();
		draw_all(&window);
		window.display();
		update();
		frameClock.restart();
	}
}