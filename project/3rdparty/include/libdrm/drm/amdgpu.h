#ifndef _AMDGPU_H_
#define _AMDGPU_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct drm_amdgpu_info_hw_ip;
struct drm_amdgpu_bo_list_entry;

#define AMDGPU_CS_MAX_IBS_PER_SUBMIT                    4
#define AMDGPU_TIMEOUT_INFINITE                         0xffffffffffffffffull
#define AMDGPU_QUERY_FENCE_TIMEOUT_IS_ABSOLUTE          (1 << 0)

enum amdgpu_bo_handle_type {
    amdgpu_bo_handle_type_gem_flink_name = 0,
    amdgpu_bo_handle_type_kms            = 1,
    amdgpu_bo_handle_type_dma_buf_fd     = 2,
    amdgpu_bo_handle_type_kms_noimport   = 3,
};

enum amdgpu_gpu_va_range {
    amdgpu_gpu_va_range_general = 0
};

enum amdgpu_sw_info {
    amdgpu_sw_info_address32_hi = 0,
};

typedef struct amdgpu_device *amdgpu_device_handle;
typedef struct amdgpu_context *amdgpu_context_handle;
typedef struct amdgpu_bo *amdgpu_bo_handle;
typedef struct amdgpu_bo_list *amdgpu_bo_list_handle;
typedef struct amdgpu_va *amdgpu_va_handle;
typedef struct amdgpu_semaphore *amdgpu_semaphore_handle;

struct amdgpu_bo_alloc_request {
    uint64_t alloc_size;
    uint64_t phys_alignment;
    uint32_t preferred_heap;
    uint64_t flags;
};

struct amdgpu_bo_metadata {
    uint64_t flags;
    uint64_t tiling_info;
    uint32_t size_metadata;
    uint32_t umd_metadata[64];
};

struct amdgpu_bo_info {
    uint64_t                  alloc_size;
    uint64_t                  phys_alignment;
    uint32_t                  preferred_heap;
    uint64_t                  alloc_flags;
    struct amdgpu_bo_metadata metadata;
};

struct amdgpu_bo_import_result {
    amdgpu_bo_handle buf_handle;
    uint64_t         alloc_size;
};

struct amdgpu_gds_resource_info {
    uint32_t gds_gfx_partition_size;
    uint32_t compute_partition_size;
    uint32_t gds_total_size;
    uint32_t gws_per_gfx_partition;
    uint32_t gws_per_compute_partition;
    uint32_t oa_per_gfx_partition;
    uint32_t oa_per_compute_partition;
};

struct amdgpu_cs_fence {
    amdgpu_context_handle context;
    uint32_t              ip_type;
    uint32_t              ip_instance;
    uint32_t              ring;
    uint64_t              fence;
};

struct amdgpu_cs_ib_info {
    uint64_t flags;
    uint64_t ib_mc_address;
    uint32_t size;
};

struct amdgpu_cs_fence_info {
    amdgpu_bo_handle handle;
    uint64_t         offset;
};

struct amdgpu_cs_request {
    uint64_t                    flags;
    unsigned                    ip_type;
    unsigned                    ip_instance;
    uint32_t                    ring;
    amdgpu_bo_list_handle       resources;
    uint32_t                    number_of_dependencies;
    struct amdgpu_cs_fence      *dependencies;
    uint32_t                    number_of_ibs;
    struct amdgpu_cs_ib_info    *ibs;
    uint64_t                    seq_no;
    struct amdgpu_cs_fence_info fence_info;
};

struct amdgpu_buffer_size_alignments {
    uint64_t size_local;
    uint64_t size_remote;
};

struct amdgpu_heap_info {
    uint64_t heap_size;
    uint64_t heap_usage;
    uint64_t max_allocation;
};

