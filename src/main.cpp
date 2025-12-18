#include <SFML/Graphics.hpp>
#include "chip8.hpp"
#include <SFML/System/Clock.hpp>
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

void drawDisplay(sf::RenderWindow& window, const Chip8& chip8, float scale)
{
    sf::RectangleShape pixel({scale, scale}); 
    pixel.setFillColor(sf::Color::Yellow);
    
    const auto& display = chip8.video();
    const int width = chip8.screenWidth();
    const int height = chip8.screenHeight();
    
    for ( int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++ ) {
            int index = y * width + x;
            
            if (display[index] != 0U) {
                pixel.setPosition({(float)x * (float)scale, (float)y * (float)scale});
                window.draw(pixel);
            }
        }
    }
}

int main()
{
    constexpr int screenScale = 10;
    Chip8 chip8;
    sf::RenderWindow window(sf::VideoMode({(128 * screenScale) ,(64 * screenScale)}), "Chip-8 Emulator");
    chip8.loadROM("gradsim.ch8");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        
        }
        for (int i = 0; i < 12; i++) {
            chip8.cycle();

            if (chip8.isDrawingInstruction()){
                break;
            }
        }
       
        chip8.delayTimer();
        
        window.clear(sf::Color(168,122,50));
        drawDisplay(window, chip8, screenScale);
        window.display();
    }
}
