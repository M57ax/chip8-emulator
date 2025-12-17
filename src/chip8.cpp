#include "chip8.hpp"
#include <cstdint>

#include <iostream>
#include <fstream>
#include <random>
#include "font.hpp"
#include <SFML/Window/Keyboard.hpp>


Chip8::Chip8() :m_index(0), m_pc(0x200), m_sp(0), m_stack() {

    //m_index = 0;
    //m_pc = 0x200;
    // m_delayTimer = 0;
    // m_soundTimer = 0;
}

void Chip8::loadROM(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (file) {
        m_memory[0x1FF] = 0;
        file.read(reinterpret_cast<char*>(&m_memory[m_pc]), 4096 - 0x200);
        std::cout << "ROM File successfully loaded" << std::endl;
        
    } else {

        std::cout << "Error while opening ROM" << std::endl;

    }
}
void Chip8::cycle() { // m_memory[m_pc] ist der Start also ab Memory 512 High und Low Byte werden kombiniert
    //High und Low werden zu einem opcode, der wird ausgeführt und der pc um 2 hochgezählt
    // danach werden die nächsten zwei Speicherblöcke zu einem Opcode usw.
    //std::cout << "Test" << std::endl;
    m_opcode = (static_cast<uint16_t>(m_memory[m_pc]) << 8) |
           static_cast<uint16_t>(m_memory[m_pc + 1]);

    m_pc += 2; // danach PC +2 hochzählen

    //Decode wie n,nn,nnn,x,y, einbauen
    uint8_t n = m_opcode & 0x000F;
    uint8_t nn = m_opcode & 0x00FF;
    uint16_t nnn = m_opcode & 0x0FFF;
    uint8_t x = (m_opcode & 0x0F00U) >> 8U;
    uint8_t y = (m_opcode & 0x00F0U) >> 4U;
    //uint16_t sum = m_register[x] + m_register[y];
    
    switch (m_opcode & 0xF000) {
        case 0x0000:
            switch (m_opcode) {
            case 0x00E0: 
                cleanScreen(); 
                break;
            case 0x00EE: 
                --m_sp; m_pc = m_stack[m_sp]; 
                break;
            case 0x00FE: 
                setHighRes(false);
                //highres = false; cleanScreen();
                std::cout << "Low" << std::endl; 
                break;
            case 0x00FF:
                setHighRes(true);
                //highres = true;  cleanScreen(); 
                std::cout << "High" << std::endl;
                break;
            case 0x00FD:
                std::cout << "Programm Stop" << std::endl;
        break;
    }
    break;

        case 0x2000: //2nnn
            //std::cout << "2nnn" << std::endl;
            m_stack[m_sp] = m_pc;
            ++m_sp;
            m_pc = m_opcode & 0x0FFF;
            break;

        case 0x1000 : //für 1nnn jump
            //std::cout << "Jump" << std::endl;
            m_pc = nnn;
            break;

        case 0x3000 :
            if (m_register[x] == nn) {
                //std::cout << "3xnn" << std::endl;
              m_pc += 2;
            }
            break;

        case 0x4000 :
            if (m_register[x] != nn) {
                //std::cout << "4xnn" << std::endl;
                m_pc +=2;
            }
            break;
        case 0x5000 : { //für 5xy0
        uint8_t vx = m_register[x];

        uint8_t vy = m_register[y];
            if (vx == vy) {
                //std::cout << "5xy0" << std::endl;
                m_pc +=2;
            }
            break;
        } 
        case 0x6000 : //für 6XNN
        //std::cout << "n register mit value laden" << std::endl;
            m_register[x] = nn;
          //normalen register sofort mit value laden
            break;
    
        case 0x7000 : //für 7XNN
            m_register[x] += nn;
            break;

        case 0x8000 : {
        uint8_t vx = m_register[x];

        uint8_t vy = m_register[y];

            switch (m_opcode & 0x000F){
            
                case 0x0000: 
                    //std::cout << "8XY0" << std::endl;
                    m_register[x] = m_register[y];
                    break;
              
                case 0x0001:
                    //std::cout << "8XY1" << std::endl;
                    m_register[x] |= m_register[y];
                    m_register[0xF] = 0;
                    break;

                case 0x0002:
                    //std::cout << "8XY2" << std::endl;
                    m_register[x] &= m_register[y];
                    m_register[0xF] = 0;
                    break;

                case 0x0003: 
                  //  std::cout << "8XY3" << std::endl;
                    m_register[x] ^= m_register[y];
                    m_register[0xF] = 0;
                    break;

             case 0x0004:
                {
                //std::cout << "8XY4" << std::endl;
                //zuerst vx und vy aus den regsitern m_register[x ud y] lesen
                //und dann VF setzten
   
                uint8_t vx = m_register[x];
                uint8_t vy = m_register[y];
                uint16_t sum = vx+vy;
                m_register[x] = sum & 0xFFu;
                if (sum > 0xFFu) {
                    m_register[0xF] = 1;
                } else {
                    m_register[0xF] = 0;
                }
                
                
                
                break;
            }
             case 0x0005:
                {
                uint8_t vx = m_register[x];
                uint8_t vy = m_register[y];
                //std::cout << "8XY5" << std::endl;
                m_register[x] = static_cast<uint8_t>(vx - vy);
                if (vx >= vy) {
                    m_register[0xF] = 1;   // Kein Borrow
                } else {
                    m_register[0xF] = 0;   // Borrow
                }
                
                break;
            }
                case 0x0006: {
                // LEtzte Bit von VW wird gespeichert, schiebt VX um 1 nach rechts 1 wenn ungerade war, 0 wenn es gerade war
                    //std::cout << "8XY6" << std::endl;
                    uint8_t vx = m_register[x];
                    uint8_t vy = m_register[y];
                    vx = vy;
                    m_register[0xF] = (vx & 0x1u);
                    m_register[x] = ( vx >>1);
                    
                    break;
                }
                case 0x000E: {
                    uint8_t vx = m_register[x];
                    uint8_t vy = m_register[y];
                    vx = vy;
                    //std::cout << "8XYE" << std::endl;
                    m_register[0xF] = (vx & 0x80u) >> 7u;
                    m_register[x] = (vx <<1);
                    //ich hatte davor das ale vVx genommen (also das ohne vx = vy)
                    
                    
                    
                    break;
                }
                case 0x0007: {
                    //std::cout << "8XY7" << std::endl;
                    uint8_t vx = m_register[x];
                    uint8_t vy = m_register[y];
                    m_register[x] = static_cast<uint8_t>(vy - vx);
                    if (vy >= vx) {
                        m_register[0xF] = 1;
                    } else {
                        m_register[0xF] = 0;
                    }
                    
                    
                break;
                }
       }
       break;
        }  
        case 0xA000 :
        //std::cout << "index regi mit value laden" << std::endl;
            m_index = nnn;
            //index register sofort mit value laden
            break;

        case 0xB000 :
        std::cout << "BNNN" << std::endl;
            m_pc = nnn + m_register[0];
            
            break;

        case 0xC000 : {
            std::random_device rd;
            std::mt19937 generate(rd());
            std::uniform_int_distribution<uint8_t> dist (0,255);
            m_register[x] = dist(generate) & nn;
            break;
        }
       case 0xD000:
            if (n == 0 && highres) {
                std::cout << "Highres??" << std::endl;
                drawSprite(m_register[x], m_register[y], n);  
            } else {
                std::cout << "lowres" << std::endl;
                drawSprite(m_register[x], m_register[y], n);  
        }
        break;

        case 0xE000 : 
            switch (m_opcode & 0x00FF) {
                    case 0x009E: {
                        auto i = m_register[x] & 0xF;
                    if (sf::Keyboard::isKeyPressed(keymap[i])) m_pc += 2;
                    break;
                    }
                    case 0x00A1: {
                        auto i = m_register[x] & 0xF;
                        if (!sf::Keyboard::isKeyPressed(keymap[i])) m_pc += 2;
                    break;
                    }
                }
            break;
        case 0xF000 : 
            switch (m_opcode & 0x00FF) {   
                case 0x007 :
                    m_register[x] = m_delayTimer;
                    break;

                case 0x015 :
                    m_delayTimer = m_register[x];
                    break;

                case 0x001E : 
                    m_index += m_register[x];
                    break;
                //Wert von VX in Dezimalstellen zerlegen (Hunderter, Zehner, Einer)
                case 0x0033 :
                {
                    uint8_t value = m_register[x];
                    m_memory[m_index + 2] = value  % 10;
                    value /= 10;
                
                    m_memory[m_index + 1] = value % 10; 
                    value /= 10;

                    m_memory[m_index] = value % 10;
                break;
                }

                case 0x000A : 
                {
                    const auto key = keymap[m_key_down];
                    if (m_key_down != 255 && !sf::Keyboard::isKeyPressed(key)) {
                        m_register[x] = m_key_down;
                        m_key_down = 255;
                        return;
                    }
                    for (uint8_t i = 0; i < static_cast<uint8_t>(keymap.size()); i++) {
                        if (sf::Keyboard::isKeyPressed(keymap[i])) {
                            m_key_down = i;
                        }
                    }
                    m_pc -= 2;
                    break;
                }
                break;
                case 0x0055 : 
                    for (int i = 0; i <= x; ++i) {  
                        m_memory[m_index + i] = m_register[i];
                    }
                m_index = m_index + x + 1;
                m_index = (uint16_t) m_index;

                break;
                
                case 0x0065 :
                    for (int i = 0; i <= x; ++i) {
                        m_register[i] = m_memory[m_index + i];
                }   
                m_index = m_index + x + 1;
                m_index = (uint16_t) m_index;

                break;
                
            } break;
        
        case 0x9000:
            if ((m_opcode & 0x000F) == 0x0 && m_register[x] != m_register[y]) {
                m_pc += 2;
            }
    break;
    
    }
}

