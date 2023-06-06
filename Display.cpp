#include "Display.h"

Display::Display(const std::size_t xdim_,
	const std::size_t ydim_, const std::size_t scale_,
	const std::size_t sleeptm_,
	const double logval_)
{
	xdim = xdim_;
	ydim = ydim_;
	scale = scale_;
	sleeptm = sleeptm_;
	logval = logval_;
	logdelta = 0.1;
	fxdim = (xdim / scale) / 2;
	fydim = ydim / scale;
	buffsize = fxdim * fydim * 4;
	pert = false;
	field.setDim(fxdim, fydim);

	contour.init(xdim/2, ydim, logval);
	contour.form(logval);
		

	pixels.resize(buffsize, 0);
	for (int i = 3; i < pixels.size(); i += 4)
		pixels[i] = 255;

	buildSixths();
}

void Display::feed()
{
	std::size_t fsize = field.size();
	if ((field.size() * 4) != (buffsize))
		throw std::out_of_range("Feeding display incorrect buffer size.");

	for (std::size_t i = 0; i < fsize; i++)
		colorByNumber(field[i], i);
	
	texture.create(fxdim, fydim);
	texture.update(pixels.data());
	sprite.setTexture(texture);
	sprite.setScale(sf::Vector2f(scale, scale));
	sprite.setPosition(0.0f, 0.0f);

}

void Display::show()
{
	window.create(sf::VideoMode(xdim, ydim), WINDOW_NAME);


	while (window.isOpen())
	{
		this->procEvents(); 

		if (pert)
		{
			pnt where = { fxdim / 2, fydim / 2 };
			std::vector<pnt> lower;
			field.surround(where, lower);
			field.perturbe(where,
				240);
			for (auto i : lower)
				field.perturbe(i, (240 / (8 / field.strength())));
			pert = false;
		}
		this->feed();

		window.clear(sf::Color::Black);
		window.draw(sprite);
		window.draw(contour.getSprite());
		window.display();

		field.step();
		std::this_thread::sleep_for(sleeptm * 1ms);
	}
}

void Display::perturbe()
{
	std::cout << "Perturbing field.\n";
	pert = true;
}

void Display::setsleeptime(const std::size_t in)
{
	sleeptm = in;
}

std::size_t Display::width() const
{
	return xdim;
}

std::size_t Display::height() const
{
	return ydim;
}

double Display::currentLog() const
{
	return logval;
}

void Display::colorByNumber(int data,
	const std::size_t i)
{
	const std::size_t loc = i * 4;

	if (data == 0)
	{
		pixels[loc] = 0;
		pixels[loc + 1] = 0;
		pixels[loc + 2] = 0;

		return;
	}

	if (data > MAX)
		data = MAX;

	double skew = std::log1p(data) / std::log1p(logval);

	if (skew < sixths[0])
	{
		// Block 1
		pixels[loc] = 255;
		pixels[loc + 1] = (skew / sixths[0]) * 255;
		pixels[loc + 2] = 0;
	}
	else if ((skew >= sixths[0]) && (skew < sixths[1]))
	{
		// Block 2
		pixels[loc] = ((sixths[0] - (skew - sixths[0]))/sixths[0]) * 255;
		pixels[loc + 1] = 255;
		pixels[loc + 2] = 0;
	}
	else if ((skew >= sixths[1]) && (skew < sixths[2]))
	{
		// Block 3
		pixels[loc] = 0;
		pixels[loc + 1] = 255;
		pixels[loc + 2] = ((skew - sixths[1]) / sixths[0]) * 255;
	}
	else if ((skew >= sixths[2]) && (skew < sixths[3]))
	{
		// Block 4
		pixels[loc] = 0;
		pixels[loc + 1] = ((sixths[0] - (skew - sixths[2]))/sixths[0]) * 255;
		pixels[loc + 2] = 255;
	}
	else if ((skew >= sixths[3]) && (skew < sixths[4]))
	{
		// Block 5
		pixels[loc] = ((skew - sixths[3]) / sixths[0]) * 255;
		pixels[loc + 1] = 0;
		pixels[loc + 2] = 255;
	}
	else
	{
		// Block 6
		pixels[loc] = 255;
		pixels[loc + 1] = 0;
		pixels[loc + 2] = ((sixths[0] - (skew - sixths[4]))/sixths[0]) * 255;
	}
}

void Display::procEvents()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			window.close();
		}
		else if (event.type == sf::Event::KeyReleased)
		{
			if (event.key.code == sf::Keyboard::Q
				|| event.key.code == sf::Keyboard::Escape)
				window.close();
			else if (event.key.code == sf::Keyboard::P)
				this->perturbe();
			else if (event.key.code == sf::Keyboard::Up)
				field.tighten();
			else if (event.key.code == sf::Keyboard::Down)
				field.relax();
			else if (event.key.code == sf::Keyboard::Left)
				this->lowerLog();
			else if (event.key.code == sf::Keyboard::Right)
				this->raiseLog();
		}
	}
}
void Display::buildSixths()
{
	sixths[5] = std::log1p(MAX) / std::log(logval);
	sixths[0] = sixths[5] / 6.0;
	sixths[1] = sixths[0] * 2;
	sixths[2] = sixths[0] * 3;
	sixths[3] = sixths[0] * 4;
	sixths[4] = sixths[0] * 5;
}

void Display::raiseLog()
{
	if (logval > (9.9 * logdelta))
		logdelta *= 10;

	logval += logdelta;
	buildSixths();
	contour.form(logval);
	std::cout << "Logarthmic +" << logdelta<< " to: " << logval << '\n';
}

void Display::lowerLog()
{
	if (logval < (1.1 * logdelta))
		logdelta /= 10;

	logval -= logdelta;
	buildSixths();
	contour.form(logval);
	std::cout << "Logarthmic -" << logdelta<< " to: " << logval << '\n';
}