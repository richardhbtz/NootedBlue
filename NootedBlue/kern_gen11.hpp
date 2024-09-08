//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#ifndef kern_gen11_hpp
#define kern_gen11_hpp
#include "kern_nblue.hpp"
#include "kern_guc.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_util.hpp>




static constexpr uint32_t ICL_REG_CDCLK_CTL = 0x46000;

static constexpr uint32_t ICL_REG_DSSM = 0x51004;


enum ICLReferenceClockFrequency {
	
	// 24 MHz
	ICL_REF_CLOCK_FREQ_24_0 = 0x0,
	
	// 19.2 MHz
	ICL_REF_CLOCK_FREQ_19_2 = 0x1,
	
	// 38.4 MHz
	ICL_REF_CLOCK_FREQ_38_4 = 0x2
};


enum ICLCoreDisplayClockDecimalFrequency {
	
	// 172.8 MHz
	ICL_CDCLK_FREQ_172_8 = 0x158,
	
	// 180 MHz
	ICL_CDCLK_FREQ_180_0 = 0x166,
	
	// 192 MHz
	ICL_CDCLK_FREQ_192_0 = 0x17E,
	
	// 307.2 MHz
	ICL_CDCLK_FREQ_307_2 = 0x264,
	
	// 312 MHz
	ICL_CDCLK_FREQ_312_0 = 0x26E,
	
	// 552 MHz
	ICL_CDCLK_FREQ_552_0 = 0x44E,
	
	// 556.8 MHz
	ICL_CDCLK_FREQ_556_8 = 0x458,
	
	// 648 MHz
	ICL_CDCLK_FREQ_648_0 = 0x50E,
	
	// 652.8 MHz
	ICL_CDCLK_FREQ_652_8 = 0x518
};

static constexpr uint32_t ICL_CDCLK_DEC_FREQ_THRESHOLD = ICL_CDCLK_FREQ_648_0;


static constexpr uint32_t ICL_CDCLK_PLL_FREQ_REF_24_0 = 24000000 * 54;

static constexpr uint32_t ICL_CDCLK_PLL_FREQ_REF_19_2 = 19200000 * 68;

static constexpr uint32_t ICL_CDCLK_PLL_FREQ_REF_38_4 = 38400000 * 34;

#define GEN11_GFX_MSTR_IRQ		(0x190010)

#define SOUTH_CHICKEN1		(0xc2000)
#define GEN8_MASTER_IRQ		(0x44200)
#define GEN11_DISPLAY_INT_CTL	(0x44200)

#define PCH_PORT_HOTPLUG		(0xc4030)
#define SHOTPLUG_CTL_DDI		(0xc4030)
#define SHOTPLUG_CTL_TC			(0xc4034)
#define SHPD_FILTER_CNT			(0xc4038)

#define SDEIMR (0xc4004)
#define SDEIIR (0xc4008)
#define SDEIER (0xc400c)

#define GEN11_DE_HPD_ISR		(0x44470)
#define GEN11_DE_HPD_IMR		(0x44474)
#define GEN11_DE_HPD_IIR		(0x44478)
#define GEN11_DE_HPD_IER		(0x4447c)

#define GEN8_DE_MISC_ISR (0x44460)
#define GEN8_DE_MISC_IMR  (0x44464)
#define GEN8_DE_MISC_IIR  (0x44468)
#define GEN8_DE_MISC_IER  (0x4446c)

#define GEN8_DE_PIPE_ISR_A  (0x44400)
#define GEN8_DE_PIPE_IMR_A  (0x44404)
#define GEN8_DE_PIPE_IIR_A  (0x44408)
#define GEN8_DE_PIPE_IER_A  (0x4440c)

#define GEN8_DE_PIPE_ISR_B  (0x44410)
#define GEN8_DE_PIPE_IMR_B  (0x44414)
#define GEN8_DE_PIPE_IIR_B  (0x44418)
#define GEN8_DE_PIPE_IER_B  (0x4441c)

#define GEN8_DE_PIPE_ISR_C  (0x44420)
#define GEN8_DE_PIPE_IMR_C  (0x44424)
#define GEN8_DE_PIPE_IIR_C  (0x44428)
#define GEN8_DE_PIPE_IER_C  (0x4442c)



constexpr uint32_t MCHBAR_MIRROR_BASE_SNB = 0x140000;
constexpr uint32_t GEN6_RP_STATE_CAP = MCHBAR_MIRROR_BASE_SNB + 0x5998;

constexpr uint32_t GEN9_FREQUENCY_SHIFT = 23;
constexpr uint32_t GEN9_FREQ_SCALER  = 3;

constexpr uint32_t FORCEWAKE_KERNEL_FALLBACK = 1 << 15;

constexpr uint32_t FORCEWAKE_ACK_TIMEOUT_MS = 50;

constexpr uint32_t FORCEWAKE_MEDIA_GEN9 = 0xa270;
constexpr uint32_t FORCEWAKE_RENDER_GEN9 = 0xa278;
constexpr uint32_t FORCEWAKE_BLITTER_GEN9 = 0xa188;

constexpr uint32_t FORCEWAKE_ACK_MEDIA_GEN9 = 0x0D88;
constexpr uint32_t FORCEWAKE_ACK_RENDER_GEN9 = 0x0D84;
constexpr uint32_t FORCEWAKE_ACK_BLITTER_GEN9 = 0x130044;

#define FORCEWAKE_MEDIA_VDBOX_GEN11(n)		(0xa540 + (n) * 4)
#define FORCEWAKE_MEDIA_VEBOX_GEN11(n)		(0xa560 + (n) * 4)
#define FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(n)	(0xd50 + (n) * 4)
#define FORCEWAKE_ACK_MEDIA_VEBOX_GEN11(n)	(0xd70 + (n) * 4)
#define FORCEWAKE_GT_GEN9			(0xa188)
#define FORCEWAKE_ACK_GT_GEN9			(0x130044)
#define FORCEWAKE_REQ_GSC			(0xa618)
#define FORCEWAKE_ACK_GSC			(0xdf8)