bool Chip8::isDrawingInstruction() {
    return (m_opcode & 0xF000) == 0xD000;
}

void Chip8::delayTimer() {
    if (m_delayTimer > 0) {
        m_delayTimer -=1;
    }
}

void Chip8::setKey(uint8_t key, bool pressed) {
    if (key < m_keypad.size()) {
        std::cout << "Teeest" << std::endl;
        m_keypad[key] = pressed;
        
    } 
};

void Chip8::cleanScreen() {
    std::cout << "screen funk" << std::endl;
    m_display.fill(0);
}

//    erklärung der drawsprite funktion
//     am anfang werden die startbit mit xpos und ypos begrentzt.
//     - das 0xF register wird für die Kollisionen auf 0 gesetzt.
//     - dann kommt der if pfad mit der prüfung ob das bit gleich 0 ist und highres true;
//     - wenn das der fall ist wird der highres pfad genutzt. 
//     - in der äußeren for schleife gehe ich durch die zeilen, da highres sind es 16.
//     - da es im schip 16 bit sind, muss es hier zwei mal das spriteByte geben, das linke und das rechte 1 ist von 0 bis 7 und spriteByte2 von 8 bis 15
//     - +1 , da es immer zwei sind ? 
//     jetzt müssen wir schauen ob das bit an oder aus ist. quasi 0 oder 1, dafür prüfen wir zuerst ob es im ersten byte ist oder im zweiten, wenn das false, wird einer weitergemacht
//     jetzt wird für jeden pixel die position berechnet.
//     dann das array vom 2d in ein 1d mit der formel umberechnen 


