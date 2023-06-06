#include "Contour.h"

Contour::Contour()
{
	xdim = 0;
	ydim = 0;
	buffsize = 0;
}

void Contour::init(const std::size_t xdim_,
	const std::size_t ydim_, const double logBase_)
{
	xdim = xdim_;
	ydim = ydim_;
	logBase = logBase_;
	buffsize = xdim * ydim * 4;	

	texture.create(xdim, ydim);
	sprite.setPosition(xdim, 0);
	pixels.resize(buffsize, 0);

	for (int i = 3; i < pixels.size(); i += 4)
		pixels[i] = 255;
}

void Contour::form(const double inLogBase)
{
	logBase = inLogBase;
	maxY = std::log1p(xdim) / std::log(logBase);

	for (std::size_t i = 0; i < xdim; i++)
		buildCol(i);
	
	texture.update(pixels.data());
	sprite.setTexture(texture);
}

const sf::Sprite& Contour::getSprite() const
{
	return sprite;
}

void Contour::buildCol(const std::size_t x)
{
	double raw =  std::log1p(x) / std::log(logBase);
	int loc = flatten( { x, (raw / maxY) * ydim } );

	loc *= 4;
	pixels[loc] = 255;
	pixels[loc + 1] = 100;
	pixels[loc + 2] = 0;
}

std::size_t Contour::flatten(const pnt in) const
{
	return (in.second * xdim) + in.first;
}
