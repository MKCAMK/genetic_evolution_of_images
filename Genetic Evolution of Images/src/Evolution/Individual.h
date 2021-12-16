#pragma once
#include "Evolution/Ellipse.h"
#include "Program/Parameters_Images.h"
#include "common/Settings.h"

class INDIVIDUAL
{
public:
	static inline EVOLUTION_PARAMETERS* EvolutionParametersPointer = nullptr;
	static inline EVOLUTION_IMAGES* EvolutionImagesPointer = nullptr;

private:
	void MutateOrder(int);

public:
	unsigned long long Rating;
	ELLIPSE* const Genotype;

	INDIVIDUAL();
	~INDIVIDUAL();
	void Rate();
	void Inherit(INDIVIDUAL*);
	void DrawToComparisonBuffer() const;
};