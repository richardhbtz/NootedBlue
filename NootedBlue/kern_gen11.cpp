//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.
#include "kern_gen11.hpp"
#include <Headers/kern_api.hpp>


static const char *pathG11FB = "/System/Library/Extensions/AppleIntelICLLPGraphicsFramebuffer.kext/Contents/MacOS/"
                               "AppleIntelICLLPGraphicsFramebuffer";
static const char *pathG11FBT = "/Library/Extensions/AppleIntelTGLGraphicsFramebuffer.kext/Contents/MacOS/"
							   "AppleIntelTGLGraphicsFramebuffer";
static const char *pathG11HW =
    "/System/Library/Extensions/AppleIntelICLGraphics.kext/Contents/MacOS/AppleIntelICLGraphics";

static KernelPatcher::KextInfo kextG11FB {"com.apple.driver.AppleIntelICLLPGraphicsFramebuffer", &pathG11FB, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextG11HW {"com.apple.driver.AppleIntelICLGraphics", &pathG11HW, 1, {}, {},
    KernelPatcher::KextInfo::Unloaded};
static KernelPatcher::KextInfo kextG11FBT {"com.xxxxx.driver.AppleIntelTGLGraphicsFramebuffer", &pathG11FB, 1, {}, {},
	KernelPatcher::KextInfo::Unloaded};

Gen11 *Gen11::callback = nullptr;

void Gen11::init() {
	callback = this;
    lilu.onKextLoadForce(&kextG11FB);
    lilu.onKextLoadForce(&kextG11HW);
	lilu.onKextLoadForce(&kextG11FBT);
}



bool Gen11::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
	if (kextG11FBT.loadIndex == index) {
		SYSLOG("nblue", "AppleIntelTGLGraphicsFramebuffer");
		
		return true;
	}
    if (kextG11FB.loadIndex == index) {
        //NBlue::callback->igfxGen = iGFXGen::Gen11;
		NBlue::callback->setRMMIOIfNecessary();
		
		SolveRequestPlus solveRequests[] = {
			
			{"_CSR_PATCH_AX", this->_CSR_PATCH_AX},
			{"_CSR_PATCH_B0plus", this->_CSR_PATCH_B0plus},
			{"__ZN31AppleIntelFramebufferController14disableCDClockEv", this->orgDisableCDClock},
			{"__ZN31AppleIntelFramebufferController19setCDClockFrequencyEy", this->orgSetCDClockFrequency},
			
			//{"_gPlatformInformationList", this->gPlatformInformationList},
			//{"__ZN5IGGuC16enableInterruptsEv", this->oenableInterrupts},
			
			//{"__ZN13IGHardwareGuC10gMetaClassE", this->GuC},
			
		};
		SYSLOG_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		
		RouteRequestPlus requests[] = {
			
		
			{"__ZL15pmNotifyWrapperjjPyPj",wrapPmNotifyWrapper,	this->orgPmNotifyWrapper},
			{"__ZN31AppleIntelFramebufferController5probeEP9IOServicePi",wprobe,	this->owprobe},
			
			
			//{"__ZN31AppleIntelFramebufferController15hwSetPanelPowerEj",hwSetPanelPower},
			//{"__ZN31AppleIntelFramebufferController21hwSetPanelPowerConfigEj",hwSetPanelPowerConfig,	this->ohwSetPanelPowerConfig},
			
			{"__ZN31AppleIntelFramebufferController21probeCDClockFrequencyEv",wrapProbeCDClockFrequency,	this->orgProbeCDClockFrequency},
			
			{"__ZN31AppleIntelFramebufferController14ReadRegister32Em",wrapReadRegister32,	this->owrapReadRegister32},
			{"__ZN31AppleIntelFramebufferController15WriteRegister32Emj",wrapWriteRegister32,	this->owrapWriteRegister32},
			
			{"__ZN21AppleIntelFramebuffer18prepareToEnterWakeEv",releaseDoorbell},
			{"__ZN21AppleIntelFramebuffer17prepareToExitWakeEv",releaseDoorbell},
			
			//{"__ZN21AppleIntelFramebuffer18prepareToExitSleepEv",prepareToEnterWake},
			//{"__ZN21AppleIntelFramebuffer19prepareToEnterSleepEv",prepareToEnterWake},
			//{"__ZN21AppleIntelFramebuffer19prepareToEnterSleepEv",prepareToEnterSleep,	this->oprepareToEnterSleep},
			
			
			//{"__ZN21AppleIntelFramebuffer17connectionChangedEv",connectionChanged,	this->oconnectionChanged},
			//{"__ZN21AppleIntelFramebuffer14disableDisplayEb",disableDisplay,	this->odisableDisplay},
			
			//{"__ZN21AppleIntelFramebuffer18setPanelPowerStateEb",setPanelPowerState,	this->osetPanelPowerState},
			
			//{"__ZN31AppleIntelFramebufferController10LightUpEDPEP21AppleIntelFramebufferP21AppleIntelDisplayPathPK29IODetailedTimingInformationV2",LightUpEDP,	this->oLightUpEDP},
			
			
			//{"__ZN14AppleIntelPort14getPortByIndexEj",getPortByIndex,	this->ogetPortByIndex},
			//{"__ZN31AppleIntelFramebufferController16setupBootDisplayEv",setupBootDisplay,	this->osetupBootDisplay},
			
			{"__ZN31AppleIntelFramebufferController5startEP9IOService",start,	this->ostart},
			
			{"__ZN20IntelFBClientControl11doAttributeEjPmmS0_S0_P25IOExternalMethodArguments",wrapFBClientDoAttribute,	this->orgFBClientDoAttribute},
			
			/*{"__ZN31AppleIntelFramebufferController16hwRegsNeedUpdateEP21AppleIntelFramebufferP21AppleIntelDisplayPathP10CRTCParamsPK29IODetailedTimingInformationV2PN16AppleIntelScaler12SCALERPARAMSE", wrapHwRegsNeedUpdate,this->owrapHwRegsNeedUpdate},*/
				//{"__ZN21AppleIntelFramebuffer19validateDisplayModeEiPPKNS_15ModeDescriptionEPPK29IODetailedTimingInformationV2", fbdebugWrapValidateDisplayMode, this->ofbdebugWrapValidateDisplayMode},
		};
		SYSLOG_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		//writeReg32(SOUTH_DSPCLK_GATE_D,PCH_GMBUSUNIT_CLOCK_GATE_DISABLE); Wa_14011294188:ehl,jsl,tgl,rkl,adl-s
		static const uint8_t f1[] = {0x74, 0x28, 0x48, 0xff, 0x05, 0x9e, 0x65, 0x0b, 0x00, 0xbe, 0x20, 0x20, 0x0c, 0x00, 0x4c, 0x89, 0xef, 0xe8, 0x3f, 0xe2, 0xff, 0xff, 0x0d, 0x00, 0x10, 0x00, 0x00};
		static const uint8_t r1[] = {0x90, 0x90, 0x48, 0xff, 0x05, 0x9e, 0x65, 0x0b, 0x00, 0xbe, 0x20, 0x20, 0x0c, 0x00, 0x4c, 0x89, 0xef, 0xe8, 0x3f, 0xe2, 0xff, 0xff, 0xb8, 0x00, 0x00, 0x00, 0x80};
		static const uint8_t f1b[] = {0x74, 0x28, 0x48, 0xff, 0x05, 0x49, 0x15, 0x0a, 0x00, 0xbe, 0x20, 0x20, 0x0c, 0x00, 0x48, 0x89, 0xdf, 0xe8, 0x4a, 0x3e, 0xfe, 0xff, 0x0d, 0x00, 0x10, 0x00, 0x00};
		static const uint8_t r1b[] = {0x90, 0x90, 0x48, 0xff, 0x05, 0x49, 0x15, 0x0a, 0x00, 0xbe, 0x20, 0x20, 0x0c, 0x00, 0x48, 0x89, 0xdf, 0xe8, 0x4a, 0x3e, 0xfe, 0xff, 0xb8, 0x00, 0x00, 0x00, 0x80};
		
		
		//catalina
		static const uint8_t f3[]= {0xe8, 0x38, 0xe7, 0xff, 0xff, 0x84, 0xc0, 0x74, 0x3d};
		static const uint8_t r3[]= {0xe8, 0x38, 0xe7, 0xff, 0xff, 0x84, 0xc0, 0xeb, 0x3d};
		
		
		//gpuinfo
		static const uint8_t f5[] = {0xC7, 0x05, 0x43, 0xDF, 0x0C, 0x00, 0x02, 0x00, 0x5C, 0x8A, 0x89, 0x1D, 0x41, 0xDF, 0x0C, 0x00, 0x4C, 0x8D, 0x25, 0x63, 0xFE, 0x02, 0x00, 0x4C, 0x89, 0x25, 0x37, 0xDF, 0x0C, 0x00, 0xB0, 0x01, 0x88, 0x05, 0x37, 0xDF, 0x0C, 0x00, 0x44, 0x88, 0x35, 0x31, 0xDF, 0x0C, 0x00, 0x44, 0x88, 0x35, 0x2B, 0xDF, 0x0C, 0x00, 0x44, 0x88, 0x35, 0x25, 0xDF, 0x0C, 0x00, 0xB8, 0x00, 0x00, 0x00, 0x04, 0x89, 0x05, 0x1B, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x19, 0xDF, 0x0C, 0x00, 0xB8, 0x00, 0x00, 0x00, 0x60, 0x89, 0x05, 0x12, 0xDF, 0x0C, 0x00, 0x48, 0x8D, 0x3D, 0x0F, 0xDF, 0x0C, 0x00, 0xBA, 0x90, 0x00, 0x00, 0x00, 0x48, 0x8D, 0x35, 0x9F, 0xB0, 0x01, 0x00, 0xE8, 0xE8, 0x31, 0x52, 0xEB, 0xB8, 0x04, 0xE3, 0x00, 0x00, 0x89, 0x05, 0x83, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x85, 0xDF, 0x0C, 0x00, 0x4C, 0x89, 0x2D, 0x86, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x88, 0xDF, 0x0C, 0x00, 0x4C, 0x89, 0x2D, 0x89, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x8B, 0xDF, 0x0C, 0x00, 0x4C, 0x89, 0x3D, 0x8C, 0xDF, 0x0C, 0x00, 0xB8, 0x07, 0x00, 0x00, 0x00, 0x89, 0x05, 0x89, 0xDF, 0x0C, 0x00, 0xB8, 0x03, 0x00, 0x00, 0x00, 0x89, 0x05, 0x86, 0xDF, 0x0C, 0x00, 0xB8, 0x04, 0x00, 0x00, 0x00, 0x89, 0x05, 0x7F, 0xDF, 0x0C, 0x00, 0xB8, 0x80, 0xDF, 0x17, 0x10, 0x89, 0x05, 0x78, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x76, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x74, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x72, 0xDF, 0x0C, 0x00, 0xB8, 0x02, 0x00, 0x00, 0x00, 0x89, 0x05, 0x6B, 0xDF, 0x0C, 0x00, 0xB8, 0x08, 0x00, 0x00, 0x00, 0x89, 0x05, 0x64, 0xDF, 0x0C, 0x00, 0xB8, 0x06, 0x00, 0x00, 0x00, 0x89, 0x05, 0x5D, 0xDF, 0x0C, 0x00, 0x88, 0x1D, 0x5B, 0xDF, 0x0C, 0x00};
		
		static const uint8_t r5[] = {0xC7, 0x05, 0x43, 0xDF, 0x0C, 0x00, 0x02, 0x00, 0x5C, 0x8A, 0x89, 0x1D, 0x41, 0xDF, 0x0C, 0x00, 0x4C, 0x8D, 0x25, 0x63, 0xFE, 0x02, 0x00, 0x4C, 0x89, 0x25, 0x37, 0xDF, 0x0C, 0x00, 0xB0, 0x01, 0x88, 0x05, 0x37, 0xDF, 0x0C, 0x00, 0x44, 0x88, 0x35, 0x31, 0xDF, 0x0C, 0x00, 0x44, 0x88, 0x35, 0x2B, 0xDF, 0x0C, 0x00, 0x44, 0x88, 0x35, 0x25, 0xDF, 0x0C, 0x00, 0xB8, 0x00, 0x00, 0x00, 0x04, 0x89, 0x05, 0x1B, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x19, 0xDF, 0x0C, 0x00, 0xB8, 0x00, 0x00, 0x00, 0xc0, 0x89, 0x05, 0x12, 0xDF, 0x0C, 0x00, 0x48, 0x8D, 0x3D, 0x0F, 0xDF, 0x0C, 0x00, 0xBA, 0x90, 0x00, 0x00, 0x00, 0x48, 0x8D, 0x35, 0x9F, 0xB0, 0x01, 0x00, 0xE8, 0xE8, 0x31, 0x52, 0xEB, 0xB8, 0x00, 0x03, 0x40, 0x00, 0x89, 0x05, 0x83, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x85, 0xDF, 0x0C, 0x00, 0x4C, 0x89, 0x2D, 0x86, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x88, 0xDF, 0x0C, 0x00, 0x4C, 0x89, 0x2D, 0x89, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x8B, 0xDF, 0x0C, 0x00, 0x4C, 0x89, 0x3D, 0x8C, 0xDF, 0x0C, 0x00, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x89, 0x05, 0x89, 0xDF, 0x0C, 0x00, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x89, 0x05, 0x86, 0xDF, 0x0C, 0x00, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x89, 0x05, 0x7F, 0xDF, 0x0C, 0x00, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x89, 0x05, 0x78, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x76, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x74, 0xDF, 0x0C, 0x00, 0x89, 0x1D, 0x72, 0xDF, 0x0C, 0x00, 0xB8, 0x02, 0x00, 0x00, 0x00, 0x89, 0x05, 0x6B, 0xDF, 0x0C, 0x00, 0xB8, 0x08, 0x00, 0x00, 0x00, 0x89, 0x05, 0x64, 0xDF, 0x0C, 0x00, 0xB8, 0x10, 0x00, 0x00, 0x00, 0x89, 0x05, 0x5D, 0xDF, 0x0C, 0x00, 0x88, 0x1D, 0x5B, 0xDF, 0x0C, 0x00};

		//connectors
		static const uint8_t f6[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0xC1, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0xC1, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
		
		static const uint8_t r6[] = {0x02, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0xC1, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

		//-IgnoreXCPMRTNotifications
		static const uint8_t f8[] = {0x0f, 0x84, 0x85, 0x00, 0x00, 0x00, 0x48, 0xff, 0x05, 0x23, 0x24, 0x0b, 0x00, 0x48, 0x8b, 0x83, 0xf8, 0x0c, 0x00, 0x00, 0x48, 0x85, 0xc0, 0x74, 0x72, 0x48, 0xff, 0x05, 0x18, 0x24, 0x0b, 0x00, 0x48, 0xff, 0x05, 0xb9, 0x37, 0x0b, 0x00, 0x81, 0x38, 0x02, 0x00, 0x52, 0x8a, 0x74, 0x0f, 0x48, 0xff, 0x05, 0xba, 0x37, 0x0b, 0x00, 0x81, 0x38, 0x02, 0x00, 0x53, 0x8a, 0x75, 0x46};
		static const uint8_t r8[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x48, 0xff, 0x05, 0x23, 0x24, 0x0b, 0x00, 0x48, 0x8b, 0x83, 0xf8, 0x0c, 0x00, 0x00, 0x48, 0x85, 0xc0, 0x90, 0x90, 0x48, 0xff, 0x05, 0x18, 0x24, 0x0b, 0x00, 0x48, 0xff, 0x05, 0xb9, 0x37, 0x0b, 0x00, 0x81, 0x38, 0x02, 0x00, 0x52, 0x8a, 0x90, 0x90, 0x48, 0xff, 0x05, 0xba, 0x37, 0x0b, 0x00, 0x81, 0x38, 0x02, 0x00, 0x53, 0x8a, 0x90, 0x90};
		
		//tgl_dmc_ver2_12.bin
		static const uint8_t f9[] = {0x83, 0xbf, 0x9c, 0x0c, 0x00, 0x00, 0x00, 0x74, 0x1f, 0x4c, 0x8d, 0x35, 0x7c, 0xab, 0x0e, 0x00, 0x41, 0xbf, 0x8b, 0x0c, 0x00, 0x00};
		static const uint8_t r9[] = {0x83, 0xbf, 0x9c, 0x0c, 0x00, 0x00, 0x00, 0x74, 0x1f, 0x4c, 0x8d, 0x35, 0x7c, 0xab, 0x0e, 0x00, 0x41, 0xbf, 0x88, 0x18, 0x00, 0x00};
		static const uint8_t f9b[] = {0x48, 0xff, 0x05, 0xed, 0x28, 0x0b, 0x00, 0x4c, 0x8d, 0x35, 0x06, 0x79, 0x0e, 0x00, 0x41, 0xbf, 0x91, 0x0c, 0x00, 0x00};
		static const uint8_t r9b[] = {0x48, 0xff, 0x05, 0xed, 0x28, 0x0b, 0x00, 0x4c, 0x8d, 0x35, 0x06, 0x79, 0x0e, 0x00, 0x41, 0xbf, 0x88, 0x12, 0x00, 0x00};
		
		//dc6/9
		static const uint8_t f10[] = {0x74, 0x3b, 0x48, 0xff, 0x05, 0x49, 0x66, 0x0b, 0x00, 0x41, 0xc6, 0x85, 0xf9, 0x0b, 0x00, 0x00, 0x00, 0x48, 0xff, 0x05, 0xea, 0x69, 0x0b, 0x00, 0x48, 0x8d, 0x3d, 0x6b, 0x66, 0xfb, 0xff};
		static const uint8_t r10[] = {0x90, 0x90, 0x48, 0xff, 0x05, 0x49, 0x66, 0x0b, 0x00, 0x41, 0xc6, 0x85, 0xf9, 0x0b, 0x00, 0x00, 0x00, 0x48, 0xff, 0x05, 0xea, 0x69, 0x0b, 0x00, 0x48, 0x8d, 0x3d, 0x6b, 0x66, 0xfb, 0xff};
		
		static const uint8_t f10b[] = {0x74, 0x3b, 0x48, 0xff, 0x05, 0xfa, 0x65, 0x0b, 0x00, 0x41, 0xc6, 0x85, 0xfa, 0x0b, 0x00, 0x00, 0x00, 0x48, 0xff, 0x05, 0x93, 0x69, 0x0b, 0x00, 0x48, 0x8d, 0x3d, 0x14, 0x66, 0xfb, 0xff};
		static const uint8_t r10b[] = {0x90, 0x90, 0x48, 0xff, 0x05, 0xfa, 0x65, 0x0b, 0x00, 0x41, 0xc6, 0x85, 0xfa, 0x0b, 0x00, 0x00, 0x00, 0x48, 0xff, 0x05, 0x93, 0x69, 0x0b, 0x00, 0x48, 0x8d, 0x3d, 0x14, 0x66, 0xfb, 0xff};
		
		//Vendor info\n","AGDC
		static const uint8_t f11[] = {0x41, 0xbd, 0xc2, 0x02, 0x00, 0xe0};
		static const uint8_t r11[] = {0x41, 0xbd, 0xc2, 0x01, 0x00, 0xe0};
        
        //rev 7
        static const uint8_t f12[] = {0xe8, 0xfc, 0xf9, 0x56, 0xeb, 0x0f, 0xb6, 0xc0, 0x41, 0x89, 0x85, 0x9c, 0x0c, 0x00, 0x00};
        static const uint8_t r12[] = {0xb8, 0x07, 0x00, 0x00, 0x00, 0x0f, 0xb6, 0xc0, 0x41, 0x89, 0x85, 0x9c, 0x0c, 0x00, 0x00};
		
		//display power
		//[drm:pps_init_delays [i915]] panel power up delay 200, power down delay 50, power cycle delay 600
		//[drm:pps_init_registers [i915]] panel power sequencer register settings: PP_ON 0x7d00001, PP_OFF 0x1f40001, PP_DIV 0x60
		static const uint8_t f13[] = {0xb8, 0xc4, 0x09, 0xe4, 0x02, 0xba, 0x34, 0x08, 0x7d, 0x00, 0x41, 0xbe, 0x60, 0x00, 0x00, 0x00};
		static const uint8_t r13[] = {0xb8, 0x01, 0x00, 0xf4, 0x01, 0xba, 0x01, 0x00, 0xd0, 0x07, 0x41, 0xbe, 0x60, 0x00, 0x00, 0x00};

		
		//prepareToExitWake
		static const uint8_t f14[] = {0x0f, 0x84, 0x3d, 0x01, 0x00, 0x00, 0x48, 0xff, 0x05, 0xe2, 0xfa, 0x0d, 0x00, 0x80, 0x7f, 0x60, 0x00, 0x74, 0x18, 0x48, 0xff, 0x05, 0xdd, 0xfa, 0x0d, 0x00};
		static const uint8_t r14[] = {0x48, 0xe9, 0x3d, 0x01, 0x00, 0x00, 0x48, 0xff, 0x05, 0xe2, 0xfa, 0x0d, 0x00, 0x80, 0x7f, 0x60, 0x00, 0x74, 0x18, 0x48, 0xff, 0x05, 0xdd, 0xfa, 0x0d, 0x00};
		
		//dutyCycle
		static const uint8_t f15[] = {0x48, 0x8d, 0x3d, 0xf4, 0xc4, 0x09, 0x00, 0xba, 0x01, 0x00, 0x00, 0x00, 0xe8, 0xe6, 0xbe, 0x59, 0xeb, 0x85, 0xc0, 0x74, 0x21};
		static const uint8_t r15[] = {0x48, 0x8d, 0x3d, 0xf4, 0xc4, 0x09, 0x00, 0xba, 0x01, 0x00, 0x00, 0x00, 0xe8, 0xe6, 0xbe, 0x59, 0xeb, 0x85, 0xc0, 0x90, 0x90};
		
		
		// AppleIntelFramebufferController::hwSetMode skip hwRegsNeedUpdate
		static const uint8_t f2[] = {0xE8, 0x31, 0xE5, 0xFF, 0xFF, 0x84, 0xC0, 0x74, 0x3D};
		static const uint8_t r2[] = {0xE8, 0x31, 0xE5, 0xFF, 0xFF, 0x84, 0xC0, 0xEB, 0x3D};
		
		//sonoma
		static const uint8_t f2b[] = {0xE8, 0x54, 0xEA, 0xFF, 0xFF, 0x84, 0xC0, 0x74, 0x5C};
		static const uint8_t r2b[] = {0xE8, 0x54, 0xEA, 0xFF, 0xFF, 0x84, 0xC0, 0xeb, 0x5C};
		
		//sequoia
		static const uint8_t f2c[] = {0xE8, 0x74, 0xEA, 0xFF, 0xFF, 0x84, 0xC0, 0x74, 0x5C};
		static const uint8_t r2c[] = {0xE8, 0x74, 0xEA, 0xFF, 0xFF, 0x84, 0xC0, 0xeb, 0x5C};

		if (getKernelVersion() <= KernelVersion::Ventura) {
			KernelPatcher::LookupPatch patch { &kextG11FB, f2, r2, sizeof(f2), 1 };
			patcher.applyLookupPatch(&patch);
		}
		
		if (getKernelVersion() == KernelVersion::Sonoma) {
		KernelPatcher::LookupPatch patchb { &kextG11FB, f2b, r2b, sizeof(f2b), 1 };
		patcher.applyLookupPatch(&patchb);
		}
		
		if (getKernelVersion() >= KernelVersion::Sequoia) {
		KernelPatcher::LookupPatch patchc { &kextG11FB, f2c, r2c, sizeof(f2c), 1 };
		patcher.applyLookupPatch(&patchc);
		}
		
		
		LookupPatchPlus const patchesc[] = {
			//{&kextG11FB, f, r, arrsize(f),	1},
			{&kextG11FB, f3, r3, arrsize(f3),	1},
		};
		
		LookupPatchPlus const patches[] = {
			{&kextG11FB, f1, r1, arrsize(f1),	1},
			{&kextG11FB, f1b, r1b, arrsize(f1b),	1},
			//{&kextG11FB, f2, r2, arrsize(f2),	1},

			
			{&kextG11FB, f5, r5, arrsize(f5),	1},
			//{&kextG11FB, f6, r6, arrsize(f6),	1},
			
			{&kextG11FB, f8, r8, arrsize(f8),	1},
			{&kextG11FB, f9, r9, arrsize(f9),	1},
			{&kextG11FB, f9b, r9b, arrsize(f9b),	1},
			
			{&kextG11FB, f10, r10, arrsize(f10),	1},
			//{&kextG11FB, f10b, r10b, arrsize(f10b),	1},
			
			{&kextG11FB, f11, r11, arrsize(f11),	1},
            {&kextG11FB, f12, r12, arrsize(f12),    1},
			
			//{&kextG11FB, f13, r13, arrsize(f13),    1},
			//{&kextG11FB, f14, r14, arrsize(f14),    1},
			//{&kextG11FB, f15, r15, arrsize(f15),    1},
			
		};
		
		this->_CSR_PATCH_AX=tgl_dmc_ver2_12_bin;
		this->_CSR_PATCH_B0plus=tgl_dmc_ver2_12_bin;
		
		
		/*auto catalina = getKernelVersion() == KernelVersion::Catalina;
		
		if (catalina)
		SYSLOG_COND(!LookupPatchPlus::applyAll(patcher, patchesc , address, size), "nblue", "cata Failed to apply patches!");
		else*/
		SYSLOG_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "vent Failed to apply patches!");
		
		
		
		auto startAddress = patcher.solveSymbol(index, "__ZN31AppleIntelFramebufferController13FBMemMgr_InitEv", address, size);
		if (startAddress){
			hde64s handle;
			uint64_t shllAddr = 0, andlAddr = 0;
			uint32_t shllSize = 0, andlSize = 0;
			uint32_t shllDstr = 0, andlDstr = 0;
			
			uint8_t movl[] = {0x41, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x90};
			uint8_t nops[] = {0x90, 0x90, 0x90, 0x90};
			
			for (auto index = 0; index < 64; index += 1) {
				auto size = Disassembler::hdeDisasm(startAddress, &handle);
				
				if (handle.flags & F_ERROR) {
					break;
				}
				
				// Instruction: shll $0x11, %???
				// 3 bytes long if DSTReg < %r8d, otherwise 4 bytes long
				if (handle.opcode == 0xC1 && handle.imm.imm8 == 0x11) {
					shllAddr = startAddress;
					shllSize = handle.len;
					shllDstr = (handle.rex_b << 3) | handle.modrm_rm;
				}
				
				// Instruction: andl $0xFE000000, %???
				// 5 bytes long if DSTReg is %eax; 6 bytes long if DSTReg < %r8d; otherwise 7 bytes long.
				if ((handle.opcode == 0x25 || handle.opcode == 0x81) && handle.imm.imm32 == 0xFE000000) {
					andlAddr = startAddress;
					andlSize = handle.len;
					andlDstr = (handle.rex_b << 3) | handle.modrm_rm;
				}
				
				// Guard: Calculate and apply the binary patch if we have found both instructions
				if (shllAddr && andlAddr) {
					// Update the `movl` instruction with the actual amount of DVMT preallocated memory
					*reinterpret_cast<uint32_t*>(movl + 2) = NBlue::callback->stolen_size;
					
					// Update the `movl` instruction with the actual destination register
					// Find the actual starting point of the patch and the number of bytes to patch
					uint8_t* patchStart;
					uint32_t patchSize;
					if (andlDstr >= 8) {
						// %r8d, %r9d, ..., %r15d
						movl[1] += (andlDstr - 8);
						patchStart = movl;
						patchSize = 7;
					} else {
						// %eax, %ecx, ..., %edi
						movl[1] += andlDstr;
						patchStart = (movl + 1);
						patchSize = andlDstr == 0 ?  5 : 6;
					}
					
					// Guard: Prepare to apply the binary patch
					if (MachInfo::setKernelWriting(true, KernelPatcher::kernelWriteLock) != KERN_SUCCESS) {
					}
					
					// Replace `shll` with `nop`s
					// The number of nops is determined by the actual instruction length
					lilu_os_memcpy(reinterpret_cast<void*>(shllAddr), nops, shllSize);
					
					// Replace `andl` with `movl`
					// The patch contents and size are determined by the destination register of `andl`
					lilu_os_memcpy(reinterpret_cast<void*>(andlAddr), patchStart, patchSize);
					
					// Finished applying the binary patch
					MachInfo::setKernelWriting(false, KernelPatcher::kernelWriteLock);
				}
				
				startAddress += size;
			}
		}
		
		
        DBGLOG("nblue", "Loaded AppleIntelICLLPGraphicsFramebuffer!");
        return true;
		
    } else if (kextG11HW.loadIndex == index) {
		NBlue::callback->setRMMIOIfNecessary();

		SolveRequestPlus solveRequests[] = {
			
			//{"__ZN26IGHardwareCommandStreamer514submitExecListEj", this->orgSubmitExecList},
			//{"__ZN5IGGuC16enableInterruptsEv", this->oenableInterrupts},
			
			//{"__ZN13IGHardwareGuC10gMetaClassE", this->GuC},
			//{"_g_timestampFreqPerDenominator", this->_g_timestampFreqPerDenominator},
			//{"__ZN16IntelAccelerator13SafeForceWakeEbj", this->orgSafeForceWake},
			
			
			
		};
		//SYSLOG_COND(!SolveRequestPlus::solveAll(patcher, index, solveRequests, address, size), "nblue",	"Failed to resolve symbols");
		 
		 RouteRequestPlus requests[] = {
			 
			 
			 {"__ZN17IGInterruptBridge17serviceInterruptsERK8IGBitSetILm41EE",serviceInterrupts, this->oserviceInterrupts},
			 {"__ZN17IGInterruptBridge22readAndClearInterruptsER8IGBitSetILm41EE",readAndClearInterrupts, this->oreadAndClearInterrupts},
			// {"__ZN5IGGuC21dumpGuCPanicDebugInfoEPKcj",releaseDoorbell},
			 {"__ZN16IntelAccelerator26SafeForceWakeMultithreadedEbjj",forceWake, this->oforceWake},
			// {"__ZN19IGAccelEventMachine21enableSchedulerEventsEv", },
			// {"__ZN26IGHardwareCommandStreamer513schedDispatchEj", },
			 
			 //{"__ZL21getInterruptTypeIndexj", getInterruptTypeIndex, this->ogetInterruptTypeIndex},
			 
			 {"__ZN16IntelAccelerator19PAVPCommandCallbackE22PAVPSessionCommandID_tjPjb", wrapPavpSessionCallback, this->orgPavpSessionCallback},
			// {"__ZN13IGHardwareGuC15releaseDoorbellEP35UK_GEN11_GUC_CONTEXT_DESCRIPTOR_REC", releaseDoorbell},
			// {"__Z16InitIclLpWaTableP9_WA_TABLEP18_SKU_FEATURE_TABLEjtt", InitIclLpWaTable, this->oInitIclLpWaTable},
			/* {"__ZN26IGHardwareCommandStreamer519csbProcessPreemptedEPK28SGfxContextStatusBufferEntry", csbProcessPreempted, this->ocsbProcessPreempted},
			 {"__ZN26IGHardwareCommandStreamer522csbProcessActiveToIdleEPK28SGfxContextStatusBufferEntry", csbProcessActiveToIdle, this->ocsbProcessActiveToIdle},
			 {"__ZN26IGHardwareCommandStreamer523csbProcessElementSwitchEPK28SGfxContextStatusBufferEntry", csbProcessElementSwitch, this->ocsbProcessElementSwitch},
			 */
			 
			 {"__ZL27ContextStatusBufferValidateRK15IGHwCsExecList5PK28SGfxContextStatusBufferEntry.cold.1", releaseDoorbell},
			 {"__ZL27ContextStatusBufferValidateRK15IGHwCsExecList5PK28SGfxContextStatusBufferEntry.cold.2", releaseDoorbell},
			 
			// {"__ZN12IGScheduler516enableInterruptsEv", enableInterrupts},
			// {"__ZN12IGScheduler415systemWillSleepEv", wrapSystemWillSleep},
			// {"__ZN12IGScheduler413systemDidWakeEv", wrapSystemDidWake},
			// {"__ZN5IGGuC18actionResponseWaitEv", actionResponseWait, this->oactionResponseWait},
			 
			// {"__ZL27ContextStatusBufferValidateRK15IGHwCsExecList5PK28SGfxContextStatusBufferEntry_cold_2", ContextStatusBufferValidate},
			 {"__ZN16IntelAccelerator10getGPUInfoEv", getGPUInfo, this->ogetGPUInfo},

		 };
		SYSLOG_COND(!RouteRequestPlus::routeAll(patcher, index, requests, address, size), "nblue","Failed to route symbols");
		
		/*
		gKmGen9GuCBinary = patcher.solveSymbol<uint8_t *>(index, "__KmGen11ICLGuCBinary", address, size);
		if (gKmGen9GuCBinary) {
			DBGLOG("nblue", "obtained __KmGen9GuCBinary");

			auto loadGuC = patcher.solveSymbol(index, "__ZN13IGHardwareGuC13loadGuCBinaryEv", address, size);
			if (loadGuC) {
				DBGLOG("nblue", "obtained IGHardwareGuC::loadGuCBinary");

				// Lookup the assignment to the size register.
				uint8_t sizeReg[] {0x10, 0xC3, 0x00, 0x00};
				auto pos    = reinterpret_cast<uint8_t *>(loadGuC);
				auto endPos = pos + PAGE_SIZE;
				while (memcmp(pos, sizeReg, sizeof(sizeReg)) != 0 && pos < endPos)
					pos++;

				// Verify and store the size pointer
				if (pos != endPos) {
					pos += sizeof(uint32_t);
					firmwareSizePointer = reinterpret_cast<uint32_t *>(pos);
					DBGLOG("nblue", "discovered firmware size: %u bytes", *firmwareSizePointer);
					// Firmware size must not be bigger than 1 MB
					if ((*firmwareSizePointer & 0xFFFFF) == *firmwareSizePointer)
						// Firmware follows the signature
						signaturePointer = gKmGen9GuCBinary + *firmwareSizePointer;
					else
						firmwareSizePointer = nullptr;
				}

				if (firmwareSizePointer) {
					orgLoadGuCBinary = patcher.routeFunction(loadGuC, reinterpret_cast<mach_vm_address_t>(wrapLoadGuCBinary), true);
					if (patcher.getError() == KernelPatcher::Error::NoError) {
						DBGLOG("nblue", "routed IGHardwareGuC::loadGuCBinary");

						KernelPatcher::RouteRequest requests[] {
							{"__ZN12IGScheduler412loadFirmwareEv", wrapLoadFirmware, orgLoadFirmware},
							{"__ZN13IGHardwareGuC16initSchedControlEv", wrapInitSchedControl, orgInitSchedControl},
							{"__ZN20IGSharedMappedBuffer11withOptionsEP11IGAccelTaskmjj", wrapIgBufferWithOptions, orgIgBufferWithOptions},
							{"__ZNK14IGMappedBuffer20getGPUVirtualAddressEv", wrapIgBufferGetGpuVirtualAddress, orgIgBufferGetGpuVirtualAddress},
						};
						patcher.routeMultiple(index, requests, address, size);
					} else {
						SYSLOG("nblue", "failed to route IGHardwareGuC::loadGuCBinary %d", patcher.getError());
						patcher.clearError();
					}
				} else {
					SYSLOG("nblue", "failed to find GuC firmware size assignment");
				}
			} else {
				SYSLOG("nblue", "failed to resolve IGHardwareGuC::loadGuCBinary %d", patcher.getError());
				patcher.clearError();
			}
		} else {
			SYSLOG("nblue", "failed to resolve __KmGen9GuCBinary %d", patcher.getError());
			patcher.clearError();
		}*/
		
		//if (!patchRCSCheck(orgSubmitExecList))
		//	SYSLOG("nblue", "Failed to patch RCS check.");
		
		
		// sku bypass cata
		static const uint8_t f[] = {
			0x83, 0xf9, 0x02, 0x0f, 0x84, 0x9d, 0x00, 0x00, 0x00, 0x83, 0xf9, 0x01, 0x0f, 0x85, 0x4e, 0x03, 0x00, 0x00, 0xff, 0xc8
		};
		static const uint8_t r[] = {
			0x83, 0xf9, 0x02, 0x48, 0xe9, 0x98, 0x01, 0x00, 0x00, 0x83, 0xf9, 0x01, 0x0f, 0x85, 0x4e, 0x03, 0x00, 0x00, 0xff, 0xc8
		};
		
		//sku bypass IntelAccelerator::getGPUInfo
		static const uint8_t f2[] = {
			0x0F, 0x87, 0x17, 0x01, 0x00, 0x00, 0x48, 0x8D, 0x0D, 0x96, 0x02, 0x00, 0x00
		};
		static const uint8_t r2[] = {
			0xe9, 0x8b, 0x00, 0x00, 0x00, 0x90, 0x48, 0x8D, 0x0D, 0x96, 0x02, 0x00, 0x00
		};
		
		
		// GEN8_GTCR			_MMIO(0x4274) to  GEN12_GUC_TLB_INV_CR		_MMIO(0xcee8)
		static const uint8_t f3[] = {
			0x74, 0x42, 0x00, 0x00
		};
		static const uint8_t r3[] = {
			0xe8, 0xce, 0x00, 0x00
		};
		
		static const uint8_t f3b[] = { // skip -allow3d
			0x41, 0x83, 0xBE, 0x40, 0x11, 0x00, 0x00, 0x02, 0x77, 0x38
		};
		static const uint8_t r3b[] = {
			0x41, 0x83, 0xBE, 0x40, 0x11, 0x00, 0x00, 0x02, 0xeb, 0x38
		};
		
		//#define HAS_PCH_ICP(dev_priv)			(INTEL_PCH_TYPE(dev_priv) == PCH_ICP)
		/*static const uint8_t f3a[] = {
			0xEB, 0x41, 0x89, 0x86, 0x48, 0x11, 0x00, 0x00, 0x41, 0xC7, 0x86, 0x4C, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00
		};
		static const uint8_t r3a[] = {
			0xEB, 0x41, 0x89, 0x86, 0x48, 0x11, 0x00, 0x00, 0x41, 0xC7, 0x86, 0x4C, 0x11, 0x00, 0x00, PCH_TGP, 0x00, 0x00
		};*/

        //rev 0x7
        static const uint8_t f3a[] = {
            0xe8, 0x47, 0x55, 0x6d, 0xeb, 0x0f, 0xb6, 0xc0, 0x41, 0x89, 0x86, 0x40, 0x11, 0x00, 0x00
        };
        static const uint8_t r3a[] = {
            0xb8, 0x07, 0x00, 0x00, 0x00, 0x0f, 0xb6, 0xc0, 0x41, 0x89, 0x86, 0x40, 0x11, 0x00, 0x00
        };
		
		//orgSubmitExecList
		static const uint8_t f6[] = {
			0x80, 0x39, 0x00, 0x0f, 0x85, 0xc2, 0x00, 0x00, 0x00
		};
		static const uint8_t r6[] = {
			0x80, 0x39, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
		};
		static const uint8_t f6a[] = {
			0x80, 0x39, 0x00, 0x0f, 0x85, 0x21, 0xff, 0xff, 0xff
		};
		static const uint8_t r6a[] = {
			0x80, 0x39, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
		};
		static const uint8_t f6b[] = {
			0x80, 0x39, 0x00, 0x0f, 0x85, 0xa4, 0x00, 0x00, 0x00
		};
		static const uint8_t r6b[] = {
			0x80, 0x39, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
		};
		
		//gen11
		static const uint8_t f7[] = {
			0x80, 0xc1, 0x00, 0x00
		};
		static const uint8_t r7[] = {
			0x40, 0x02, 0x19, 0x00
		};
		

		
		LookupPatchPlus const patchesc[] = {
			{&kextG11HW, f, r, arrsize(f),	1},
			{&kextG11HW, f3, r3, arrsize(f3),	8},

		};

		LookupPatchPlus const patches[] = {

			{&kextG11HW, f2, r2, arrsize(f2),	1},
			
			{&kextG11HW, f3, r3, arrsize(f3),	22},
			{&kextG11HW, f3a, r3a, arrsize(f3a),	1},
			
			{&kextG11HW, f3b, r3b, arrsize(f3b),	1},
			
			{&kextG11HW, f6, r6, arrsize(f6),	1},
			{&kextG11HW, f6a, r6a, arrsize(f6a),	1},
			{&kextG11HW, f6b, r6b, arrsize(f6b),	1},
			
			{&kextG11HW, f7, r7, arrsize(f7),	9},//12
			
		};
		
		
		/*auto catalina = getKernelVersion() == KernelVersion::Catalina;
		if (catalina)
			PANIC_COND(!LookupPatchPlus::applyAll(patcher, patchesc , address, size), "nblue", "cata Failed to apply patches!");
		else*/
		SYSLOG_COND(!LookupPatchPlus::applyAll(patcher, patches , address, size), "nblue", "vent Failed to apply patches!");
        DBGLOG("nblue", "Loaded AppleIntelICLGraphics!");

		return true;
    }

    return false;
}

IOReturn Gen11::wrapPavpSessionCallback(void *intelAccelerator, int32_t sessionCommand, uint32_t sessionAppId, uint32_t *a4, bool flag) {

	if (sessionCommand == 4) {
		return kIOReturnTimeout; // or kIOReturnSuccess
	}

	return FunctionCast(wrapPavpSessionCallback, callback->orgPavpSessionCallback)(intelAccelerator, sessionCommand, sessionAppId, a4, flag);
}

IOReturn Gen11::wrapFBClientDoAttribute(void *fbclient, uint32_t attribute, unsigned long *unk1, unsigned long unk2, unsigned long *unk3, unsigned long *unk4, void *externalMethodArguments) {
	if (attribute == 0x923) {
		return kIOReturnUnsupported;
	}
	
	return FunctionCast(wrapFBClientDoAttribute, callback->orgFBClientDoAttribute)(fbclient, attribute, unk1, unk2, unk3, unk4, externalMethodArguments);
}

bool  Gen11::getGPUInfo(void *that)
{
	
#define RPM_CONFIG0				(0xd00)
#define   GEN9_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_SHIFT	3
#define   GEN9_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_MASK	(1 << GEN9_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_SHIFT)
#define   GEN9_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_19_2_MHZ	0
#define   GEN9_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_24_MHZ	1
#define   GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_SHIFT	3
#define   GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_MASK	(0x7 << GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_SHIFT)
#define   GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_24_MHZ	0
#define   GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_19_2_MHZ	1
#define   GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_38_4_MHZ	2
#define   GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_25_MHZ	3
#define   GEN10_RPM_CONFIG0_CTC_SHIFT_PARAMETER_SHIFT	1
#define   GEN10_RPM_CONFIG0_CTC_SHIFT_PARAMETER_MASK	(0x3 << GEN10_RPM_CONFIG0_CTC_SHIFT_PARAMETER_SHIFT)
	
	auto ret=FunctionCast(getGPUInfo, callback->ogetGPUInfo)(that);
	
	/*callback->orgSafeForceWake(that,true, 5u);
	
	UInt32 *gpuDataPtr = getMember<UInt32*>(that, 590);
	UInt32 c0 = gpuDataPtr[RPM_CONFIG0];
	callback->orgSafeForceWake(that,false, 5u);*/
	
	//gpuSkuData >>= SKU_SHIFT;
	//*getMember<UInt32*>(that, 1108)= gpuSkuData;//9
	
	
	unsigned int numSlices = 1;
	
   getMember<UInt32>(that, 1124) = numSlices;
	
	unsigned int numSubSlices = 5;//__builtin_popcount(~subSliceCountFuseData);
   getMember<UInt32>(that, 1122) = numSubSlices;
	
	//getMember<UInt32>(that, 1123) = 0x0000001f;//subSliceCountFuseData; ????
	
	/* linux
	slice total: 1, mask=0001
	subslice total: 5
	slice0: 5 subslices, mask=0000001f
	EU total: 80
	EU per subslice: 16
	*/
	
	//MTLIGAccelDevice::initializeDevice 1x6x8
	unsigned int maxEUPerSubSlice = 16;//16;//8 - __builtin_popcount((UInt8)euDisableFuseData);
  getMember<UInt32>(that, 1128) = maxEUPerSubSlice;
  unsigned int totalExecutionUnits = maxEUPerSubSlice * numSubSlices;
  getMember<UInt32>(that, 1109) = totalExecutionUnits;
	
	
  getMember<UInt32>(that, 1203) = numSlices;
  getMember<UInt32>(that, 1204) = numSubSlices;
	
  getMember<UInt32>(that, 1126) =numSubSlices;
	
	
	UInt32 freq=0;
	/*UInt32 crystal_clock =
		(c0 & GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_MASK) >>
		GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_SHIFT;
	
	UInt32 f19_2_mhz = 19200000;
	UInt32 f24_mhz = 24000000;
	UInt32 f25_mhz = 25000000;
	UInt32 f38_4_mhz = 38400000;

	switch (crystal_clock) {
		case GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_24_MHZ:
			freq= f24_mhz;
		case GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_19_2_MHZ:
			freq=  f19_2_mhz;
		case GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_38_4_MHZ:
			freq=  f38_4_mhz;
		case GEN11_RPM_CONFIG0_CRYSTAL_CLOCK_FREQ_25_MHZ:
			freq=  f25_mhz;
	}
	
	freq >>= 3 - ((c0 & GEN10_RPM_CONFIG0_CTC_SHIFT_PARAMETER_MASK) >>
			  GEN10_RPM_CONFIG0_CTC_SHIFT_PARAMETER_SHIFT);*/
	
	//getMember<UInt32>(that, 1135) = 100000 * freq;
	
	//*callback->_g_timestampFreqPerDenominator = freq;
			
	return ret;
}


UInt8 Gen11::wrapLoadGuCBinary(void *that) {

	if (callback->firmwareSizePointer)
		callback->performingFirmwareLoad = true;

	auto r = FunctionCast(wrapLoadGuCBinary, callback->orgLoadGuCBinary)(that);
	DBGLOG("nblue", "loadGuCBinary returned %d", r);

	callback->performingFirmwareLoad = false;

	return r;
}

bool Gen11::wrapLoadFirmware(void *that) {

	//(*reinterpret_cast<uintptr_t **>(that))[35] = reinterpret_cast<uintptr_t>(wrapSystemWillSleep);
	//(*reinterpret_cast<uintptr_t **>(that))[36] = reinterpret_cast<uintptr_t>(wrapSystemDidWake);
	return FunctionCast(wrapLoadFirmware, callback->orgLoadFirmware)(that);
}

void Gen11::wrapSystemWillSleep(void *that) {
	DBGLOG("nblue", "systemWillSleep GuC callback");
}

void Gen11::wrapSystemDidWake(void *that) {
	DBGLOG("nblue", "systemDidWake GuC callback");

	// This is IGHardwareGuC class instance.
	auto &GuC = (reinterpret_cast<OSObject **>(that))[76];

	if (GuC)
	if (GuC->metaCast("IGHardwareGuC")) {
		DBGLOG("igfx", "reloading firmware on wake; discovered IGHardwareGuC - releasing");
		GuC->release();
		GuC = nullptr;
	}

	FunctionCast(wrapLoadFirmware, callback->orgLoadFirmware)(that);
}

bool Gen11::wrapInitSchedControl(void *that) {
	DBGLOG("nblue", "attempting to init sched control with load %d", callback->performingFirmwareLoad);
	bool perfLoad = callback->performingFirmwareLoad;
	callback->performingFirmwareLoad = false;
	bool r = FunctionCast(wrapInitSchedControl, callback->orgInitSchedControl)(that);

	callback->performingFirmwareLoad = perfLoad;
	return r;
}

void *Gen11::wrapIgBufferWithOptions(void *accelTask, void* size, unsigned int type, unsigned int flags) {
	void *r = nullptr;

	if (callback->performingFirmwareLoad) {
		callback->dummyFirmwareBuffer = Buffer::create<uint8_t>(*(unsigned long*)size);

		const void *fw = nullptr;
		const void *fwsig = nullptr;
		size_t fwsize = 0;
		size_t fwsigsize = 0;


		fw = GuCFirmwareKBL;
		fwsig = GuCFirmwareKBLSignature;
		fwsize = GuCFirmwareKBLSize;
		fwsigsize = GuCFirmwareSignatureSize;

		unsigned long newsize = fwsize > *(unsigned long*)size ? ((fwsize + 0xFFFF) & (~0xFFFF)) : *(unsigned long*)size;
		r = FunctionCast(wrapIgBufferWithOptions, callback->orgIgBufferWithOptions)(accelTask, (void*)newsize,type,flags);
		if (r && callback->dummyFirmwareBuffer) {
			auto status = MachInfo::setKernelWriting(true, KernelPatcher::kernelWriteLock);
			if (status == KERN_SUCCESS) {
				callback->realFirmwareBuffer = static_cast<uint8_t **>(r)[7];
				static_cast<uint8_t **>(r)[7] = callback->dummyFirmwareBuffer;
				lilu_os_memcpy(callback->realFirmwareBuffer, fw, fwsize);
				lilu_os_memcpy(callback->signaturePointer, fwsig, fwsigsize);
				callback->realBinarySize = static_cast<uint32_t>(fwsize);
				*callback->firmwareSizePointer = static_cast<uint32_t>(fwsize);
				MachInfo::setKernelWriting(false, KernelPatcher::kernelWriteLock);
			} else {
				//SYSLOG("igfx", "ig buffer protection upgrade failure %d", status);
			}
		} else if (callback->dummyFirmwareBuffer) {
			//SYSLOG("igfx", "ig shared buffer allocation failure");
			Buffer::deleter(callback->dummyFirmwareBuffer);
			callback->dummyFirmwareBuffer = nullptr;
		} else {
			//SYSLOG("igfx", "dummy buffer allocation failure");
		}
	} else {
		r = FunctionCast(wrapIgBufferWithOptions, callback->orgIgBufferWithOptions)(accelTask, size,type,flags);
	}

	return r;
}

UInt64 Gen11::wrapIgBufferGetGpuVirtualAddress(void *that) {
	if (callback->performingFirmwareLoad && callback->realFirmwareBuffer) {
		static_cast<uint8_t **>(that)[7] = callback->realFirmwareBuffer;
		callback->realFirmwareBuffer = nullptr;
		Buffer::deleter(callback->dummyFirmwareBuffer);
		callback->dummyFirmwareBuffer = nullptr;
	}

	return FunctionCast(wrapIgBufferGetGpuVirtualAddress, callback->orgIgBufferGetGpuVirtualAddress)(that);
}


uint32_t Gen11::wrapReadRegister32(void *controller, uint32_t address) {

	uint32_t retVal = FunctionCast(wrapReadRegister32, callback->owrapReadRegister32)(controller,address);

	return retVal;
}

void Gen11::wrapWriteRegister32(void *controller, uint32_t address, uint32_t value) {

	FunctionCast(wrapWriteRegister32, callback->owrapWriteRegister32)(controller,address,value);

}


//#define  BXT_CDCLK_CD2X_DIV_SEL_MASK	REG_GENMASK(23, 22)
#define  BXT_CDCLK_CD2X_DIV_SEL_MASK	(3 << 22)
#define  BXT_CDCLK_CD2X_DIV_SEL_1	(0 << 22)
#define  BXT_CDCLK_CD2X_DIV_SEL_1_5	(1 << 22)
#define  BXT_CDCLK_CD2X_DIV_SEL_2	(2 << 22)
#define  BXT_CDCLK_CD2X_DIV_SEL_4	(3 << 22)
#define  BXT_CDCLK_CD2X_PIPE(pipe)	((pipe) << 20)
#define  CDCLK_DIVMUX_CD_OVERRIDE	(1 << 19)
#define  BXT_CDCLK_CD2X_PIPE_NONE	BXT_CDCLK_CD2X_PIPE(3)
#define  ICL_CDCLK_CD2X_PIPE_NONE	(7 << 19)
#define  BXT_CDCLK_SSA_PRECHARGE_ENABLE	(1 << 16)
#define  CDCLK_FREQ_DECIMAL_MASK	(0x7ff)
#define SKL_DSSM				(0x51004)
#define ICL_DSSM_CDCLK_PLL_REFCLK_MASK		(7 << 29)
#define ICL_DSSM_CDCLK_PLL_REFCLK_24MHz		(0 << 29)
#define ICL_DSSM_CDCLK_PLL_REFCLK_19_2MHz	(1 << 29)
#define ICL_DSSM_CDCLK_PLL_REFCLK_38_4MHz	(2 << 29)
#define BXT_DE_PLL_ENABLE		(0x46070)
#define   BXT_DE_PLL_PLL_ENABLE		(1 << 31)
#define   BXT_DE_PLL_LOCK		(1 << 30)
#define   BXT_DE_PLL_FREQ_REQ		(1 << 23)
#define   BXT_DE_PLL_FREQ_REQ_ACK	(1 << 22)
#define   ICL_CDCLK_PLL_RATIO(x)	(x)
#define   ICL_CDCLK_PLL_RATIO_MASK	0xff
#define CDCLK_CTL			(0x46000)

#define DIV_ROUND_CLOSEST(x, divisor)(			\
{							\
__typeof(x) __x = x;				\
__typeof(divisor) __d = divisor;			\
	(((__typeof(x))-1) > 0 ||				\
	 ((__typeof(divisor))-1) > 0 || (__x) > 0) ?	\
		(((__x) + ((__d) / 2)) / (__d)) :	\
		(((__x) - ((__d) / 2)) / (__d));	\
}							\
)

