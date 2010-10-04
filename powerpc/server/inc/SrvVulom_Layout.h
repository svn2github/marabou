//_______________________________________________________[C++ BIT DEFINITIONS]
//////////////////////////////////////////////////////////////////////////////
// Name:           SrvVulom_Layout.h
// Purpose:        Register Layout
// Description:    Definitions for VULOM modules (GSI)
//                 (originally: vulom3def.h)
// Keywords:
//////////////////////////////////////////////////////////////////////////////

#define VULOM_FLASHBASE 			0x00c00000
#define VULOM_FPGABASE				0x00000000
#define VULOM_RANGE_REG 			0x00800000
#define VULOM_FPGA_DIRECT			0x00800004

#define VULOM_CSRREG				0x00000000
#define VULOM_DISPRAM				0x00010000
#define VULOM_DISPREG				0x00020000
#define VULOM_SRAM					0x00100000

#define VULOM_PERIODREG 			0x0
#define VULOM_WIDTHREG				0x040
#define VULOM_NPREG 				0x080
#define VULOM_ENABLEREG 			0x0c0
#define VULOM_SCALERREG 			0x100

#define VULOM_CSR_DONE				0x20
#define VULOM_CSR_INIT				0x40

#define VULOM_TBOX_ENA				0x0
#define VULOM_TBOX_SCALL			0x4
#define VULOM_TBOX_SCALH			0x8
#define VULOM_TBOX_LATCH			0xc
#define VULOM_TBOX_SCALER1			0x40
#define VULOM_TBOX_SCALER2			0x80
#define VULOM_TBOX_SCALER3			0xc0

#define VULOM_TBOX_SCAL_ENA 		0x8
#define VULOM_TBOX_SCAL_WIDTH		0x0

#define VULOM_SCAL_SCAL1			0x40
#define VULOM_SCAL_SCAL2			0xc0

#define VULOM_GATE_DELAY			0x80
#define VULOM_GATE_WIDTH			0xc0

