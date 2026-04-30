void kernel_main() {
    char* video_memory = (char*) 0xb8000;
    const char* msg = "hello from your kernel";

    for (int i = 0; msg[i] != '\0'; i++) {
        video_memory[i * 2] = msg[i];
        video_memory[i * 2 + 1] = 0x07;
    }
__asm__ volatile ("sti");

}
#include <stdint.h>

struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_entry idt[256];

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_ptr idtp;

static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

char keyboard_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0
};

void keyboard_handler() {
    unsigned char scancode = inb(0x60);

    if (scancode & 0x80) return; // key release, ignore

    char c = keyboard_map[scancode];

    if (c) {
        // print to screen (reuse your VGA function)
        char* video = (char*)0xb8000;
        static int pos = 0;
        video[pos++] = c;
        video[pos++] = 0x07;
    }
}

void pic_remap() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20); // IRQ0-7 -> INT 32+
    outb(0xA1, 0x28); // IRQ8-15 -> INT 40+

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}