struct amdgpu_gpu_info {
    uint32_t asic_id;
    uint32_t chip_rev;
    uint32_t chip_external_rev;
    uint32_t family_id;
    uint64_t ids_flags;
    uint64_t max_engine_clk;
    uint64_t max_memory_clk;
    uint32_t num_shader_engines;
    uint32_t num_shader_arrays_per_engine;
    uint32_t avail_quad_shader_pipes;
    uint32_t max_quad_shader_pipes;
    uint32_t cache_entries_per_quad_pipe;
    uint32_t num_hw_gfx_contexts;
    uint32_t rb_pipes;
    uint32_t enabled_rb_pipes_mask;
    uint32_t gpu_counter_freq;
    uint32_t backend_disable[4];
    uint32_t mc_arb_ramcfg;
    uint32_t gb_addr_cfg;
    uint32_t gb_tile_mode[32];
    uint32_t gb_macro_tile_mode[16];
    uint32_t pa_sc_raster_cfg[4];
    uint32_t pa_sc_raster_cfg1[4];
    uint32_t cu_active_number;
    uint32_t cu_ao_mask;
    uint32_t cu_bitmap[4][4];
    uint32_t vram_type;
    uint32_t vram_bit_width;
    uint32_t ce_ram_size;
    uint32_t vce_harvest_config;
    uint32_t pci_rev_id;
};

int amdgpu_device_initialize(int fd, uint32_t *major_version, uint32_t *minor_version, amdgpu_device_handle *device_handle);
int amdgpu_device_deinitialize(amdgpu_device_handle device_handle);

int amdgpu_device_get_fd(amdgpu_device_handle device_handle);

int amdgpu_bo_alloc(amdgpu_device_handle dev, struct amdgpu_bo_alloc_request *alloc_buffer, amdgpu_bo_handle *buf_handle);

int amdgpu_bo_set_metadata(amdgpu_bo_handle buf_handle, struct amdgpu_bo_metadata *info);

int amdgpu_bo_query_info(amdgpu_bo_handle buf_handle, struct amdgpu_bo_info *info);

int amdgpu_bo_export(amdgpu_bo_handle buf_handle, enum amdgpu_bo_handle_type type, uint32_t *shared_handle);
int amdgpu_bo_import(amdgpu_device_handle dev, enum amdgpu_bo_handle_type type, uint32_t shared_handle, struct amdgpu_bo_import_result *output);

int amdgpu_create_bo_from_user_mem(amdgpu_device_handle dev, void *cpu, uint64_t size, amdgpu_bo_handle *buf_handle);
int amdgpu_find_bo_by_cpu_mapping(amdgpu_device_handle dev, void *cpu, uint64_t size, amdgpu_bo_handle *buf_handle, uint64_t *offset_in_bo);

int amdgpu_bo_free(amdgpu_bo_handle buf_handle);
void amdgpu_bo_inc_ref(amdgpu_bo_handle bo);

int amdgpu_bo_cpu_map(amdgpu_bo_handle buf_handle, void **cpu);
int amdgpu_bo_cpu_unmap(amdgpu_bo_handle buf_handle);

int amdgpu_bo_wait_for_idle(amdgpu_bo_handle buf_handle, uint64_t timeout_ns, bool *buffer_busy);
int amdgpu_bo_list_create_raw(amdgpu_device_handle dev, uint32_t number_of_buffers, struct drm_amdgpu_bo_list_entry *buffers, uint32_t *result);

int amdgpu_bo_list_destroy_raw(amdgpu_device_handle dev, uint32_t bo_list);
int amdgpu_bo_list_create(amdgpu_device_handle dev, uint32_t number_of_resources, amdgpu_bo_handle *resources, uint8_t *resource_prios, amdgpu_bo_list_handle *result);

int amdgpu_bo_list_destroy(amdgpu_bo_list_handle handle);
int amdgpu_bo_list_update(amdgpu_bo_list_handle handle, uint32_t number_of_resources, amdgpu_bo_handle *resources, uint8_t *resource_prios);

int amdgpu_cs_ctx_create2(amdgpu_device_handle dev, uint32_t priority, amdgpu_context_handle *context);
int amdgpu_cs_ctx_create(amdgpu_device_handle dev, amdgpu_context_handle *context);

int amdgpu_cs_ctx_free(amdgpu_context_handle context);
int amdgpu_cs_ctx_override_priority(amdgpu_device_handle dev, amdgpu_context_handle context, int master_fd, unsigned priority);

int amdgpu_cs_ctx_stable_pstate(amdgpu_context_handle context, uint32_t op, uint32_t flags, uint32_t *out_flags);

