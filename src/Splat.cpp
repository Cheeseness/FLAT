/*
Copyright 2012 Johan "SteelRaven7" Hassel

This file is part of FLAT.

FLAT is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

FLAT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FLAT.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "Splat.h"

Splat::Splat()
{
	//ctor
}

Splat::Splat(FrameContainer c)
{
	frame = c;
	index = 0;
}

Splat::~Splat()
{
	//dtor
}

void Splat::update() {
	index++;
	frame.setIndex(index);
	destroy = index == frame.getNumber()-1;
}

void Splat::setImageParameters(vec2 _position, float _scale, float _rotation) {
	frame.setImageParameters(_position, _scale*10.0f, _rotation);
}

void Splat::draw() {
	frame.draw();
}
