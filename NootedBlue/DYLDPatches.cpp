//! Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.5.
//! See LICENSE for details.

#include "DYLDPatches.hpp"
#include "kern_nblue.hpp"
#include <Headers/kern_api.hpp>
#include <Headers/kern_devinfo.hpp>
#include <IOKit/IODeviceTreeSupport.h>

DYLDPatches *DYLDPatches::callback = nullptr;

void DYLDPatches::init() { callback = this; }

void DYLDPatches::processPatcher(KernelPatcher &patcher) {

	return;

    auto *entry = IORegistryEntry::fromPath("/", gIODTPlane);
    if (entry) {
        DBGLOG("DYLD", "Setting hwgva-id to iMacPro1,1");
        entry->setProperty("hwgva-id", const_cast<char *>(kHwGvaId), arrsize(kHwGvaId));
        OSSafeReleaseNULL(entry);
    }

    KernelPatcher::RouteRequest request {"_cs_validate_page", wrapCsValidatePage, this->orgCsValidatePage};

    SYSLOG_COND(!patcher.routeMultipleLong(KernelPatcher::KernelID, &request, 1), "DYLD",
        "Failed to route kernel symbols");
}

void DYLDPatches::wrapCsValidatePage(vnode *vp, memory_object_t pager, memory_object_offset_t page_offset,
    const void *data, int *validated_p, int *tainted_p, int *nx_p) {
    FunctionCast(wrapCsValidatePage, callback->orgCsValidatePage)(vp, pager, page_offset, data, validated_p, tainted_p,
        nx_p);

    char path[PATH_MAX];
    int pathlen = PATH_MAX;
    if (vn_getpath(vp, path, &pathlen) != 0) { return; }

    if (!UserPatcher::matchSharedCachePath(path)) {
        if (LIKELY(strncmp(path, kCoreLSKDMSEPath, arrsize(kCoreLSKDMSEPath))) ||
            LIKELY(strncmp(path, kCoreLSKDPath, arrsize(kCoreLSKDPath)))) {
            return;
        }
        const DYLDPatch patch = {kCoreLSKDOriginal, kCoreLSKDPatched, "CoreLSKD streaming CPUID to Haswell"};
        patch.apply(const_cast<void *>(data), PAGE_SIZE);
        return;
    }

    if (UNLIKELY(KernelPatcher::findAndReplace(const_cast<void *>(data), PAGE_SIZE, kVideoToolboxDRMModelOriginal,
            arrsize(kVideoToolboxDRMModelOriginal), BaseDeviceInfo::get().modelIdentifier, 20))) {
        DBGLOG("DYLD", "Applied 'VideoToolbox DRM model check' patch");
    }

    const DYLDPatch patches[] = {
        {kAGVABoardIdOriginal, kAGVABoardIdPatched, "iMacPro1,1 spoof (AppleGVA)"},
		{kHEVCEncBoardIdOriginal, kHEVCEncBoardIdPatched, "iMacPro1,1 spoof (AppleGVAHEVCEncoder)"},
    };
    DYLDPatch::applyAll(patches, const_cast<void *>(data), PAGE_SIZE);
	
	
	
	//opengl
	static const uint8_t f1[] = {0x77, 0x2E, 0x4C, 0x8D, 0x3D, 0x08, 0xCC, 0xB5, 0x00, 0x48, 0x8D, 0x35, 0xD3, 0xCB, 0xB5, 0x00, 0x48, 0x8D, 0x0D, 0xE3, 0x00, 0x00, 0x00, 0x48, 0x63, 0x04, 0x81, 0x48, 0x01, 0xC8, 0xFF, 0xE0};
	static const uint8_t r1[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	
	//metal
	static const uint8_t f2[] = {0x81, 0xFF, 0x86, 0x80, 0x5C, 0x8A, 0x74, 0x3F, 0x81, 0xFF, 0x86, 0x80, 0x5D, 0x8A, 0x74, 0x37, 0xE8, 0xB1, 0x05, 0x00, 0x00, 0x84, 0xC0, 0x75, 0x2E};
	static const uint8_t r2[] = {0x81, 0xFF, 0x86, 0x80, 0x5C, 0x8A, 0x74, 0x3F, 0x81, 0xFF, 0x86, 0x80, 0x5D, 0x8A, 0x74, 0x37, 0xE8, 0xB1, 0x05, 0x00, 0x00, 0x84, 0xC0, 0xEB, 0x2E};
	
	
	
    if (getKernelVersion() >= KernelVersion::Ventura) {
        const DYLDPatch patches[] = {
			
			
			{f1, r1, "f1"},
			{f2, r2, "f2"},

        };
        DYLDPatch::applyAll(patches, const_cast<void *>(data), PAGE_SIZE);
    }

	
}
