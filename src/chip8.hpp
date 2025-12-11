#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <SFML/Window/Keyboard.hpp>

class Chip8{
    public:
        static constexpr int screenWidth = 64;
        static constexpr int screenHeight = 32;

    private:
        // hatte davor char(war blöd, ich weiß) und das kann sehr leicht vom compiler 
        // als signed behandelt werden
        std::array<std::uint8_t, 4096> m_memory = {};
        std::array<std::uint8_t, 16> m_register = {};
        std::uint16_t m_index{};
        std::uint16_t m_pc{};     //program count
        std::array<std::uint16_t, 16> m_stack = {};
        std::uint16_t m_sp; // stackPointer
        std::uint8_t m_delayTimer{};
        // std::uint8_t m_soundTimer{};
        std::uint8_t m_key_down{255};
        std::uint16_t m_opcode{};
        std::array<uint8_t, screenHeight * screenWidth> m_display = {};  
        std::array<bool, 16> m_keypad{};
       
    public:
        
        void setKey(uint8_t key, bool pressed);
        void cycle();
        void drawSprite(uint8_t xPos, uint8_t yPos, int height);
        void cleanScreen();
        void loadROM(const std::string& filePath);
        void delayTimer();
        const std::array<uint8_t, screenWidth*screenHeight>& video() const { return m_display; }
        bool isDrawingInstruction();

        static constexpr std::array<sf::Keyboard::Scancode, 16> keymap {
    sf::Keyboard::Scancode::X, sf::Keyboard::Scancode::Num1, sf::Keyboard::Scancode::Num2, sf::Keyboard::Scancode::Num3, sf::Keyboard::Scancode::Q,
     sf::Keyboard::Scancode::W, sf::Keyboard::Scancode::E, sf::Keyboard::Scancode::A, sf::Keyboard::Scancode::S, sf::Keyboard::Scancode::D, sf::Keyboard::Scancode::Z, 
     sf::Keyboard::Scancode::C, sf::Keyboard::Scancode::Num4, sf::Keyboard::Scancode::R, sf::Keyboard::Scancode::F, sf::Keyboard::Scancode::V,
};



    Chip8();
};