enum FORCEWAKE_DOM_BITS : unsigned {
	DOM_RENDER = 0b001,
	DOM_MEDIA = 0b010,
	DOM_BLITTER = 0b100,
	DOM_LAST = DOM_BLITTER,
	DOM_FIRST = DOM_RENDER
};
/*
enum forcewake_domain_id {
	FW_DOMAIN_ID_RENDER = 0,
	FW_DOMAIN_ID_BLITTER,
	FW_DOMAIN_ID_MEDIA,
	FW_DOMAIN_ID_MEDIA_VDBOX0,
	FW_DOMAIN_ID_MEDIA_VDBOX1,
	FW_DOMAIN_ID_MEDIA_VDBOX2,
	FW_DOMAIN_ID_MEDIA_VDBOX3,
	FW_DOMAIN_ID_MEDIA_VEBOX0,
	FW_DOMAIN_ID_MEDIA_VEBOX1,

	FW_DOMAIN_ID_COUNT
};*/

enum forcewake_domain_id {
	FW_DOMAIN_ID_RENDER = 0,
	FW_DOMAIN_ID_GT,        /* also includes blitter engine */
	FW_DOMAIN_ID_MEDIA,
	FW_DOMAIN_ID_MEDIA_VDBOX0,
	FW_DOMAIN_ID_MEDIA_VDBOX1,
	FW_DOMAIN_ID_MEDIA_VDBOX2,
	FW_DOMAIN_ID_MEDIA_VDBOX3,
	FW_DOMAIN_ID_MEDIA_VDBOX4,
	FW_DOMAIN_ID_MEDIA_VDBOX5,
	FW_DOMAIN_ID_MEDIA_VDBOX6,
	FW_DOMAIN_ID_MEDIA_VDBOX7,
	FW_DOMAIN_ID_MEDIA_VEBOX0,
	FW_DOMAIN_ID_MEDIA_VEBOX1,
	FW_DOMAIN_ID_MEDIA_VEBOX2,
	FW_DOMAIN_ID_MEDIA_VEBOX3,
	FW_DOMAIN_ID_GSC,

	FW_DOMAIN_ID_COUNT
};
#define BIT(n) (1<< n)
enum forcewake_domains {
	FORCEWAKE_RENDER	= BIT(FW_DOMAIN_ID_RENDER),
	FORCEWAKE_GT		= BIT(FW_DOMAIN_ID_GT),
	FORCEWAKE_MEDIA		= BIT(FW_DOMAIN_ID_MEDIA),
	FORCEWAKE_MEDIA_VDBOX0	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX0),
	FORCEWAKE_MEDIA_VDBOX1	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX1),
	FORCEWAKE_MEDIA_VDBOX2	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX2),
	FORCEWAKE_MEDIA_VDBOX3	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX3),
	FORCEWAKE_MEDIA_VDBOX4	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX4),
	FORCEWAKE_MEDIA_VDBOX5	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX5),
	FORCEWAKE_MEDIA_VDBOX6	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX6),
	FORCEWAKE_MEDIA_VDBOX7	= BIT(FW_DOMAIN_ID_MEDIA_VDBOX7),
	FORCEWAKE_MEDIA_VEBOX0	= BIT(FW_DOMAIN_ID_MEDIA_VEBOX0),
	FORCEWAKE_MEDIA_VEBOX1	= BIT(FW_DOMAIN_ID_MEDIA_VEBOX1),
	FORCEWAKE_MEDIA_VEBOX2	= BIT(FW_DOMAIN_ID_MEDIA_VEBOX2),
	FORCEWAKE_MEDIA_VEBOX3	= BIT(FW_DOMAIN_ID_MEDIA_VEBOX3),
	FORCEWAKE_GSC		= BIT(FW_DOMAIN_ID_GSC),

	FORCEWAKE_ALL = BIT(FW_DOMAIN_ID_COUNT) - 1,
};






enum intel_engine_id {
	RCS0 = 0,
	BCS0,
	BCS1,
	BCS2,
	BCS3,
	BCS4,
	BCS5,
	BCS6,
	BCS7,
	BCS8,
#define _BCS(n) (BCS0 + (n))
	VCS0,
	VCS1,
	VCS2,
	VCS3,
	VCS4,
	VCS5,
	VCS6,
	VCS7,
#define _VCS(n) (VCS0 + (n))
	VECS0,
	VECS1,
	VECS2,
	VECS3,
#define _VECS(n) (VECS0 + (n))
	CCS0,
	CCS1,
	CCS2,
	CCS3,
#define _CCS(n) (CCS0 + (n))
	GSC0,
	I915_NUM_ENGINES
#define INVALID_ENGINE ((enum intel_engine_id)-1)
};
#define __HAS_ENGINE(engine_mask, id) ((engine_mask) & BIT(id))

#define NEEDS_FORCE_WAKE(reg) ({ \
	u32 __reg = (reg); \
	__reg < 0x40000 || __reg >= 0x116000; \
})

#define GEN_FW_RANGE(s, e, d) \
	{ .start = (s), .end = (e), .domains = (d) }

