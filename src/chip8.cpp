#include "chip8.hpp"
#include <cstdint>

#include <iostream>
#include <fstream>

Chip8::Chip8() :m_index(0), m_pc(0x200), m_sp(0), m_stack() {

    //m_index = 0;
    //m_pc = 0x200;
    // m_delayTimer = 0;
    // m_soundTimer = 0;

//     const int FONTSET_START_ADDRESS = 0x50;
//     const int FONTSET_SIZE = 80;


//     uint8_t fontset[FONTSET_SIZE] =
//     {
// 	0xF0, 0x90, 0x90, 0x90, 0xF0,      // 0
// 	0x20, 0x60, 0x20, 0x20, 0x70,      // 1
// 	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
// 	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
// 	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
// 	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
// 	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
// 	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
// 	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
// 	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
// 	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
// 	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
// 	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
// 	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
// 	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
// 	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
//     };

}

void Chip8::loadROM(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (file) {
        // explizieter cast zu char*
        // example : reinterpret_cast<char*>(&d), sizeof d)
        // mit christoph geredet: streams sind doof
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
    m_opcode = (m_memory[m_pc] << 8) | m_memory[m_pc + 1]; 
    m_pc += 2; // danach PC +2 hochzählen

    //Decode wie n,nn,nnn,x,y, einbauen
    int n = m_opcode & 0x000F;
    int nn = m_opcode & 0x00FF;
    int nnn = m_opcode & 0x0FFF;
    int x = (m_opcode & 0x0F00) >> 8;
    int y = (m_opcode & 0x00F0) >> 4;
    
    switch (m_opcode & 0xF000) {
        case 0x0000 :
            
          if (m_opcode == 0x00E0) {
           // std::cout << "clear screen..." << std::endl;
            cleanScreen();
            
           }  else if (m_opcode == 0x00EE) {
            std::cout << "00EE" << std::endl;
            --m_sp;
            m_pc = m_stack[m_sp];
            
          } 
          break;

        case 0x2000: //2nnn
            std::cout << "2nnn" << std::endl;
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
                std::cout << "3xnn" << std::endl;
              m_pc += 2;
            }
            break;

        case 0x4000 :
            if (m_register[x] != nn) {
                std::cout << "4xnn" << std::endl;
                m_pc +=2;
            }

            break;
        case 0x5000 : //für 5xy0
            if (m_register[x] == m_register[y]) {
                std::cout << "5xy0" << std::endl;
                m_pc +=2;
            }
            break;
                        
        case 0x6000 : //für 6XNN
        //std::cout << "n register mit value laden" << std::endl;
            m_register[x] = nn;
          //normalen register sofort mit value laden
            break;
    
        case 0x7000 : //für 7XNN
            m_register[x] += nn;
            break;

        case 0x8000 : 

            switch (m_opcode & 0x000F){
            
                case 0x0000: 
                    std::cout << "8XY0" << std::endl;
                    m_register[x] = m_register[y];
                    break;
              
                case 0x0001:
                    std::cout << "8XY1" << std::endl;
                    m_register[x] |= m_register[y];
                    break;

                case 0x0002:
                    std::cout << "8XY2" << std::endl;
                    m_register[x] &= m_register[y];
                    break;

                case 0x0003: 
                    std::cout << "8XY3" << std::endl;
                    m_register[x] ^= m_register[y];
                    break;

                case 0x0004:
                    std::cout << "8XY4" << std::endl;
                    m_register[x] += m_register[y];
                    if (m_register[x] > 255) {
                        m_register[0xF] = 1;
                        
                    } else {
                        m_register[0xF] = 0;
                        
                    }
                    break;

                case 0x0005:
                    std::cout << "8XY5" << std::endl;
                     
                    if (m_register[y] > m_register[x]) {
                        m_register[0xF] = 0;
                        
                    } else {
                        m_register[0xF] = 1;
                    }
                   m_register[x] -= m_register[y];
                break;

                case 0x0006:
                // LEtzte Bit von VW wird gespeichert, schiebt VX um 1 nach rechts 1 wenn ungerade war, 0 wenn es gerade war
                    std::cout << "8XY6" << std::endl;
                    m_register[0xF] = m_register[x] & 0x1;
                    m_register[x] >>=1;
                   
                    break;

                case 0x000E:
                
                    std::cout << "8XYE" << std::endl;
                    m_register[0xF] = m_register[x] >> 7;
                    m_register[x] <<=1;
                    //(m_register[x] = m_register[y] << 1) || (m_register[x] = m_register[x] << 1);
                    break;

                case 0x0007:
                    std::cout << "8XY7" << std::endl;
                    m_register[x] = m_register[y] - m_register[x];
                    
                break;
       }
       break;
          
        case 0xA000 :
        //std::cout << "index regi mit value laden" << std::endl;
            m_index = nnn;
            //index register sofort mit value laden
            break;

        case 0xD000 : 
          //  std::cout << "Zeichnet?" << std::endl;
            drawSprite(m_register[x], m_register[y], n);
            //zeichnet n poxel and position Vx / Vy
            //(m_register[x], m_register[y], n); 
            break;

        case 0xF000 : 
            switch (m_opcode & 0x00FF) {    //FX55? 
                case 0x001E : 
                std::cout << "FX1E" << std::endl;
                m_index += m_register[x];
                break;
                //Wert von VX in Dezimalstellen zerlegen (Hunderter, Zehner, Einer)
                case 0x0033 :
                std::cout << "FX33" << std::endl;

                m_memory[m_index + 2] = m_register[x]  % 10;
                m_register[x] /= 10;
                
                m_memory[m_index + 1] = m_register[x] % 10; 
                m_register[x] /= 10;

                m_memory[m_index] = m_register[x] % 100;
                break;

                case 0x0055 : 
                std::cout << "FX55?" << std::endl;
                for (int i = 0; i <= x; ++i) {
                    m_memory[m_index + i] = m_register[i];
                }
                m_index += x + 1;
                m_pc += 2;  
                break;
                
                case 0x0065 :
                std::cout << "FX65" << std::endl;
                for (int i = 0; i <= x; ++i) {
                    m_register[i] = m_memory[m_index + i];
                
                } 
                // m_index += x + 1;
                // m_pc += 2;  
                break;
                
            } break;
        
        case 0x9000:
            std::cout << "9xy0" << std::endl;
            if (m_register[x] != m_register[y]) {
                m_pc += 2;
            }
    
    }

}

