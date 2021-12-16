#include "Program/Program.h"
#include "Evolution/Individual.h"
#include "common/RNG_Engine.h"

#include <memory>

RNG_ENGINE g_RNG_Engine;

DWORD WINAPI MAIN_PROGRAM_CLASS::StaticThreadStart(LPVOID That)
{
	((MAIN_PROGRAM_CLASS*)That)->MainEvolutionThread();
	return 0;
}

inline void ShuffleIndividuals(INDIVIDUAL** const& Individuals, const int& NumberOfIndividuals)
{
	for (int i = NumberOfIndividuals - 1; i > 0; --i)
	{
		int j = g_RNG_Engine.RandomFromOne(NumberOfIndividuals);
		--j;
		INDIVIDUAL* copy = Individuals[i];
		Individuals[i] = Individuals[j];
		Individuals[j] = copy;
	}
}

inline void SortIndividuals(INDIVIDUAL** const& Individuals, const int& NumberOfIndividuals)
{
	//insertion sort algorithm
	for (int i = 1; i < NumberOfIndividuals; ++i)
	{
		INDIVIDUAL* key_pointer = Individuals[i];
		unsigned long long key = Individuals[i]->Rating;

		int j = i - 1;
		while (j >= 0 && Individuals[j]->Rating > key)
		{
			Individuals[j + 1] = Individuals[j];
			--j;
		}
		Individuals[j + 1] = key_pointer;
	}
}

void MAIN_PROGRAM_CLASS::MainEvolutionThread()
{
#if SETTING_x_DEBUG_VERSION == true
	g_RNG_Engine.Seed(0);
#else
	{
		LARGE_INTEGER PartOfSeed{ 0 };
		QueryPerformanceCounter(&PartOfSeed);
		g_RNG_Engine.Seed(static_cast<unsigned int>(std::random_device{}() + PartOfSeed.QuadPart));
	}
#endif

	INDIVIDUAL::EvolutionParametersPointer = &EvolutionParameters;
	INDIVIDUAL::EvolutionImagesPointer = &EvolutionImages;

	ELLIPSE::EvolutionParametersPointer = &EvolutionParameters;
	ELLIPSE::EvolutionImagesPointer = &EvolutionImages;

	const std::unique_ptr<INDIVIDUAL[]> guard_Population = std::make_unique<INDIVIDUAL[]>(EvolutionParameters.NumberOfImages);
	INDIVIDUAL* const Population = guard_Population.get();
	const std::unique_ptr<INDIVIDUAL* []> guard_Individuals = std::make_unique<INDIVIDUAL* []>(EvolutionParameters.NumberOfImages);
	INDIVIDUAL** const Individuals = guard_Individuals.get();
	for (int i = 0; i < EvolutionParameters.NumberOfImages; ++i)
		Individuals[i] = &Population[i];

	SortIndividuals(Individuals, EvolutionParameters.NumberOfImages);

	Individuals[0]->DrawToComparisonBuffer();
	BitBlt(EvolutionImages.EvolvedImage_PresentationBuffer_DeviceContextHandle, 0, 0, EvolutionImages.OriginalImage_Info.bmWidth, EvolutionImages.OriginalImage_Info.bmHeight, EvolutionImages.EvolvedImage_ComparisonBuffer_DeviceContextHandle, 0, 0, SRCCOPY);
	CurrentGeneration = 1;
	CurrentRating = Individuals[0]->Rating;

	ReportedGeneration.store(CurrentGeneration, std::memory_order_relaxed);
	ReportedRating = CurrentRating;
	ReportedPrecision = EvolutionParameters.ComparisonPrecision;

	CurrentStatus.store(PROGRAM_STATUS::WORKING, std::memory_order_relaxed);
	EnableWindow(WindowControls.Pause_Resume_Button, true);

	bool WasPausedLastLoop = false;
	const int HalfOfIndividuals = EvolutionParameters.NumberOfImages / 2;
	for (;;)
	{
		switch (CurrentStatus.load(std::memory_order_relaxed))
		{
			case PROGRAM_STATUS::WORKING:
			{
				WasPausedLastLoop = false;

				if (CurrentRating == 0)
				{
					if (EvolutionParameters.ComparisonPrecision > 1)
					{
						EvolutionParameters.ComparisonPrecision /= 2;

						for (int i = 0; i < EvolutionParameters.NumberOfImages; ++i)
							Population[i].Rate();
					}
					else
					{
						Pause_Resume_ButtonOperation(true);
						continue;
					}
				}

				for (int i = 0; i < HalfOfIndividuals; ++i)
				{
					Individuals[(EvolutionParameters.NumberOfImages - 1) - i]->Inherit(Individuals[i]);
				}

				ShuffleIndividuals(Individuals, EvolutionParameters.NumberOfImages);
				SortIndividuals(Individuals, EvolutionParameters.NumberOfImages);

				Individuals[0]->DrawToComparisonBuffer();

				++CurrentGeneration;
				CurrentRating = Individuals[0]->Rating;

				EnterCriticalSection(&CriticalSection_EvolvedImage_PresentationBuffer);

				BitBlt(EvolutionImages.EvolvedImage_PresentationBuffer_DeviceContextHandle, 0, 0, EvolutionImages.OriginalImage_Info.bmWidth, EvolutionImages.OriginalImage_Info.bmHeight, EvolutionImages.EvolvedImage_ComparisonBuffer_DeviceContextHandle, 0, 0, SRCCOPY);

				ReportedGeneration.store(CurrentGeneration, std::memory_order_relaxed);
				ReportedRating = CurrentRating;
				ReportedPrecision = EvolutionParameters.ComparisonPrecision;

				LeaveCriticalSection(&CriticalSection_EvolvedImage_PresentationBuffer);

				continue;
			}
			case PROGRAM_STATUS::PAUSED:
			{
				if (WasPausedLastLoop == false)
				{
					SaveImage_ButtonEnable();
					WasPausedLastLoop = true;
				}

				Sleep(200);

				continue;
			}
			case PROGRAM_STATUS::SHUTDOWN:
			default: {}
		}

		break;
	}
}