#define GEN11_GACB_PERF_CTRL			(0x4b80)
#define   GEN11_HASH_CTRL_MASK			(0x3 << 12 | 0xf << 0)
#define   GEN11_HASH_CTRL_BIT0			(1 << 0)
#define   GEN11_HASH_CTRL_BIT4			(1 << 12)
#define GEN11_LSN_UNSLCVC			(0xb43c)
#define   GEN11_LSN_UNSLCVC_GAFS_HALF_CL2_MAXALLOC	(1 << 9)
#define   GEN11_LSN_UNSLCVC_GAFS_HALF_SF_MAXALLOC	(1 << 7)
#define GEN8_GAMW_ECO_DEV_RW_IA			(0x4080)
#define   GAMW_ECO_ENABLE_64K_IPS_FIELD		0xF
#define   GAMW_ECO_DEV_CTX_RELOAD_DISABLE	(1 << 7)
#define GAMT_CHKN_BIT_REG			(0x4ab8)
#define   GAMT_CHKN_DISABLE_L3_COH_PIPE		(1 << 31)
#define   GAMT_CHKN_DISABLE_DYNAMIC_CREDIT_SHARING	(1 << 28)
#define   GAMT_CHKN_DISABLE_I2M_CYCLE_ON_WR_PORT	(1 << 24)
#define UNSLICE_UNIT_LEVEL_CLKGATE2		(0x94e4)
#define   VSUNIT_CLKGATE_DIS_TGL		BIT(19)
#define   PSDUNIT_CLKGATE_DIS			BIT(5)
#define UNSLICE_UNIT_LEVEL_CLKGATE		(0x9434)
#define   VFUNIT_CLKGATE_DIS			BIT(20)
#define   CG3DDISCFEG_CLKGATE_DIS		BIT(17) /* DG2 */
#define   GAMEDIA_CLKGATE_DIS			BIT(11)
#define   HSUNIT_CLKGATE_DIS			BIT(8)
#define   VSUNIT_CLKGATE_DIS			BIT(3)
#define GEN10_DFR_RATIO_EN_AND_CHICKEN		(0x9550)
#define   DFR_DISABLE				(1 << 9)
#define GEN11_SUBSLICE_UNIT_LEVEL_CLKGATE	(0x9524)
#define   DSS_ROUTER_CLKGATE_DIS		BIT(28)
#define   GWUNIT_CLKGATE_DIS			BIT(16)

#define HSW_NDE_RSTWRN_OPT	(0x46408)
#define  MTL_RESET_PICA_HANDSHAKE_EN	BIT(6)
#define  RESET_PCH_HANDSHAKE_ENABLE	BIT(4)

#define _MASKED_FIELD(mask, value) ((mask) << 16 | (value))
#define _MASKED_BIT_ENABLE(a)	(_MASKED_FIELD((a), a))
#define _MASKED_BIT_DISABLE(a)	(_MASKED_FIELD((a), 0))
#define GEN9_GAMT_ECO_REG_RW_IA (0x4ab0)
#define   GAMT_ECO_ENABLE_IN_PLACE_DECOMPRESS	(1 << 18)


struct intel_forcewake_range {
	uint32_t start;
	uint32_t end;

	enum forcewake_domains domains;
};

const struct intel_forcewake_range __gen11_fw_ranges[] = {
	//GEN_FW_RANGE(0x0, 0x1fff, 0), /* uncore range */
	GEN_FW_RANGE(0x2000, 0x26ff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x2700, 0x2fff, FORCEWAKE_GT),
	GEN_FW_RANGE(0x3000, 0x3fff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x4000, 0x51ff, FORCEWAKE_GT),
	GEN_FW_RANGE(0x5200, 0x7fff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x8000, 0x813f, FORCEWAKE_GT),
	GEN_FW_RANGE(0x8140, 0x815f, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x8160, 0x82ff, FORCEWAKE_GT),
	GEN_FW_RANGE(0x8300, 0x84ff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x8500, 0x87ff, FORCEWAKE_GT),
	//GEN_FW_RANGE(0x8800, 0x8bff, 0),
	GEN_FW_RANGE(0x8c00, 0x8cff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x8d00, 0x94cf, FORCEWAKE_GT),
	GEN_FW_RANGE(0x94d0, 0x955f, FORCEWAKE_RENDER),
	//GEN_FW_RANGE(0x9560, 0x95ff, 0),
	GEN_FW_RANGE(0x9600, 0xafff, FORCEWAKE_GT),
	GEN_FW_RANGE(0xb000, 0xb47f, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0xb480, 0xdeff, FORCEWAKE_GT),
	GEN_FW_RANGE(0xdf00, 0xe8ff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0xe900, 0x16dff, FORCEWAKE_GT),
	GEN_FW_RANGE(0x16e00, 0x19fff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x1a000, 0x23fff, FORCEWAKE_GT),
	//GEN_FW_RANGE(0x24000, 0x2407f, 0),
	GEN_FW_RANGE(0x24080, 0x2417f, FORCEWAKE_GT),
	GEN_FW_RANGE(0x24180, 0x242ff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x24300, 0x243ff, FORCEWAKE_GT),
	GEN_FW_RANGE(0x24400, 0x24fff, FORCEWAKE_RENDER),
	GEN_FW_RANGE(0x25000, 0x3ffff, FORCEWAKE_GT),
	//GEN_FW_RANGE(0x40000, 0x1bffff, 0),
	GEN_FW_RANGE(0x1c0000, 0x1c3fff, FORCEWAKE_MEDIA_VDBOX0),
	//GEN_FW_RANGE(0x1c4000, 0x1c7fff, 0),
	GEN_FW_RANGE(0x1c8000, 0x1cffff, FORCEWAKE_MEDIA_VEBOX0),
	GEN_FW_RANGE(0x1d0000, 0x1d3fff, FORCEWAKE_MEDIA_VDBOX2),
	//GEN_FW_RANGE(0x1d4000, 0x1dbfff, 0)
};
#define  HSW_EDRAM_CAP				(0x120010)
#define    EDRAM_NUM_BANKS(cap)			(((cap) >> 1) & 0xf)
#define    EDRAM_WAYS_IDX(cap)			(((cap) >> 5) & 0x7)
#define    EDRAM_SETS_IDX(cap)			(((cap) >> 8) & 0x3)
#define   FORCEWAKE_KERNEL			BIT(0)


