// Generate opcodes utility
#include <stdio.h>

static void opcode1(unsigned p1, unsigned p2, unsigned rs, unsigned p4, unsigned p5, unsigned p6, const char * comment)
{
	unsigned result =
		((p1 & 0x7F) << 25) |	/* 31..25 */
		((p2 & 0x1F) << 20) |	/* 24..20 */
		((rs & 0x1F) << 15) |	/* 19..15 */
		((p4 & 0x07) << 12) |	/* 14..12 */
		((p5 & 0x1F) << 7) |	/* 11..7 */
		((p6 & 0x7F) << 0) |	/* 6..0 */
	0;

	printf("__asm__ __volatile__(\".4byte 0x%08x\":::\"memory\"); /* %s */\n", result, comment);
}

int main(int argc, char* argv[])
{
	unsigned rs = 10;	/* x10 = a0 */

	opcode1(0x01, 0x04, rs, 0x00, 0x00, 0x0b, "dcache.cva a0");	
	opcode1(0x01, 0x08, rs, 0x00, 0x00, 0x0b, "dcache.cpa a0");	
	opcode1(0x01, 0x06, rs, 0x00, 0x00, 0x0b, "dcache.iva a0");	
	opcode1(0x01, 0x0A, rs, 0x00, 0x00, 0x0b, "dcache.ipa a0");	
	opcode1(0x01, 0x07, rs, 0x00, 0x00, 0x0b, "dcache.civa a0");	
	opcode1(0x01, 0x0B, rs, 0x00, 0x00, 0x0b, "dcache.cipa a0");	

	opcode1(0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, "dcache.call");	
	opcode1(0x00, 0x19, 0x00, 0x00, 0x00, 0x0b, "sync.s");	

	return 0;
}