void Chip8::drawSprite(uint8_t xPos, uint8_t yPos, int height) {
    xPos = xPos % screenWidth(); //so nur für Startbit
    yPos = yPos % screenHeight();
    m_register[0xF] = 0; //Wegen Kollision

   
    if (height == 0 && highres) {
        for (int row = 0; row < 16; row++) {
            
            //jeder zeile hat 16 bits also 2 bytes
            uint8_t spriteByte1 = m_memory[m_index + row ]; // 0-7 *2 um den Start jeweils zu verschieben
            uint8_t spriteByte2 = m_memory[m_index + row  +1]; // 8 -15
            // +1 weil jede Zeile aus 2 aufeinanderf. Bytes besteht 
            for (int bit = 0; bit < 16; bit++) {
                bool pixelOn = false;
                if (bit < 8) {
                     pixelOn = (spriteByte1 & (0x80 >> bit)); // != 0 dann true;
                    //bit isolierung
                } else {
                   pixelOn = (spriteByte2 & (0x80 >> (bit -8 )));
                } 
                
                if (!pixelOn) {
                    continue;
                }
               
                int px = (xPos + bit); //jetzt für jeden Pixel die Position berechen
                int py = (yPos + row); 

                if (px >= screenWidth() || py >= screenHeight()) {
                    continue;
                }
                // umwandlung meines 2d arrays in ein 1d index mit dieser Formel: 
                int index = py * screenWidth() + px ;
                //kollision testen, wenn auf ein Pixel gezeichnet wird was 1 war, soll 0 werden
                //und anders herum 0 zu 1, das ganze dann mit XOR
                if (m_display[index] == 1) { //ist der pixel schon 1 ?
                    m_register[0xF] = 1; // dann flag register auf 1
                }
                m_display[index] ^= 1;
            }
        }
        return;
    }

    //Ein Sprite ist immer 8 Pixel/Bit breit aber die Höhe variiert, deswegen hier < height
    // deshalb hier schleife über die Zeilen(also höhe)
    for (int row = 0; row < height; row++) {
        //Jedes Zeile des Sprites ist 8 Bit also 1 Byte 
        uint8_t spriteByte = m_memory[m_index + row]; //indexregister
        //row = 0 → m_memory[m_index + 0] -> 1. Sprite-Zeile, dann row++
        //row = 1 → m_memory[m_index + 1] -> 2. Sprite-Zeile
        // für jede der 8 Bit spalten in dem Byte
        // 8 Pixel pro zeile
        for (int bit = 0; bit < 8; bit++) {
            //Ich muss jetzt das linkeste Bit bzw Pixel holen
            // verschiebung der pixel im Sprite
            bool pixelOn = false;
            pixelOn  = (spriteByte & (0x80 >> bit));
            if (!pixelOn) { 
                continue;
            };  //binär 1000 0000
            int px = (xPos + bit); //jetzt für jeden Pixel die Position berechen
            int py = (yPos + row); //mit Modulo den Wrap nach links und rechts richtig machen
            if (px >= screenWidth() || py >= screenHeight()) {
                continue;
            }
            // umwandlung meines 2d arrays in ein 1d index mit dieser Formel: 
            int index = py * screenWidth() + px ;
            //kollision testen, wenn auf ein Pixel gezeichnet wird was 1 war, soll 0 werden
            //und anders herum 0 zu 1, das ganze dann mit XOR
            if (m_display[index] == 1) { //ist der pixel schon 1 ?
                m_register[0xF] = 1; // dann flag register auf 1
            }
            m_display[index] ^= 1;
            //1 XOR 1 = 0 → Pixel wird gelöscht
            //0 XOR 1 = 1 → Pixel wird gesetzt
        }
    }
    
}
