#include "Field.h"

/* ========================================================
 * Field::Field()
 *	purpose: Default constructor
 *	input: none
 *  output: none
 */
Field::Field()
{
	xdim = 0;
	ydim = 0;
	live = false;
	coupling = 1.0f;
}

/* ========================================================
 * Field::Field()
 *	purpose: Primary constructor
 *	input: Initial values for xdim and ydim
 *  output: none
 */
Field::Field(const std::size_t xdim_, const std::size_t ydim_)
{
	xdim = xdim_;
	ydim = ydim_;
	live = false;
	coupling = 1.0f;

	data[live].resize(xdim * ydim, 0);
	data[!live].resize(xdim * ydim, 0);
}

/* ========================================================
 * Field::setDim()
 *	purpose: Set function for X and Y dimensions
 *	input: Set values for xdim and ydim
 *  output: This field's xdim and ydim are set
 */
void Field::setDim(const std::size_t xdim_, const std::size_t ydim_)
{
	xdim = xdim_;
	ydim = ydim_;
	live = false;

	data[live].resize(xdim * ydim, 0);
	data[!live].resize(xdim * ydim, 0);
}

/* ========================================================
 * Field::setCoupling()
 *	purpose: Set function for the field's coupling value
 *	input: in - new coupling value
 *  output: none
 */
void Field::setCoupling(const float in)
{
	coupling = in;
}

/* ========================================================
 * Field::tighten()
 *	purpose: Raise the field's coupling value by 10%
 *	input: none
 *  output: Coupling value is raised 
 */
void Field::tighten()
{
	coupling += 0.1;
	std::cout << "Coupling raised to: " << coupling << '\n';
}

/* ========================================================
 * Field::relax()
 *	purpose: Lower the field's coupling value by 10%
 *	input: none
 *  output: Coupling value is lowered
 */
void Field::relax()
{
	coupling -= 0.1;
	std::cout << "Coupling lowerd to: " << coupling << '\n';
}

/* ========================================================
 * Field::size()
 *	purpose: Give the size of the underlying data object
 *	input: none
 *  output: RETURNED - Size of data object
 */
std::size_t Field::size() const
{
	return data[live].size();
}

/* ========================================================
 * Field::strength()
 *	purpose: Get function for coupling value
 *	input: none
 *  output: RETURNED - value of coupling constant
 */
float Field::strength() const
{
	return coupling;
}

/* ========================================================
 * Field::operator [] ()
 *	purpose: Get data value by an index, internalize 
 *		which frame is live
 *	input: i - index value of data being addressed
 *  output: RETURNED - Value of live data being addressed
 */
int Field::operator[] (const std::size_t i) const
{
	return data[live][i];
}

/* ========================================================
 * Field::step()
 *	purpose: Propagate the wave function
 *	input: none
 *  output: The data object is altered as needed
 */
void Field::step()
{
	for (std::size_t i = 0; i < data[live].size(); i++)
	{
		std::vector<pnt> srd;
		int total = 0, cnt = 0;
		pnt loc = expand(i);

		surround(loc, srd);

		for (auto j : srd)
		{
			total += data[live][flatten(j)];
			cnt++;
		}
		cnt = cnt / coupling;
		data[!live][i] = (total / cnt);
	}
	
	live = !live;
}

/* ========================================================
 * Field::randomPopulate()
 *	purpose: Propagate the wave function
 *	input: none
 *  output: The data object is altered as needed
 */
void Field::randomPopulate()
{
	for (std::size_t i = 0; i < data[live].size(); i++)
	{
		data[live][i] = (200 * ((double)i / 
			(double)(data[live].size()))) - 100;
		data[!live][i] = (200 * ((double)i / 
			(double)(data[live].size()))) - 100;
	}
}

void Field::perturbe(const pnt loc, const int val)
{
	if (!boundChk(loc))
		throw std::out_of_range("Perturbe location out of range!");
	if ((val > 255) || (val < -255))
		throw std::out_of_range("Perturbe value out of range!");

	data[live][flatten(loc)] = val;
	data[!live][flatten(loc)] = val;
}

pnt Field::expand(const std::size_t in) const
{
	return { in % ydim, in / xdim };
}

std::size_t Field::flatten(const pnt& in) const
{
	return (in.second * xdim) + in.first;
}

bool Field::boundChk(const pnt& in) const
{
	if (in.first < 0 ||
		in.first >= xdim ||
		in.second < 0 ||
		in.second >= ydim)
		return false;
	else
		return true;

}
void Field::surround(pnt& in, std::vector<pnt>& out) const
{
	const std::size_t& X = in.first;
	const std::size_t& Y = in.second;

	// a b c  
	// d _ e 
	// f g h

	if (X > 1)
	{
		out.push_back({ X - 1, Y }); // d
		if (Y > 1)
			out.push_back({ X - 1, Y - 1 }); // a
		if (Y < ydim - 1)
			out.push_back({ X - 1, Y + 1 }); // f
	}
	if (X < xdim - 1)
	{
		out.push_back({ X + 1, Y }); // e
		if (Y > 1)
			out.push_back({ X + 1, Y - 1 }); // c
		if (Y < ydim - 1)
			out.push_back({ X + 1, Y + 1 }); // h
	}
	if(Y > 1)
		out.push_back({ X, Y - 1 }); // b
	if(Y < ydim - 1)
		out.push_back({ X, Y + 1 }); // g
}