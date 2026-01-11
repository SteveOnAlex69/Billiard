#include <SFML/Graphics.hpp>
#include <iostream>
#include <Helper.hpp>
#include <GameLogic.hpp>

sf::RenderWindow appWindow;


int main(int argv, char* args[]) {
	appWindow = sf::RenderWindow(sf::VideoMode(windowSize), "3D Tester",
		sf::Style::Titlebar | sf::Style::Close);

	if (appWindow.isOpen() == false) {
		std::cerr << "Error creating window" << std::endl;
		return false;
	}

	appStart(appWindow);

	clock_t prev = clock();
	while (appWindow.isOpen()) {
		clock_t cur = clock();
		int efn = cur - prev;
		if (efn > 1) {
			prev = cur;
			appLoop(appWindow, efn * 0.001f);
		}
	}

	return EXIT_SUCCESS;
}