void Gen11::sanitizeCDClockFrequency(void *that) {
	
	struct intel_cdclk_config {
		unsigned int cdclk, vco, ref, bypass;
		uint8_t voltage_level;
	};
	
	struct intel_cdclk_config *cdclk_config;
	
	uint32_t val;

	//auto referenceFrequency = callback->wrapReadRegister32(that, SKL_DSSM) & ICL_DSSM_CDCLK_PLL_REFCLK_MASK;
	auto referenceFrequency = callback->wrapReadRegister32(that, ICL_REG_DSSM) >> 29;
	uint32_t newCdclkFrequency = 0;
	uint32_t newPLLFrequency = 0;
	switch (referenceFrequency) {
		case ICL_REF_CLOCK_FREQ_19_2:
			newCdclkFrequency = ICL_CDCLK_FREQ_652_8;
			newPLLFrequency = ICL_CDCLK_PLL_FREQ_REF_19_2;
			break;
			
		case ICL_REF_CLOCK_FREQ_24_0:
			newCdclkFrequency = ICL_CDCLK_FREQ_648_0;
			newPLLFrequency = ICL_CDCLK_PLL_FREQ_REF_24_0;
			break;
			
		case ICL_REF_CLOCK_FREQ_38_4:
			newCdclkFrequency = ICL_CDCLK_FREQ_652_8;
			newPLLFrequency = ICL_CDCLK_PLL_FREQ_REF_38_4;
			break;
			
		default:
			return;
	}
	
	/*val = wrapReadRegister32(that, BXT_DE_PLL_ENABLE);
	if ((val & BXT_DE_PLL_PLL_ENABLE) == 0 ||
		(val & BXT_DE_PLL_LOCK) == 0) {
		return;
	}*/
	
	callback->orgDisableCDClock(that);
	
	callback->orgSetCDClockFrequency(that, newPLLFrequency);
	
	//auto cdclk = wrapReadRegister32(that, ICL_REG_CDCLK_CTL) & CDCLK_FREQ_DECIMAL_MASK;

}

