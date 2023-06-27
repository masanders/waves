#include "Display.h"

/* ========================================================
 * Display::Display()
 *	purpose: Primary constructor
 *	input: Initial values for xdim, ydim, scale, sleeptm,
 *		and logval
 *  output: none
 */
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
	//CONTOUR
	//fxdim = (xdim / scale) / 2;
	fxdim = xdim / scale;
	fydim = ydim / scale;
	buffsize = fxdim * fydim * 4;
	pert = false;
	field.setDim(fxdim, fydim);

	//CONTOUR
	//contour.init(xdim/2, ydim, logval);
	//contour.form(logval);
		

	pixels.resize(buffsize, 0);
	for (int i = 3; i < pixels.size(); i += 4)
		pixels[i] = 255;

	buildSixths();
}

/* ========================================================
 * Display::feed()
 *	purpose: Take raw data from field object and feed it 
 *		into a trexture to build a sprite
 *	input: Pulled from this display's field object
 *  output: Constructs this display's texture and sprite
 */
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

/* ========================================================
 * Display::show()
 *	purpose: Form the window and run the main render loop
 *	input: none
 *  output: none
 */
void Display::show()
{
	window.create(sf::VideoMode(xdim, ydim), WINDOW_NAME);

	// MAIN RENDER LOOP
	while (window.isOpen())
	{
		this->procEvents(); 

		if (pert)
			this->perturbe();
		this->feed();

		window.clear(sf::Color::Black);
		window.draw(sprite);
		//window.draw(contour.getSprite());
		window.display();

		field.step();
		std::this_thread::sleep_for(sleeptm * 1ms);
	}
}

/* ========================================================
 * Display::perturbe()
 *	purpose: Perturbe the field object in a concentric block
 *	input: none
 *  output: Perturbed display object
 */
void Display::perturbe()
{
	std::cout << "Perturbing field.\n";

	pnt where = { fxdim / 2, fydim / 2 };
	std::vector<pnt> lower;

	field.surround(where, lower);
	field.perturbe(where, 240);
	for (auto i : lower)
		field.perturbe(i, (240 / (8 / field.strength())));

	pert = false;
}

/* ========================================================
 * Display::setSleeptTime()
 *	purpose: Set the delay time between render loops
 *		in milliseconds
 *	input: in - new delay time in milliseconds
 *  output: none
 */
void Display::setSleepTime(const std::size_t in)
{
	sleeptm = in;
}

/* ========================================================
 * Display::width()
 *	purpose: Get display's width
 *	input: none
 *  output: RETURNED - width
 */
std::size_t Display::width() const
{
	return xdim;
}

/* ========================================================
 * Display::height()
 *	purpose: Get display's height
 *	input: none
 *  output: RETURNED - height
 */
std::size_t Display::height() const
{
	return ydim;
}

/* ========================================================
 * Display::currentLog()
 *	purpose: Get display's current log scaling
 *	input: none
 *  output: RETURNED - current log scaling
 */
double Display::currentLog() const
{
	return logval;
}

/* ========================================================
 * Display::colorByNumber()
 *	purpose: Linear approxamation of the HSV to RBG 
 *		translation, colors one pixel based on its value
 *	input: data - linear data value
 *		   i - index of location to be colored
 *  output: The indexed pixel is colored
 */
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
		blk1(loc, skew);
	else if ((skew >= sixths[0]) && (skew < sixths[1]))
		blk2(loc, skew);
	else if ((skew >= sixths[1]) && (skew < sixths[2]))
		blk3(loc, skew);
	else if ((skew >= sixths[2]) && (skew < sixths[3]))
		blk4(loc, skew);
	else if ((skew >= sixths[3]) && (skew < sixths[4]))
		blk5(loc, skew);
	else
		blk6(loc, skew);
}

/* ========================================================
 * Display::procEvents()
 *	purpose: Called each render loop to process user input
 *	input: Events are polled from the window object
 *  output: Reaction to encountered event
 */
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
				pert = true;
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

/* ========================================================
 * Display::buildSixths()
 *	purpose: The colorByNumber function requires breaking
 *		spectrum into sixths to turn a linear value into
 *		a three dimensional one
 *	input: Display's logval
 *  output: A new vector of sixths is produced
 */
void Display::buildSixths()
{
	sixths[5] = std::log(MAX) / std::log(logval);
	sixths[0] = sixths[5] / 6.0;
	sixths[1] = sixths[0] * 2;
	sixths[2] = sixths[0] * 3;
	sixths[3] = sixths[0] * 4;
	sixths[4] = sixths[0] * 5;
}

/* ========================================================
 * Display::raiseLog()
 *	purpose: Raise the display's log value by 10%
 *	input: none
 *  output: Display's logval is raised
 */
void Display::raiseLog()
{
	if (logval > (9.9 * logdelta))
		logdelta *= 10;

	logval += logdelta;
	buildSixths();
	contour.form(logval);
	std::cout << "Logarthmic +" << logdelta<< " to: " << logval << '\n';
}

/* ========================================================
 * Display::lowerLog()
 *	purpose: Lower the display's log value by 10%
 *	input: none
 *  output: Display's log value is lowered
 */
void Display::lowerLog()
{
	if (logval < (1.1 * logdelta))
		logdelta /= 10;

	logval -= logdelta;
	buildSixths();
	contour.form(logval);
	std::cout << "Logarthmic -" << logdelta<< " to: " << logval << '\n';
}

/* ========================================================
 * Display::blk1-6
 *	purpose: Helper functions of colorByNumber which
 *		perfrom the specific scaling of the individual
 *		segments
 *	input: loc - The location of the pixel in the vector
 *		   skew - the skewed data value as produced by
 *				colorByNumber
 *  output: The individual pixel's values are set
 */
void Display::blk1(const std::size_t loc, const double skew)
{
	pixels[loc] = 255;
	pixels[loc + 1] = (skew / sixths[0]) * 255;
	pixels[loc + 2] = 0;
}
void Display::blk2(const std::size_t loc, const double skew)
{
	pixels[loc] = ((sixths[0] - (skew - sixths[0]))/sixths[0]) * 255;
	pixels[loc + 1] = 255;
	pixels[loc + 2] = 0;
}
void Display::blk3(const std::size_t loc, const double skew)
{
	pixels[loc] = 0;
	pixels[loc + 1] = 255;
	pixels[loc + 2] = ((skew - sixths[1]) / sixths[0]) * 255;
}
void Display::blk4(const std::size_t loc, const double skew)
{
	pixels[loc] = 0;
	pixels[loc + 1] = ((sixths[0] - (skew - sixths[2]))/sixths[0]) * 255;
	pixels[loc + 2] = 255;
}
void Display::blk5(const std::size_t loc, const double skew)
{
	pixels[loc] = ((skew - sixths[3]) / sixths[0]) * 255;
	pixels[loc + 1] = 0;
	pixels[loc + 2] = 255;
}
void Display::blk6(const std::size_t loc, const double skew)
{
	pixels[loc] = 255;
	pixels[loc + 1] = 0;
	pixels[loc + 2] = ((sixths[0] - (skew - sixths[4]))/sixths[0]) * 255;
}