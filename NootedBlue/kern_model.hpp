//  Copyright © 2022-2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

// Not final, will be re-done eventually.

#ifndef kern_model_hpp
#define kern_model_hpp
#include "kern_nblue.hpp"
#include <Headers/kern_util.hpp>

struct DeviceList {
    uint16_t dev {0};
    const char *name {nullptr};
};

struct DevicePair {
    const DeviceList *models;
    size_t modelNum;
};

static constexpr DeviceList g7[] = {
    {0x0402, "Intel HD Graphics (Haswell)"},
    {0x0406, "Intel HD Graphics (Haswell)"},
    {0x040A, "Intel HD Graphics (Haswell)"},
    {0x040B, "Intel HD Graphics (Haswell)"},
    {0x040E, "Intel HD Graphics (Haswell)"},
    {0x041E, "Intel HD 4400 Graphics"},
    {0x0A02, "Intel HD Graphics (Haswell ULT)"},
    {0x0A06, "Intel HD Graphics (Haswell ULT)"},
    {0x0A0A, "Intel HD Graphics (Haswell ULT)"},
    {0x0A0B, "Intel HD Graphics (Haswell ULT)"},
    {0x0A0E, "Intel HD Graphics (Haswell ULT)"},
    {0x0A16, "Intel HD 4400 Graphics"},
};

static constexpr DeviceList g8[] = {
    // TBF
    {0x1606, "Intel HD Graphics (Broadwell)"},
    {0x22B0, "Intel HD Graphics 40X"},
    {0x22B1, "Intel HD Graphics 40X"},
    {0x22B2, "Intel HD Graphics 40X"},
    {0x22B3, "Intel HD Graphics 400"},
};

static constexpr DeviceList g9_5[] = {
    // TBF
    {0x3184, "Intel UHD Graphics 605"},
    {0x3185, "Intel UHD Graphics 600"},
};

static constexpr DeviceList g11[] = {
    // TBF
    {0x0000, "Intel UHD Graphics (Jasper Lake)"},
};

static constexpr DevicePair devices[] = {
    {g7, arrsize(g7)},
    {g8, arrsize(g8)},
    {g9_5, arrsize(g9_5)},
};

inline const char *getBranding(uint16_t dev) {
    for (auto &device : devices) {
        for (size_t i = 0; i < device.modelNum; i++) {
            auto &model = device.models[i];
            if (model.dev == dev) { return model.name; }
            break;
        }
    }

    return "Intel HD/UHD Graphics";
}

#endif /* kern_model_hpp */
