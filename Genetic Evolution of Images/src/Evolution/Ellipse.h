#pragma once
#include "Program/Parameters_Images.h"

struct ELLIPSE
{
	static inline EVOLUTION_PARAMETERS* EvolutionParametersPointer = nullptr;
	static inline EVOLUTION_IMAGES* EvolutionImagesPointer = nullptr;
	RECT Shape;
	int Red;
	int Green;
	int Blue;
	COLORREF Color;

	ELLIPSE();
	void operator=(const ELLIPSE&);
	void MutateShape();
	void MutateColor();
	void CheckBoundary();
};