#pragma once

class Mandelbrot
{
public:
	int maxIt;
	double RealStart, ImagStart;
	float width;

	Mandelbrot(int i, double rStart, double iStart, float w);
	~Mandelbrot();
};