void Chip8::cleanScreen() {
    std::cout << "screen funk" << std::endl;
    m_display.fill(0);
}

void Chip8::drawSprite(uint8_t xPos, uint8_t yPos, int height) {
    //VF register auf 0 setzten
    m_register[0xF] = 0; //Wegen Kollision
    //Ein Sprite ist immer 8 Pixel/Bit breit aber die Höhe variiert, deswegen hier < height
    // deshalb hier schleife über die Zeilen(also höhe)
    for (int row = 0; row < height; row++) {
        //Jedes Zeile des Sprites ist 8 Bit also 1 Byte 
        uint8_t spriteByte = m_memory[m_index + row]; //indexregister
        //row = 0 → m_memory[m_index + 0] → 1. Sprite-Zeile, dann row++
        //row = 1 → m_memory[m_index + 1] → 2. Sprite-Zeile
        // weiß nicht ob "bit" hier der richtige Name ist für diese Sprite pixel
        // für jede der 8 Bit spalten in dem Byte
        for (int bit = 0; bit < 8; bit++) {
            //Ich muss jetzt das linkeste Bit bzw Pixel holen
            // verschiebung der pixel im Sprite
            if (spriteByte & (0x80 >> bit)) { //binär 1000 0000
                int px = (xPos + bit); //jetzt für jeden Pixel die Position berechen
                int py = (yPos + row);
                if (px <0 || px >= screenWidth || py <0 || py >= screenHeight) {
                    continue;
                }
                // umwandlung meines 2d arrays in ein 1d index mit dieser Formel: 
                int index = py * screenWidth + px;
                //kollision testen, wenn auf ein Pixel gezeichnet wird was 1 war, soll 0 werden
                //und anders herum 0 zu 1, das ganze dann mit XOR
                if (m_display[index] == 1) { //ist der pixel schon 1 ?
                    m_register[0xF] = 1; // dann flag register auf 1
                }
                m_display[index] = m_display[index] ^ 1;
                //1 XOR 1 = 0 → Pixel wird gelöscht
                //0 XOR 1 = 1 → Pixel wird gesetzt
            }
        }
    }
}
