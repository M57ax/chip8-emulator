#include <SFML/Graphics.hpp>
#include "chip8.hpp"
#include <optional>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <iostream>

void drawDisplay(sf::RenderWindow& window, const Chip8& chip8, int scale)
{
    sf::RectangleShape pixel({(float)scale, (float)scale});
    pixel.setFillColor(sf::Color::White);
    //Display(64x32 von Chip8 holen)
    auto display = chip8.video();
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
        if (event->is<sf::Event::Closed>())
            window.close();
    }

    chip8.cycle(); // einmal pro Frame

    window.clear();
    drawDisplay(window, chip8, screenScale);
    window.display();
}
}
