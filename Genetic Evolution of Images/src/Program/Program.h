#pragma once
#include "Program/Window.h"
#include "Program/Parameters_Images.h"

#include "common/Windows_include.h"
#include <shellapi.h>

#include <atomic>
#include <string>

class MAIN_PROGRAM_CLASS
{
public:
	void StartupWindow(HWND, HBRUSH, HICON);
	bool StaticTextColor(HDC, HWND, HBRUSH*) const;
	void Button(HWND);
	void FileDrop(HDROP) const;
	void Paint(HWND);
	void Shutdown();

private:
	WINDOW_CONTROLS WindowControls;
	EVOLUTION_PARAMETERS EvolutionParameters;
	EVOLUTION_IMAGES EvolutionImages;

	std::atomic<PROGRAM_STATUS> CurrentStatus;
	CRITICAL_SECTION CriticalSection_CurrentStatus;
	CRITICAL_SECTION CriticalSection_EvolvedImage_PresentationBuffer;

	HANDLE MainWorkThreadHandle;
	static DWORD WINAPI StaticThreadStart(LPVOID);
	void MainEvolutionThread();

	std::wstring NameOfTheOpenedFile;

	unsigned long long CurrentRating;
	unsigned long long CurrentGeneration;

	unsigned long long ReportedRating;
	std::atomic<unsigned long long> ReportedGeneration;
	int ReportedPrecision;

	void PositionButtonsDuringWorkPhase(const RECT, const LONG);
	void Pause_Resume_ButtonOperation(bool);
	void SaveImage_ButtonEnable();
	void BrowseForFile(HWND);
	void ValidateEnteredParameters(HWND);
	bool StartWork(const std::wstring&);
	bool DecodeImage(const std::wstring&);
	bool EncodeImage(const std::wstring&, const std::wstring&) const;
	bool BitmapPreparation();
	void BitmapCleanup();
	void SaveImage(HWND);
};