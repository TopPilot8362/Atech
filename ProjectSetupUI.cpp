#include <windows.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cwctype>
#include <vector>

namespace fs = std::filesystem;

static const fs::path kWorkspaceRoot = L"D:\\AtechWorkspace\\ProjectBootstrap";
static const fs::path kStructuredRoot = kWorkspaceRoot / L"src";
static const fs::path kSingleRoot = kWorkspaceRoot / L"AtechProject";

static constexpr int ID_CREATE_STRUCTURED = 1001;
static constexpr int ID_CREATE_SINGLE = 1002;
static constexpr int ID_CLOSE = 1003;

static constexpr COLORREF kBackground = RGB(12, 18, 26);
static constexpr COLORREF kCard = RGB(23, 34, 44);
static constexpr COLORREF kAccent = RGB(94, 168, 255);
static constexpr COLORREF kAccent2 = RGB(40, 180, 130);
static constexpr COLORREF kText = RGB(233, 240, 248);
static constexpr COLORREF kSubText = RGB(164, 181, 198);

static HBRUSH gBackgroundBrush = nullptr;
static HWND gStatusEdit = nullptr;

static const wchar_t* kFolders[] = {
    L"D:\\src",
    L"D:\\src\\app",
    L"D:\\src\\components",
    L"D:\\src\\services",
    L"D:\\include",
    L"D:\\lib",
    L"D:\\build",
    L"D:\\assets"
};

static void AppendStatus(const std::wstring& message)
{
    if (!gStatusEdit)
        return;

    const int length = GetWindowTextLengthW(gStatusEdit);
    SendMessageW(gStatusEdit, EM_SETSEL, static_cast<WPARAM>(length), static_cast<LPARAM>(length));
    SendMessageW(gStatusEdit, EM_REPLACESEL, 0, reinterpret_cast<LPARAM>(message.c_str()));
    SendMessageW(gStatusEdit, EM_REPLACESEL, 0, reinterpret_cast<LPARAM>(L"\r\n"));
}

static bool WriteTextFile(const fs::path& path, const std::wstring& content)
{
    std::wofstream output(path, std::ios::binary);
    if (!output)
        return false;

    output << content;
    output.close();
    return output.good();
}

static bool CreateStarterTemplate(const fs::path& root)
{
    if (!fs::exists(root))
        return false;

    const std::wstring readme =
        L"# Project bootstrap\r\n\r\n"
        L"This folder was created by the D: drive setup UI.\r\n\r\n"
        L"## Structure\r\n"
        L"- app\r\n"
        L"- components\r\n"
        L"- services\r\n\r\n"
        L"Use this as a clean starting point for your project.\r\n";

    const std::wstring mainCpp =
        L"#include <iostream>\r\n\r\n"
        L"int main()\r\n"
        L"{\r\n"
        L"    std::cout << \"Project scaffold ready.\" << std::endl;\r\n"
        L"    return 0;\r\n"
        L"}\r\n";

    const std::wstring settings =
        L"// Project settings\r\n"
        L"#pragma once\r\n\r\n"
        L"constexpr const char* PROJECT_NAME = \"Atech Project\";\r\n";

    bool ok = true;
    ok = WriteTextFile(root / L"README.md", readme) && ok;
    ok = WriteTextFile(root / L"app" / L"main.cpp", mainCpp) && ok;
    ok = WriteTextFile(root / L"components" / L"ProjectSettings.h", settings) && ok;
    return ok;
}

static bool CreateStructuredProject(bool reportToUi = true)
{
    if (reportToUi)
        AppendStatus(L"Creating structured project folders in the workspace root...");

    std::error_code ec;
    if (!fs::create_directories(kWorkspaceRoot, ec))
    {
        if (reportToUi)
            AppendStatus(L"Could not create the workspace root.");
        return false;
    }

    bool ok = true;
    const std::vector<fs::path> folders = {
        kWorkspaceRoot,
        kStructuredRoot,
        kStructuredRoot / L"app",
        kStructuredRoot / L"components",
        kStructuredRoot / L"services",
        kWorkspaceRoot / L"include",
        kWorkspaceRoot / L"lib",
        kWorkspaceRoot / L"build",
        kWorkspaceRoot / L"assets"
    };

    for (const auto& folder : folders)
    {
        if (!fs::create_directories(folder, ec))
        {
            ok = false;
            if (reportToUi)
                AppendStatus(L"Failed: " + folder.wstring());
        }
        else if (reportToUi)
        {
            AppendStatus(L"Created: " + folder.wstring());
        }
    }

    if (reportToUi)
        AppendStatus(L"Generating starter files...");

    if (!CreateStarterTemplate(kStructuredRoot))
    {
        ok = false;
        if (reportToUi)
            AppendStatus(L"Starter template creation failed.");
    }
    else if (reportToUi)
    {
        AppendStatus(L"Starter template created successfully.");
    }

    if (reportToUi)
    {
        if (ok)
            AppendStatus(L"Structured project setup completed successfully.");
        else
            AppendStatus(L"One or more structured setup steps failed.");
    }

    return ok;
}

