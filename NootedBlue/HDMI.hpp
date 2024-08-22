//  Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#pragma once
//#include "AMDCommon.hpp"
#include <Headers/kern_patcher.hpp>
#include <Headers/kern_util.hpp>

class HDMI {
    public:
    static HDMI *callback;
    void init();
    bool processKext(KernelPatcher &patcher, size_t id, mach_vm_address_t slide, size_t size);
};

/* ---- Patches ---- */

// TODO: Provide an explanation here.
static const UInt8 kCreateAppleHDAFunctionGroup1Original[] = {0x3D, 0xD7, 0xAA, 0x00, 0x00};
static const UInt8 kCreateAppleHDAFunctionGroup1Patched[] = {0x3D, 0x00, 0x00, 0x00, 0x00};

// TODO: Provide an explanation here.
static const UInt8 kCreateAppleHDAFunctionGroup2Original[] = {0x3D, 0xF7, 0xAA, 0x00, 0x00, 0x7F};
static const UInt8 kCreateAppleHDAFunctionGroup2Patched[] = {0x3D, 0xF7, 0xAA, 0x00, 0x00, 0xEB};

// TODO: Provide an explanation here.
static const UInt8 kCreateAppleHDACatalinaOriginal[] = {0x3D, 0x1F, 0xAB, 0x00, 0x00, 0x7F, 0x00, 0x3D, 0xE0, 0xAA,
    0x00, 0x00, 0x74, 0x00};
static const UInt8 kCreateAppleHDACatalinaOriginalMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x00};
static const UInt8 kCreateAppleHDACatalinaPatched[] = {0x66, 0x90, 0x66, 0x90, 0x66, 0x90, 0x66, 0x90, 0x66, 0x90, 0x66,
    0x90, 0xEB, 0x00};
static const UInt8 kCreateAppleHDACatalinaPatchedMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x00};

// Force else path for `deviceID < 0xAA88` and `deviceID < 0xAAE0` check.
static const UInt8 kCreateAppleHDAWidget1Original[] = {0x3D, 0x87, 0xAA, 0x00, 0x00, 0x7E, 0x00, 0x3D, 0xDF, 0xAA, 0x00,
    0x00, 0x7F, 0x00};
static const UInt8 kCreateAppleHDAWidget1OriginalMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x00};
static const UInt8 kCreateAppleHDAWidget1Patched[] = {0x66, 0x90, 0x66, 0x90, 0x66, 0x90, 0x90, 0x66, 0x90, 0x66, 0x90,
    0x90, 0xEB, 0x00};
static const UInt8 kCreateAppleHDAWidget1PatchedMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x00};

// Force else path for `deviceID < 0xAB20` check.
static const UInt8 kCreateAppleHDAWidget2Original[] = {0x3D, 0x1F, 0xAB, 0x00, 0x00, 0x7E, 0x00};
static const UInt8 kCreateAppleHDAWidget2OriginalMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
static const UInt8 kCreateAppleHDAWidget2Patched[] = {0x66, 0x90, 0x66, 0x90, 0x66, 0x90, 0x90};

// Force else path for `0x18 < deviceID - 0xAB20` check.
static const UInt8 kCreateAppleHDAOriginal[] = {0x8D, 0x88, 0xE0, 0x54, 0xFF, 0xFF, 0x83, 0xF9, 0x18, 0x77, 0x00};
static const UInt8 kCreateAppleHDAOriginalMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
static const UInt8 kCreateAppleHDAPatched[] = {0x66, 0x90, 0x66, 0x90, 0x66, 0x90, 0x66, 0x90, 0x90, 0xEB, 0x00};
static const UInt8 kCreateAppleHDAPatchedMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

// Force `deviceID == 0xA*F8` path.
static const UInt8 kCreateAppleHDA2Original[] = {0x3D, 0xF8, 0xA0, 0x00, 0x00, 0x74, 0x00};
static const UInt8 kCreateAppleHDA2OriginalMask[] = {0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0x00};
static const UInt8 kCreateAppleHDA2Patched[] = {0x66, 0x90, 0x66, 0x90, 0x90, 0xEB, 0x00};
static const UInt8 kCreateAppleHDA2PatchedMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

// TODO: Provide an explanation here.
static const UInt8 kAHDACreate1Original[] = {0x87, 0xAA, 0x00, 0x00, 0x0F, 0x8F};
static const UInt8 kAHDACreate1Patched[] = {0x66, 0x90, 0x66, 0x90, 0x48, 0xE9};

// TODO: Provide an explanation here.
static const UInt8 kAHDACreate2Original[] = {0x83, 0x00, 0x38, 0x77, 0x00, 0xB0};
static const UInt8 kAHDACreate2OriginalMask[] = {0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xF0};
static const UInt8 kAHDACreate2Patched[] = {0x66, 0x90, 0x90, 0xEB, 0x00, 0x00};
static const UInt8 kAHDACreate2PatchedMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00};

// TODO: Provide an explanation here.
static const UInt8 kAHDACreate3Original[] = {0xD8, 0xAA, 0x00, 0x00, 0x75, 0x00};
static const UInt8 kAHDACreate3Mask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
static const UInt8 kAHDACreate3Patched[] = {0x66, 0x90, 0x66, 0x90, 0x66, 0x90};