int amdgpu_cs_query_reset_state(amdgpu_context_handle context, uint32_t *state, uint32_t *hangs);
int amdgpu_cs_query_reset_state2(amdgpu_context_handle context, uint64_t *flags);

int amdgpu_cs_submit(amdgpu_context_handle context, uint64_t flags, struct amdgpu_cs_request *ibs_request, uint32_t number_of_requests);

int amdgpu_cs_query_fence_status(struct amdgpu_cs_fence *fence, uint64_t timeout_ns, uint64_t flags, uint32_t *expired);
int amdgpu_cs_wait_fences(struct amdgpu_cs_fence *fences, uint32_t fence_count, bool wait_all, uint64_t timeout_ns, uint32_t *status, uint32_t *first);

int amdgpu_query_buffer_size_alignment(amdgpu_device_handle dev, struct amdgpu_buffer_size_alignments *info);
int amdgpu_query_firmware_version(amdgpu_device_handle dev, unsigned fw_type, unsigned ip_instance, unsigned index, uint32_t *version, uint32_t *feature);

int amdgpu_query_hw_ip_count(amdgpu_device_handle dev, unsigned type, uint32_t *count);
int amdgpu_query_hw_ip_info(amdgpu_device_handle dev, unsigned type, unsigned ip_instance, struct drm_amdgpu_info_hw_ip *info);

int amdgpu_query_heap_info(amdgpu_device_handle dev, uint32_t heap, uint32_t flags, struct amdgpu_heap_info *info);
int amdgpu_query_crtc_from_id(amdgpu_device_handle dev, unsigned id, int32_t *result);

int amdgpu_query_gpu_info(amdgpu_device_handle dev, struct amdgpu_gpu_info *info);
int amdgpu_query_info(amdgpu_device_handle dev, unsigned info_id, unsigned size, void *value);

int amdgpu_query_sw_info(amdgpu_device_handle dev, enum amdgpu_sw_info info, void *value);
int amdgpu_query_gds_info(amdgpu_device_handle dev, struct amdgpu_gds_resource_info *gds_info);

int amdgpu_query_sensor_info(amdgpu_device_handle dev, unsigned sensor_type, unsigned size, void *value);
int amdgpu_query_video_caps_info(amdgpu_device_handle dev, unsigned cap_type, unsigned size, void *value);

int amdgpu_read_mm_registers(amdgpu_device_handle dev, unsigned dword_offset, unsigned count, uint32_t instance, uint32_t flags, uint32_t *values);

#define AMDGPU_VA_RANGE_32_BIT                          0x1
#define AMDGPU_VA_RANGE_HIGH                            0x2
#define AMDGPU_VA_RANGE_REPLAYABLE                      0x4

int amdgpu_va_range_alloc(amdgpu_device_handle dev, enum amdgpu_gpu_va_range va_range_type, uint64_t size, uint64_t va_base_alignment, uint64_t va_base_required, uint64_t *va_base_allocated, amdgpu_va_handle *va_range_handle, uint64_t flags);
int amdgpu_va_range_free(amdgpu_va_handle va_range_handle);

int amdgpu_va_range_query(amdgpu_device_handle dev, enum amdgpu_gpu_va_range type, uint64_t *start, uint64_t *end);

int amdgpu_bo_va_op(amdgpu_bo_handle bo, uint64_t offset, uint64_t size, uint64_t addr, uint64_t flags, uint32_t ops);
int amdgpu_bo_va_op_raw(amdgpu_device_handle dev, amdgpu_bo_handle bo, uint64_t offset, uint64_t size, uint64_t addr, uint64_t flags, uint32_t ops);

int amdgpu_cs_create_semaphore(amdgpu_semaphore_handle *sem);
int amdgpu_cs_signal_semaphore(amdgpu_context_handle ctx, uint32_t ip_type, uint32_t ip_instance, uint32_t ring, amdgpu_semaphore_handle sem);
int amdgpu_cs_wait_semaphore(amdgpu_context_handle ctx, uint32_t ip_type, uint32_t ip_instance, uint32_t ring, amdgpu_semaphore_handle sem);

