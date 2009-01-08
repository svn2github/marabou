
#define FLASHBASE   0x00c00000
#define FPGABASE    0x00000000
#define RANGE_REG   0x00800000
#define FPGA_DIRECT 0x00800004

#define CSRREG      0x00000000
#define DISPRAM     0x00010000
#define DISPREG     0x00020000
#define SRAM        0x00100000

#define PERIODREG   0x0
#define WIDTHREG    0x040
#define NPREG       0x080
#define ENABLEREG   0x0c0
#define SCALERREG	0x100
//#define MODEREG		0x140

#define CSR_DONE    0x20
#define CSR_INIT    0x40


#define TBOX_ENA	0x0
#define TBOX_SCALL	0x4
#define TBOX_SCALH	0x8
#define TBOX_LATCH	0xc
#define TBOX_SCALER1 0x40
#define TBOX_SCALER2 0x80
#define TBOX_SCALER3 0xc0

#define TBOX_SCAL_ENA	0x8
#define TBOX_SCAL_WIDTH 0x0

#define SCAL_SCAL1	0x40
#define SCAL_SCAL2  0xc0

#define GATE_DELAY	0x80
#define GATE_WIDTH	0xc0


#define N_SCALER	32



#define MAXBLOCK    63
#define BS  19
