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
    chip8.loadROM("flags.ch8");
    // 1 Checkmark = Ergebniswert korrekt
    // 2 Checkmark = Flag Register VF korrekt
    // 3 Checkmark = VF als Eingabe Register Vy verwendbar also Verwendet mein Emulator den alten Wert von VF, wenn VF = Vy verwendet wird?
    // 4 Checkmark = VF als Eingabe Register Vx verwendbar also Verwendet mein Emulator den alten Wert von VF, wenn VF = Vx verwendet wird?
   
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
//Bin gerade dabei die Mathematischen Instructions anzupassen, damit die Flags richtig gesetzt sind.
// Fehler bei mir, ich überschreibe das Flagregister zu früh.
