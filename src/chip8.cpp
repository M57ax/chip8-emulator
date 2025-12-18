#include "chip8.hpp"
#include <cstdint>

#include <cstddef>
#include <iostream>
#include <fstream>
#include <random>
#include "font.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <system_error>


Chip8::Chip8() :m_index(0), m_pc(0x200), m_sp(0), m_stack() {

    //m_index = 0;
    //m_pc = 0x200;
    // m_delayTimer = 0;
    // m_soundTimer = 0;
    for (std::size_t i = 0; i < fontset.size(); ++i) {
        m_memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

     for (std::size_t i = 0; i < schipset.size(); ++i) {
        m_memory[SCHIPSET_START_ADDRESS + i] = schipset[i];
    }
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
                --m_sp; 
                m_pc = m_stack[m_sp]; 
                break;
            case 0x00FE: 
                setHighRes(false);
                 std::cout << "Low\n";
                //setSchip(true);
                //cleanScreen();
                break;
            case 0x00FF:
                setHighRes(true);
                std::cout << "High\n";
                //setSchip(true);
                //cleanScreen();
                break;
            case 0x00FD:
                std::cout << "Programm Stop" << std::endl;
                break;
        }
    break;

        case 0x2000: //2nnn
            m_stack[m_sp] = m_pc;
            ++m_sp;
            m_pc = m_opcode & 0x0FFF;
            break;

        case 0x1000 : //für 1nnn jump
            m_pc = nnn;
            break;

        case 0x3000 :
            if (m_register[x] == nn) {
              m_pc += 2;
            }
            break;

        case 0x4000 :
            if (m_register[x] != nn) {
                m_pc +=2;
            }
            break;
        case 0x5000 : { //für 5xy0
        uint8_t vx = m_register[x];

        uint8_t vy = m_register[y];
            if (vx == vy) {
                m_pc +=2;
            }
            break;
        } 
        case 0x6000 : //für 6XNN
            m_register[x] = nn;
            break;
    
        case 0x7000 : //für 7XNN
            m_register[x] += nn;
            break;

        case 0x8000 : {
        uint8_t vx = m_register[x];

        uint8_t vy = m_register[y];

            switch (m_opcode & 0x000F){
            
                case 0x0000: 
                    m_register[x] = m_register[y];
                    break;
              
                case 0x0001:
                    m_register[x] |= m_register[y];
                    
                    if (!superChip) {
                        m_register[0xF] = 0;
                    }             
                break;


                case 0x0002:
                    m_register[x] &= m_register[y];
                     if (!superChip) {
                        m_register[0xF] = 0;
                    }             
                break;
                case 0x0003: 
                    m_register[x] ^= m_register[y];
                     if (!superChip) {
                        m_register[0xF] = 0;
                    }             
                break;
             case 0x0004:
                {
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
            break;
             case 0x0005: //8XY5
                {
                uint8_t vx = m_register[x];
                uint8_t vy = m_register[y];
                m_register[x] = static_cast<uint8_t>(vx - vy);
                if (vx >= vy) {
                    m_register[0xF] = 1;   // Kein Borrow
                } else {
                    m_register[0xF] = 0;   // Borrow
                }
                
                break;
            }
            break;
                case 0x0006: { //8XY6
                    uint8_t vx = m_register[x];
                    uint8_t vy = m_register[y];
                    if (superChip) {
                        m_register[0xF] = m_register[x] & 0x1u;
                        m_register[x] >>= 1;
                    } else { 
                    vx = vy;
                    m_register[0xF] = (vx & 0x1u);
                    m_register[x] = ( vx >>1);
                    }     
                break;
                }
                break;
                case 0x000E: { //8XYE
                    uint8_t vx = m_register[x];
                    uint8_t vy = m_register[y];
                    if (superChip) {
                        m_register[0xF] =  (m_register[x] & 0x80u) >> 7u;
                        m_register[x] <<= 1;
                    } else {
                        vx = vy;
                        m_register[0xF] = (vx & 0x80u) >> 7u;
                        m_register[x] = (vx <<1);
                        }
                    break;
                }
                break;
                case 0x0007: { //8XY/
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
            break;
                
       }
       break;
        }  
        case 0xA000 : //ANNN
            m_index = nnn;
            break;

        case 0xB000: //BNNN
            if (superChip) {        
                m_pc = nnn + m_register[x];
            } else {
                m_pc = nnn + m_register[0];
            }
        break;

        
        case 0xC000 : {
            std::random_device rd;
            std::mt19937 generate(rd());
            std::uniform_int_distribution<uint8_t> dist (0,255);
            m_register[x] = dist(generate) & nn;
            break;
        }
       case 0xD000:
            if (n == 0) {
                //setSchip(true);
                //std::cout << "Highres??" << std::endl;
                drawSprite(m_register[x], m_register[y], n);  
            } else {
                //setSchip(false);
                //std::cout << "lowres" << std::endl;
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

                case 0x0029:
                    m_index = 
                        FONTSET_START_ADDRESS + (m_register[x] * 5u);
                    break;

                case 0x0030:
                    m_index = SCHIPSET_START_ADDRESS + (m_register[x] * 10u);
                    break;
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
                    } if (!superChip) {
                        m_index = m_index + x + 1;
                        m_index = (uint16_t) m_index;
                    }    
                break;
            
                case 0x0065 :
                    for (int i = 0; i <= x; ++i) {
                        m_register[i] = m_memory[m_index + i];
                }  if (!superChip) {
                    m_index = m_index + x + 1;
                    m_index = (uint16_t) m_index;
                }
                break;
                
            } break;
        
        case 0x9000:
            if ((m_opcode & 0x000F) == 0x0 && m_register[x] != m_register[y]) {
                m_pc += 2;
            }
    break;
    default:
            std::cerr << m_opcode << std::endl;
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
        m_keypad[key] = pressed;
        
    } 
};

void Chip8::cleanScreen() {
    m_display.fill(0);
}


void Chip8::drawSprite(uint8_t xPos, uint8_t yPos, int height) {
    xPos = xPos % screenWidth(); //so nur für Startbit
    yPos = yPos % screenHeight();
    m_register[0xF] = 0; //Wegen Kollision

    if (height == 0) {
        for (int row = 0; row < 16; row++) {
            uint8_t spriteByte1 = m_memory[m_index + row *2]; // 0-7 *2 um den Start jeweils zu verschieben
            uint8_t spriteByte2 = m_memory[m_index + row *2 +1]; // 8 -15
            for (int bit = 0; bit < 16; bit++) {
                bool pixelOn = false;
                if (bit < 8) {
                     pixelOn = (spriteByte1 & (0x80 >> bit)); // != 0 dann true;
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
                int index = py * screenWidth() + px ;
                if (m_display[index] == 1) { //ist der pixel schon 1 ?
                    m_register[0xF] = 1; // dann flag register auf 1
                }
                m_display[index] ^= 1;
            }
        }
        return;
    }

    for (int row = 0; row < height; row++) {
        uint8_t spriteByte = m_memory[m_index + row]; //indexregister
        for (int bit = 0; bit < 8; bit++) {
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
            int index = py * screenWidth() + px ;
            if (m_display[index] == 1) { //ist der pixel schon 1 ?
                m_register[0xF] = 1; // dann flag register auf 1
            }
            m_display[index] ^= 1;
        }
    }   
}
