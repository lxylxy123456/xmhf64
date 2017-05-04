/*
	broadcom 2837 (raspberry pi 3) definitions

	author: amit vasudevan (amitvasudevan@acm.org)
*/

#ifndef __BCM2837_H__
#define __BCM2837_H__

#define BCM2837_PERIPHERALS_BASE		0x3f000000
#define BCM2837_MAXCPUS				4

//GPIO registers
#define GPFSEL1         (BCM2837_PERIPHERALS_BASE+0x00200004)
#define GPPUD           (BCM2837_PERIPHERALS_BASE+0x00200094)
#define GPPUDCLK0       (BCM2837_PERIPHERALS_BASE+0x00200098)

//Auxillary registers control
#define AUX_ENABLES     (BCM2837_PERIPHERALS_BASE+0x00215004)

//Auxillary registers -- Mini UART
#define AUX_MU_IO_REG   (BCM2837_PERIPHERALS_BASE+0x00215040)
#define AUX_MU_IER_REG  (BCM2837_PERIPHERALS_BASE+0x00215044)
#define AUX_MU_IIR_REG  (BCM2837_PERIPHERALS_BASE+0x00215048)
#define AUX_MU_LCR_REG  (BCM2837_PERIPHERALS_BASE+0x0021504C)
#define AUX_MU_MCR_REG  (BCM2837_PERIPHERALS_BASE+0x00215050)
#define AUX_MU_LSR_REG  (BCM2837_PERIPHERALS_BASE+0x00215054)
#define AUX_MU_MSR_REG  (BCM2837_PERIPHERALS_BASE+0x00215058)
#define AUX_MU_SCRATCH  (BCM2837_PERIPHERALS_BASE+0x0021505C)
#define AUX_MU_CNTL_REG (BCM2837_PERIPHERALS_BASE+0x00215060)
#define AUX_MU_STAT_REG (BCM2837_PERIPHERALS_BASE+0x00215064)
#define AUX_MU_BAUD_REG (BCM2837_PERIPHERALS_BASE+0x00215068)


#define ARMLOCALREGISTERS_BASE					0x40000000
#define ARMLOCALREGISTERS_MAILBOXWRITE_BASE		(ARMLOCALREGISTERS_BASE + 0x80)
#define ARMLOCALREGISTERS_MAILBOXREADCLEAR_BASE	(ARMLOCALREGISTERS_BASE + 0xC0)


//DMA controller
#define BCM2837_DMA0_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007000)
#define BCM2837_DMA1_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007100)
#define BCM2837_DMA2_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007200)
#define BCM2837_DMA3_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007300)
#define BCM2837_DMA4_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007400)
#define BCM2837_DMA5_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007500)
#define BCM2837_DMA6_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007600)
#define BCM2837_DMA7_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007700)
#define BCM2837_DMA8_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007800)
#define BCM2837_DMA9_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007900)
#define BCM2837_DMA10_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007a00)
#define BCM2837_DMA11_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007b00)
#define BCM2837_DMA12_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007c00)
#define BCM2837_DMA13_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007d00)
#define BCM2837_DMA14_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00007e00)
#define BCM2837_DMA15_REGS_BASE	(BCM2837_PERIPHERALS_BASE + 0x00e05000)
#define BCM2837_DMA_INT_STATUS_BASE (BCM2837_PERIPHERALS_BASE + 0x00007fe0)
#define BCM2837_DMA_ENABLE_BASE (BCM2837_PERIPHERALS_BASE + 0x00007ff0)

#define BCM2837_DMA_NUMCHANNELS	16
#define BCM2837_DMA_MAXCBRECORDS	4096


//USB controller
#define DWC_REGS_BASE		(BCM2837_PERIPHERALS_BASE + 0x00980000)
#define DWC_NUM_CHANNELS 	8

#ifndef __ASSEMBLY__

typedef struct {
	u32 mailbox0write;
	u32 mailbox1write;
	u32 mailbox2write;
	u32 mailbox3write;
} armlocalregisters_mailboxwrite_t __attribute__((packed));


typedef struct {
	u32 mailbox0readclear;
	u32 mailbox1readclear;
	u32 mailbox2readclear;
	u32 mailbox3readclear;
} armlocalregisters_mailboxreadclear_t __attribute__((packed));


#endif // __ASSEMBLY__

#endif //__BCM2837_H__
