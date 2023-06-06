#ifndef CONTOUR_H
#define CONTOUR_H

#include <iostream>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Field.h"

constexpr char CONTOUR_WINDOW_NAME[] = "Logarthmic Contour";

class Contour {
public:
	Contour();

	void init(const std::size_t xdim_, const std::size_t ydim_,
		const double logBase_);
	void form(const double inLogBase);

	const sf::Sprite& getSprite() const;

private:
	void buildCol(const std::size_t x);
	std::size_t flatten(const pnt in) const;

	std::size_t xdim, ydim, buffsize;
	double logBase, maxY;
	sf::Sprite sprite;
	sf::Texture texture;
	std::vector<sf::Uint8> pixels;

};

#endif  // CONTOUR_H