#define GEN11_GT_INTR_DW0		(0x190018)
#define  GEN11_CSME			(31)
#define  GEN11_GUNIT			(28)
#define  GEN11_GUC			(25)
#define  GEN11_WDPERF			(20)
#define  GEN11_KCR			(19)
#define  GEN11_GTPM			(16)
#define  GEN11_BCS			(15)
#define  GEN11_RCS0			(0)

#define GEN11_GT_INTR_DW1		(0x19001c)
#define  GEN11_VECS(x)			(31 - (x))
#define  GEN11_VCS(x)			(x)

#define GEN11_GT_INTR_DW(x)		(0x190018 + ((x) * 4))

#define GEN11_INTR_IDENTITY_REG0	(0x190060)
#define GEN11_INTR_IDENTITY_REG1	(0x190064)
#define  GEN11_INTR_DATA_VALID		(1 << 31)
//#define  GEN11_INTR_ENGINE_CLASS(x)	(((x) & GENMASK(18, 16)) >> 16)
//#define  GEN11_INTR_ENGINE_INSTANCE(x)	(((x) & GENMASK(25, 20)) >> 20)
#define  GEN11_INTR_ENGINE_INTR(x)	((x) & 0xffff)
/* irq instances for OTHER_CLASS */
#define OTHER_GUC_INSTANCE	0
#define OTHER_GTPM_INSTANCE	1

#define GEN11_INTR_IDENTITY_REG(x)	(0x190060 + ((x) * 4))

#define GEN11_IIR_REG0_SELECTOR		(0x190070)
#define GEN11_IIR_REG1_SELECTOR		(0x190074)

#define GEN11_IIR_REG_SELECTOR(x)	(0x190070 + ((x) * 4))

#define GEN11_RENDER_COPY_INTR_ENABLE	(0x190030)
#define GEN11_VCS_VECS_INTR_ENABLE	(0x190034)
#define GEN11_GUC_SG_INTR_ENABLE	(0x190038)
#define GEN11_GPM_WGBOXPERF_INTR_ENABLE	(0x19003c)
#define GEN11_CRYPTO_RSVD_INTR_ENABLE	(0x190040)
#define GEN11_GUNIT_CSME_INTR_ENABLE	(0x190044)

#define GEN11_RCS0_RSVD_INTR_MASK	(0x190090)
#define GEN11_BCS_RSVD_INTR_MASK	(0x1900a0)
#define GEN11_VCS0_VCS1_INTR_MASK	(0x1900a8)
#define GEN11_VCS2_VCS3_INTR_MASK	(0x1900ac)
#define GEN12_VCS4_VCS5_INTR_MASK	(0x1900b0)
#define GEN12_VCS6_VCS7_INTR_MASK	(0x1900b4)
#define GEN11_VECS0_VECS1_INTR_MASK	(0x1900d0)
#define GEN12_VECS2_VECS3_INTR_MASK	(0x1900d4)
#define GEN11_GUC_SG_INTR_MASK		(0x1900e8)
#define GEN11_GPM_WGBOXPERF_INTR_MASK	(0x1900ec)
#define GEN11_CRYPTO_RSVD_INTR_MASK	(0x1900f0)
#define GEN11_GUNIT_CSME_INTR_MASK	(0x1900f4)

#define GT_BLT_FLUSHDW_NOTIFY_INTERRUPT		(1 << 26)
#define GT_BLT_CS_ERROR_INTERRUPT		(1 << 25)
#define GT_BLT_USER_INTERRUPT			(1 << 22)
#define GT_BSD_CS_ERROR_INTERRUPT		(1 << 15)
#define GT_BSD_USER_INTERRUPT			(1 << 12)
#define GT_RENDER_L3_PARITY_ERROR_INTERRUPT_S1	(1 << 11) /* hsw+; rsvd on snb, ivb, vlv */
#define GT_CONTEXT_SWITCH_INTERRUPT		(1 <<  8)
#define GT_RENDER_L3_PARITY_ERROR_INTERRUPT	(1 <<  5) /* !snb */
#define GT_RENDER_PIPECTL_NOTIFY_INTERRUPT	(1 <<  4)
#define GT_RENDER_CS_MASTER_ERROR_INTERRUPT	(1 <<  3)
#define GT_RENDER_SYNC_STATUS_INTERRUPT		(1 <<  2)
#define GT_RENDER_DEBUG_INTERRUPT		(1 <<  1)
#define GT_RENDER_USER_INTERRUPT		(1 <<  0)

#define GEN11_GFX_MSTR_IRQ		(0x190010)
#define  GEN11_MASTER_IRQ		(1 << 31)
#define  GEN11_PCU_IRQ			(1 << 30)
#define  GEN11_GU_MISC_IRQ		(1 << 29)
#define  GEN11_DISPLAY_IRQ		(1 << 16)
#define  GEN11_GT_DW_IRQ(x)		(1 << (x))
#define  GEN11_GT_DW1_IRQ		(1 << 1)
#define  GEN11_GT_DW0_IRQ		(1 << 0)

#define GEN11_DISPLAY_INT_CTL		(0x44200)
#define  GEN11_DISPLAY_IRQ_ENABLE	(1 << 31)
#define  GEN11_AUDIO_CODEC_IRQ		(1 << 24)
#define  GEN11_DE_PCH_IRQ		(1 << 23)
#define  GEN11_DE_MISC_IRQ		(1 << 22)
#define  GEN11_DE_HPD_IRQ		(1 << 21)
#define  GEN11_DE_PORT_IRQ		(1 << 20)
#define  GEN11_DE_PIPE_C		(1 << 18)
#define  GEN11_DE_PIPE_B		(1 << 17)
#define  GEN11_DE_PIPE_A		(1 << 16)