uint32_t Gen11::wrapProbeCDClockFrequency(void *that) {

	auto cdclk = callback->wrapReadRegister32(that, ICL_REG_CDCLK_CTL) & CDCLK_FREQ_DECIMAL_MASK;

	
	if (cdclk < ICL_CDCLK_DEC_FREQ_THRESHOLD) {
		sanitizeCDClockFrequency(that);
	}
	
	auto retVal = callback->orgProbeCDClockFrequency(that);
	return retVal;
}


bool Gen11::start(void *that,void  *param_1)
{
	callback->framecont=that;
	return false;
	
	auto ret= FunctionCast(start, callback->ostart)(that,param_1);
	
	return ret;
}
int Gen11::wrapPmNotifyWrapper(unsigned int a0, unsigned int a1, unsigned long long *a2, unsigned int *freq) {
	
	/*struct intel_rps_freq_caps *caps;
	
	caps->rp0_freq *= GEN9_FREQ_SCALER;
	caps->rp1_freq *= GEN9_FREQ_SCALER;
	caps->min_freq *= GEN9_FREQ_SCALER;
	
	uint32_t mult=GEN9_FREQ_SCALER;
	uint32_t ddcc_status = 0;
	
	*freq =caps->rp1_freq;
	return 0;*/
	
	uint32_t cfreq = 0;

	FunctionCast(wrapPmNotifyWrapper, callback->orgPmNotifyWrapper)(a0, a1, a2, &cfreq);
	
	if (!callback->freq_max) {
		callback->freq_max = wrapReadRegister32(callback->framecont, GEN6_RP_STATE_CAP) & 0xFF;

	}
	
	*freq = (GEN9_FREQ_SCALER << GEN9_FREQUENCY_SHIFT) * callback->freq_max;
	return 0;
}

