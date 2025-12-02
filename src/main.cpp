#include <SFML/Graphics.hpp>
#include "chip8.hpp"
#include <optional>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <iostream>

void drawDisplay(sf::RenderWindow& window, const Chip8& chip8, float scale)
{
    sf::RectangleShape pixel({scale, scale}); //den cast muss ich wegen sfml machen oder?
    pixel.setFillColor(sf::Color::White);
    //Display(64x32) von Chip8 holen
    auto& display = chip8.video();
    //disply holt das interne chip8 bild(aktueller zustand) = 
    // brücke zw. chip8 u. sfml
    for ( int y = 0; y < chip8.screenWidth; y++) {
        for (int x = 0; x < chip8.screenHeight; x++ ) {
            int index = y * chip8.screenWidth + x;

            if (display[index]) {
                pixel.setPosition({x * (float)scale, y * (float)scale});
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
    chip8.loadROM("chip8-logo.ch8");
   
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        chip8.cycle(); // einmal pro Frame
        window.clear();
        drawDisplay(window, chip8, screenScale);
        window.display();
    }
}

//hier morgen code nochmal überarbeiten, die funktionen
//genau beschreiben und nochmal vernünftiger cleanup
