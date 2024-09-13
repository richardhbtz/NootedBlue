//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#include "kern_nblue.hpp"
#include "kern_gen11.hpp"
#include "kern_model.hpp"
#include "DYLDPatches.hpp"
#include "HDMI.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_api.hpp>
#include <Headers/kern_devinfo.hpp>

static const char *pathAGDP = "/System/Library/Extensions/AppleGraphicsControl.kext/Contents/PlugIns/"
							  "AppleGraphicsDevicePolicy.kext/Contents/MacOS/AppleGraphicsDevicePolicy";
static const char *pathBacklight = "/System/Library/Extensions/AppleBacklight.kext/Contents/MacOS/AppleBacklight";
static const char *pathMCCSControl = "/System/Library/Extensions/AppleMCCSControl.kext/Contents/MacOS/AppleMCCSControl";
static const char *pathIOGraphics= "/System/Library/Extensions/IOGraphicsFamily.kext/IOGraphicsFamily";

static KernelPatcher::KextInfo kextAGDP {"com.apple.driver.AppleGraphicsDevicePolicy", &pathAGDP, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextBacklight {"com.apple.driver.AppleBacklight", &pathBacklight, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextMCCSControl {"com.apple.driver.AppleMCCSControl", &pathMCCSControl, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextIOGraphics { "com.apple.iokit.IOGraphicsFamily", &pathIOGraphics, 1, {true}, {},
	KernelPatcher::KextInfo::Unloaded };

NBlue *NBlue::callback = nullptr;


static Gen11 gen11;
static DYLDPatches dyldpatches;
static HDMI agfxhda;

void NBlue::init() {
    callback = this;
	
	lilu.onKextLoadForce(&kextAGDP);
	lilu.onKextLoadForce(&kextBacklight);
	lilu.onKextLoadForce(&kextMCCSControl);
	lilu.onKextLoadForce(&kextIOGraphics);
	
	gen11.init();
	agfxhda.init();
	dyldpatches.init();
	
    lilu.onPatcherLoadForce(
        [](void *user, KernelPatcher &patcher) { static_cast<NBlue *>(user)->processPatcher(patcher); }, this);
    lilu.onKextLoadForce(
        nullptr, 0,
        [](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
            static_cast<NBlue *>(user)->processKext(patcher, index, address, size);
        },
        this);
	
}


void NBlue::processPatcher(KernelPatcher &patcher) {
    auto *devInfo = DeviceInfo::create();
    if (devInfo) {
        devInfo->processSwitchOff();
		
		

        this->iGPU = OSDynamicCast(IOPCIDevice, devInfo->videoBuiltin);
        PANIC_COND(!this->iGPU, "nblue", "videoBuiltin is not IOPCIDevice");
		
		this->iGPU->enablePCIPowerManagement(kPCIPMCSPowerStateD0);
		this->iGPU->setBusMasterEnable(true);
		this->iGPU->setMemoryEnable(true);
		

        static uint8_t builtin[] = {0x00};
		//static uint8_t builtin2[] = {0x02, 0x00, 0x5c, 0x8A};
		//static uint8_t builtin3[] = {0x5c, 0x8A,0x00,0x00};
		
		static uint8_t builtin2[] = {0x00, 0x00, 0x49, 0x9A};
		static uint8_t builtin3[] = {0x49, 0x9A,0x00,0x00};

		WIOKit::renameDevice(this->iGPU, "IGPU");
		WIOKit::awaitPublishing(this->iGPU);
		
		this->iGPU->setProperty("built-in", builtin, arrsize(builtin));
		this->iGPU->setProperty("AAPL,slot-name", const_cast<char *>("built-in"), 9);
		this->iGPU->setProperty("hda-gfx", const_cast<char *>("onboard-1"), 10);
		this->iGPU->setProperty("model", const_cast<char *>("Intel Iris Xe Graphics"), 23);
		
		auto *prop = OSDynamicCast(OSData, this->iGPU->getProperty("AAPL,ig-platform-id"));
		if (!prop){
			this->iGPU->setProperty("AAPL,ig-platform-id", builtin2, arrsize(builtin2));
			this->iGPU->setProperty("device-id", builtin3, arrsize(builtin3));
		}

		//auto x = OSDynamicCast(OSData, this->iGPU->getProperty("AAPL,ig-platform-id"));
		//framebufferId = *(uint32_t*)x->getBytesNoCopy();
		
		//NETLOG("gen11", "framebufferId: = %x", framebufferId);
		setRMMIOIfNecessary();

        this->deviceId = WIOKit::readPCIConfigValue(this->iGPU, WIOKit::kIOPCIConfigDeviceID);
        this->pciRevision = WIOKit::readPCIConfigValue(NBlue::callback->iGPU, WIOKit::kIOPCIConfigRevisionID);
		
		auto gms = WIOKit::readPCIConfigValue(devInfo->videoBuiltin, WIOKit::kIOPCIConfigGraphicsControl, 0, 16) >> 8;
		
		if (gms < 0x10) {
			stolen_size = gms * 32;
		} else if (gms == 0x20 || gms == 0x30 || gms == 0x40) {
			stolen_size = gms * 32;
		} else if (gms >= 0xF0 && gms <= 0xFE) {
			stolen_size = ((gms & 0x0F) + 1) * 4;
		} else {
			SYSLOG( "nblue", "PANIC stolen_size=0 check DVMT in bios");
		}
		stolen_size *= (1024 * 1024);
		SYSLOG("nblue", "stolen_size 0x%x",stolen_size);
		
#if 0
		/* 1. Enable PCH reset handshake. */
		auto reg=readReg32(HSW_NDE_RSTWRN_OPT);
		writeReg32(HSW_NDE_RSTWRN_OPT ,reg|RESET_PCH_HANDSHAKE_ENABLE );
		
		writeReg32(0x00044200 ,0x80000000 );
		writeReg32(0x00044404 ,0x6f8ff07e );
		writeReg32(0x0004440c ,0x90700f89 );
		writeReg32(0x00044444 ,0xc1800000 );
		writeReg32(0x0004444c ,0x00003f07 );
		writeReg32(0x00044464 ,0xdb8480f8 );
		writeReg32(0x000444e4 ,0xffffffff );
		writeReg32(0x00045400 ,1 );
		writeReg32(0x00045404 ,3 );
		writeReg32(0x0004540c ,1 );
		writeReg32(0x0006c054 ,0x418a01ca );
		writeReg32(0x0006c058 ,0x012800e7 );
		writeReg32(0x0006c05c ,0x00000300 );
		writeReg32(0x00046000 ,0x00380158 );
		writeReg32(0x00046010 ,0xcc000000 );
		writeReg32(0x00046140 ,0x10000000 );
		
		writeReg32(0x0007027c ,0x07df0000 );
		writeReg32(0x00070180 ,0x84000400 );
		writeReg32(0x000701a0 ,0xff000000 );
		writeReg32(0x00070190 ,0x0383063f );
		writeReg32(0x0007019c ,0x027c0000 );
		writeReg32(0x000701ac ,0x00880000 );
		writeReg32(0x00070240 ,0x8000400e );
		writeReg32(0x00070244 ,0x80010039 );
		writeReg32(0x00070248 ,0x80010039 );
		writeReg32(0x0007024c ,0x80010039 );
		writeReg32(0x00070250 ,0x80010039 );
		writeReg32(0x00070254 ,0x80014047 );
		writeReg32(0x00070258 ,0x80020071 );
		writeReg32(0x0007025c ,0x80020071 );
		writeReg32(0x00070268 ,0x8000001c );
		writeReg32(0x0007017c ,0x07ff07e0 );
		writeReg32(0x00070084 ,0x010c0000 );
		writeReg32(0x00070080 ,0x00000023 );
		writeReg32(0x00070088 ,0x020300e9 );
		writeReg32(0x000700ac ,0x010c0000 );
		writeReg32(0x00070140 ,0x80008006 );
		writeReg32(0x00070144 ,0x8001000d );
		writeReg32(0x00070148 ,0x8001000d );
		writeReg32(0x0007014c ,0x8001000d );
		writeReg32(0x00070150 ,0x8001000d );
		writeReg32(0x00070154 ,0x80014010 );
		writeReg32(0x00070158 ,0x80020019 );
		writeReg32(0x0007015c ,0x80020019 );
		writeReg32(0x00070168 ,0x80000014 );
		writeReg32(0x00068194 ,0x00003aaa );
		writeReg32(0x00068190 ,0x00006aab );
		writeReg32(0x00068290 ,0x00000708 );
		writeReg32(0x00068188 ,0x00003aaa );
		writeReg32(0x00068184 ,0x00006aab );
		writeReg32(0x00068284 ,0x00000100 );
		writeReg32(0x00068174 ,0x07800438 );
		writeReg32(0x00070008 ,0xc0000000 );
		writeReg32(0x00060004 ,0x0833077f );
		writeReg32(0x00060008 ,0x081b07eb );
		writeReg32(0x00060000 ,0x0833077f );
		writeReg32(0x00060010 ,0x04650437 );
		writeReg32(0x00060014 ,0x044b0441 );
		writeReg32(0x0006000c ,0x04650437 );
		writeReg32(0x00060030 ,0x7e64fa4f );
		writeReg32(0x00060040 ,0x00043518 );
		writeReg32(0x00060400 ,0x8a000002 );
		writeReg32(0x00060410 ,0x00000021 );
		writeReg32(0x00045270 ,0x00000077 );
		writeReg32(0x00045260 ,0x20000000 );
		writeReg32(0x000701cc ,0x00002000 );
		writeReg32(0x0006b400 ,0x00030000 );
		writeReg32(0x0006bc00 ,0x00030000 );
		writeReg32(0x00045038 ,0xa2913050 );
		writeReg32(0x0007003c ,0xb1038c02 );
		writeReg32(0x00164294 ,0x018001d4 );
		writeReg32(0x00164298 ,0x00002644 );
		writeReg32(0x00164b10 ,0x401320ff );
		writeReg32(0x00164c10 ,0x401320ff );
		writeReg32(0x00164e10 ,0xffffffff );
		writeReg32(0x0007b008 ,0x00000024 );
		writeReg32(0x0007b808 ,0x00000024 );
		writeReg32(0x0006b400 ,0x00030000 );
		writeReg32(0x0006bc00 ,0x00030000 );

		
		
		/* WaModifyGamTlbPartitioning:icl */
		//writeReg32(GEN11_GACB_PERF_CTRL ,~GEN11_HASH_CTRL_MASK );
		//writeReg32(GEN11_GACB_PERF_CTRL ,GEN11_HASH_CTRL_BIT0 | GEN11_HASH_CTRL_BIT4 );

		/* Wa_1405766107:icl
		 * Formerly known as WaCL2SFHalfMaxAlloc
		 */
	  reg=readReg32(GEN11_LSN_UNSLCVC);
		writeReg32(GEN11_LSN_UNSLCVC ,reg|GEN11_LSN_UNSLCVC_GAFS_HALF_SF_MAXALLOC |
						GEN11_LSN_UNSLCVC_GAFS_HALF_CL2_MAXALLOC );

		/* Wa_220166154:icl
		 * Formerly known as WaDisCtxReload
		 */
	reg=readReg32(GEN8_GAMW_ECO_DEV_RW_IA);
		writeReg32(GEN8_GAMW_ECO_DEV_RW_IA ,reg|GAMW_ECO_DEV_CTX_RELOAD_DISABLE );

		/* Wa_1406463099:icl
		 * Formerly known as WaGamTlbPendError
		 */
	reg=readReg32(GAMT_CHKN_BIT_REG);
		writeReg32(GAMT_CHKN_BIT_REG ,reg|GAMT_CHKN_DISABLE_L3_COH_PIPE );

		/*
		 * Wa_1408615072:icl,ehl  (vsunit)
		 * Wa_1407596294:icl,ehl  (hsunit)
		 */
	reg=readReg32(UNSLICE_UNIT_LEVEL_CLKGATE);
		writeReg32(UNSLICE_UNIT_LEVEL_CLKGATE ,reg|VSUNIT_CLKGATE_DIS | HSUNIT_CLKGATE_DIS );


		/* Wa_1407352427:icl,ehl */
	reg=readReg32(UNSLICE_UNIT_LEVEL_CLKGATE2);
		writeReg32(UNSLICE_UNIT_LEVEL_CLKGATE2 ,reg|PSDUNIT_CLKGATE_DIS );
		
		/* Wa_1406680159:icl,ehl */
		//reg=readReg32(GEN11_SUBSLICE_UNIT_LEVEL_CLKGATE);
		//	writeReg32(GEN11_SUBSLICE_UNIT_LEVEL_CLKGATE ,reg|GWUNIT_CLKGATE_DIS );
		
		//to reduce sampler power.
		//writeReg32(GEN10_DFR_RATIO_EN_AND_CHICKEN ,DFR_DISABLE );
#endif
		KernelPatcher::routeVirtual(this->iGPU, WIOKit::PCIConfigOffset::ConfigRead16, configRead16, &orgConfigRead16);
		KernelPatcher::routeVirtual(this->iGPU, WIOKit::PCIConfigOffset::ConfigRead32, configRead32, &orgConfigRead32);

        DeviceInfo::deleter(devInfo);
		

		
    } else {
        SYSLOG("nblue", "Failed to create DeviceInfo");
    }
	

	dyldpatches.processPatcher(patcher);
}

void NBlue::setRMMIOIfNecessary() {
	if (UNLIKELY(!this->rmmio || !this->rmmio->getLength())) {
		this->rmmio = this->iGPU->mapDeviceMemoryWithRegister(kIOPCIConfigBaseAddress0);// ????
		this->rmmioPtr = reinterpret_cast<volatile uint32_t *>(this->rmmio->getVirtualAddress());
	}
}

bool NBlue::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
	if (kextIOGraphics.loadIndex == index) {
		//const LookupPatchPlus patch {&kextAGDP, kAGDPBoardIDKeyOriginal, kAGDPBoardIDKeyPatched, 1};
		//SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply AGDP board-id patch");
		/*
		KernelPatcher::RouteRequest requests[] = {
				{"__ZN13IOFramebuffer25extValidateDetailedTimingEP8OSObjectPvP25IOExternalMethodArguments", wrapValidateDetailedTiming},
			};
			patcher.routeMultiple(index, requests, address, size);
			patcher.clearError();*/
		
	}  else if (kextAGDP.loadIndex == index) {
		const LookupPatchPlus patch {&kextAGDP, kAGDPBoardIDKeyOriginal, kAGDPBoardIDKeyPatched, 1};
		SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply AGDP board-id patch");

		if (getKernelVersion() == KernelVersion::Ventura) {
			const LookupPatchPlus patch {&kextAGDP, kAGDPFBCountCheckVenturaOriginal, kAGDPFBCountCheckVenturaPatched,
				1};
			SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply AGDP fb count check patch");
		} else {
			const LookupPatchPlus patch {&kextAGDP, kAGDPFBCountCheckOriginal, kAGDPFBCountCheckPatched, 1};
			SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply AGDP fb count check patch");
		}
	}  else if (kextBacklight.loadIndex == index) {
		KernelPatcher::RouteRequest request {"__ZN15AppleIntelPanel10setDisplayEP9IODisplay", wrapApplePanelSetDisplay,
	  orgApplePanelSetDisplay};
			if (patcher.routeMultiple(kextBacklight.loadIndex, &request, 1, address, size)) {
				const UInt8 find[] = {"F%uT%04x"};
				const UInt8 replace[] = {"F%uTxxxx"};
				const LookupPatchPlus patch {&kextBacklight, find, replace, 1};
				SYSLOG_COND(!patch.apply(patcher, address, size), "NBlue", "Failed to apply backlight patch");
			}
} else if (kextMCCSControl.loadIndex == index) {
		KernelPatcher::RouteRequest requests[] = {
				{"__ZN25AppleMCCSControlGibraltar5probeEP9IOServicePi", wrapFunctionReturnZero},
				{"__ZN21AppleMCCSControlCello5probeEP9IOServicePi", wrapFunctionReturnZero},
			};
			patcher.routeMultiple(index, requests, address, size);
			patcher.clearError();
} else if (gen11.processKext(patcher, index, address, size)) {
        DBGLOG("nblue", "Processed Generation 11 configuration");
    } else if (agfxhda.processKext(patcher, index, address, size)) {
		DBGLOG("nblue", "Processed AppleGFXHDA");
	}
    return true;
}



uint16_t NBlue::configRead16(IORegistryEntry *service, uint32_t space, uint8_t offset) {
	if (callback && callback->orgConfigRead16) {
		auto result = callback->orgConfigRead16(service, space, offset);
		if (offset == WIOKit::kIOPCIConfigDeviceID && service != nullptr) {
			auto name = service->getName();
			if (name && name[0] == 'I' && name[1] == 'G' && name[2] == 'P' && name[3] == 'U') {
				uint32_t device;
				if (WIOKit::getOSDataValue(service, "device-id", device) && device != result) {
					return device;
				}
			}
		}

		return result;
	}

	return 0;
}

uint32_t NBlue::configRead32(IORegistryEntry *service, uint32_t space, uint8_t offset) {
	if (callback && callback->orgConfigRead32) {
		auto result = callback->orgConfigRead32(service, space, offset);
		// According to lvs unaligned reads may happen
		if ((offset == WIOKit::kIOPCIConfigDeviceID || offset == WIOKit::kIOPCIConfigVendorID) && service != nullptr) {
			auto name = service->getName();
			if (name && name[0] == 'I' && name[1] == 'G' && name[2] == 'P' && name[3] == 'U') {
				uint32_t device;
				if (WIOKit::getOSDataValue(service, "device-id", device) && device != (result & 0xFFFF)) {
					device = (result & 0xFFFF) | (device << 16);
					return device;
				}
			}
		}

		return result;
	}

	return 0;
}

size_t NBlue::wrapFunctionReturnZero() { return 0; }

struct ApplePanelData {
	const char *deviceName;
	UInt8 deviceData[36];
};

static ApplePanelData appleBacklightData[] = {
	{"F14Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x34, 0x00, 0x52, 0x00, 0x73, 0x00, 0x94, 0x00, 0xBE, 0x00, 0xFA, 0x01,
					 0x36, 0x01, 0x72, 0x01, 0xC5, 0x02, 0x2F, 0x02, 0xB9, 0x03, 0x60, 0x04, 0x1A, 0x05, 0x0A, 0x06,
					 0x0E, 0x07, 0x10}},
	{"F15Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x36, 0x00, 0x54, 0x00, 0x7D, 0x00, 0xB2, 0x00, 0xF5, 0x01, 0x49, 0x01,
					 0xB1, 0x02, 0x2B, 0x02, 0xB8, 0x03, 0x59, 0x04, 0x13, 0x04, 0xEC, 0x05, 0xF3, 0x07, 0x34, 0x08,
					 0xAF, 0x0A, 0xD9}},
	{"F16Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x18, 0x00, 0x27, 0x00, 0x3A, 0x00, 0x52, 0x00, 0x71, 0x00, 0x96, 0x00,
					 0xC4, 0x00, 0xFC, 0x01, 0x40, 0x01, 0x93, 0x01, 0xF6, 0x02, 0x6E, 0x02, 0xFE, 0x03, 0xAA, 0x04,
					 0x78, 0x05, 0x6C}},
	{"F17Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x4F, 0x00, 0x71, 0x00, 0x9B, 0x00, 0xCF, 0x01,
					 0x0E, 0x01, 0x5D, 0x01, 0xBB, 0x02, 0x2F, 0x02, 0xB9, 0x03, 0x60, 0x04, 0x29, 0x05, 0x1E, 0x06,
					 0x44, 0x07, 0xA1}},
	{"F18Txxxx", {0x00, 0x11, 0x00, 0x00, 0x00, 0x53, 0x00, 0x8C, 0x00, 0xD5, 0x01, 0x31, 0x01, 0xA2, 0x02, 0x2E, 0x02,
					 0xD8, 0x03, 0xAE, 0x04, 0xAC, 0x05, 0xE5, 0x07, 0x59, 0x09, 0x1C, 0x0B, 0x3B, 0x0D, 0xD0, 0x10,
					 0xEA, 0x14, 0x99}},
	{"F19Txxxx", {0x00, 0x11, 0x00, 0x00, 0x02, 0x8F, 0x03, 0x53, 0x04, 0x5A, 0x05, 0xA1, 0x07, 0xAE, 0x0A, 0x3D, 0x0E,
					 0x14, 0x13, 0x74, 0x1A, 0x5E, 0x24, 0x18, 0x31, 0xA9, 0x44, 0x59, 0x5E, 0x76, 0x83, 0x11, 0xB6,
					 0xC7, 0xFF, 0x7B}},
	{"F24Txxxx", {0x00, 0x11, 0x00, 0x01, 0x00, 0x34, 0x00, 0x52, 0x00, 0x73, 0x00, 0x94, 0x00, 0xBE, 0x00, 0xFA, 0x01,
					 0x36, 0x01, 0x72, 0x01, 0xC5, 0x02, 0x2F, 0x02, 0xB9, 0x03, 0x60, 0x04, 0x1A, 0x05, 0x0A, 0x06,
					 0x0E, 0x07, 0x10}},
};

bool NBlue::wrapApplePanelSetDisplay(IOService *that, IODisplay *display) {
	static bool once = false;
	if (!once) {
		once = true;
		auto *panels = OSDynamicCast(OSDictionary, that->getProperty("ApplePanels"));
		if (panels) {
			auto *rawPanels = panels->copyCollection();
			panels = OSDynamicCast(OSDictionary, rawPanels);

			if (panels) {
				for (auto &entry : appleBacklightData) {
					auto pd = OSData::withBytes(entry.deviceData, sizeof(entry.deviceData));
					if (pd) {
						panels->setObject(entry.deviceName, pd);
						//! No release required by current AppleBacklight implementation.
					} else {
					}
				}
				that->setProperty("ApplePanels", panels);
			}

			OSSafeReleaseNULL(rawPanels);
		} else {
		}
	}

	bool ret = FunctionCast(wrapApplePanelSetDisplay, callback->orgApplePanelSetDisplay)(that, display);
	return ret;
}

