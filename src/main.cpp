#include <SFML/Graphics.hpp>
#include "chip8.hpp"

#include <iostream>


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
            chip8.cycle();

            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        //window.draw();
        window.display();
    }
}
