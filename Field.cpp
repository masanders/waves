#include "Field.h"

Field::Field()
{
	xdim = 0;
	ydim = 0;
	live = false;
	coupling = 1.0f;
}

Field::Field(const std::size_t xdim_, const std::size_t ydim_)
{
	xdim = xdim_;
	ydim = ydim_;
	live = false;
	coupling = 1.0f;

	data[live].resize(xdim * ydim, 0);
	data[!live].resize(xdim * ydim, 0);
}

void Field::setDim(const std::size_t xdim_, const std::size_t ydim_)
{
	xdim = xdim_;
	ydim = ydim_;
	live = false;

	data[live].resize(xdim * ydim, 0);
	data[!live].resize(xdim * ydim, 0);
}

void Field::setCoupling(const float in)
{
	coupling = in;
}

void Field::tighten()
{
	coupling += 0.1;
	std::cout << "Coupling raised to: " << coupling << '\n';
}

void Field::relax()
{
	coupling -= 0.1;
	std::cout << "Coupling lowerd to: " << coupling << '\n';
}

std::size_t Field::size() const
{
	return data[live].size();
}

float Field::strength() const
{
	return coupling;
}

int Field::operator[] (const std::size_t i) const
{
	return data[live][i];
}

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
		//data[!live][i] = std::abs((total / cnt) - data[live][i]);
		data[!live][i] = (total / cnt);
	}
	
	live = !live;
}

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
	std::size_t& X = in.first;
	std::size_t& Y = in.second;

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