#ifndef DISPLAY_H
#define DISPLAY_H

#include <iostream>
#include <vector>
#include <string>
#include <exception>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <limits>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "Field.h"
#include "Contour.h"

using namespace std::chrono_literals;

constexpr char WINDOW_NAME[] = "Data";
constexpr double DEFEXP = 2.0;

const int MAX = std::numeric_limits<int>::max() / 2;
const double pi = std::acos(-1);

class Display {
public:
	Display(const std::size_t xdim_,
		const std::size_t ydim_, const std::size_t scale_,
		const std::size_t sleeptm_ = 100,
		const double logval_ = 2.0);

	void feed();
	void show();

	void perturbe();
	void setsleeptime(const std::size_t in);

	std::size_t width() const;
	std::size_t height() const;
	double currentLog() const;
	
private:
	void procEvents();
	void colorByNumber(int data,
		const std::size_t i);
	void buildSixths();
	void raiseLog();
	void lowerLog();

	std::size_t xdim, ydim, fxdim, fydim,
		buffsize, scale, sleeptm;
	double sixths[6];
	double logval, logdelta;

	Field field;
	Contour contour;

	sf::RenderWindow window;
	sf::Sprite sprite;
	sf::Texture texture;
	std::vector<sf::Uint8> pixels;
	bool pert;

};

#endif // !DISPLAY_H
