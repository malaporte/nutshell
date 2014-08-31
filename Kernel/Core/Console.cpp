//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#include "Global.h"
#include "Core/Console.h"

namespace Nutshell {
namespace Core {

char* const Console::SCREEN = reinterpret_cast<char*>(0xB8000);

//******************************************************************************
// Constructor.
//******************************************************************************
Console::Console()
:   m_X(0),
    m_Y(0),
    m_Bright(false)
{
    assert(this != 0);

    // Clear the screen
    memset(SCREEN, 0, CONSOLE_WIDTH * CONSOLE_HEIGHT * 2);
}

//******************************************************************************
// Destructor.
//******************************************************************************
Console::~Console()
{
    assert(this != 0);
}

//******************************************************************************
// char stream operator.
//******************************************************************************
Console& Console::operator<<(char p_Character)
{
    assert(this != 0);

    // Check if the character is a newline
    if (p_Character == '\n') {
        // Go to the next line
        m_X = 0;
        ++m_Y;
    } else {
        // Write the character
        SCREEN[m_Y * CONSOLE_WIDTH * 2 + m_X * 2] = p_Character;
        SCREEN[m_Y * CONSOLE_WIDTH * 2 + m_X * 2 + 1] = m_Bright ? COLOR_BRIGHT : COLOR_NORMAL;

        // Advance the cursor
        ++m_X;
    }

    // Check if the cursor is at the end of a line
    if (m_X == CONSOLE_WIDTH) {
        // Bring the cursor back to the left and advance one line
        m_X = 0;
        ++m_Y;
    }

    // Check if we're at the bottom of the console
    if (m_Y == CONSOLE_HEIGHT) {
        // Scroll the console
        memmove(SCREEN, SCREEN + CONSOLE_WIDTH * 2, CONSOLE_WIDTH * CONSOLE_HEIGHT * 2);
        memset(SCREEN + (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH * 2, 0, CONSOLE_WIDTH * 2);
        --m_Y;
    }

    return *this;
}


//******************************************************************************
// char* stream operator.
//******************************************************************************
Console& Console::operator<<(const char* p_pString)
{
    assert(this != 0);

    // Write all the characters in the string
    while (*p_pString != 0) {
        // Check if the current character is a color code
        if (*p_pString == '#') {
            // Toggle the bright attribute
            m_Bright = !m_Bright;
        } else {
            // Write the character
            *this << *p_pString;
        }

        // Go to the next character
        ++p_pString;
    }

    return *this;
}

//******************************************************************************
// unsigned stream operator.
//******************************************************************************
Console& Console::operator<<(unsigned p_Value)
{
    assert(this != 0);

    // If the value is 0 we treat it differently
    if (p_Value != 0) {           
        // This is the radix used to extract digits from the value
        unsigned radix = 1000000000;

        // Skip until we find one significant digit
        while (radix != 0 && p_Value / radix == 0) {
            radix /= 10;
        }

        // Write all significant digits
        while (radix != 0) {
            // Write the current digit
            *this << static_cast<char>(p_Value / radix + '0');

            // Update values
            p_Value %= radix;
            radix /= 10;
        }
    } else {
        *this << '0';
    }

    return *this;
}

//******************************************************************************
// int stream operator.
//******************************************************************************
Console& Console::operator<<(int p_Value)
{
    assert(this != 0);

    // Check if the value is negative
    if (p_Value < 0) {
        // Write the minus character and inverse it
        *this << '-';
        p_Value = -p_Value;
    }

    // Use the unsigned stream operator to write the positive value
    *this << static_cast<unsigned>(p_Value);

    return *this;
}

//******************************************************************************
// size_t stream operator.
//******************************************************************************
/*
Console& Console::operator<<(size_t p_Value)
{
    assert(this != 0);

    // Write the hexadecimal prefix
    *this << "0x";

    // Write all digits
    for (int i = 0; i < 8; ++i) {
        // Compute the current digit
        int digit = (p_Value & 0xF0000000) >> 28;

        // Write the current digit
        if (digit < 10) {
            *this << static_cast<char>(digit + '0');
        } else {
            *this << static_cast<char>((digit - 10) + 'A');
        }

        // Update the value
        p_Value <<= 4;        
    }

    return *this;
}
*/
//******************************************************************************
// void* stream operator.
//******************************************************************************
Console& Console::operator<<(void* p_pValue)
{
    assert(this != 0);

    // Use the size_t operator
    return operator<<(reinterpret_cast<size_t>(p_pValue));
}

//******************************************************************************
// Scrolls the console down.
//******************************************************************************
void Console::ScrollConsole()
{
    assert(this != 0);

    // Copy all the lines on line up
    for (int i = 1; i < CONSOLE_HEIGHT; ++i) {
        for (int j = 0; j < CONSOLE_WIDTH * 2; ++j) {
            SCREEN[i * CONSOLE_WIDTH + j] = SCREEN[(i + 1) * CONSOLE_HEIGHT + j];
        }
    }
}

} // namespace Core
} // namespace Nutshell
