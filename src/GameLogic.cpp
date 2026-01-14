#include <Helper.hpp>
#include <GameLogic.hpp>
#include <Entity.hpp>
#include <MediaPlayer.hpp>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <concepts>
#include <math.h>

#define ll long long
#define GETBIT(mask, i) (((mask) >> (i)) & 1)
#define ALL(v) (v).begin(), (v).end()

sf::Font font;
std::string FONT_PATH = std::string(PROJECT_DIR) + "assets/Font/english.otf";

int collision_count = 0;
std::vector<Entity> o;
MediaPlayer med;
int frame_cnt = 0, fps = 0;
double tot_delta = 0;

const Point2 billiard_table(1500, 750);
std::vector<Point2> hole;
Point2 screen_center;
const double R = 20;
Point2 affare;

bool board_idle() {
	for (auto &i : o)
		if (i.get_kinetic_energy() > 0) return false;
	return true;
}


void new_game() {
	o.clear();
	o.push_back(Entity(1, 0, Point2(windowSize.x / 2 - 500, windowSize.y / 2), Point2(0, 0)));

	std::vector<int> color(15);
	for (int i = 1; i < 15; ++i) {
		if (i <= 7) color[i] = 1;
		else color[i] = 2;
	}
	color[0] = 3;

	std::random_device rd;
	std::mt19937_64 rng(rd());
	std::shuffle(1 + ALL(color), rng);

	std::swap(color[0], color[4]);

	const int R1 = 20;
	int cnt = 0;

	for (int i = 1; i <= 5; ++i) {
		Point2 root_point = Point2(windowSize.x / 2 + 300, windowSize.y / 2);
		root_point.x += std::sqrt(3) * R1 * (i - 1);
		root_point.y -= R1 * (i - 1);

		for (int j = 0; j < i; ++j) {
			o.push_back(Entity(1, color[cnt++], Point2(root_point.x, root_point.y + R1 * j * 2)));
		}
	}
	affare = Point2(0, 0);

	med.play_audio(SoundEffect::DING);
}


void appStart(sf::RenderWindow& appwindow) {
	screen_center = Point2(windowSize.x / 2, windowSize.y / 2);
	font.openFromFile(FONT_PATH.c_str());

	med.init();
	med.setAudioVolume(100);
	collision_count = 0;

	new_game();

	for(int i = -1; i <= 1; i += 2)
		for (int j = -1; j <= 1; ++j) {
			Point2 cur = screen_center + Point2(billiard_table.x * j * 0.5, billiard_table.y * i * 0.5);
			hole.push_back(cur);
		}
}

float getRadius(Entity o) {
	return R;
}

bool pressing = false;
void handle_keypress(float delta) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N)) {
		pressing = true;
	}
	else {
		if (pressing == true) {
			new_game();
			pressing = false;
		}
	}
}


Point2 get_mouse_pos(sf::RenderWindow& appwindow) {
	auto tmp = sf::Mouse::getPosition(appwindow);
	return Point2(tmp.x, tmp.y);
}

bool is_clicking = false;
Point2 pre_mouse_pos;
void handle_mouse(sf::RenderWindow& appwindow, float delta) {
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		if (is_clicking == false) {
			pre_mouse_pos = get_mouse_pos(appwindow);
			is_clicking = true;
		}
	}
	else {
		if (is_clicking && board_idle()) {
			Point2 ball_pos = o[0].u;
			Point2 mouse_pos = get_mouse_pos(appwindow);
			float strength = dotProduct(pre_mouse_pos - ball_pos, pre_mouse_pos - mouse_pos)
				/ (pre_mouse_pos - ball_pos).length();
			if (strength > 10) {
				strength = pow(strength, 1.2);
				strength = std::min(strength, 5000.0f);
				Point2 diff = pre_mouse_pos - ball_pos;
				diff = diff.normalized();

				o[0].v += diff * strength;
				o[0].r_z = (affare.x / 100) * strength;

				o[0].r -= diff * (affare.y / 100) * strength;

				affare = Point2(0, 0);
			}
			else {
				Point2 center = Point2(windowSize.x - 110.0f, 110.0f);
				if ((mouse_pos - center).length() <= 100.0f) {
					affare = mouse_pos - center;
				}
			}

			is_clicking = false;
		}
	}
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


