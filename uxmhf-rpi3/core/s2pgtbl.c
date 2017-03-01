/*
	ARM 8 stage-2 page table translation functions

	author: amit vasudevan (amitvasudevan@acm.org)
*/

#include <types.h>
#include <arm8-32.h>
#include <bcm2837.h>
#include <miniuart.h>
#include <debug.h>

/* setup CPU to support stage-2 table translation */
void s2pgtbl_initialize(void){
	u32 vtcr, hdcr, hcptr, hstr;

	vtcr = sysreg_read_vtcr();
	bcm2837_miniuart_puts("VTCR before= ");
	debug_hexdumpu32(vtcr);

	vtcr = 0;
	vtcr |= VTCR_RES1_MASK;	//reserved 1 bits
	vtcr |= ((0 << VTCR_T0SZ_SHIFT) & VTCR_T0SZ_MASK);	//T0SZ=0; 32 bits physical address
	vtcr |= ((0 << VTCR_S_SHIFT) & VTCR_S_MASK);		//S=0
	vtcr |= ((1 << VTCR_SL0_SHIFT) & VTCR_SL0_MASK);	//SL0=1; 3-level page table
	vtcr |= ((MEM_WRITEBACK_READALLOCATE_WRITEALLOCATE << VTCR_IRGN0_SHIFT) & VTCR_IRGN0_MASK);	//L1 cache attribute
	vtcr |= ((MEM_WRITEBACK_READALLOCATE_WRITEALLOCATE << VTCR_ORGN0_SHIFT) & VTCR_ORGN0_MASK);	//L2 cache attribute
	vtcr |= ((MEM_OUTER_SHAREABLE << VTCR_SH0_SHIFT) & VTCR_SH0_MASK);	//shareability attribute

	sysreg_write_vtcr(vtcr);

	vtcr = sysreg_read_vtcr();
	bcm2837_miniuart_puts("VTCR after= ");
	debug_hexdumpu32(vtcr);

	hdcr = sysreg_read_hdcr();
	bcm2837_miniuart_puts("HDCR before= ");
	debug_hexdumpu32(hdcr);

	hdcr &= HDCR_HPMN_MASK;
	sysreg_write_hdcr(hdcr);

	hdcr = sysreg_read_hdcr();
	bcm2837_miniuart_puts("HDCR after= ");
	debug_hexdumpu32(hdcr);

	hcptr = sysreg_read_hcptr();
	bcm2837_miniuart_puts("HCPTR before= ");
	debug_hexdumpu32(hcptr);

	hcptr = 0;
	hcptr |= HCPTR_RES1_MASK;
	sysreg_write_hcptr(hcptr);

	hcptr = sysreg_read_hcptr();
	bcm2837_miniuart_puts("HCPTR after= ");
	debug_hexdumpu32(hcptr);

	hstr = sysreg_read_hstr();
	bcm2837_miniuart_puts("HSTR before= ");
	debug_hexdumpu32(hstr);

	sysreg_write_hstr(0);

	hstr = sysreg_read_hstr();
	bcm2837_miniuart_puts("HSTR after= ");
	debug_hexdumpu32(hstr);

}

__attribute__((section(".data"))) u64 l1_ldesc_table[L1_LDESC_TABLE_MAXENTRIES];
__attribute__((section(".data"))) u64 l2_ldesc_table[L1_LDESC_TABLE_ENTRIES * L2_LDESC_TABLE_MAXENTRIES];
__attribute__((section(".data"))) u64 l3_ldesc_table[L1_LDESC_TABLE_ENTRIES * L2_LDESC_TABLE_MAXENTRIES * L3_LDESC_TABLE_MAXENTRIES];

void s2pgtbl_populate_tables(void){
	u32 i;
	u64 attrs;

	//populate l1 ldesc table
	for(i=0; i < L1_LDESC_TABLE_MAXENTRIES; i++){
		if( i < L1_LDESC_TABLE_ENTRIES)
			l1_ldesc_table[i] = ldesc_make_s2_l1e_table((u32)&l2_ldesc_table[i * L2_LDESC_TABLE_MAXENTRIES]);
		else
			l1_ldesc_table[i] = ldesc_make_s2_l1e_invalid();
	}

	//debug
	//bcm2837_miniuart_puts("L1 LDESC table dump follows:\n");
	//for(i=0; i < L1_LDESC_TABLE_ENTRIES; i++){
	//	debug_hexdumpu32(l1_ldesc_table[i] >> 32);
	//	debug_hexdumpu32((u32)l1_ldesc_table[i]);
	//}

	//populate l2 ldesc table
	for(i=0; i < (L1_LDESC_TABLE_ENTRIES * L2_LDESC_TABLE_MAXENTRIES); i++){
		l2_ldesc_table[i] = ldesc_make_s2_l2e_table((u32)&l3_ldesc_table[i * L3_LDESC_TABLE_MAXENTRIES]);
	}

	attrs = (LDESC_S2_MC_OUTER_WRITE_BACK_CACHEABLE_INNER_WRITE_BACK_CACHEABLE << LDESC_S2_MEMATTR_MC_SHIFT) |
			(LDESC_S2_S2AP_READ_WRITE << LDESC_S2_MEMATTR_S2AP_SHIFT) |
			(MEM_OUTER_SHAREABLE << LDESC_S2_MEMATTR_SH_SHIFT);

	//debug
	bcm2837_miniuart_puts("L3 attrs=\n");
	debug_hexdumpu32(attrs >> 32);
	debug_hexdumpu32((u32)attrs);


	//populate l3 ldesc table
	for(i=0; i < (L1_LDESC_TABLE_ENTRIES * L2_LDESC_TABLE_MAXENTRIES * L3_LDESC_TABLE_MAXENTRIES); i++){
		l3_ldesc_table[i] = ldesc_make_s2_l3e_page((i * PAGE_SIZE_4K), attrs);
	}


}


void s2pgtbl_loadpgtblbase(void){
	u64 vttbr;

	vttbr = sysreg_read_vttbr();
	bcm2837_miniuart_puts("VTTBR before=");
	debug_hexdumpu32(vttbr >> 32);
	debug_hexdumpu32((u32)vttbr);

	vttbr = 0;
	vttbr |= ((u64)&l1_ldesc_table & VTTBR_BADDR_MASK);
	vttbr |= (((u64)0x2 << VTTBR_VMID_SHIFT) & VTTBR_VMID_MASK);
	sysreg_write_vttbr(vttbr);


	vttbr = sysreg_read_vttbr();
	bcm2837_miniuart_puts("VTTBR after=");
	debug_hexdumpu32(vttbr >> 32);
	debug_hexdumpu32((u32)vttbr);

}
