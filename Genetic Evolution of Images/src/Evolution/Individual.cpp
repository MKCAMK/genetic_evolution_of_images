#include "Evolution/Individual.h"
#include "common/RNG_Engine.h"

extern RNG_ENGINE g_RNG_Engine;

INDIVIDUAL::INDIVIDUAL(): Rating(0), Genotype(new ELLIPSE[EvolutionParametersPointer->NumberOfEllipses])
{
	Rate();
}

INDIVIDUAL::~INDIVIDUAL()
{
	delete[] Genotype;
}

void INDIVIDUAL::Rate()
{
	DrawToComparisonBuffer();

	Rating = 0;

	const BYTE* OriginalPixel_Pointer = EvolutionImagesPointer->OriginalImage_ComparisonBuffer_DeviceIndependentBitmapPointer;
	const BYTE* CurrentPixel_Pointer = EvolutionImagesPointer->EvolvedImage_ComparisonBuffer_DeviceIndependentBitmapPointer;
	const int l_NumberOfPixels = EvolutionImagesPointer->OriginalImage_Info_NumberOfPixels;
	const int l_ComparisonPrecision = EvolutionParametersPointer->ComparisonPrecision;
	constexpr int BytesPerPixel = CONSTANT_x_BYTES_PER_PIXEL;
	const int ComparisonPrecision_InBytes = l_ComparisonPrecision * BytesPerPixel;
	for (int i = g_RNG_Engine.RandomFromZero(l_ComparisonPrecision - 1); i < l_NumberOfPixels; i += l_ComparisonPrecision)
	{
		const int OriginalBlue = OriginalPixel_Pointer[0];
		const int OriginalGreen = OriginalPixel_Pointer[1];
		const int OriginalRed = OriginalPixel_Pointer[2];

		int CurrentBlue = CurrentPixel_Pointer[0];
		int CurrentGreen = CurrentPixel_Pointer[1];
		int CurrentRed = CurrentPixel_Pointer[2];

		CurrentRed -= OriginalRed;
		CurrentGreen -= OriginalGreen;
		CurrentBlue -= OriginalBlue;

		if (CurrentRed < 0)
			CurrentRed = -CurrentRed;
		if (CurrentGreen < 0)
			CurrentGreen = -CurrentGreen;
		if (CurrentBlue < 0)
			CurrentBlue = -CurrentBlue;

		Rating += static_cast<unsigned long long>(CurrentRed) + static_cast<unsigned long long>(CurrentGreen) + static_cast<unsigned long long>(CurrentBlue);

		OriginalPixel_Pointer += ComparisonPrecision_InBytes;
		CurrentPixel_Pointer += ComparisonPrecision_InBytes;
	}
}

void INDIVIDUAL::MutateOrder(int index)
{
	int SwitchPartner = g_RNG_Engine.RandomFromOne(EvolutionParametersPointer->NumberOfEllipses);
	--SwitchPartner;

	ELLIPSE copy = Genotype[index];
	Genotype[index] = Genotype[SwitchPartner];
	Genotype[SwitchPartner] = copy;
}

void INDIVIDUAL::Inherit(INDIVIDUAL* Parent)
{
	for (int i = 0; i < EvolutionParametersPointer->NumberOfEllipses; ++i)
		Genotype[i] = Parent->Genotype[i];
		
	for (int i = 0; i < EvolutionParametersPointer->NumberOfEllipses; ++i)
	{
		if (g_RNG_Engine.RandomFromOne(EvolutionParametersPointer->EllipseMutationChance) == 1)
		{
			switch (g_RNG_Engine.RandomFromZero(2))
			{
				case 0:
					Genotype[i].MutateShape();
					break;
				case 1:
					Genotype[i].MutateColor();
					break;
				case 2:
					MutateOrder(i);
					break;
			}
		}
	}
	Rate();
}

void INDIVIDUAL::DrawToComparisonBuffer() const
{
	const HDC l_hdc = EvolutionImagesPointer->EvolvedImage_ComparisonBuffer_DeviceContextHandle;
	FillRect(l_hdc, &EvolutionImagesPointer->OriginalImage_Info_AreaRect, EvolutionImagesPointer->EvolvedImageBackgroundBrush);
	for (int i = 0; i < EvolutionParametersPointer->NumberOfEllipses; ++i)
	{
		const COLORREF l_Color = Genotype[i].Color;
		DeleteObject(SelectObject(l_hdc, CreateSolidBrush(l_Color)));
		Ellipse(l_hdc, Genotype[i].Shape.left, Genotype[i].Shape.top, Genotype[i].Shape.right, Genotype[i].Shape.bottom);
	}
}