static bool CreateSingleFolderProject(bool reportToUi = true)
{
    const fs::path root = kSingleRoot;

    if (reportToUi)
        AppendStatus(L"Creating single-folder project at D:\\AtechWorkspace\\ProjectBootstrap\\AtechProject...");

    std::error_code ec;
    if (!fs::create_directories(root, ec))
    {
        if (reportToUi)
            AppendStatus(L"Could not create the single-folder project root.");
        return false;
    }

    const std::wstring readme =
        L"# Atech Project\r\n\r\n"
        L"Single-folder project created from the bootstrap UI.\r\n";

    const std::wstring mainCpp =
        L"#include <iostream>\r\n\r\n"
        L"int main()\r\n"
        L"{\r\n"
        L"    std::cout << \"Atech project ready.\" << std::endl;\r\n"
        L"    return 0;\r\n"
        L"}\r\n";

    bool ok = true;
    ok = WriteTextFile(root / L"README.md", readme) && ok;
    ok = WriteTextFile(root / L"main.cpp", mainCpp) && ok;

    if (reportToUi)
    {
        if (ok)
            AppendStatus(L"Single-folder project created successfully.");
        else
            AppendStatus(L"Single-folder project creation failed.");
    }

    return ok;
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
            SetWindowLongPtrW(hwnd, GWL_STYLE, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
            SetWindowPos(hwnd, nullptr, 0, 0, 480, 395, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

            const HINSTANCE instance = ((LPCREATESTRUCTW(lParam))->hInstance);

            auto* title = CreateWindowExW(
                0,
                L"STATIC",
                L"Atech Project Bootstrap",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                22,
                18,
                260,
                30,
                hwnd,
                nullptr,
                instance,
                nullptr);
            SendMessageW(title, WM_SETFONT, reinterpret_cast<WPARAM>(CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                L"Segoe UI")), TRUE);

            auto* subtitle = CreateWindowExW(
                0,
                L"STATIC",
                L"Create a clean workspace in D:\\AtechWorkspace\\ProjectBootstrap.",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                24,
                52,
                410,
                22,
                hwnd,
                nullptr,
                instance,
                nullptr);
            SendMessageW(subtitle, WM_SETFONT, reinterpret_cast<WPARAM>(CreateFontW(11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                L"Segoe UI")), TRUE);

            auto* structuredBtn = CreateWindowW(
                L"BUTTON",
                L"Structured D:\\src",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                22,
                92,
                170,
                38,
                hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_CREATE_STRUCTURED)),
                instance,
                nullptr);
            SendMessageW(structuredBtn, WM_SETFONT, reinterpret_cast<WPARAM>(CreateFontW(12, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                L"Segoe UI")), TRUE);

            auto* singleBtn = CreateWindowW(
                L"BUTTON",
                L"Single folder",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                210,
                92,
                170,
                38,
                hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_CREATE_SINGLE)),
                instance,
                nullptr);
            SendMessageW(singleBtn, WM_SETFONT, reinterpret_cast<WPARAM>(CreateFontW(12, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                L"Segoe UI")), TRUE);

            auto* closeBtn = CreateWindowW(
                L"BUTTON",
                L"Close",
                WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
                386,
                92,
                70,
                38,
                hwnd,
                reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_CLOSE)),
                instance,
                nullptr);
            SendMessageW(closeBtn, WM_SETFONT, reinterpret_cast<WPARAM>(CreateFontW(11, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                L"Segoe UI")), TRUE);

            gStatusEdit = CreateWindowExW(
                WS_EX_CLIENTEDGE,
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                22,
                145,
                434,
                180,
                hwnd,
                nullptr,
                instance,
                nullptr);
            SendMessageW(gStatusEdit, WM_SETFONT, reinterpret_cast<WPARAM>(CreateFontW(10, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                L"Consolas")), TRUE);

            AppendStatus(L"Ready to create your clean project workspace.");
            return 0;
        }

        case WM_COMMAND:
        {
            if (LOWORD(wParam) == ID_CREATE_STRUCTURED)
            {
                CreateStructuredProject();
                return 0;
            }

            if (LOWORD(wParam) == ID_CREATE_SINGLE)
            {
                CreateSingleFolderProject();
                return 0;
            }

            if (LOWORD(wParam) == ID_CLOSE)
            {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        }

        case WM_DESTROY:
            if (gBackgroundBrush)
            {
                DeleteObject(gBackgroundBrush);
                gBackgroundBrush = nullptr;
            }
            PostQuitMessage(0);
            return 0;

        default:
            break;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nShowCmd)
{
    std::wstring cmdLine = GetCommandLineW();
    std::transform(cmdLine.begin(), cmdLine.end(), cmdLine.begin(), [](unsigned char ch) { return static_cast<wchar_t>(std::towlower(ch)); });

    if (cmdLine.find(L"/create") != std::wstring::npos ||
        cmdLine.find(L"--create") != std::wstring::npos)
    {
        const bool ok = CreateStructuredProject(false);
        return ok ? 0 : 1;
    }

    if (cmdLine.find(L"/single") != std::wstring::npos ||
        cmdLine.find(L"--single") != std::wstring::npos)
    {
        const bool ok = CreateSingleFolderProject(false);
        return ok ? 0 : 1;
    }

    const wchar_t kClassName[] = L"ProjectSetupWindowClass";
    gBackgroundBrush = CreateSolidBrush(kBackground);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = gBackgroundBrush;
    wc.lpszClassName = kClassName;

    if (!RegisterClassW(&wc))
    {
        DeleteObject(gBackgroundBrush);
        gBackgroundBrush = nullptr;
        MessageBoxW(nullptr, L"Failed to register the app window.", L"Project setup", MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowExW(
        0,
        kClassName,
        L"Project Setup",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        480,
        395,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (!hwnd)
    {
        DeleteObject(gBackgroundBrush);
        gBackgroundBrush = nullptr;
        MessageBoxW(nullptr, L"Failed to create the app window.", L"Project setup", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}
