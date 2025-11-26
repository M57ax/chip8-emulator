#include <SFML/Graphics.hpp>
#include "chip8.hpp"


int main()
{
    constexpr int screenScale = 15;
    sf::RenderWindow window(sf::VideoMode({Chip8::screenWidth * screenScale , Chip8::screenWidth * screenScale}), "Chip-8 Emulator");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}
