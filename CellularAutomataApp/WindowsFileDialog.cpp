#include "WindowsFileDialog.h"
#include <windows.h>
#include <commdlg.h>
#include <fstream>
#include <cstring>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;


static std::wstring GetInitialDirectory()
{
    wchar_t exePath[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    std::wstring exeDir(exePath);
    size_t pos = exeDir.find_last_of(L"\\/");
    if (pos != std::wstring::npos)
    {
        exeDir = exeDir.substr(0, pos); // directory containing .exe
    }
    return exeDir;
}

static void createRulesDirectory()
{
    fs::path folder = L"Rules";
    if (!fs::exists(folder))
    {
        try
        {
            fs::create_directory(folder);
        }
        catch (const fs::filesystem_error& e)
        {
            std::wcerr << L"Error creating folder: " << e.what() << std::endl;
            return;
        }
    }
}

std::wstring WindowsFileDialog::OpenFileDialog(const wchar_t* filter)
{
	const auto initialDir = GetInitialDirectory();
    createRulesDirectory();

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
    ofn.lpstrInitialDir = initialDir.c_str();  // starting folder
    ofn.lpstrTitle = L"Open File";  // dialog title
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    return {};
}

std::wstring WindowsFileDialog::SaveFileDialog(const wchar_t* filter)
{
    const auto initialDir = GetInitialDirectory();
    createRulesDirectory();

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
    ofn.lpstrInitialDir = initialDir.c_str();
    ofn.lpstrTitle = L"Save As";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE)
    {
        return ofn.lpstrFile;
    }
    return {};
}
