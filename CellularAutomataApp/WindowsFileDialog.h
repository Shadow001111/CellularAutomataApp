#pragma once
#include <string>

class WindowsFileDialog
{
public:
    static std::wstring OpenFileDialog(const wchar_t* filter = L"All Files\0*.*\0");
    static std::wstring SaveFileDialog(const wchar_t* filter = L"All Files\0*.*\0");
};

