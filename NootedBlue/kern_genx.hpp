//  Copyright © 2023 ChefKiss Inc. Licensed under the Thou Shalt Not Profit License version 1.0. See LICENSE for
//  details.

#ifndef kern_genx_hpp
#define kern_genx_hpp
#include "kern_nblue.hpp"
#include "kern_guc.hpp"
#include "kern_patcherplus.hpp"
#include <Headers/kern_util.hpp>


class Genx {
	friend class Gen11;

private:
	
	static bool start(void *that,void  *param_1);
	mach_vm_address_t ostart {};
	
	static void * wprobe(void *that,void *param_1,int *param_2);
	mach_vm_address_t owprobe {};
	
	static void dovoid();
	static IOReturn wrapFBClientDoAttribute(void *fbclient, uint32_t attribute, unsigned long *unk1, unsigned long unk2, unsigned long *unk3, unsigned long *unk4, void *externalMethodArguments);
	mach_vm_address_t orgFBClientDoAttribute {};
	
	static uint32_t hwSaveNVRAM();
	
	
public:

	void init();
	static Genx *callback;
	bool processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size);
	
};

#endif /* kern_gen8_hpp */
