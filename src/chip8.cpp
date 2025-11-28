#include "chip8.hpp"
#include <cstdint>

#include <iostream>
#include <fstream>

Chip8::Chip8() {

    m_index = 0;
    m_pc = 0x200;
    m_delayTimer = 0;
    m_soundTimer = 0;

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
        file.read(&m_memory[m_pc], 4096 - 0x200);
        std::cout << "ROM File successfully loaded" << std::endl;
        
    } else {

        std::cout << "Error while opening ROM" << std::endl;

    }
}
void Chip8::cycle() { // m_memory[m_pc] ist der Start also ab Memory 512 High und Low Byte werden kombiniert
    //High und Low werden zu einem opcode, der wird ausgeführt und der pc um 2 hochgezählt
    // danach werden die nächsten zwei Speicherblöcke zu einem Opcode usw.
    std::cout << "Test" << std::endl;
    m_opcode = (m_memory[m_pc] << 8) | m_memory[m_pc + 1]; 
    m_pc += 2; // danach PC +2 hochzählen

    //Decode wie n,nn,nnn,x,y, einbauen
    int n = m_opcode & 0x000F;
    int nn = m_opcode & 0x00FF;
    int nnn = m_opcode & 0x0FFF;
    int x = m_opcode & 0x0F00 >> 8;
    int y = m_opcode & 0x00F0 >> 4;
    
    //Jetzt dann Prüfung für die 4 Instructions
    //00E0
    //6xnn
    //Annn
    //Dxyn
    switch (m_opcode & 0xF000) {
        case 0x0000 :
          if (m_opcode == 0x00E0) {
            std::cout << "clear screen..." << std::endl;
            cleanScreen();
          }  
          break;
        
        case 0x6000 : //für 6XNN
            m_register[x] = nn;
          //normalen register sofort mit value laden
            break;

        case 0xA000 :
            m_index = nnn;
            //index register sofort mit value laden
            break;

        case 0xD000 : 
            //(m_register[x], m_register[y], n); 
            break;

    }

}

void Chip8::cleanScreen() {
    std::cout << "screen funk" << std::endl;
    m_video.fill(0);
}