bool check_collide(Entity o1, Entity o2, float delta) {
	o1.progress(delta);
	o2.progress(delta);
	float r = getRadius(o1) + getRadius(o2);
	return ((o1.u - o2.u).length() <= r);
}

int check_collide_edge(Entity o,float delta) {
	o.progress(delta);
	int r = getRadius(o);
	int ans = 0;
	if (o.u.x - r <= windowSize.x / 2 - billiard_table.x / 2) ans |= 1;
	if (o.u.y - r <= windowSize.y / 2 - billiard_table.y / 2) ans |= 2;
	if (o.u.x + r >= windowSize.x / 2 + billiard_table.x / 2) ans |= 1;
	if (o.u.y + r >= windowSize.y / 2 + billiard_table.y / 2) ans |= 2;
	return ans;
}

void collision(Entity& o1, Entity& o2) {
	Point2 diff = (o1.u - o2.u).normalized();
	float kin = o1.get_kinetic_energy() + o2.get_kinetic_energy();
	float weight_diff = o1.m / o2.m;
		
	float l = 0.01, r = 4000;
	for (int it = 0; it < 100; ++it) {
		float mid = (l + r) / 2;
		Entity o3 = o1, o4 = o2;
		o3.v += diff * mid;
		o4.v -= diff * mid * weight_diff;

		if (o3.get_kinetic_energy() + o4.get_kinetic_energy() <= kin) l = mid;
		else r = mid;
	}
	o1.v += diff * l;
	o2.v -= diff * l * weight_diff;

	float half_spin = (o1.r_z * o1.m + o2.r_z * o2.m) / (o1.m + o2.m);
	o1.r_z -= half_spin;
	o2.r_z -= half_spin;
}

void handle_physics(float delta) {
	while (true) {
		bool found = false;
		for (int i = 0; i < (int)o.size(); ++i)
			for (int j = i + 1; j < (int)o.size(); ++j)
				if (check_collide(o[i], o[j], delta)) {
					found = true;
					collision(o[i], o[j]);
					o[i].r *= 0.9f;
					o[j].r *= 0.9f;
					med.play_audio(SoundEffect::CLACK);
					collision_count++;
				}
		if (found == false) break;
	}
	
	for (auto& o1 : o) {
		if (check_collide_edge(o1, delta)) {
			int cur = check_collide_edge(o1, delta);
			float amount = o1.r_z * 0.5f;
			o1.r_z *= 0.5f;
			if (GETBIT(cur, 0)) {
				if (o1.v.x > 0) 
					o1.v.y += amount;
				else o1.v.y -= amount;
				o1.v.x *= -1;
			}
			if (GETBIT(cur, 1)) {
				if (o1.v.y > 0)
					o1.v.x -= amount;
				else o1.v.x += amount;
				o1.v.y *= -1;
			}
			o1.v *= 0.9f;
			o1.r *= 0.9f;
			med.play_audio(SoundEffect::CLACK);
			collision_count++;
		}
	}

	for (auto& o1 : o) {
		o1.progress(delta);
	}
}

