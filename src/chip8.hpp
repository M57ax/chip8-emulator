#pragma once
#include <array>
#include <cstdint>
#include <string>

class Chip8{
    private:
        std::array<char, 4096> m_memory = {};
        std::array<std::uint8_t, 16> m_register = {};
        std::uint16_t m_index{};
        std::uint16_t m_pc{};     //program count
        std::uint16_t m_stack{};
        std::uint8_t m_delayTimer{};
        std::uint8_t m_soundTimer{};
        std::uint16_t m_opcode{};
        
        

    public:
        static constexpr int screenWidth = 64;
        static constexpr int screenHeight = 32;

        void cycle();

        void loadROM(const std::string& filePath);

    Chip8();
};