#define GEN11_GT_INTR_DW0		(0x190018)
#define  GEN11_CSME			(31)
#define  GEN11_GUNIT			(28)
#define  GEN11_GUC			(25)
#define  GEN11_WDPERF			(20)
#define  GEN11_KCR			(19)
#define  GEN11_GTPM			(16)
#define  GEN11_BCS			(15)
#define  GEN11_RCS0			(0)

#define GEN11_GT_INTR_DW1		(0x19001c)
#define  GEN11_VECS(x)			(31 - (x))
#define  GEN11_VCS(x)			(x)
#define GT_CS_MASTER_ERROR_INTERRUPT		(3)
#define GT_WAIT_SEMAPHORE_INTERRUPT		(11)

enum ack_type {
	ACK_CLEAR = 0,
	ACK_SET
};

struct intel_rps_freq_caps {
	uint8_t rp0_freq;
	uint8_t rp1_freq;
	uint8_t min_freq;
};

constexpr uint32_t regForDom(unsigned d) {
	
	
	if (d == FORCEWAKE_GT)
		return FORCEWAKE_GT_GEN9;
	if (d == FORCEWAKE_RENDER)
		return FORCEWAKE_RENDER_GEN9;
	
		
	if (d == FORCEWAKE_MEDIA_VDBOX0)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(0);
	if (d == FORCEWAKE_MEDIA_VDBOX1)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(1);
	if (d == FORCEWAKE_MEDIA_VDBOX2)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(2);
	if (d == FORCEWAKE_MEDIA_VDBOX3)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(3);
	if (d == FORCEWAKE_MEDIA_VDBOX4)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(4);
	if (d == FORCEWAKE_MEDIA_VDBOX5)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(5);
	if (d == FORCEWAKE_MEDIA_VDBOX6)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(6);
	if (d == FORCEWAKE_MEDIA_VDBOX7)
			return FORCEWAKE_MEDIA_VDBOX_GEN11(7);


	if (d == FORCEWAKE_MEDIA_VEBOX0)
			return FORCEWAKE_MEDIA_VEBOX_GEN11(0);
	if (d == FORCEWAKE_MEDIA_VEBOX1)
			return FORCEWAKE_MEDIA_VEBOX_GEN11(1);
	if (d == FORCEWAKE_MEDIA_VEBOX2)
			return FORCEWAKE_MEDIA_VEBOX_GEN11(2);
	if (d == FORCEWAKE_MEDIA_VEBOX3)
			return FORCEWAKE_MEDIA_VEBOX_GEN11(3);
	
	if (d == FORCEWAKE_GSC)
		return FORCEWAKE_REQ_GSC;

	assertf(false, "Unknown force wake domain %d", d);
	return 0;
}

constexpr uint32_t ackForDom(unsigned d) {
	if (d == FORCEWAKE_GT)
		return FORCEWAKE_ACK_GT_GEN9;
	if (d == FORCEWAKE_RENDER)
		return FORCEWAKE_ACK_RENDER_GEN9;
	
	if (d == FORCEWAKE_MEDIA_VDBOX0)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(0);
	if (d == FORCEWAKE_MEDIA_VDBOX1)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(1);
	if (d == FORCEWAKE_MEDIA_VDBOX2)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(2);
	if (d == FORCEWAKE_MEDIA_VDBOX3)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(3);
	if (d == FORCEWAKE_MEDIA_VDBOX4)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(4);
	if (d == FORCEWAKE_MEDIA_VDBOX5)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(5);
	if (d == FORCEWAKE_MEDIA_VDBOX6)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(6);
	if (d == FORCEWAKE_MEDIA_VDBOX7)
			return FORCEWAKE_ACK_MEDIA_VDBOX_GEN11(7);


	if (d == FORCEWAKE_MEDIA_VEBOX0)
			return FORCEWAKE_ACK_MEDIA_VEBOX_GEN11(0);
	if (d == FORCEWAKE_MEDIA_VEBOX1)
			return FORCEWAKE_ACK_MEDIA_VEBOX_GEN11(1);
	if (d == FORCEWAKE_MEDIA_VEBOX2)
			return FORCEWAKE_ACK_MEDIA_VEBOX_GEN11(2);
	if (d == FORCEWAKE_MEDIA_VEBOX3)
			return FORCEWAKE_ACK_MEDIA_VEBOX_GEN11(3);
	
	if (d == FORCEWAKE_GSC)
		return FORCEWAKE_ACK_GSC;
	
	assertf(false, "Unknown force wake domain %d", d);
	return 0;
}

constexpr const char* const strForDom(unsigned d) {
	if (d == DOM_RENDER)
		return "Render";
	if (d == DOM_MEDIA)
		return "Media";
	if (d == DOM_BLITTER)
		return "Blitter";
	return "(unk)";
}

constexpr uint32_t masked_field(uint32_t mask, uint32_t value) {
	return (mask << 16) | value;
}

constexpr uint32_t fw_set(uint32_t v) {
	return masked_field(v, v);
}

constexpr uint32_t fw_clear(uint32_t v) {
	return masked_field(v, 0);
}


enum ConnectorType : uint32_t {
	ConnectorZero       = 0x0,
	ConnectorDummy      = 0x1,   /* Always used as dummy, seems to sometimes work as VGA */
	ConnectorLVDS       = 0x2,   /* Just like on AMD LVDS is used for eDP */
	ConnectorDigitalDVI = 0x4,   /* This is not eDP despite a common misbelief */
	ConnectorSVID       = 0x8,
	ConnectorVGA        = 0x10,
	ConnectorDP         = 0x400,
	ConnectorHDMI       = 0x800,
	ConnectorAnalogDVI  = 0x2000
};