int amdgpu_cs_destroy_semaphore(amdgpu_semaphore_handle sem);

const char *amdgpu_get_marketing_name(amdgpu_device_handle dev);

int amdgpu_cs_create_syncobj2(amdgpu_device_handle dev, uint32_t  flags, uint32_t *syncobj);
int amdgpu_cs_create_syncobj(amdgpu_device_handle dev, uint32_t *syncobj);

int amdgpu_cs_destroy_syncobj(amdgpu_device_handle dev, uint32_t syncobj);
int amdgpu_cs_syncobj_reset(amdgpu_device_handle dev, const uint32_t *syncobjs, uint32_t syncobj_count);

int amdgpu_cs_syncobj_signal(amdgpu_device_handle dev, const uint32_t *syncobjs, uint32_t syncobj_count);
int amdgpu_cs_syncobj_timeline_signal(amdgpu_device_handle dev, const uint32_t *syncobjs, uint64_t *points, uint32_t syncobj_count);

int amdgpu_cs_syncobj_wait(amdgpu_device_handle dev, uint32_t *handles, unsigned num_handles, int64_t timeout_nsec, unsigned flags, uint32_t *first_signaled);
int amdgpu_cs_syncobj_timeline_wait(amdgpu_device_handle dev, uint32_t *handles, uint64_t *points, unsigned num_handles, int64_t timeout_nsec, unsigned flags, uint32_t *first_signaled);

int amdgpu_cs_syncobj_query(amdgpu_device_handle dev, uint32_t *handles, uint64_t *points, unsigned num_handles);
int amdgpu_cs_syncobj_query2(amdgpu_device_handle dev, uint32_t *handles, uint64_t *points, unsigned num_handles, uint32_t flags);

int amdgpu_cs_export_syncobj(amdgpu_device_handle dev, uint32_t syncobj, int *shared_fd);
int amdgpu_cs_import_syncobj(amdgpu_device_handle dev, int shared_fd, uint32_t *syncobj);

int amdgpu_cs_syncobj_export_sync_file(amdgpu_device_handle dev, uint32_t syncobj, int *sync_file_fd);
int amdgpu_cs_syncobj_import_sync_file(amdgpu_device_handle dev, uint32_t syncobj, int sync_file_fd);

int amdgpu_cs_syncobj_export_sync_file2(amdgpu_device_handle dev, uint32_t syncobj, uint64_t point, uint32_t flags, int *sync_file_fd);
int amdgpu_cs_syncobj_import_sync_file2(amdgpu_device_handle dev, uint32_t syncobj, uint64_t point, int sync_file_fd);

int amdgpu_cs_syncobj_transfer(amdgpu_device_handle dev, uint32_t dst_handle, uint64_t dst_point, uint32_t src_handle, uint64_t src_point, uint32_t flags);
int amdgpu_cs_fence_to_handle(amdgpu_device_handle dev, struct amdgpu_cs_fence *fence, uint32_t what, uint32_t *out_handle);

struct drm_amdgpu_cs_chunk;
struct drm_amdgpu_cs_chunk_dep;
struct drm_amdgpu_cs_chunk_data;

int amdgpu_cs_submit_raw(amdgpu_device_handle dev, amdgpu_context_handle context, amdgpu_bo_list_handle bo_list_handle, int num_chunks, struct drm_amdgpu_cs_chunk *chunks, uint64_t *seq_no);
int amdgpu_cs_submit_raw2(amdgpu_device_handle dev, amdgpu_context_handle context, uint32_t bo_list_handle, int num_chunks, struct drm_amdgpu_cs_chunk *chunks, uint64_t *seq_no);

void amdgpu_cs_chunk_fence_to_dep(struct amdgpu_cs_fence *fence, struct drm_amdgpu_cs_chunk_dep *dep);
void amdgpu_cs_chunk_fence_info_to_data(struct amdgpu_cs_fence_info *fence_info, struct drm_amdgpu_cs_chunk_data *data);

int amdgpu_vm_reserve_vmid(amdgpu_device_handle dev, uint32_t flags);
int amdgpu_vm_unreserve_vmid(amdgpu_device_handle dev, uint32_t flags);

#ifdef __cplusplus
}
#endif

#endif
