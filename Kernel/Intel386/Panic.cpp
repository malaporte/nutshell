//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Intel386/Intel386.h"
#include "Intel386/Interrupts.h"
#include "Intel386/Paging.h"

namespace Nutshell {
namespace Intel386 {

namespace {

    // Screen constants
    unsigned short* const   SCREEN = reinterpret_cast<unsigned short*>(0xB8000);
    const int               SCREEN_WIDTH = 80;
    const int               SCREEN_HEIGHT = 25;
    const int               NORMAL_COLOR = 15;
    const int               NORMAL_BACKGROUND = 4;

    //**************************************************************************
    // Prints a string on the screen.
    //
    // Parameters:
    //  p_X          - X position.
    //  p_Y          - Y position.
    //  p_Text       - String to write.
    //  p_Foreground - Foreground color.
    //  p_Background - Background color.
    //**************************************************************************
    void PrintOnScreen(int p_X,
                       int p_Y,
                       const char* p_Text,
                       int p_Foreground = NORMAL_COLOR,
                       int p_Background = NORMAL_BACKGROUND)
    {
        // Output the string.
        while (*p_Text != 0)
        {
            SCREEN[p_Y * SCREEN_WIDTH + p_X++] = p_Background << 12 | p_Foreground << 8 | *(p_Text++);
        }
    }

    //**************************************************************************
    // Prints an integer on the screen.
    //
    // Parameters:
    //  p_X          - X position.
    //  p_Y          - Y position.
    //  p_Value      - Integer to write.
    //  p_Foreground - Foreground color.
    //  p_Background - Background color.
    //**************************************************************************
    void PrintOnScreen(int p_X,
                       int p_Y,
                       int p_Value,
                       int p_Foreground = NORMAL_COLOR,
                       int p_Background = NORMAL_BACKGROUND)
    {
        char buffer[15];

        // Account for negative values
        char* cur = buffer;
        if (p_Value < 0) {
            *(cur++) = '-';
            p_Value = -p_Value;
        }

        // Skip leading 0's
        int radix = 1000000000;
        while (radix != 0 && p_Value / radix == 0) {
            radix /= 10;
        }

        // Write the value
        while (radix != 0) {
            *(cur++) = '0' + p_Value / radix;
            p_Value %= radix;
            radix /= 10;
        }
        *cur = 0;

        // Output the string.
        PrintOnScreen(p_X, p_Y, buffer, p_Foreground, p_Background);
    }

    //**************************************************************************
    // Displays a panic and halts the system.
    //
    // Parameters:
    //  p_pMessage - The panic message.
    //  p_pFile    - The file where the panic occured.
    //  p_Line     - The line where the panic occured.
    //**************************************************************************
    extern "C" void DisplayPanic(const char* p_pMessage,
                                 const char* p_pFile,
                                 int p_Line)
    {
        // Blank the whole screen with red.
        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
            for (int x = 0; x < SCREEN_WIDTH; ++x) {
                SCREEN[y * SCREEN_WIDTH + x] = 4 << 12;
            }
        }

        // Write the panic information
        PrintOnScreen(2, 1, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! KERNEL PANIC !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        PrintOnScreen(2, 3, p_pMessage);
        PrintOnScreen(2, 5, "File:");
        PrintOnScreen(8, 5, p_pFile);
        PrintOnScreen(2, 6, "Line:");
        PrintOnScreen(8, 6, p_Line);

        // Halt the machine
        asm("hlt");
    }

} // anonymous namespace

//******************************************************************************
// Called when a kernel panic occurs. Never returns.
//
// Parameters:
//  p_pMessage - The panic message.
//  p_pFile    - The file where the panic occured.
//  p_Line     - The line where the panic occured.
//******************************************************************************
void Panic(const char* p_pMessage,
           const char* p_pFile,
           int p_Line)
{
    // Prevent us from being disturbed
    DisableInterrupts();

    // TODO: Retrieve information about the panic (stack trace, etc.)

    // To be  able to display the panic, we must restore the virtual  memory to
    // a  state  we know  is OK. We do this by  loading back the page directory
    // that was used for kernel initialization.
    //
    // Doing this will  most probably render our stack  invalid (we'll  use the
    // emergency one), but the kernel  code should remain available.  We should
    // not try to access any dynamically allocated data starting from here.

    // If the page directory isn't created yet, do not load it
    if (g_pPD == 0) {
        DisplayPanic(p_pMessage, p_pFile, p_Line);
    }

    // Load the page directory and display the panic
    asm(
        // Load the initialization page directory
        "movl   %0, %%cr3 ;"

        // Switch to the emergency stack
        "movl   %1, %%esp ;"

        // TODO: Restore video mode, etc...

        // Call the function that'll display the panic        
        "pushl  %4 ;"
        "pushl  %3 ;"
        "pushl  %2 ;"
        "call   _DisplayPanic ;"

        :
        : "r" (GetPhysicalAddress(g_pPD)),
          "r" (g_EmergencyStack + sizeof(g_EmergencyStack)),
          "r" (p_pMessage),
          "r" (p_pFile),
          "r" (p_Line)
    );
}

} // namespace Intel386
} // namespace Nutshell