/* I can see very few mentioned in the code (0x1, 0x8, 0x40), though connectors themselves define way more! */

union ConnectorFlags {
	struct ConnectorFlagBits {
		/* Bits 1, 2, 8 are mentioned in AppleIntelFramebufferController::GetGPUCapability */
		/* Lets apperture memory to be not required AppleIntelFramebuffer::isApertureMemoryRequired */
		uint8_t CNAlterAppertureRequirements :1;  /* 0x1 */
		uint8_t CNUnknownFlag_2              :1;  /* 0x2 */
		uint8_t CNUnknownFlag_4              :1;  /* 0x4 */
		/* Normally set for LVDS displays (i.e. built-in displays) */
		uint8_t CNConnectorAlwaysConnected   :1;  /* 0x8 */
		/* AppleIntelFramebuffer::maxSupportedDepths checks this and returns 2 IODisplayModeInformation::maxDepthIndex ?? */
		uint8_t CNUnknownFlag_10             :1;  /* 0x10 */
		uint8_t CNUnknownFlag_20             :1;  /* 0x20 */
		/* Disable blit translation table? AppleIntelFramebufferController::ConfigureBufferTranslation */
		uint8_t CNDisableBlitTranslationTable:1;  /* 0x40 */
		/* Used in AppleIntelFramebufferController::setPowerWellState */
		/* Activates MISC IO power well (SKL_DISP_PW_MISC_IO) */
		uint8_t CNUseMiscIoPowerWell         :1;  /* 0x80 */
		/* Used in AppleIntelFramebufferController::setPowerWellState */
		/* Activates Power Well 2 usage (SKL_PW_CTL_IDX_PW_2) */
		/* May help with HDMI audio configuration issues */
		/* REF: https://github.com/acidanthera/bugtracker/issues/1189 */
		uint8_t CNUsePowerWell2              :1;  /* 0x100 */
		uint8_t CNUnknownFlag_200            :1;  /* 0x200 */
		uint8_t CNUnknownFlag_400            :1;  /* 0x400 */
		/* Sets fAvailableLaneCount to 30 instead of 20 when specified */
		uint8_t CNIncreaseLaneCount          :1;  /* 0x800 */
		uint8_t CNUnknownFlag_1000           :1;  /* 0x1000 */
		uint8_t CNUnknownFlag_2000           :1;  /* 0x2000 */
		uint8_t CNUnknownFlag_4000           :1;  /* 0x4000 */
		uint8_t CNUnknownFlag_8000           :1;  /* 0x8000 */
		uint16_t CNUnknownZeroFlags;
	} bits;
	uint32_t value;
};

struct PACKED ConnectorInfo {
	/* Watch out, this is really messy (see AppleIntelFramebufferController::MapFBToPort).
	 * I am not fully sure why this exists, and recommend setting index to array index (i.e. the sequential number from 0).
	 *
	 * The only accepted values are 0, 1, 2, 3, and -1 (0xFF). When index is equal to array index the logic is simple:
	 * Port with index    0    is always considered built-in (of LVDS type) regardless of any other values.
	 * Ports with indexes 1~3  are checked against type, HDMI will allow the use of digital audio, otherwise DP is assumed.
	 * Port with index    0xFF is ignored and skipped.
	 *
	 * When index != array index port type will be read from connector[index].type.
	 * Say, we have 2 active ports:
	 * 0 - [1]     busId 4 type LVDS
	 * 1 - [2]     busId 5 type DP
	 * 2 - [3]     busId 6 type HDMI
	 * 3 - [-1]    busId 0 type Dummy
	 * This will result in 2 framebuffers which types will be shifted:
	 * 0 - busId 4 type DP
	 * 1 - busId 5 type HDMI
	 * In fact BusId values are also read as connector[index].busId, but are later mapped back via
	 * AppleIntelFramebufferController::getGMBusIDfromPort by looking up a connector with the specified index.
	 * The lookup will stop as soon as a special marker connector (-1) is found. To illustrate, if we have 2 active ports:
	 * 0 - [1]     busId 4 type LVDS
	 * 1 - [2]     busId 5 type DP
	 * 2 - [-1]    busId 6 type HDMI
	 * 3 - [-1]    busId 0 type Dummy
	 * The result will be 2 framebuffers which types and the second busId will be shifted:
	 * 0 - busId 4 type DP
	 * 1 - busId 6 type HDMI
	 * It is also used for port-number calculation.
	 * - LVDS displays (more precisely, displays with CNConnectorAlwaysConnected flag set) get port-number 0.
	 * - Other displays go through index - port-number mapping: 1 - 5, 2 - 6, 3 - 7, or fallback to 0.
	 */
	int8_t index;
	/* Proven by AppleIntelFramebufferController::MapFBToPort, by a call to AppleIntelFramebufferController::getGMBusIDfromPort.
	 * This is GMBUS (Graphic Management Bus) ID described in https://01.org/sites/default/files/documentation/intel-gfx-prm-osrc-hsw-display_0.pdf.
	 * The use could be found in Intel Linux Graphics Driver source code:
	 * https://github.com/torvalds/linux/blob/6481d5ed076e69db83ca75e751ad492a6fb669a7/drivers/gpu/drm/i915/intel_i2c.c#L43
	 * https://github.com/torvalds/linux/blob/605dc7761d2701f73c17183649de0e3044609817/drivers/gpu/drm/i915/i915_reg.h#L3053
	 * However, it should be noted that Apple identifiers are slightly different from Linux driver.
	 * In Linux 0 means disabled, however, for Apple it has some special meaning and is used for internal display.
	 * Other than that the values are the same:
	 * - GMBUS_PIN_DPC    (4)  HDMIC
	 * - GMBUS_PIN_DPB    (5)  SDVO, HDMIB
	 * - GMBUS_PIN_DPD    (6)  HDMID
	 * - GMBUS_PIN_VGADDC (2)  VGA until Broadwell inclusive.
	 * So basically you could use 4, 5, 6 for arbitrary HDMI or DisplayPort displays.
	 * Since 5 supports SDVO (https://en.wikipedia.org/wiki/Serial_Digital_Video_Out), it may also be used to support DVI displays.
	 * Starting with Skylake VGA works via SDVO too (instead of a dedicated GMBUS_PIN_VGADDC id).
	 */
	uint8_t busId;
	/* Appears to be used for grouping ports just like Piker says, but I cannot find the usage. */
	uint8_t pipe;
	uint8_t pad;
	ConnectorType type;
	/* These are connector flags, they have nothing to do with delays regardless of what Piker says.
	 * I tried to describe some in ConnectorFlags.
	 */
	ConnectorFlags flags;
};

