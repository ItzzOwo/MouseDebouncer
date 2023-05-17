#include <Windows.h>
#include <iostream>

// Global variables
HHOOK mouseHook;
bool blockNextLeftClick = false;
bool blockNextRightClick = false;
bool leftClickRegistered = false;
bool rightClickRegistered = false;
DWORD lastLeftClickTime = 0;
DWORD lastRightClickTime = 0;
DWORD leftDebounceTime = 0;
DWORD rightDebounceTime = 0;

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        if (wParam == WM_LBUTTONDOWN) {
            DWORD currentTime = GetTickCount();
            DWORD timeSinceLastClick = currentTime - lastLeftClickTime;
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
            DWORD currentTime = GetTickCount();
            DWORD timeSinceLastClick = currentTime - lastRightClickTime;
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

int main() {
    std::cout << "Enter the debounce time for left click (in milliseconds): ";
    std::cin >> leftDebounceTime;
    std::cout << "Enter the debounce time for right click (in milliseconds): ";
    std::cin >> rightDebounceTime;
    std::cout << "Mouse Debouncer has been initialized." << std::endl;

    // Install the mouse hook
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, 0);

    // Run the message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Uninstall the mouse hook
    UnhookWindowsHookEx(mouseHook);

    return 0;
}

//made by ItzOwo