void draw_text(sf::RenderWindow& appwindow) {
	sf::Text dih1(font);
	std::string cur1 = "Collision Count: " + std::to_string(collision_count) 
		+ ", FPS: " + std::to_string(fps);

	dih1.setFillColor(sf::Color::White);
	dih1.setPosition(sf::Vector2f(0, 0));
	dih1.setCharacterSize(30);
	dih1.setString(cur1);

	appwindow.draw(dih1);

	sf::Text dih2(font);
	std::string cur2 = "Ball count: " + std::to_string(o.size());

	dih2.setFillColor(sf::Color::White);
	dih2.setPosition(sf::Vector2f(0, 40));
	dih2.setCharacterSize(30);
	dih2.setString(cur2);

	appwindow.draw(dih2);


	sf::Text dih3(font);
	std::string cur3 = "";
	if (is_clicking) {
		cur3 = "Is clicking! Pre mouse pos: " + std::to_string(pre_mouse_pos.x) + " " + std::to_string(pre_mouse_pos.y);
	}
	else cur3 = "Is not clicking! ";
	

	dih3.setFillColor(sf::Color::White);
	dih3.setPosition(sf::Vector2f(0, 80));
	dih3.setCharacterSize(30);
	dih3.setString(cur3);

	appwindow.draw(dih3);
}

void draw_object(sf::RenderWindow& appwindow) {
	for (auto &o1 : o) {
		sf::CircleShape cyka1(getRadius(o1));

		sf::Color color;
		switch (o1.t) {
			case 0: 
				color = sf::Color(255, 255, 255);
				break;
			case 1:
				color = sf::Color(149, 225, 211);
				break;
			case 2:
				color = sf::Color(243, 129, 129);
				break;
			case 3:
				color = sf::Color(30, 30, 30);
				break;

		}
		
		
		cyka1.setFillColor(color);
		cyka1.setOrigin(cyka1.getLocalBounds().size * 0.5f);
		cyka1.setPosition(o1.u);
		appwindow.draw(cyka1);
	}
}

void draw_billiard_table(sf::RenderWindow& appwindow) {
	sf::Vector2f sz = sf::Vector2f(windowSize.x, windowSize.y);
	sf::Vector2f center = sz * 0.5f;
	sf::Vector2f offsetX(billiard_table.x / 2, 0), offsetY(0, billiard_table.y / 2);
	sf::ConvexShape table(4);
	table.setPoint(0, center - offsetX - offsetY);
	table.setPoint(1, center + offsetX - offsetY);
	table.setPoint(2, center + offsetX + offsetY);
	table.setPoint(3, center - offsetX + offsetY);
	table.setFillColor(sf::Color(214, 247, 173));

	table.setOutlineThickness(20);
	table.setOutlineColor(sf::Color(252, 227, 138));
	appwindow.draw(table);

	for (auto& i : hole) {
		sf::CircleShape cyka(40.0f);
		cyka.setFillColor(sf::Color::Black);
		cyka.setOrigin(cyka.getLocalBounds().size * 0.5f);
		cyka.setPosition(i);
		appwindow.draw(cyka);
	}

}

void draw_billiard_cue(sf::RenderWindow& appwindow) {
	if (!board_idle()) return;
	Point2 ball_pos = o[0].u;
	Point2 mouse_pos = get_mouse_pos(appwindow);

	float strength = 0;
	if (is_clicking) {
		strength = dotProduct(pre_mouse_pos - ball_pos, pre_mouse_pos - mouse_pos)
			/ (pre_mouse_pos - ball_pos).length();
		if (strength <= 10) strength = 0;

		mouse_pos = pre_mouse_pos;
	}

	sf::RectangleShape rectangle;
	rectangle.setSize(Point2(500, 20));
	rectangle.setFillColor(sf::Color(44, 76, 59));
	rectangle.setOrigin(Point2(-30 - std::pow(strength, 0.7), 10));
	rectangle.setPosition(ball_pos);
	rectangle.setRotation((ball_pos - mouse_pos).angle());
	appwindow.draw(rectangle);
	
}

