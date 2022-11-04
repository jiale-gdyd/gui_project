#include <libdrm/xf86drm.h>
#include <libdrm/drm/amdgpu.h>
#include <libdrm/drm/amdgpu_drm.h>

#include "amdgpu_internal.h"

drm_public int amdgpu_vm_reserve_vmid(amdgpu_device_handle dev, uint32_t flags)
{
    union drm_amdgpu_vm vm;

    vm.in.op = AMDGPU_VM_OP_RESERVE_VMID;
    vm.in.flags = flags;

    return drmCommandWriteRead(dev->fd, DRM_AMDGPU_VM, &vm, sizeof(vm));
}

drm_public int amdgpu_vm_unreserve_vmid(amdgpu_device_handle dev, uint32_t flags)
{
    union drm_amdgpu_vm vm;

    vm.in.op = AMDGPU_VM_OP_UNRESERVE_VMID;
    vm.in.flags = flags;

    return drmCommandWriteRead(dev->fd, DRM_AMDGPU_VM, &vm, sizeof(vm));
}
