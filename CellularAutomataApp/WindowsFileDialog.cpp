#include "WindowsFileDialog.h"
#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <cstring>
#include <iostream>

std::wstring WindowsFileDialog::OpenFileDialog(const wchar_t* filter)
{
	OPENFILENAME ofn;            // common dialog box structure
    wchar_t szFile[MAX_PATH] = { 0 }; // buffer for file name (Unicode)

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.lpstrTitle = L"Save As";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    return {};
}

std::wstring WindowsFileDialog::SaveFileDialog(const wchar_t* filter)
{
    OPENFILENAME ofn;               // common dialog box structure
    wchar_t szFile[MAX_PATH] = { 0 }; // buffer for file name

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr; // owner window (if you have a GUI window, pass its HWND)
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;

    // File filters: pairs of "description\0pattern\0"
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;  // starting folder
    ofn.lpstrTitle = L"Open File";  // dialog title
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    return {};
}