struct PACKED ConnectorInfoICL {
	uint32_t index;
	uint32_t busId;
	uint32_t pipe;
	uint32_t pad;
	ConnectorType type;
	ConnectorFlags flags;
};
struct PACKED FramebufferCNLCurrents {
	uint32_t value1;
	uint32_t pad;
	uint64_t valu2;
};
struct PACKED FramebufferICLLP {
	uint32_t framebufferId;
	/* Unclear what values really are, yet 4 stands for non-LP chipset.
	 * See AppleIntelFramebufferController::start.
	 */
	uint32_t fPchType;
	uint64_t fModelNameAddr;
	/* While it is hard to be sure, because having 0 here results in online=true returned by
	 * AppleIntelFramebuffer::GetOnlineInfo, after all it appears to be the case, and the unused
	 * so-called mobile framebufers are simply set to fail-safe defaults.
	 * For some reason it is often called fDisabled...
	 */
	uint8_t  fMobile;
	uint8_t  fPipeCount;
	uint8_t  fPortCount;
	uint8_t  fFBMemoryCount;
	/* This one is per framebuffer fStolenMemorySize * fFBMemoryCount */
	uint32_t fStolenMemorySize;
	/* This is for boot framebuffer from what I can understand */
	uint32_t fFramebufferMemorySize;
	uint32_t fUnifiedMemorySize;
	ConnectorInfoICL connectors[6];
	/* Flags are quite different in ICL now */
	union { uint32_t value; } flags;
	uint32_t unk2;
	FramebufferCNLCurrents currents[3];
	uint32_t unk3[2];
	uint32_t camelliaVersion;
	uint32_t unk4[3];
	uint32_t fNumTransactionsThreshold;
	/* Defaults to 14, used when UseVideoTurbo bit is set */
	uint32_t fVideoTurboFreq;
	uint32_t fSliceCount;
	uint32_t fEuCount;
	uint32_t unk5;
	uint8_t unk6;
	uint8_t pad[3];
};

struct PACKED FramebufferICL {

	uint32_t framebufferId;

	uint32_t fPchType;
	uint64_t fModelNameAddr;

	uint8_t  fMobile;
	uint8_t  fPipeCount;
	uint8_t  fPortCount;
	uint8_t  fFBMemoryCount;

	uint32_t fStolenMemorySize;
	uint32_t fFramebufferMemorySize;
	uint32_t fUnifiedMemorySize;
	
	ConnectorInfoICL connectors[3];//144 bytes
	uint64_t flags;
	uint64_t empty0;
	
	uint64_t combo1;
	uint64_t empty1;
	uint64_t combo2;
	uint64_t empty2;
	uint64_t combo3;
	uint64_t field1;
	uint32_t camelliaVersion;
	uint32_t fNumTransactionsThreshold;
	uint32_t fVideoTurboFreq;
	uint32_t empty3;
	uint32_t empty4;
	uint32_t empty5;
	uint32_t slice;
	uint32_t eu;
	uint32_t subslice;
	uint32_t empty6;
	
};

enum intel_pch {
	PCH_NOP = -1,	/* PCH without south display */
	PCH_NONE = 0,	/* No PCH present */
	PCH_IBX,	/* Ibexpeak PCH */
	PCH_CPT,	/* Cougarpoint/Pantherpoint PCH */
	PCH_LPT,	/* Lynxpoint/Wildcatpoint PCH */
	PCH_SPT,        /* Sunrisepoint/Kaby Lake PCH */
	PCH_CNP,        /* Cannon/Comet Lake PCH */
	PCH_ICP,	/* Ice Lake/Jasper Lake PCH */
	PCH_TGP,	/* Tiger Lake/Mule Creek Canyon PCH */
	PCH_ADP,	/* Alder Lake PCH */

	/* Fake PCHs, functionality handled on the same PCI dev */
	PCH_DG1 = 1024,
	PCH_DG2,
	PCH_MTL,
	PCH_LNL,
};



class Gen11 {
	//friend class Gen9_5;

private:
	
	
	static UInt8 wrapLoadGuCBinary(void *that);
	mach_vm_address_t orgLoadGuCBinary {};
	
	static bool wrapLoadFirmware(void *that);
	static bool wrapInitSchedControl(void *that);
	static void *wrapIgBufferWithOptions(void *accelTask, void* size, unsigned int type, unsigned int flags);
	static UInt64 wrapIgBufferGetGpuVirtualAddress(void *that);
	
	static int wrapPmNotifyWrapper(unsigned int a0, unsigned int a1, unsigned long long *a2, unsigned int *freq);
	mach_vm_address_t orgPmNotifyWrapper {};
	
	static bool start(void *that,void  *param_1);
	mach_vm_address_t ostart {};
	
