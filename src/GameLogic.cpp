#include <Helper.hpp>
#include <GameLogic.hpp>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <concepts>
#include <math.h>

#define ll long long
#define GETBIT(mask, i) (((mask) >> (i)) & 1)

const int FACE = 3e5;
const float PI = atan(1) * 4;
sf::Font font;
std::string FONT_PATH = std::string(PROJECT_DIR) + "assets/Font/english.otf";
sf::VertexArray tri(sf::PrimitiveType::Triangles, FACE);

Point3 player_pos;
double pitch, yaw;
double current_second;
int frame_count;
int fps;

const double MOVEMENT_SPEED = 5, SENSITIVITY = 1;

void appStart(sf::RenderWindow& appwindow) {
	appwindow.setMouseCursorVisible(false);
	sf::Mouse::setPosition(sf::Vector2i(windowSize.x / 2, windowSize.y / 2), appwindow);
	player_pos = Point3(0, 0, 0);
	pitch = PI / 2;
	yaw = 0;

	current_second = 0;
	frame_count = 0;
	fps = 0;

	font.openFromFile(FONT_PATH.c_str());
}

void handle_keypress(float delta) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
		player_pos.y += delta * MOVEMENT_SPEED;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
		player_pos.y -= delta * MOVEMENT_SPEED;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		player_pos.z += delta * MOVEMENT_SPEED * cos(yaw);
		player_pos.x += delta * MOVEMENT_SPEED * sin(yaw);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		player_pos.z -= delta * MOVEMENT_SPEED * cos(yaw);
		player_pos.x -= delta * MOVEMENT_SPEED * sin(yaw);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		player_pos.x -= delta * MOVEMENT_SPEED * cos(yaw);
		player_pos.z += delta * MOVEMENT_SPEED * sin(yaw);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		player_pos.x += delta * MOVEMENT_SPEED * cos(yaw);
		player_pos.z -= delta * MOVEMENT_SPEED * sin(yaw);
	}
}


void handle_mouse_movement(sf::RenderWindow& appwindow) {
	sf::Vector2i center = sf::Vector2i(windowSize.x / 2, windowSize.y / 2);
	sf::Vector2i mousePos = sf::Mouse::getPosition(appwindow);
	sf::Vector2i mouseDelta = mousePos - center;
	sf::Mouse::setPosition(center, appwindow);
	
	yaw += mouseDelta.x * 0.001f * SENSITIVITY;
	pitch += mouseDelta.y * 0.001f * SENSITIVITY;
}

int pollEvent(sf::RenderWindow& appwindow) { // if window is closed, return 0
	int return_val = 1;
	while (const std::optional event = appwindow.pollEvent()) {
		if (event->is <sf::Event::Closed>() || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
			std::cerr << "Closing the window" << std::endl;
			appwindow.close();
			return 0;
		}
	}

	return return_val;
}

Point2 toCameraSpace(Point3 p) {
	Point2 tmp(p.x / p.z * 0.8 + 0.5, -p.y / p.z + 0.5);
	return Point2(tmp.x * windowSize.x, tmp.y * windowSize.y);
}

int face_indx = 0;
void reset_face() {
	face_indx = 0;
	for (int i = 0; i < FACE; ++i) {
		tri[i].position = Point2(0, 0);
	}
}

void draw_all_face(sf::RenderWindow& appwindow) {
	appwindow.draw(tri);
}

void add_triangle(Point2 a, Point2 b, Point2 c, sf::Color color) {
	for (int i = 0; i < 3; ++i) {
		tri[face_indx].position = a;
		tri[face_indx].color = color;


		std::swap(a, b);
		std::swap(a, c);

		face_indx++;
	}
}

void add_triangle_3d(Point3 a, Point3 b, Point3 c, sf::Color color) {
	if (std::min({ a.z, b.z, c.z }) < 0.2) return;
	add_triangle(toCameraSpace(a), toCameraSpace(b), toCameraSpace(c), color);
}

Point3 externalTransform(Point3 p) {
	p -= player_pos;
	// yaw transform
	Point3 u(p.x * cos(yaw) - p.z * sin(yaw), p.y, p.z * cos(yaw) + p.x * sin(yaw));
	// pitch transform
	Point3 v(u.x, u.y * sin(pitch) - u.z * cos(pitch), u.z * sin(pitch) + u.y * cos(pitch));

	return v;
}

void draw_cube(Point3 bruh) {
	Point3 vertex[8];
	for (int i = 0; i < 8; ++i) {
		if (GETBIT(i, 2)) vertex[i].y = bruh.y + 0.5;
		else vertex[i].y = bruh.y - 0.5;
		if (GETBIT(i, 0)) vertex[i].x = bruh.x + 0.5;
		else vertex[i].x = bruh.x - 0.5;
		if (GETBIT(i, 1)) vertex[i].z = bruh.z + 0.5;
		else vertex[i].z = bruh.z - 0.5;

		vertex[i] = externalTransform(vertex[i]);
	}

	Point3 diff;

	// dot product < 0 means visible. That's how geometry work I think
	Point3 normal[6] = {Point3(0, -1, 0), Point3(0, 1, 0) ,
						Point3(-1, 0, 0), Point3(1, 0, 0) ,
						Point3(0, 0, -1), Point3(0, 0, 1) };
	int used_vertex[6][4] = {
		{0, 1, 2, 3},
		{4, 5, 6, 7},
		{0, 2, 4, 6},
		{1, 3, 5, 7},
		{0, 1, 4, 5},
		{2, 3, 6, 7},
	};
	Point3 light_cast(-0.5, -2, -1);
	
	for (int f = 0; f < 6; ++f) {
		float light_intensity = dotProduct(light_cast, normal[f]);
		float brightness = 140 - light_intensity * 40;
		sf::Color cur = sf::Color(brightness, brightness, brightness);

		diff = bruh - player_pos + normal[f] * 0.5f;
		if (dotProduct(diff, normal[f]) < 0) {
			int a = used_vertex[f][0];
			int b = used_vertex[f][1];
			int c = used_vertex[f][2];
			int d = used_vertex[f][3];
			add_triangle_3d(vertex[a], vertex[b], vertex[c], cur);
			add_triangle_3d(vertex[d], vertex[b], vertex[c], cur);
		}
	}
}

void draw_text(sf::RenderWindow& appwindow) {
	sf::Text cur(font);
	cur.setCharacterSize(25);
	cur.setFillColor(sf::Color::White);
	
	std::string text = "FPS: " + std::to_string(fps) + ", Face count: " + std::to_string(face_indx / 3);
	cur.setString(text);
	
	appwindow.draw(cur);
}

void handle_timer(double delta) {
	current_second += delta;
	frame_count++;
	if (current_second >= 1) {
		fps = frame_count;
		frame_count = 0;
		current_second = 0;
	}
}

void appLoop(sf::RenderWindow& appwindow, float delta) {
	int cur = pollEvent(appwindow);
	if (cur == 0) return;
	appwindow.clear(sf::Color::Black);
	reset_face();
	handle_timer(delta);
	handle_keypress(delta);
	handle_mouse_movement(appwindow);

	draw_cube(Point3(1, -2, 7));

	draw_all_face(appwindow);
	draw_text(appwindow);
	appwindow.display();
}