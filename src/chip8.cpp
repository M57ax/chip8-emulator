#include "chip8.hpp"
#include <iostream>

Chip8::Chip8() {

    m_memory.fill(0);
    m_index = 0;
    m_pc = 0x200;
    m_delayTimer = 0;
    m_soundTimer = 0;
}