	static bool patchRCSCheck(mach_vm_address_t& start);
	static void forceWake(void *that, bool set, uint32_t dom, uint8_t ctx);
	mach_vm_address_t oforceWake {};
	static bool pollRegister(uint32_t reg, uint32_t val, uint32_t mask, uint32_t timeout);
	static bool forceWakeWaitAckFallback(uint32_t d, uint32_t val, uint32_t mask);
	
	static void * serviceInterrupts(void *param_1);
	mach_vm_address_t oserviceInterrupts {};
	
	static void releaseDoorbell();
	
	void *framecont;
	
	mach_vm_address_t orgSubmitExecList {};
	mach_vm_address_t orgLoadFirmware {};
	mach_vm_address_t orgInitSchedControl {};
	mach_vm_address_t orgIgBufferWithOptions {};
	mach_vm_address_t orgIgBufferGetGpuVirtualAddress {};
	
	mach_vm_address_t GuC {};
	
	uint32_t freq_max {0};
	uint8_t *gKmGen9GuCBinary {nullptr};
	uint8_t *signaturePointer {nullptr};
	uint32_t *firmwareSizePointer {nullptr};
	uint8_t *dummyFirmwareBuffer {nullptr};
	uint8_t *realFirmwareBuffer {nullptr};
	uint32_t realBinarySize {};
	
	bool performingFirmwareLoad {false};
	
	static void wrapSystemWillSleep(void *that);
	static void wrapSystemDidWake(void *that);
	
	
	static bool  getGPUInfo(void *that);
	mach_vm_address_t ogetGPUInfo {};
	
	
	static IOReturn wrapFBClientDoAttribute(void *fbclient, uint32_t attribute, unsigned long *unk1, unsigned long unk2, unsigned long *unk3, unsigned long *unk4, void *externalMethodArguments);
	mach_vm_address_t orgFBClientDoAttribute {};
	
	static IOReturn wrapPavpSessionCallback(void *intelAccelerator, int32_t sessionCommand, uint32_t sessionAppId, uint32_t *a4, bool flag);
	mach_vm_address_t orgPavpSessionCallback {};
	
	const uint8_t *_CSR_PATCH_B0plus;
	const uint8_t *_CSR_PATCH_AX;
	
	static void wrapWriteRegister32(void *controller, uint32_t address, uint32_t value);
	mach_vm_address_t owrapWriteRegister32 {};
	
	static uint32_t wrapReadRegister32(void *controller, uint32_t address);
	mach_vm_address_t owrapReadRegister32 {};
	
	static void sanitizeCDClockFrequency(void *that);
	static uint32_t wrapProbeCDClockFrequency(void *that);
	
	static void  readAndClearInterrupts(void *that,void *param_1);
	mach_vm_address_t oreadAndClearInterrupts {};
	
	uint32_t (*orgProbeCDClockFrequency)(void *) {nullptr};
	void (*orgDisableCDClock)(void *) {nullptr};
	void (*orgSetCDClockFrequency)(void *, unsigned long long) {nullptr};
	
	static void * wprobe(void *that,void *param_1,int *param_2);
	mach_vm_address_t owprobe {};
	
	static bool  tgstart(void *that,void *param_1);
	mach_vm_address_t otgstart {};
	

	static uint32_t probePortMode();
	mach_vm_address_t oprobePortMode {};
	
	static uint32_t wdepthFromAttribute(void *that,uint param_1);
	
	static uint32_t raReadRegister32(void *that,unsigned long param_1);
	mach_vm_address_t oraReadRegister32 {};
	
	static unsigned long raReadRegister32b(void *that,void *param_1,unsigned long param_2);
	
	static uint64_t raReadRegister64(void *that,unsigned long param_1);
	mach_vm_address_t oraReadRegister64 {};
	
	static uint64_t raReadRegister64b(void *that,void *param_1,unsigned long param_2);
	
	static void radWriteRegister32(void *that,unsigned long param_1, unsigned int param_2);
	mach_vm_address_t oradWriteRegister32 {};
	static void radWriteRegister32f(void *that,unsigned long param_1, unsigned int param_2);
	mach_vm_address_t oradWriteRegister32f {};
	
	static void raWriteRegister32(void *that,unsigned long param_1, unsigned int param_2);
	mach_vm_address_t oraWriteRegister32 {};
	
	static void raWriteRegister32b(void *that,void *param_1,unsigned long param_2, unsigned int param_3);
	
	static void raWriteRegister32f(void *that,unsigned long param_1, unsigned int param_2);
	mach_vm_address_t oraWriteRegister32f {};
	
	static void raWriteRegister64(void *that,unsigned long param_1,unsigned long long param_2);
	mach_vm_address_t oraWriteRegister64 {};
	
	static void raWriteRegister64b(void *that,void *param_1,unsigned long param_2,unsigned long long param_3);
	
	static unsigned long getHPDState(void *that);
	
	static void setupPlanarSurfaceDBUF();
	mach_vm_address_t osetupPlanarSurfaceDBUF {};
	
	static void updateDBUF(void *that,uint param_1,uint param_2,bool param_3);
	
	static void FBMemMgr_Init();
	mach_vm_address_t oFBMemMgr_Init {};
	
	static int 	probeBootPipe(void *that,bool *param_1,uint32_t *param_2);
	mach_vm_address_t oprobeBootPipe {};
	
public:

	void init();
	static Gen11 *callback;
	bool processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
	
	static void tWriteRegister32(unsigned long a, unsigned int b);
	static void tWriteRegister64(void volatile* a, unsigned long b, unsigned long long c);
	static unsigned int tReadRegister32(unsigned long a);
	static unsigned long long tReadRegister64(void volatile* a, unsigned long b);
	static uint64_t tgetPMTNow();
	static bool thwSetupDSBMemory();
	static uint32_t tprobePortMode();
	
	
};

#endif /* kern_gen8_hpp */
