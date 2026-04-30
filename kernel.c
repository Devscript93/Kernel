#include <stdint.h>

#define VGA_WIDTH 80

void print(const char* str) {
    static char* video = (char*)0xb8000;
    static int pos = 0;
    
    for (int i = 0; str[i]; i++) {
        video[pos++] = str[i];
        video[pos++] = 0x07;
        if (pos >= VGA_WIDTH * 25 * 2) pos = 0;
    }
}

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

char keyboard_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0
};

static int cursor_pos = 160;

void keyboard_handler() {
    unsigned char scancode = inb(0x60);
    if (scancode & 0x80) return;

    char c = keyboard_map[scancode];
    if (c) {
        char* video = (char*)0xb8000;
        video[cursor_pos++] = c;
        video[cursor_pos++] = 0x07;
        
        if (cursor_pos >= 80*25*2) cursor_pos = 160;
    }
}

void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

void kernel_main() {
    char* video_memory = (char*) 0xb8000;
    const char* msg = "hello from your kernel - type keys below!";

    for (int i = 0; msg[i] != '\0'; i++) {
        video_memory[i * 2] = msg[i];
        video_memory[i * 2 + 1] = 0x07;
    }
    
    pic_remap();
    __asm__ volatile ("sti");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
