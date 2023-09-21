#include <Windows.h>
#include <iostream>
#include <thread>

HHOOK mouseHook;
bool blockNextLeftClick = false;
bool blockNextRightClick = false;
bool leftClickRegistered = false;
bool rightClickRegistered = false;
int leftDebounceTime = 0;
int rightDebounceTime = 0;
int lastLeftClickTime = 0;
int lastRightClickTime = 0; //swapped to int because of ste


bool isHidden = false; // flag for window being hidden

// colour!
void SetConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void RestartProgram() {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;

    TCHAR szAppName[MAX_PATH];
    if (GetModuleFileName(NULL, szAppName, MAX_PATH) != 0) {
        // creates a new instance
        if (CreateProcess(szAppName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }

    exit(0);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_LBUTTONDOWN) {
            int currentTime = GetTickCount64();
            int timeSinceLastClick = currentTime - lastLeftClickTime;
            lastLeftClickTime = currentTime;

            if (blockNextLeftClick || timeSinceLastClick <= leftDebounceTime) {
                blockNextLeftClick = false;
                leftClickRegistered = true;
                return 1;  // Block the left click event
            }
        }
        else if (wParam == WM_LBUTTONUP) {
            if (leftClickRegistered) {
                leftClickRegistered = false;
                return 1;  // Block the left release event
            }
        }
        else if (wParam == WM_RBUTTONDOWN) {
            int currentTime = GetTickCount64();
            int timeSinceLastClick = currentTime - lastRightClickTime;
            lastRightClickTime = currentTime;

            if (blockNextRightClick || timeSinceLastClick <= rightDebounceTime) {
                blockNextRightClick = false;
                rightClickRegistered = true;
                return 1;  // Block the right click event
            }
        }
        else if (wParam == WM_RBUTTONUP) {
            if (rightClickRegistered) {
                rightClickRegistered = false;
                return 1;  // Block the right release event
            }
        }
    }

    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}

void ToggleVisibility(HWND hWnd) {
    if (IsWindowVisible(hWnd)) {
        ShowWindow(hWnd, SW_HIDE);
        isHidden = true;
    }
    else {
        ShowWindow(hWnd, SW_SHOW);
        isHidden = false;
    }
}

void KeyPressHandler() {
    while (true) {
        if (GetAsyncKeyState(VK_F8) & 0x8000) {
            ToggleVisibility(GetConsoleWindow());
            Sleep(100);  // Delay to prevent multiple keypresses
            while (GetAsyncKeyState(VK_F8) & 0x8000) {}
        }

        if (GetAsyncKeyState(VK_F9) & 0x8000) {
            UnhookWindowsHookEx(mouseHook);
            std::cout << "Restarting the program..." << std::endl;
            system("cls");
            RestartProgram();       //Restart the program
            return;
        }

        Sleep(10); //apparently reduces cpu usage
    }
}

int main() {
    SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN);
        std::cout << R"(                                               
 ____   _____  ____    ___   _   _  _   _   ____  _____  ____  
|  _ \ | ____|| __ )  / _ \ | | | || \ | | / ___|| ____||  _ \ 
| | | ||  _|  |  _ \ | | | || | | ||  \| || |    |  _|  | |_) |
| |_| || |___ | |_) || |_| || |_| || |\  || |___ | |___ |  _ < 
|____/ |_____||____/  \___/  \___/ |_| \_| \____||_____||_| \_\                                                                                                                                                                                                                                   
        )" << '\n';

    std::cout << "Enter Left Debounce (Milliseconds): ";
    while (!(std::cin >> leftDebounceTime)) {
        std::cin.clear();
        std::cout << "Invalid input. Please enter an integer: ";
    }

    std::cout << "Enter Right Debounce (Milliseconds): ";
    while (!(std::cin >> rightDebounceTime)) {
        std::cin.clear();
        std::cout << "Invalid input. Please enter an integer: ";
    }

    HWND consoleWindow = GetConsoleWindow();
    system("cls");
    std::cout << R"(                                              
 ____   _____  ____    ___   _   _  _   _   ____  _____  ____  
|  _ \ | ____|| __ )  / _ \ | | | || \ | | / ___|| ____||  _ \ 
| | | ||  _|  |  _ \ | | | || | | ||  \| || |    |  _|  | |_) |
| |_| || |___ | |_) || |_| || |_| || |\  || |___ | |___ |  _ < 
|____/ |_____||____/  \___/  \___/ |_| \_| \____||_____||_| \_\                                                                                                                                                                                                                              
        )" << '\n';

    std::cout << "Mouse Debouncer has been initialized." << std::endl;
    std::cout << "Press F8 to toggle window visibility." << std::endl;
    std::cout << "Press F9 to restart the program." << std::endl;

    // Install the mouse hook
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);
    std::thread keyPressThread(KeyPressHandler);

    // Run the message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(mouseHook);
    keyPressThread.join();

    return 0;
}
// by itzowo :D
