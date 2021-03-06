#include "Program/Window.h"
#include "Program/Program.h"
#include "common/Settings.h"

#include <atlbase.h>
#include <cwctype>
#include <algorithm>

constexpr int FILEPATH_MAX_LENGTH = MAX_PATH;

#define STRINGIFY2(MACRO_VALUE) L#MACRO_VALUE
#define STRINGIFY(MACRO) STRINGIFY2(MACRO)

bool IsPositiveInteger(wchar_t* String, int String_Length)
{
	if (String_Length <= 0)
		return false;

	if (String[0] == '0' && String_Length != 1)
		return false;

	for (int i = 0; i < String_Length; ++i)
	{
		if (String[i] == '0' || String[i] == '1' || String[i] == '2' || String[i] == '3' || String[i] == '4' || String[i] == '5' || String[i] == '6' || String[i] == '7' || String[i] == '8' || String[i] == '9')
			continue;
		else
			return false;
	}
	return true;
}

void ResizeWhileKeepingAspect(RECT& Rectangle, int DimensionChange, bool IsItHeight)
{
	float fRectRight = static_cast<float>(Rectangle.right);
	float fRectBottom = static_cast<float>(Rectangle.bottom);
	float fDimensionChange = static_cast<float>(DimensionChange);
	float ChangePercent;
	if (IsItHeight)
		ChangePercent = fDimensionChange / fRectBottom;
	else
		ChangePercent = fDimensionChange / fRectRight;
	fRectBottom += fRectBottom * ChangePercent;
	fRectRight += fRectRight * ChangePercent;
	Rectangle.bottom = static_cast<LONG>(fRectBottom);
	Rectangle.right = static_cast<LONG>(fRectRight);
}

bool DoesFileExists(const std::wstring& Filename)
{
	DWORD Attributes = GetFileAttributes(Filename.c_str());
	if (Attributes != INVALID_FILE_ATTRIBUTES && Attributes != FILE_ATTRIBUTE_DIRECTORY)
		return true;
	else
		return false;
}

void GetExtensionFromFilePath(const std::wstring& Filepath, std::wstring& FileExtension)
{
	FileExtension = PathFindExtension(Filepath.c_str());
	if (FileExtension.length() > 0)
	{
		std::transform(FileExtension.begin(), FileExtension.end(), FileExtension.begin(), [](wchar_t c) {return std::towlower(c); });
	}
}

inline void GetNameFromFilePath(const std::wstring& Filepath, std::wstring& FileName)
{
	FileName = PathFindFileName(Filepath.c_str());
	for (int i = static_cast<int>(FileName.length()) - 1; i > 0; --i)
	{
		if (FileName[i] == L'.')
		{
			FileName = FileName.substr(0, i);
		}
	}
}

std::wstring& FormatNumeral(std::wstring& Numeral)
{
	for (int length = static_cast<int>(Numeral.length()); length > 3; )
	{
		length -= 3;
		Numeral.insert(length, L" ");
	}
	return Numeral;
}

void CreateWindow_StaticText_Helper(LABELED_INPUT_FIELD& LabeledField, const std::wstring& Text, HWND WindowHandle, bool IsLocationAbove)
{
	int CreationPositionX;
	int CreationPositionY;

	if (IsLocationAbove)
	{
		CreationPositionX = LabeledField.Position_X;
		CreationPositionY = LabeledField.Position_Y - SETTING_x_TEXT_AND_FIELD_VERTICAL_OFFSET;
	}
	else
	{
		CreationPositionX = LabeledField.Position_X + SETTING_x_FIELD_WIDTH + SETTING_x_TEXT_AND_FIELD_HORIZONTAL_GAP;
		CreationPositionY = LabeledField.Position_Y;
	}

	LabeledField.StaticText = CreateWindow(L"STATIC", Text.c_str(), WS_CHILD | WS_VISIBLE, CreationPositionX, CreationPositionY, SETTING_x_TEXT_WIDTH, SETTING_x_FIELD_HEIGHT, WindowHandle, NULL, NULL, NULL);
}

void CreateWindow_InputField_Helper(LABELED_INPUT_FIELD& LabeledField, const wchar_t* Text, HWND WindowHandle)
{
	LabeledField.InputField = CreateWindow(L"EDIT", Text, WS_CHILD | WS_BORDER | WS_VISIBLE, LabeledField.Position_X, LabeledField.Position_Y, SETTING_x_FIELD_WIDTH, SETTING_x_FIELD_HEIGHT, WindowHandle, NULL, NULL, NULL);
}

void PositionY_ToNextField(int& PositionY)
{
	 PositionY += SETTING_x_FIELD_HEIGHT + SETTING_x_FIELDS_GAP;
}

