//******************************************************************************
// Copyright (C) Martin Laporte.
//******************************************************************************

#ifndef CORE_CONSOLE_H
#define CORE_CONSOLE_H

namespace Nutshell {
namespace Core {

//******************************************************************************
// This class encapsulates the kernel console.
//******************************************************************************
class Console : boost::noncopyable {
private:

    // Pointer to the screen
    static char* const  SCREEN;

    // The dimensions of the console
    static const int    CONSOLE_WIDTH   = 80;
    static const int    CONSOLE_HEIGHT  = 25;

    // The various colors used
    static const int    COLOR_NORMAL    = 0x07;
    static const int    COLOR_BRIGHT    = 0x0F;

    int         m_X;        // The current X position of the cursor.
    int         m_Y;        // The current Y position of the cursor.
    bool        m_Bright;   // Whether we're currently outputing in bright.

public:

    // Construction / destruction
    Console();
    ~Console();

    // Stream operators
    Console& operator<<(char p_Character);
    Console& operator<<(const char* p_pString);
    Console& operator<<(unsigned p_Value);
    Console& operator<<(int p_Value);
//    Console& operator<<(size_t p_Value);
    Console& operator<<(void* p_pValue);

private:

    // Misceallenous
    void ScrollConsole();
};

} // namespace Core
} // namespace Nutshell

#endif // !CORE_CONSOLE_H
