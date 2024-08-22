//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#pragma once
#include "kern_netdbg.hpp"
#include <Headers/kern_patcher.hpp>
#include <Headers/kern_iokit.hpp>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/graphics/IOFramebuffer.h>
#include <IOKit/acpi/IOACPIPlatformExpert.h>

//! Hack
class AppleACPIPlatformExpert : IOACPIPlatformExpert {
	friend class NBlue;
};

struct intel_ip_version {
	UInt8 ver;
	UInt8 rel;
	UInt8 step;
};

enum struct iGFXGen {
    Haswell,    // Haswell had no lower end rebrands, so it gets it's name plastered here
    Gen8,       // Broadwell & Braswell/Cherry Trail/Cherry View
    Gen9,       // Skylake & Apollo Lake/Broxton
    Gen9_5,     // Coffee Lake, Comet Lake, Gemini Lake, Kaby Lake
    Gen11,      // Ice Lake kexts support G1 with device-id spoofs, this, however, is inconvenient, also Elkhart/Jasper
                // Lake exist, pain.
    Gen12_1,    // Tiger Lake, these placeholders, don't get your hopes up
    Gen12_2,    // Alder Lake & Raptor Lake
    Unknown,
};

constexpr UInt32 mmPCIE_INDEX2 = 0xE;
constexpr UInt32 mmPCIE_DATA2 = 0xF;
/*
class EXPORT PRODUCT_NAME : public IOService {
	OSDeclareDefaultStructors(PRODUCT_NAME);

	public:
	IOService *probe(IOService *provider, SInt32 *score) override;
	bool start(IOService *provider) override;
};*/

class NBlue {
    friend class Gen8;
    friend class Gen9;
    friend class Gen9_5;
    friend class Gen11;
    friend class HSW;
	friend class DYLDPatches;

    public:
    static NBlue *callback;
    void init();
    void processPatcher(KernelPatcher &patcher);
    bool processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
	void setRMMIOIfNecessary();
	
	static uint16_t configRead16(IORegistryEntry *service, uint32_t space, uint8_t offset);
	static uint32_t configRead32(IORegistryEntry *service, uint32_t space, uint8_t offset);
	WIOKit::t_PCIConfigRead16 orgConfigRead16 {nullptr};
	WIOKit::t_PCIConfigRead32 orgConfigRead32 {nullptr};
	
	static size_t wrapFunctionReturnZero();
	
	mach_vm_address_t orgApplePanelSetDisplay {0};
	static bool wrapApplePanelSetDisplay(IOService *that, IODisplay *display);
	
	UInt32 stolen_size;
	uint32_t framebufferId {0};
	
    private:
	
	UInt32 readReg32(UInt32 reg) {
		if (reg * 4 < this->rmmio->getLength()) {
			return this->rmmioPtr[reg];
		} else {
			this->rmmioPtr[mmPCIE_INDEX2] = reg;
			return this->rmmioPtr[mmPCIE_DATA2];
		}
	}

	void writeReg32(UInt32 reg, UInt32 val) {
		if ((reg * 4) < this->rmmio->getLength()) {
			this->rmmioPtr[reg] = val;
		} else {
			this->rmmioPtr[mmPCIE_INDEX2] = reg;
			this->rmmioPtr[mmPCIE_DATA2] = val;
		}
	}
	
    bool isCflDerivative = false;
    bool isJslDerivative = false;
    bool isGen9LPDerivative = false;
    bool isGen8LPDerivative = false;
    iGFXGen igfxGen = iGFXGen::Unknown;
    uint32_t deviceId {0};
    uint16_t revision {0};
    uint32_t pciRevision {0};
    IOPCIDevice *iGPU {nullptr};
	
	IOMemoryMap *rmmio {nullptr};
	volatile UInt32 *rmmioPtr {nullptr};

    
	
};

//! Change frame-buffer count >= 2 check to >= 1.
static const UInt8 kAGDPFBCountCheckOriginal[] = {0x02, 0x00, 0x00, 0x83, 0xF8, 0x02};
static const UInt8 kAGDPFBCountCheckPatched[] = {0x02, 0x00, 0x00, 0x83, 0xF8, 0x01};

//! Ditto
static const UInt8 kAGDPFBCountCheckVenturaOriginal[] = {0x41, 0x83, 0xBE, 0x14, 0x02, 0x00, 0x00, 0x02};
static const UInt8 kAGDPFBCountCheckVenturaPatched[] = {0x41, 0x83, 0xBE, 0x14, 0x02, 0x00, 0x00, 0x01};

//! Neutralise access to AGDP configuration by board identifier.
static const UInt8 kAGDPBoardIDKeyOriginal[] = "board-id";
static const UInt8 kAGDPBoardIDKeyPatched[] = "applehax";