void MAIN_PROGRAM_CLASS::StartupWindow(HWND WindowHandle, HBRUSH BackgroundBrush, HICON EvolvedIcon)
{
	CurrentStatus.store(PROGRAM_STATUS::STARTUP, std::memory_order_relaxed);

	WindowControls.EvolvedIcon = EvolvedIcon;

	{
		WindowControls.BackgroundBrush = BackgroundBrush;
		LOGBRUSH BrushInfo;
		GetObject(BackgroundBrush, GetObject(BackgroundBrush, 0, NULL), &BrushInfo);
		WindowControls.BackgroundBrushColor = BrushInfo.lbColor;
	}

	{
		WindowControls.NumberOfImages.MinimumValue = SETTING_x_NUMBER_OF_IMAGES_MIN;
		WindowControls.NumberOfImages.MaximumValue = SETTING_x_NUMBER_OF_IMAGES_MAX;
		WindowControls.NumberOfEllipses.MinimumValue = SETTING_x_NUMBER_OF_ELLIPSES_MIN;
		WindowControls.NumberOfEllipses.MaximumValue = SETTING_x_NUMBER_OF_ELLIPSES_MAX;
		WindowControls.EllipseMutationChance.MinimumValue = SETTING_x_ELLIPSE_MUTATION_CHANCE_MIN;
		WindowControls.EllipseMutationChance.MaximumValue = SETTING_x_ELLIPSE_MUTATION_CHANCE_MAX;
		WindowControls.EllipseSizeAtCreation.MinimumValue = SETTING_x_ELLIPSE_SIZE_AT_CREATION_MIN;
		WindowControls.EllipseSizeAtCreation.MaximumValue = SETTING_x_ELLIPSE_SIZE_AT_CREATION_MAX;
		WindowControls.EllipseShapeMutationRange.MinimumValue = SETTING_x_ELLIPSE_SHAPE_MUTATION_RANGE_MIN;
		WindowControls.EllipseShapeMutationRange.MaximumValue = SETTING_x_ELLIPSE_SHAPE_MUTATION_RANGE_MAX;
		WindowControls.EllipseColorMutationRange.MinimumValue = SETTING_x_ELLIPSE_COLOR_MUTATION_RANGE_MIN;
		WindowControls.EllipseColorMutationRange.MaximumValue = SETTING_x_ELLIPSE_COLOR_MUTATION_RANGE_MAX;
		WindowControls.BoundingBoxMargin.MinimumValue = SETTING_x_BOUNDING_BOX_MARGIN_MIN;
		WindowControls.BoundingBoxMargin.MaximumValue = SETTING_x_BOUNDING_BOX_MARGIN_MAX;
		WindowControls.ComparisonPrecision.MinimumValue = SETTING_x_COMPARISON_PRECISION_MIN;
		WindowControls.ComparisonPrecision.MaximumValue = SETTING_x_COMPARISON_PRECISION_MAX;
	}

	{
		int PositionX = SETTING_x_MARGIN_LEFT;
		int PositionY = SETTING_x_MARGIN_TOP;

		WindowControls.NumberOfImages.Position_X =
		WindowControls.NumberOfEllipses.Position_X = 
		WindowControls.EllipseMutationChance.Position_X =
		WindowControls.EllipseSizeAtCreation.Position_X = 
		WindowControls.EllipseShapeMutationRange.Position_X =
		WindowControls.EllipseColorMutationRange.Position_X =
		WindowControls.BoundingBoxMargin.Position_X =
		WindowControls.ComparisonPrecision.Position_X =
		WindowControls.FileSelect.Position_X =
		PositionX;

		WindowControls.NumberOfImages.Position_Y = PositionY;
		
		PositionY_ToNextField(PositionY);
		WindowControls.NumberOfEllipses.Position_Y = PositionY;

		PositionY_ToNextField(PositionY);
		WindowControls.EllipseMutationChance.Position_Y = PositionY;

		PositionY_ToNextField(PositionY);
		WindowControls.EllipseSizeAtCreation.Position_Y = PositionY;

		PositionY_ToNextField(PositionY);
		WindowControls.EllipseShapeMutationRange.Position_Y = PositionY;

		PositionY_ToNextField(PositionY);
		WindowControls.EllipseColorMutationRange.Position_Y = PositionY;

		PositionY_ToNextField(PositionY);
		WindowControls.BoundingBoxMargin.Position_Y = PositionY;

		PositionY_ToNextField(PositionY);
		WindowControls.ComparisonPrecision.Position_Y = PositionY;

		PositionY_ToNextField(PositionY);
		PositionY_ToNextField(PositionY);
		WindowControls.FileSelect.Position_Y = PositionY;
	}

	{
		CreateWindow_StaticText_Helper(WindowControls.NumberOfImages, L"Number of images used in the evolution  [ " STRINGIFY(SETTING_x_NUMBER_OF_IMAGES_MIN) L" - " STRINGIFY(SETTING_x_NUMBER_OF_IMAGES_MAX) L" ]", WindowHandle, false);

		CreateWindow_StaticText_Helper(WindowControls.NumberOfEllipses, L"Number of ellipses used in the evolution  [ " STRINGIFY(SETTING_x_NUMBER_OF_ELLIPSES_MIN) L" - " STRINGIFY(SETTING_x_NUMBER_OF_ELLIPSES_MAX) L" ]", WindowHandle, false);

		CreateWindow_StaticText_Helper(WindowControls.EllipseMutationChance, L"Chance for a single ellipse to mutate - described as 1 / N  [ " STRINGIFY(SETTING_x_ELLIPSE_MUTATION_CHANCE_MIN) L" - " STRINGIFY(SETTING_x_ELLIPSE_MUTATION_CHANCE_MAX) L" ]", WindowHandle, false);

		CreateWindow_StaticText_Helper(WindowControls.EllipseSizeAtCreation, L"Limit to the size of an ellipse at its creation  [ " STRINGIFY(SETTING_x_ELLIPSE_SIZE_AT_CREATION_MIN) L" - " STRINGIFY(SETTING_x_ELLIPSE_SIZE_AT_CREATION_MAX) L" ]", WindowHandle, false);

		CreateWindow_StaticText_Helper(WindowControls.EllipseShapeMutationRange, L"Limit to the size of an ellipse's shape's mutation  [ " STRINGIFY(SETTING_x_ELLIPSE_SHAPE_MUTATION_RANGE_MIN) L" - " STRINGIFY(SETTING_x_ELLIPSE_SHAPE_MUTATION_RANGE_MAX) L" ]", WindowHandle, false);

		CreateWindow_StaticText_Helper(WindowControls.EllipseColorMutationRange, L"Limit to the size of an ellipse's color's mutation  [ " STRINGIFY(SETTING_x_ELLIPSE_COLOR_MUTATION_RANGE_MIN) L" - " STRINGIFY(SETTING_x_ELLIPSE_COLOR_MUTATION_RANGE_MAX) L" ]", WindowHandle, false);

		CreateWindow_StaticText_Helper(WindowControls.BoundingBoxMargin, L"Off-image margin of the evolution area [ " STRINGIFY(SETTING_x_BOUNDING_BOX_MARGIN_MIN) L" - " STRINGIFY(SETTING_x_BOUNDING_BOX_MARGIN_MAX) L" ]", WindowHandle, false);

		CreateWindow_StaticText_Helper(WindowControls.ComparisonPrecision, L"Comparison's precision - described as a number of pixels in a stride  [ " STRINGIFY(SETTING_x_COMPARISON_PRECISION_MIN) L" - " STRINGIFY(SETTING_x_COMPARISON_PRECISION_MAX) L" ]", WindowHandle, false);

		CreateWindow_StaticText_Helper(WindowControls.FileSelect, L"Image used as the evolution's target", WindowHandle, true);
	}

	{
		CreateWindow_InputField_Helper(WindowControls.NumberOfImages, NULL, WindowHandle);

		CreateWindow_InputField_Helper(WindowControls.NumberOfEllipses, NULL, WindowHandle);

		CreateWindow_InputField_Helper(WindowControls.EllipseMutationChance, NULL, WindowHandle);

		CreateWindow_InputField_Helper(WindowControls.EllipseSizeAtCreation, NULL, WindowHandle);

		CreateWindow_InputField_Helper(WindowControls.EllipseShapeMutationRange, NULL, WindowHandle);

		CreateWindow_InputField_Helper(WindowControls.EllipseColorMutationRange, NULL, WindowHandle);

		CreateWindow_InputField_Helper(WindowControls.BoundingBoxMargin, NULL, WindowHandle);

		CreateWindow_InputField_Helper(WindowControls.ComparisonPrecision, NULL, WindowHandle);


		WindowControls.FileSelect.InputField = CreateWindow(L"EDIT", NULL, WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL, WindowControls.FileSelect.Position_X, WindowControls.FileSelect.Position_Y, SETTING_x_FILEPATH_FIELD_WIDTH, SETTING_x_FIELD_HEIGHT, WindowHandle, NULL, NULL, NULL);
		WindowControls.FileSelect_Button = CreateWindow(L"BUTTON", L"Browse", WS_CHILD | WS_BORDER | WS_VISIBLE, WindowControls.FileSelect.Position_X + SETTING_x_FILEPATH_FIELD_WIDTH, WindowControls.FileSelect.Position_Y, SETTING_x_BROWSE_BUTTON_LENGTH, SETTING_x_FIELD_HEIGHT, WindowHandle, NULL, NULL, NULL);

		WindowControls.Start_Button = CreateWindow(L"BUTTON", L"Evolve", WS_CHILD | WS_BORDER | WS_VISIBLE, WindowControls.FileSelect.Position_X, WindowControls.FileSelect.Position_Y + SETTING_x_FIELD_HEIGHT + SETTING_x_EVOLVE_BUTTON_GAP, SETTING_x_FILEPATH_FIELD_WIDTH + SETTING_x_BROWSE_BUTTON_LENGTH, SETTING_x_EVOLVE_BUTTON_HEIGHT, WindowHandle, NULL, NULL, NULL);

		WindowControls.Pause_Resume_Button = CreateWindow(L"BUTTON", L"Pause", WS_CHILD | WS_BORDER | WS_DISABLED, NULL, NULL, NULL, NULL, WindowHandle, NULL, NULL, NULL);

		WindowControls.SaveImage_Button = CreateWindow(L"BUTTON", L"Save Image", WS_CHILD | WS_BORDER | WS_DISABLED, NULL, NULL, NULL, NULL, WindowHandle, NULL, NULL, NULL);
	}

	{
		NONCLIENTMETRICS Metrics;
		Metrics.cbSize = sizeof(NONCLIENTMETRICS);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &Metrics, NULL);
		LOGFONT FontDefinition = Metrics.lfCaptionFont;
		FontDefinition.lfHeight = SETTING_x_FONT_SIZE;
		WindowControls.ControlsFont = CreateFontIndirect(&FontDefinition);
		
		

		SendMessage(WindowControls.NumberOfImages.InputField, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
		SendMessage(WindowControls.NumberOfImages.StaticText, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.NumberOfEllipses.InputField, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
		SendMessage(WindowControls.NumberOfEllipses.StaticText, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.EllipseMutationChance.InputField, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
		SendMessage(WindowControls.EllipseMutationChance.StaticText, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.EllipseSizeAtCreation.InputField, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
		SendMessage(WindowControls.EllipseSizeAtCreation.StaticText, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.EllipseShapeMutationRange.InputField, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
		SendMessage(WindowControls.EllipseShapeMutationRange.StaticText, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.EllipseColorMutationRange.InputField, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
		SendMessage(WindowControls.EllipseColorMutationRange.StaticText, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.BoundingBoxMargin.InputField, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
		SendMessage(WindowControls.BoundingBoxMargin.StaticText, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.ComparisonPrecision.InputField, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
		SendMessage(WindowControls.ComparisonPrecision.StaticText, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.FileSelect.InputField, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
		SendMessage(WindowControls.FileSelect.StaticText, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
		SendMessage(WindowControls.FileSelect_Button, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.Start_Button, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.Pause_Resume_Button, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);

		SendMessage(WindowControls.SaveImage_Button, WM_SETFONT, (WPARAM)WindowControls.ControlsFont, NULL);
	}

	{
		SetWindowText(WindowControls.NumberOfImages.InputField, STRINGIFY(SETTING_x_NUMBER_OF_IMAGES_DEFAULT));
		SetWindowText(WindowControls.NumberOfEllipses.InputField, STRINGIFY(SETTING_x_NUMBER_OF_ELLIPSES_DEFAULT));
		SetWindowText(WindowControls.EllipseMutationChance.InputField, STRINGIFY(SETTING_x_ELLIPSE_MUTATION_CHANCE_DEFAULT));
		SetWindowText(WindowControls.EllipseSizeAtCreation.InputField, STRINGIFY(SETTING_x_ELLIPSE_SIZE_AT_CREATION_DEFAULT));
		SetWindowText(WindowControls.EllipseShapeMutationRange.InputField, STRINGIFY(SETTING_x_ELLIPSE_SHAPE_MUTATION_RANGE_DEFAULT));
		SetWindowText(WindowControls.EllipseColorMutationRange.InputField, STRINGIFY(SETTING_x_ELLIPSE_COLOR_MUTATION_RANGE_DEFAULT));
		SetWindowText(WindowControls.BoundingBoxMargin.InputField, STRINGIFY(SETTING_x_BOUNDING_BOX_MARGIN_DEFAULT));
		SetWindowText(WindowControls.ComparisonPrecision.InputField, STRINGIFY(SETTING_x_COMPARISON_PRECISION_DEFAULT));
	}

	{
		WindowControls.NumberOfImages.Error = false;
		WindowControls.NumberOfEllipses.Error = false;
		WindowControls.EllipseMutationChance.Error = false;
		WindowControls.EllipseSizeAtCreation.Error = false;
		WindowControls.EllipseShapeMutationRange.Error = false;
		WindowControls.EllipseColorMutationRange.Error = false;
		WindowControls.BoundingBoxMargin.Error = false;
		WindowControls.ComparisonPrecision.Error = false;
		WindowControls.FileSelect.Error = false;
	}

	WindowControls.WorkWindowButtonsAreVisible = false;

	DragAcceptFiles(WindowHandle, true);
}

bool MAIN_PROGRAM_CLASS::StaticTextColor(HDC DeviceContext, HWND StaticTextHandle, HBRUSH* BackgroundBrush) const
{
	if(CurrentStatus.load(std::memory_order_relaxed) == PROGRAM_STATUS::STARTUP)
	{
		bool ShouldTextBeRed;

		if (StaticTextHandle == WindowControls.NumberOfImages.StaticText)
			ShouldTextBeRed = WindowControls.NumberOfImages.Error;
		else if (StaticTextHandle == WindowControls.NumberOfEllipses.StaticText)
			ShouldTextBeRed = WindowControls.NumberOfEllipses.Error;
		else if (StaticTextHandle == WindowControls.EllipseMutationChance.StaticText)
			ShouldTextBeRed = WindowControls.EllipseMutationChance.Error;
		else if (StaticTextHandle == WindowControls.EllipseSizeAtCreation.StaticText)
			ShouldTextBeRed = WindowControls.EllipseSizeAtCreation.Error;
		else if (StaticTextHandle == WindowControls.EllipseShapeMutationRange.StaticText)
			ShouldTextBeRed = WindowControls.EllipseShapeMutationRange.Error;
		else if (StaticTextHandle == WindowControls.EllipseColorMutationRange.StaticText)
			ShouldTextBeRed = WindowControls.EllipseColorMutationRange.Error;
		else if (StaticTextHandle == WindowControls.BoundingBoxMargin.StaticText)
			ShouldTextBeRed = WindowControls.BoundingBoxMargin.Error;
		else if (StaticTextHandle == WindowControls.ComparisonPrecision.StaticText)
			ShouldTextBeRed = WindowControls.ComparisonPrecision.Error;
		else if (StaticTextHandle == WindowControls.FileSelect.StaticText)
			ShouldTextBeRed = WindowControls.FileSelect.Error;
		else
			return false;

		if (ShouldTextBeRed)
			SetTextColor(DeviceContext, RGB(255, 0, 0));
		SetBkColor(DeviceContext, WindowControls.BackgroundBrushColor);
		*BackgroundBrush = WindowControls.BackgroundBrush;
		return true;
	}
	return false;
}

void MAIN_PROGRAM_CLASS::PositionButtonsDuringWorkPhase(const RECT ClientArea, const LONG ButtonsHeight)
{
	constexpr int NumberOfButtons = 2;
	HDWP MultipleWindowPosition = BeginDeferWindowPos(NumberOfButtons);

	const LONG a = ClientArea.bottom - ButtonsHeight;
	const LONG b = ClientArea.right / 2;
	DeferWindowPos(MultipleWindowPosition, WindowControls.Pause_Resume_Button, NULL, ClientArea.left, a, b, ButtonsHeight, SWP_NOZORDER);
	DeferWindowPos(MultipleWindowPosition, WindowControls.SaveImage_Button, NULL, b, a, b + (ClientArea.right % 2), ButtonsHeight, SWP_NOZORDER);

	EndDeferWindowPos(MultipleWindowPosition);
}

void MAIN_PROGRAM_CLASS::Pause_Resume_ButtonOperation(bool IntentionIsPause)
{
	EnterCriticalSection(&CriticalSection_CurrentStatus);

	if (IntentionIsPause && CurrentStatus.load(std::memory_order_relaxed) == PROGRAM_STATUS::WORKING)
	{
		SetWindowText(WindowControls.Pause_Resume_Button, L"Resume");

		CurrentStatus.store(PROGRAM_STATUS::PAUSED, std::memory_order_relaxed);
	}
	else if(!IntentionIsPause && CurrentStatus.load(std::memory_order_relaxed) == PROGRAM_STATUS::PAUSED)
	{
		SetWindowText(WindowControls.Pause_Resume_Button, L"Pause");

		EnableWindow(WindowControls.SaveImage_Button, false);

		CurrentStatus.store(PROGRAM_STATUS::WORKING, std::memory_order_relaxed);
	}

	LeaveCriticalSection(&CriticalSection_CurrentStatus);
}

void MAIN_PROGRAM_CLASS::SaveImage_ButtonEnable()
{
	EnterCriticalSection(&CriticalSection_CurrentStatus);

	if (CurrentStatus.load(std::memory_order_relaxed) == PROGRAM_STATUS::PAUSED)
	{
		EnableWindow(WindowControls.SaveImage_Button, true);
	}

	LeaveCriticalSection(&CriticalSection_CurrentStatus);
}

void MAIN_PROGRAM_CLASS::Button(HWND ButtonHandle)
{
	switch (CurrentStatus.load(std::memory_order_relaxed))
	{
		case PROGRAM_STATUS::STARTUP:
		{
			if (ButtonHandle == WindowControls.FileSelect_Button)
			{
				HWND DialogOwnerHandle = GetParent(ButtonHandle);
				BrowseForFile(DialogOwnerHandle);
				return;
			}
			else if (ButtonHandle == WindowControls.Start_Button)
			{
				HWND IconOwnerHandle = GetParent(ButtonHandle);
				ValidateEnteredParameters(IconOwnerHandle);
				return;
			}
			break;
		}
		case PROGRAM_STATUS::WORKING:
		{
			if (ButtonHandle == WindowControls.Pause_Resume_Button)
			{
				Pause_Resume_ButtonOperation(true);
			}
			break;
		}
		case PROGRAM_STATUS::PAUSED:
		{
			if (ButtonHandle == WindowControls.Pause_Resume_Button)
			{
				Pause_Resume_ButtonOperation(false);
				return;
			}
			else if (ButtonHandle == WindowControls.SaveImage_Button)
			{
				HWND DialogOwnerHandle = GetParent(ButtonHandle);
				SaveImage(DialogOwnerHandle);
				return;
			}
			break;
		}
	}
}

void MAIN_PROGRAM_CLASS::FileDrop(HDROP DroppedFilenameHandle) const
{
	if (CurrentStatus.load(std::memory_order_relaxed) == PROGRAM_STATUS::STARTUP)
	{
		if (1 == DragQueryFile(DroppedFilenameHandle, 0xFFFFFFFF, 0, 0))
		{
			unsigned int FilenameLength = DragQueryFile(DroppedFilenameHandle, 0, 0, 0) + 1;
			wchar_t* Filename = new wchar_t[FilenameLength];
			if ((DragQueryFile(DroppedFilenameHandle, 0, Filename, FilenameLength) + 1) == FilenameLength)
				SetWindowText(WindowControls.FileSelect.InputField, Filename);
			delete[] Filename;
		}
	}

	DragFinish(DroppedFilenameHandle);
}

bool ValidateSingleParameter(LABELED_INPUT_FIELD& EnteredParameter, int& ParameterToSet)
{
	EnteredParameter.Error = false;

	constexpr int Buffer_Size = 32;
	wchar_t Buffer[Buffer_Size];
	int String_Length;
	String_Length = GetWindowText(EnteredParameter.InputField, Buffer, Buffer_Size);
	if (IsPositiveInteger(Buffer, String_Length))
	{
		int Parameter = _wtoi(Buffer);

		if ((EnteredParameter.MinimumValue <= Parameter) && (Parameter <= (EnteredParameter.MaximumValue)))
		{
			ParameterToSet = Parameter;
		}
		else
		{
			EnteredParameter.Error = true;
			return false;
		}
	}
	else
	{
		EnteredParameter.Error = true;
		return false;
	}
	return true;
}

void EvolveIcon(HWND IconOwnerHandle, HICON EvolvedIcon)
{
	SendMessage(IconOwnerHandle, WM_SETICON, ICON_BIG, (LPARAM)EvolvedIcon);
	SendMessage(IconOwnerHandle, WM_SETICON, ICON_SMALL, (LPARAM)EvolvedIcon);
}

void MAIN_PROGRAM_CLASS::ValidateEnteredParameters(HWND IconOwnerHandle)
{
	bool ValidatedSuccessfully = true;


	if (false == ValidateSingleParameter(WindowControls.NumberOfImages, EvolutionParameters.NumberOfImages))
		ValidatedSuccessfully = false;
	if (false == ValidateSingleParameter(WindowControls.NumberOfEllipses, EvolutionParameters.NumberOfEllipses))
		ValidatedSuccessfully = false;
	if (false == ValidateSingleParameter(WindowControls.EllipseMutationChance, EvolutionParameters.EllipseMutationChance))
		ValidatedSuccessfully = false;
	if (false == ValidateSingleParameter(WindowControls.EllipseSizeAtCreation, EvolutionParameters.EllipseSizeAtCreation))
		ValidatedSuccessfully = false;
	if (false == ValidateSingleParameter(WindowControls.EllipseShapeMutationRange, EvolutionParameters.EllipseShapeMutationRange))
		ValidatedSuccessfully = false;
	if (false == ValidateSingleParameter(WindowControls.EllipseColorMutationRange, EvolutionParameters.EllipseColorMutationRange))
		ValidatedSuccessfully = false;
	if (false == ValidateSingleParameter(WindowControls.BoundingBoxMargin, EvolutionParameters.BoundingBoxMargin))
		ValidatedSuccessfully = false;
	if (false == ValidateSingleParameter(WindowControls.ComparisonPrecision, EvolutionParameters.ComparisonPrecision))
		ValidatedSuccessfully = false;

	std::wstring FilePath;

	{
		WindowControls.FileSelect.Error = false;

		wchar_t FilePathBuffer[FILEPATH_MAX_LENGTH];

		GetWindowText(WindowControls.FileSelect.InputField, FilePathBuffer, FILEPATH_MAX_LENGTH);

		FilePath = FilePathBuffer;

		if (DoesFileExists(FilePath) == true)
		{
			GetNameFromFilePath(FilePath, NameOfTheOpenedFile);
		}
		else
		{
			WindowControls.FileSelect.Error = true;
			ValidatedSuccessfully = false;
		}
	}

	if (ValidatedSuccessfully)
	{
		if (StartWork(FilePath) == false)
		{
			WindowControls.FileSelect.Error = true;
			ValidatedSuccessfully = false;
		}
		else
			EvolveIcon(IconOwnerHandle, WindowControls.EvolvedIcon);
	}

	if (ValidatedSuccessfully == false)
	{
		MessageBeep(MB_ICONWARNING);
	}
}

void DestroyWindow_Helper(HWND& Window)
{
	if (Window != NULL)
	{
		DestroyWindow(Window);
		Window = NULL;
	}
}

bool MAIN_PROGRAM_CLASS::StartWork(const std::wstring& FilePath)
{
	if (DecodeImage(FilePath) == false)
		return false;

	if (BitmapPreparation())
	{
		InitializeCriticalSection(&CriticalSection_CurrentStatus);
		InitializeCriticalSection(&CriticalSection_EvolvedImage_PresentationBuffer);

		MainWorkThreadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StaticThreadStart, (LPVOID)this, 0, NULL);
		if (MainWorkThreadHandle != NULL)
		{
			DestroyWindow_Helper(WindowControls.NumberOfImages.InputField);
			DestroyWindow_Helper(WindowControls.NumberOfImages.StaticText);

			DestroyWindow_Helper(WindowControls.NumberOfEllipses.InputField);
			DestroyWindow_Helper(WindowControls.NumberOfEllipses.StaticText);

			DestroyWindow_Helper(WindowControls.EllipseMutationChance.InputField);
			DestroyWindow_Helper(WindowControls.EllipseMutationChance.StaticText);

			DestroyWindow_Helper(WindowControls.EllipseSizeAtCreation.InputField);
			DestroyWindow_Helper(WindowControls.EllipseSizeAtCreation.StaticText);

			DestroyWindow_Helper(WindowControls.EllipseShapeMutationRange.InputField);
			DestroyWindow_Helper(WindowControls.EllipseShapeMutationRange.StaticText);

			DestroyWindow_Helper(WindowControls.EllipseColorMutationRange.InputField);
			DestroyWindow_Helper(WindowControls.EllipseColorMutationRange.StaticText);

			DestroyWindow_Helper(WindowControls.BoundingBoxMargin.InputField);
			DestroyWindow_Helper(WindowControls.BoundingBoxMargin.StaticText);

			DestroyWindow_Helper(WindowControls.ComparisonPrecision.InputField);
			DestroyWindow_Helper(WindowControls.ComparisonPrecision.StaticText);

			DestroyWindow_Helper(WindowControls.FileSelect.InputField);
			DestroyWindow_Helper(WindowControls.FileSelect.StaticText);

			DestroyWindow_Helper(WindowControls.FileSelect_Button);

			DestroyWindow_Helper(WindowControls.Start_Button);

			while (CurrentStatus.load(std::memory_order_relaxed) != PROGRAM_STATUS::WORKING)
			{
				Sleep(100);
			}

			DragAcceptFiles(GetParent(WindowControls.Start_Button), false);

			return true;
		}

		DeleteCriticalSection(&CriticalSection_CurrentStatus);
		DeleteCriticalSection(&CriticalSection_EvolvedImage_PresentationBuffer);
	}

	BitmapCleanup();
	
	return false;
}

void MAIN_PROGRAM_CLASS::Paint(HWND WindowHandle)
{
	switch (CurrentStatus.load(std::memory_order_relaxed))
	{
		case PROGRAM_STATUS::WORKING:
		case PROGRAM_STATUS::PAUSED:
		{
			PAINTSTRUCT ps;
			HDC Client_DeviceContext = BeginPaint(WindowHandle, &ps);

			RECT Client_Rect;
			GetClientRect(WindowHandle, &Client_Rect);

			constexpr LONG WorkWindowButtonsHeight = SETTING_x_WORK_WINDOW_BUTTONS_HEIGHT;

			if (WindowControls.WorkWindowButtonsAreVisible == false)
			{
				PositionButtonsDuringWorkPhase(Client_Rect, WorkWindowButtonsHeight);

				ShowWindowAsync(WindowControls.Pause_Resume_Button, SW_SHOWNA);
				ShowWindowAsync(WindowControls.SaveImage_Button, SW_SHOWNA);

				WindowControls.WorkWindowButtonsAreVisible = true;
			}

			bool ResizeOccured = true;

			if (EvolutionImages.ClientBuffer_Rect.right != Client_Rect.right || EvolutionImages.ClientBuffer_Rect.bottom != Client_Rect.bottom)
			{
				EvolutionImages.ClientBuffer_DeviceDependentBitmapHandle = CreateCompatibleBitmap(Client_DeviceContext, Client_Rect.right, Client_Rect.bottom);
				DeleteObject(SelectObject(EvolutionImages.ClientBuffer_DeviceContextHandle, EvolutionImages.ClientBuffer_DeviceDependentBitmapHandle));
				EvolutionImages.ClientBuffer_Rect = Client_Rect;

				PositionButtonsDuringWorkPhase(Client_Rect, WorkWindowButtonsHeight);

				ResizeOccured = true;
			}

			Client_Rect.bottom -= WorkWindowButtonsHeight;

			static RECT Position = { 0,0,0,0 };

			if (ResizeOccured)
			{
				FillRect(EvolutionImages.ClientBuffer_DeviceContextHandle, &Client_Rect, EvolutionImages.WindowBackgroundBrush);

				Position.left = 0;
				Position.top = 0;
				Position.right = EvolutionImages.OriginalImage_Info.bmWidth;
				Position.bottom = EvolutionImages.OriginalImage_Info.bmHeight;
				if (Position.bottom > Client_Rect.bottom)
					ResizeWhileKeepingAspect(Position, Client_Rect.bottom - Position.bottom, true);
				if (Position.right > Client_Rect.right / 2)
					ResizeWhileKeepingAspect(Position, Client_Rect.right / 2 - Position.right, false);
				Position.top += (Client_Rect.bottom - Position.bottom) / 2;
				Position.left += (Client_Rect.right / 2 - Position.right) / 2;

				StretchBlt(EvolutionImages.ClientBuffer_DeviceContextHandle, Position.left, Position.top, Position.right, Position.bottom, EvolutionImages.OriginalImage_PresentationBuffer_DeviceContextHandle, 0, 0, EvolutionImages.OriginalImage_Info.bmWidth, EvolutionImages.OriginalImage_Info.bmHeight, SRCCOPY);
				Position.left += Client_Rect.right - 2 * (Position.left + Position.right) + Position.right;
			}

			{
				static unsigned long long PreviousGeneration = 0;
				unsigned long long l_CurrentGeneration;
				static unsigned long long l_CurrentRating;
				static int l_ComparisonPrecision;

				if (ResizeOccured || (PreviousGeneration != ReportedGeneration.load(std::memory_order_relaxed)))
				{
					EnterCriticalSection(&CriticalSection_EvolvedImage_PresentationBuffer);

					StretchBlt(EvolutionImages.ClientBuffer_DeviceContextHandle, Position.left, Position.top, Position.right, Position.bottom, EvolutionImages.EvolvedImage_PresentationBuffer_DeviceContextHandle, 0, 0, EvolutionImages.OriginalImage_Info.bmWidth, EvolutionImages.OriginalImage_Info.bmHeight, SRCCOPY);

					l_CurrentGeneration = ReportedGeneration.load(std::memory_order_relaxed);
					l_CurrentRating = ReportedRating;
					l_ComparisonPrecision = ReportedPrecision;

					LeaveCriticalSection(&CriticalSection_EvolvedImage_PresentationBuffer);

					if (l_CurrentGeneration != PreviousGeneration)
					{
						PreviousGeneration = l_CurrentGeneration;

						unsigned long long Normalized_CurrentRating = l_CurrentRating * l_ComparisonPrecision;

						std::wstring CurrentGeneration_String = std::to_wstring(l_CurrentGeneration);
						std::wstring CurrentRating_String = std::to_wstring(l_CurrentRating);
						std::wstring Normalized_CurrentRating_String = std::to_wstring(Normalized_CurrentRating);
						std::wstring ComparisonPrecision_String = std::to_wstring(l_ComparisonPrecision);

#if SETTING_x_DEBUG_VERSION == true
						std::wstring GenerationAndRating = L"Generation:  " + FormatNumeral(CurrentGeneration_String) + L"     Rating:  " + FormatNumeral(CurrentRating_String) + L"     Normalized Rating:  " + FormatNumeral(Normalized_CurrentRating_String) + L"     Precision:  " + FormatNumeral(ComparisonPrecision_String);
#else	
						std::wstring GenerationAndRating = L"Generation:  " + FormatNumeral(CurrentGeneration_String) + L"     Rating:  " + FormatNumeral(Normalized_CurrentRating_String) + L"     Precision:  " + FormatNumeral(ComparisonPrecision_String);
#endif

						SetWindowText(WindowHandle, GenerationAndRating.c_str());
					}
				}
			}

			BitBlt(Client_DeviceContext, Client_Rect.left, Client_Rect.top, Client_Rect.right, Client_Rect.bottom, EvolutionImages.ClientBuffer_DeviceContextHandle, 0, 0, SRCCOPY);

			EndPaint(WindowHandle, &ps);

			break;
		}
		default: {}
	}
}

void MAIN_PROGRAM_CLASS::Shutdown()
{
	if (CurrentStatus.load(std::memory_order_relaxed) == PROGRAM_STATUS::WORKING)
	{
		EnterCriticalSection(&CriticalSection_CurrentStatus);
		CurrentStatus.store(PROGRAM_STATUS::SHUTDOWN, std::memory_order_relaxed);
		LeaveCriticalSection(&CriticalSection_CurrentStatus);

		if (WaitForSingleObject(MainWorkThreadHandle, 30000))
		{
			TerminateThread(MainWorkThreadHandle, 0);
		}
		CloseHandle(MainWorkThreadHandle);

		DeleteCriticalSection(&CriticalSection_CurrentStatus);
		DeleteCriticalSection(&CriticalSection_EvolvedImage_PresentationBuffer);

		BitmapCleanup();
	}

	DestroyWindow_Helper(WindowControls.NumberOfImages.InputField);
	DestroyWindow_Helper(WindowControls.NumberOfImages.StaticText);

	DestroyWindow_Helper(WindowControls.NumberOfEllipses.InputField);
	DestroyWindow_Helper(WindowControls.NumberOfEllipses.StaticText);

	DestroyWindow_Helper(WindowControls.EllipseMutationChance.InputField);
	DestroyWindow_Helper(WindowControls.EllipseMutationChance.StaticText);

	DestroyWindow_Helper(WindowControls.EllipseSizeAtCreation.InputField);
	DestroyWindow_Helper(WindowControls.EllipseSizeAtCreation.StaticText);

	DestroyWindow_Helper(WindowControls.EllipseShapeMutationRange.InputField);
	DestroyWindow_Helper(WindowControls.EllipseShapeMutationRange.StaticText);

	DestroyWindow_Helper(WindowControls.EllipseColorMutationRange.InputField);
	DestroyWindow_Helper(WindowControls.EllipseColorMutationRange.StaticText);

	DestroyWindow_Helper(WindowControls.BoundingBoxMargin.InputField);
	DestroyWindow_Helper(WindowControls.BoundingBoxMargin.StaticText);

	DestroyWindow_Helper(WindowControls.ComparisonPrecision.InputField);
	DestroyWindow_Helper(WindowControls.ComparisonPrecision.StaticText);

	DestroyWindow_Helper(WindowControls.FileSelect.InputField);
	DestroyWindow_Helper(WindowControls.FileSelect.StaticText);

	DestroyWindow_Helper(WindowControls.FileSelect_Button);

	DestroyWindow_Helper(WindowControls.Start_Button);

	DestroyWindow_Helper(WindowControls.Pause_Resume_Button);

	DestroyWindow_Helper(WindowControls.SaveImage_Button);

	DeleteObject(WindowControls.ControlsFont);
}