void draw_billard_line(sf::RenderWindow& appwindow) {
	if (!board_idle()) return;
	Point2 ball_pos = o[0].u;
	Point2 mouse_pos = get_mouse_pos(appwindow);
	if (is_clicking) {
		mouse_pos = pre_mouse_pos;
	}

	Point2 diff = (mouse_pos - ball_pos).normalized();
	Point2 projected_ball = ball_pos;
	while (true) {
		bool found = false;

		for (auto i : o) if (i.t != 0) {
			Point2 mixi = i.u;
			if ((mixi - projected_ball).length() <= R * 2)
				found = true;
		}

		if (projected_ball.x - R <= windowSize.x / 2 - billiard_table.x / 2) found = true;
		if (projected_ball.y - R <= windowSize.y / 2 - billiard_table.y / 2) found = true;
		if (projected_ball.x + R >= windowSize.x / 2 + billiard_table.x / 2) found = true;
		if (projected_ball.y + R >= windowSize.y / 2 + billiard_table.y / 2) found = true;

		if (found) {
			sf::CircleShape cyka(R - 4);
			cyka.setOutlineColor(sf::Color::White);
			cyka.setOutlineThickness(4);
			cyka.setFillColor(sf::Color::Transparent);
			cyka.setPosition(projected_ball);
			cyka.setOrigin(cyka.getLocalBounds().size * 0.5f);


			sf::RectangleShape line;
			line.setSize(Point2((projected_ball - ball_pos).length() - R, 4));
			line.setOrigin(Point2(-R, 2));
			line.setFillColor(sf::Color::White);
			line.setPosition(ball_pos);
			line.setRotation((projected_ball - ball_pos).angle());
			

			appwindow.draw(cyka);
			appwindow.draw(line);
			break;
		}
		
		projected_ball += diff;
	}
}

void draw_billiard_spin(sf::RenderWindow& appwindow) {
	Point2 center = Point2(windowSize.x - 110.0f, 110.0f);

	sf::CircleShape cyka(100.0f);
	cyka.setOrigin(cyka.getLocalBounds().size * 0.5f);
	cyka.setFillColor(sf::Color(220, 220, 220));
	cyka.setPosition(center);
	appwindow.draw(cyka);

	sf::CircleShape af(10.0f);
	af.setOrigin(af.getLocalBounds().size * 0.5f);
	af.setFillColor(sf::Color::Red);
	af.setPosition(center + affare);
	appwindow.draw(af);
}

void handle_ball(float delta) {
	for (int i = 0; i < (int)o.size(); ) {
		Point2 ball_pos = o[i].u;
		bool die_for_sho = false;
		for (auto hole_pos : hole) {
			if ((hole_pos - ball_pos).length() <= 40.f) {
				die_for_sho = true;
			}
		}
		if (die_for_sho) {
			o.erase(o.begin() + i);
			med.play_audio(SoundEffect::DING);
		}
		else {
			i++;
		}
	}

	bool check = false;
	for (auto i : o) if (i.t == 3) check = true;
	if (check == false) {
		new_game();
		return;
	}

	if (board_idle() && (o.empty() || o[0].t != 0)) {
		while (true) {
			Point2 lmao(rngesus(-200, 200), rngesus(-200, 200));
			lmao += screen_center;
			
			bool check = true;
			for (auto i : o) {
				Point2 bruh = i.u;
				if ((bruh - lmao).length() < getRadius(i) * 2)
					check = false;
			}
			if (check) {
				o.insert(o.begin(), Entity(1, 0, lmao));
				break;
			}
		}
	}
}

void appLoop(sf::RenderWindow& appwindow, float delta) {
	int cur = pollEvent(appwindow);
	if (cur == 0) return;
	appwindow.clear(sf::Color(30, 30, 30));
	handle_mouse(appwindow, delta);

	draw_billiard_table(appwindow);

	tot_delta += delta;
	frame_cnt++;
	if (tot_delta >= 1) {
		tot_delta = 0;
		fps = frame_cnt;
		frame_cnt = 0;
	}

	handle_ball(delta);
	handle_keypress(delta);
	handle_physics(delta);
	draw_object(appwindow);
	draw_text(appwindow);
	draw_billiard_cue(appwindow);
	draw_billard_line(appwindow);
	draw_billiard_spin(appwindow);

	appwindow.display();
}