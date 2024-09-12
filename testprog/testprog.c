#include <stdint.h>

char C = 'O';

static inline void outb(int port, unsigned char data)
{
	asm("out %0, %1" : : "a"(data), "Nd"(port));
}

void print(char *s)
{
	while(*s) {
		asm("out %0, %1" : : "a" (*s), "Nd" (0x3f8) : );
		s++;
	}
}

__attribute__((naked)) void irq(void)
{
	print("Abacaba\n");
	asm("iretq");
}

#define _packed __attribute__((packed))

struct _packed idt_desc {
	uint16_t limit;
	uint64_t base;
};

struct _packed idt_entry {
	uint16_t offset0;
	uint16_t segment;
	uint8_t ist;
	uint8_t flags;
	uint16_t offset1;
	uint32_t offset2;
	uint32_t reserved;
};

static struct idt_entry idt_entries[256];

static struct idt_desc idt_table = {
	.limit = 0xFFF,
};

static void build_idt_entry(struct idt_entry *e, void *addr, uint16_t segment, uint8_t ist, uint16_t flags)
{
	uint64_t a = (uint64_t)addr;
	e->offset0 = a & 0xFFFFULL;
	e->offset1 = (a >> 16) & 0xFFFFULL;
	e->offset2 = (a >> 32) & 0xFFFFFFFFULL;
	e->segment = segment;
	e->ist = ist;
	e->flags = flags;
}

void __attribute__((naked)) sb_e(void) {
	print("Boom!\n");
	while(1);
}

#define PM_C 0x20
#define PS_C 0xA0
#define PM_D 0x21
#define PS_D 0xA1
int _start(void)
{
	print("Hello!\n");

	outb(PM_C, 0x11);
	outb(PS_C, 0x11);

	outb(PM_D, 0x20);
	outb(PS_D, 0x28);
	
	outb(PM_D, 0x02);
	outb(PS_D, 0x04);

	outb(PM_D, 0x01);
	outb(PS_D, 0x01);

	// Software interrupt and the actual eveng (e.g. int $0x03 and the breakpoint)
	// are not equal! (Try to replace it with cd 03 in the hext editor, otherwise it
	// is compiled into "CC" (int3))
	//asm("int $0x04");
	for(int i = 0; i < 256; i++)
		build_idt_entry(&idt_entries[i], sb_e, 0x08, 0, 0x8E);
	idt_table.base = 0xDEAD000;
	asm("lidt %0"::"m"(idt_table));

	//asm("int3");

	*((char *)0xfffff0101) = 0x12;

	print("Hello2!\n");
	//asm("sti");
	while(1);

	print("Hello2!\n");
	asm("hlt");
	return 0;
}
