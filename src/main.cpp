#include <SFML/Graphics.hpp>
#include "chip8.hpp"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <cstdint>
#include <optional>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <iostream>
#include <unordered_map>

void drawDisplay(sf::RenderWindow& window, const Chip8& chip8, float scale)
{
    sf::RectangleShape pixel({scale, scale}); //den cast muss ich wegen sfml machen oder?
    pixel.setFillColor(sf::Color::Yellow);
    //Display(64x32) von Chip8 holen
    const auto& display = chip8.video();
    //disply holt das interne chip8 bild(aktueller zustand) = 
    // brücke zw. chip8 u. sfml
    for ( int y = 0; y < Chip8::screenHeight; y++) {
        for (int x = 0; x < Chip8::screenWidth; x++ ) {
            int index = y * Chip8::screenWidth + x;
            //wenn pixel "an", dann draw
            if (display[index] != 0U) {
                pixel.setPosition({(float)x * (float)scale, (float)y * (float)scale});
                window.draw(pixel);
            }
        }
    }
}




int main()
{
    constexpr int screenScale = 15;
    sf::RenderWindow window(sf::VideoMode({Chip8::screenWidth * screenScale , Chip8::screenHeight * screenScale}), "Chip-8 Emulator");
    Chip8 chip8;
    chip8.loadROM("keypad.ch8");
    window.setFramerateLimit(60);

    
 
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            //     for (int i = 0; i < 16; i++) {
            //         if (Chip8::keymap[i] == key->scancode) {
            //             chip8.setKey(i, true);
            //         }
            //     }
            // }

            // if (const auto* key = event->getIf<sf::Event::KeyReleased>()) {
            //     for (int j = 0; j < 16; j++) {
            //         if (Chip8::keymap[j] == key->scancode) {
            //             chip8.setKey(j, false);s
            //         }
            //     }
            // }
        
        }
        for (int i = 0; i < 10; i++) {
            chip8.cycle();
        }
        
        

       
        
        window.clear();
        drawDisplay(window, chip8, screenScale);
        window.display();
    }
}
