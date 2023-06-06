#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <utility>
#include <memory>
#include <iostream>

#include "tmr.hpp"

typedef std::pair<std::size_t, std::size_t> pnt;

class Field {
public:
	Field();
	Field(const std::size_t xdim_, const std::size_t ydim_);

	void setDim(const std::size_t xdim_, const std::size_t ydim_);
	void setCoupling(const float in);
	void tighten();
	void relax();
	
	void step();
	void randomPopulate();
	void perturbe(const pnt loc, const int val);

	std::size_t size() const;
	float strength() const;
	int operator [] (const std::size_t i) const;

	void surround(pnt& in, std::vector<pnt>& out) const;
	pnt expand(const std::size_t in) const;
	std::size_t flatten(const pnt& in) const;

private:
	bool boundChk(const pnt& in) const;

	std::size_t xdim, ydim;
	float coupling;
	std::vector<int> data[2];
	bool live;
};

#endif // !FIELD_H