bool Gen11::patchRCSCheck(mach_vm_address_t& start) {
	constexpr unsigned ninsts_max {256};
	
	hde64s dis;
	
	bool found_cmp = false;
	bool found_jmp = false;

	for (size_t i = 0; i < ninsts_max; i++) {
		auto sz = Disassembler::hdeDisasm(start, &dis);

		if (dis.flags & F_ERROR) {
			break;
		}

		/* cmp byte ptr [rcx], 0 */
		if (!found_cmp && dis.opcode == 0x80 && dis.modrm_reg == 7 && dis.modrm_rm == 1)
			found_cmp = true;
		/* jnz rel32 */
		if (found_cmp && dis.opcode == 0x0f && dis.opcode2 == 0x85) {
			found_jmp = true;
			break;
		}

		start += sz;
	}
	
	if (found_jmp) {
		auto status = MachInfo::setKernelWriting(true, KernelPatcher::kernelWriteLock);
		if (status == KERN_SUCCESS) {
			constexpr uint8_t nop6[] {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
			lilu_os_memcpy(reinterpret_cast<void*>(start), nop6, arrsize(nop6));
			MachInfo::setKernelWriting(false, KernelPatcher::kernelWriteLock);
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

int forceWakei=1;
void Gen11::forceWake(void *that, bool set, uint32_t dom, uint8_t ctx) {
	// ctx 2: IRQ, ctx 1: normal
	
	uint32_t ack_exp = set << ctx;
	uint32_t mask = 1 << ctx;
	uint32_t wr = ack_exp | (1 << ctx << 16);
	unsigned d2;
	
	
	if (forceWakei){
		/*uint32_t cap=	wrapReadRegister32(callback->framecont,HSW_EDRAM_CAP);
		const unsigned int ways[8] = { 4, 8, 12, 16, 16, 16, 16, 16 };
		const unsigned int sets[4] = { 1, 1, 2, 2 };
		uint64_t size= EDRAM_NUM_BANKS(cap) *
				ways[EDRAM_WAYS_IDX(cap)] *
				sets[EDRAM_SETS_IDX(cap)] *
				1024 * 1024;
		
		SYSLOG("nblue","Found %lluMB of eDRAM\n", size / (1024 * 1024));*/
		

		
		
		for (unsigned d = FW_DOMAIN_ID_RENDER; d <= FW_DOMAIN_ID_COUNT;d++){
			wrapWriteRegister32(callback->framecont, regForDom(BIT(d)), fw_clear(FORCEWAKE_KERNEL));
			wrapWriteRegister32(callback->framecont, regForDom(BIT(d)), fw_clear(0xffff));
		}
		forceWakei=0;
	}

	/*enum forcewake_domain_id {
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
	};

	enum forcewake_domain_id {
		FW_DOMAIN_ID_RENDER = 0,
		FW_DOMAIN_ID_GT,
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
	};*/
	
	//if (dom & 7) dom=BIT(FW_DOMAIN_ID_MEDIA_VEBOX1);
	//if (dom & 8) dom=BIT(FW_DOMAIN_ID_MEDIA_VEBOX2);
	
	/*for (unsigned d = DOM_FIRST; d <= DOM_LAST; d <<= 1)
	if (dom & d) {
		wrapWriteRegister32(callback->framecont, regForDom(d), wr);
		IOPause(100);
		if (!pollRegister(ackForDom(d), ack_exp, mask, FORCEWAKE_ACK_TIMEOUT_MS) &&
			!forceWakeWaitAckFallback(d, ack_exp, mask) &&
			!pollRegister(ackForDom(d), ack_exp, mask, FORCEWAKE_ACK_TIMEOUT_MS))
			SYSLOG("nblue", "PANIC ForceWake timeout for %d domain %d, expected 0x%x", d,(dom), ack_exp);
	}
	return;*/
	
	if (dom == 7) dom=BIT(FW_DOMAIN_ID_MEDIA_VEBOX0);
	if (dom == 8) dom=BIT(FW_DOMAIN_ID_MEDIA_VEBOX1);
	
	/*if (dom & 1) dom=BIT(FW_DOMAIN_ID_RENDER);
	if (dom & 2) dom=BIT(FW_DOMAIN_ID_MEDIA_VDBOX0)|BIT(FW_DOMAIN_ID_MEDIA_VDBOX2)|BIT(FW_DOMAIN_ID_MEDIA_VEBOX0);
	if (dom & 4) dom=BIT(FW_DOMAIN_ID_MEDIA_VDBOX0)|BIT(FW_DOMAIN_ID_MEDIA_VDBOX2)|BIT(FW_DOMAIN_ID_MEDIA_VEBOX0)|BIT(FW_DOMAIN_ID_RENDER)|BIT(FW_DOMAIN_ID_GT);*/
	
	for (unsigned d = FW_DOMAIN_ID_RENDER; d <= FW_DOMAIN_ID_COUNT;d++){
		d2 = d << 1;
		if (dom & d2) {
			//if (d != FW_DOMAIN_ID_RENDER && d != FW_DOMAIN_ID_GT && d != FW_DOMAIN_ID_MEDIA_VDBOX0 && d != FW_DOMAIN_ID_MEDIA_VDBOX2 && d != FW_DOMAIN_ID_MEDIA_VEBOX0) continue;
			wrapWriteRegister32(callback->framecont, regForDom(d2), wr);
			IOPause(100);
			if (!pollRegister(ackForDom(d2), ack_exp, mask, FORCEWAKE_ACK_TIMEOUT_MS) &&
				!forceWakeWaitAckFallback(d2, ack_exp, mask) &&
				!pollRegister(ackForDom(d2), ack_exp, mask, FORCEWAKE_ACK_TIMEOUT_MS))
				SYSLOG("nblue", "PANIC ForceWake timeout for %d domain %d, expected 0x%x", d,(dom), ack_exp);
		}
	}
}

bool Gen11::pollRegister(uint32_t reg, uint32_t val, uint32_t mask, uint32_t timeout) {
	AbsoluteTime now, deadline;
	
	clock_interval_to_deadline(timeout, kMillisecondScale, &deadline);
	
	for (clock_get_uptime(&now); now < deadline; clock_get_uptime(&now)) {
		auto rd = wrapReadRegister32(callback->framecont, reg);

//		DBGLOG(log, "Rd 0x%x = 0x%x, expected 0x%x", reg, rd, val);

		if ((rd & mask) == val)
			return true;
	}

	return false;
}

bool Gen11::forceWakeWaitAckFallback(uint32_t d, uint32_t val, uint32_t mask) {
	unsigned pass = 1;
	bool ack = false;
	auto controller = callback->framecont;
	
	do {
		pollRegister(ackForDom(d), 0, FORCEWAKE_KERNEL_FALLBACK, FORCEWAKE_ACK_TIMEOUT_MS);
		wrapWriteRegister32(controller, regForDom(d), fw_set(FORCEWAKE_KERNEL_FALLBACK));
		
		IODelay(10 * pass);
		pollRegister(ackForDom(d), FORCEWAKE_KERNEL_FALLBACK, FORCEWAKE_KERNEL_FALLBACK, FORCEWAKE_ACK_TIMEOUT_MS);

		ack = (wrapReadRegister32(controller, ackForDom(d)) & mask) == val;

		wrapWriteRegister32(controller, regForDom(d), fw_clear(FORCEWAKE_KERNEL_FALLBACK));
	} while (!ack && pass++ < 10);
	
//	DBGLOG(log, "Force wake fallback used to %s %s in %u passes", set ? "set" : "clear", strForDom(d), pass);
	
	return ack;
}

void Gen11::releaseDoorbell()
{
	
	
}
int iniin=1;
void  Gen11::readAndClearInterrupts(void *that,void *param_1)
{
	
	if (iniin){
		iniin=0;
		
		wrapWriteRegister32(callback->framecont, GEN11_GFX_MSTR_IRQ, 0);
		wrapWriteRegister32(callback->framecont,GEN11_DISPLAY_INT_CTL, 0);
		uint32_t master_ctl=wrapReadRegister32(callback->framecont,GEN11_GFX_MSTR_IRQ);
		
		//Disable RCS, BCS, VCS and VECS class engines.
		wrapWriteRegister32(callback->framecont, GEN11_RENDER_COPY_INTR_ENABLE, 0);
		wrapWriteRegister32(callback->framecont, GEN11_VCS_VECS_INTR_ENABLE,	  0);
		
		// Restore masks irqs on RCS, BCS, VCS and VECS engines.
		wrapWriteRegister32(callback->framecont, GEN11_RCS0_RSVD_INTR_MASK,	~0);
		wrapWriteRegister32(callback->framecont, GEN11_BCS_RSVD_INTR_MASK,	~0);
		wrapWriteRegister32(callback->framecont, GEN11_VCS0_VCS1_INTR_MASK,	~0);
		wrapWriteRegister32(callback->framecont, GEN11_VCS2_VCS3_INTR_MASK,	~0);
		wrapWriteRegister32(callback->framecont, GEN11_VECS0_VECS1_INTR_MASK,	~0);
		
		wrapWriteRegister32(callback->framecont, GEN11_GPM_WGBOXPERF_INTR_ENABLE, 0);
		wrapWriteRegister32(callback->framecont, GEN11_GPM_WGBOXPERF_INTR_MASK,  ~0);
		wrapWriteRegister32(callback->framecont, GEN11_GUC_SG_INTR_ENABLE, 0);
		wrapWriteRegister32(callback->framecont, GEN11_GUC_SG_INTR_MASK,  ~0);
		
		
		
		uint32_t irqs = GT_RENDER_USER_INTERRUPT;
		uint32_t guc_mask = /*intel_uc_wants_guc(&gt->uc) ? GUC_INTR_GUC2HOST :*/ 0;
		uint32_t gsc_mask = 0;
		uint32_t heci_mask = 0;
		uint32_t dmask;
		uint32_t smask;
		
		irqs |= GT_CS_MASTER_ERROR_INTERRUPT |
		GT_CONTEXT_SWITCH_INTERRUPT |
		GT_WAIT_SEMAPHORE_INTERRUPT;
		
		dmask = irqs << 16 | irqs;
		smask = irqs << 16;
		
		
		/* Enable RCS, BCS, VCS and VECS class interrupts. */
		wrapWriteRegister32(callback->framecont, GEN11_RENDER_COPY_INTR_ENABLE, dmask);
		wrapWriteRegister32(callback->framecont, GEN11_VCS_VECS_INTR_ENABLE, dmask);
		
		/* Unmask irqs on RCS, BCS, VCS and VECS engines. */
		wrapWriteRegister32(callback->framecont, GEN11_RCS0_RSVD_INTR_MASK, ~smask);
		wrapWriteRegister32(callback->framecont, GEN11_BCS_RSVD_INTR_MASK, ~smask);
		wrapWriteRegister32(callback->framecont, GEN11_VCS0_VCS1_INTR_MASK, ~dmask);
		wrapWriteRegister32(callback->framecont, GEN11_VCS2_VCS3_INTR_MASK, ~dmask);
		wrapWriteRegister32(callback->framecont, GEN11_VECS0_VECS1_INTR_MASK, ~dmask);
		
		/*
		 * RPS interrupts will get enabled/disabled on demand when RPS itself
		 * is enabled/disabled.
		 */
		
		
		wrapWriteRegister32(callback->framecont, GEN11_GPM_WGBOXPERF_INTR_ENABLE, 0);
		wrapWriteRegister32(callback->framecont, GEN11_GPM_WGBOXPERF_INTR_MASK,  ~0);
		
		/* Same thing for GuC interrupts */
		wrapWriteRegister32(callback->framecont, GEN11_GUC_SG_INTR_ENABLE, 0);
		wrapWriteRegister32(callback->framecont, GEN11_GUC_SG_INTR_MASK,  ~0);
		
		wrapWriteRegister32(callback->framecont,GEN11_DISPLAY_INT_CTL, GEN11_DISPLAY_IRQ_ENABLE);
		wrapWriteRegister32(callback->framecont, GEN11_GFX_MSTR_IRQ, GEN11_MASTER_IRQ);
	}
	
	
	FunctionCast(readAndClearInterrupts, callback->oreadAndClearInterrupts)(that,param_1);
}


void * Gen11::serviceInterrupts(void *param_1)
{
	return FunctionCast(serviceInterrupts, callback->oserviceInterrupts)(param_1);
	
}

void * Gen11::wprobe(void *that,void *param_1,int *param_2)
{
	return that;
	
}
