/*
 *  Flo's Open libRary (floor)
 *  Copyright (C) 2004 - 2015 Florian Ziesche
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License only.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __FLOOR_CUDA_API_HPP__
#define __FLOOR_CUDA_API_HPP__

#include <floor/core/essentials.hpp>
#include <floor/math/vector_lib.hpp>

enum class CU_RESULT : uint32_t {
	SUCCESS = 0,
	INVALID_VALUE = 1,
	OUT_OF_MEMORY = 2,
	NOT_INITIALIZED = 3,
	DEINITIALIZED = 4,
	PROFILER_DISABLED = 5,
	PROFILER_NOT_INITIALIZED = 6,
	PROFILER_ALREADY_STARTED = 7,
	PROFILER_ALREADY_STOPPED = 8,
	NO_DEVICE = 100,
	INVALID_DEVICE = 101,
	INVALID_IMAGE = 200,
	INVALID_CONTEXT = 201,
	CONTEXT_ALREADY_CURRENT = 202,
	ERROR_MAP_FAILED = 205,
	UNMAP_FAILED = 206,
	ARRAY_IS_MAPPED = 207,
	ALREADY_MAPPED = 208,
	NO_BINARY_FOR_GPU = 209,
	ALREADY_ACQUIRED = 210,
	NOT_MAPPED = 211,
	NOT_MAPPED_AS_ARRAY = 212,
	NOT_MAPPED_AS_POINTER = 213,
	ECC_UNCORRECTABLE = 214,
	UNSUPPORTED_LIMIT = 215,
	CONTEXT_ALREADY_IN_USE = 216,
	PEER_ACCESS_UNSUPPORTED = 217,
	INVALID_PTX = 218,
	INVALID_GRAPHICS_CONTEXT = 219,
	INVALID_SOURCE = 300,
	FILE_NOT_FOUND = 301,
	SHARED_OBJECT_SYMBOL_NOT_FOUND = 302,
	SHARED_OBJECT_INIT_FAILED = 303,
	OPERATING_SYSTEM = 304,
	INVALID_HANDLE = 400,
	NOT_FOUND = 500,
	NOT_READY = 600,
	ILLEGAL_ADDRESS = 700,
	LAUNCH_OUT_OF_RESOURCES = 701,
	LAUNCH_TIMEOUT = 702,
	LAUNCH_INCOMPATIBLE_TEXTURING = 703,
	PEER_ACCESS_ALREADY_ENABLED = 704,
	PEER_ACCESS_NOT_ENABLED = 705,
	PRIMARY_CONTEXT_ACTIVE = 708,
	CONTEXT_IS_DESTROYED = 709,
	ASSERT = 710,
	TOO_MANY_PEERS = 711,
	HOST_MEMORY_ALREADY_REGISTERED = 712,
	HOST_MEMORY_NOT_REGISTERED = 713,
	HARDWARE_STACK_ERROR = 714,
	ILLEGAL_INSTRUCTION = 715,
	MISALIGNED_ADDRESS = 716,
	INVALID_ADDRESS_SPACE = 717,
	INVALID_PC = 718,
	LAUNCH_FAILED = 719,
	NOT_PERMITTED = 800,
	NOT_SUPPORTED = 801,
	UNKNOWN = 999
};
enum class CU_DEVICE_ATTRIBUTE : uint32_t {
	MAX_THREADS_PER_BLOCK = 1,
	MAX_BLOCK_DIM_X = 2,
	MAX_BLOCK_DIM_Y = 3,
	MAX_BLOCK_DIM_Z = 4,
	MAX_GRID_DIM_X = 5,
	MAX_GRID_DIM_Y = 6,
	MAX_GRID_DIM_Z = 7,
	MAX_SHARED_MEMORY_PER_BLOCK = 8,
	SHARED_MEMORY_PER_BLOCK = 8,
	TOTAL_CONSTANT_MEMORY = 9,
	WARP_SIZE = 10,
	MAX_PITCH = 11,
	MAX_REGISTERS_PER_BLOCK = 12,
	REGISTERS_PER_BLOCK = 12,
	CLOCK_RATE = 13,
	TEXTURE_ALIGNMENT = 14,
	GPU_OVERLAP = 15,
	MULTIPROCESSOR_COUNT = 16,
	KERNEL_EXEC_TIMEOUT = 17,
	INTEGRATED = 18,
	CAN_MAP_HOST_MEMORY = 19,
	COMPUTE_MODE = 20,
	MAXIMUM_TEXTURE1D_WIDTH = 21,
	MAXIMUM_TEXTURE2D_WIDTH = 22,
	MAXIMUM_TEXTURE2D_HEIGHT = 23,
	MAXIMUM_TEXTURE3D_WIDTH = 24,
	MAXIMUM_TEXTURE3D_HEIGHT = 25,
	MAXIMUM_TEXTURE3D_DEPTH = 26,
	MAXIMUM_TEXTURE2D_LAYERED_WIDTH = 27,
	MAXIMUM_TEXTURE2D_LAYERED_HEIGHT = 28,
	MAXIMUM_TEXTURE2D_LAYERED_LAYERS = 29,
	MAXIMUM_TEXTURE2D_ARRAY_WIDTH = 27,
	MAXIMUM_TEXTURE2D_ARRAY_HEIGHT = 28,
	MAXIMUM_TEXTURE2D_ARRAY_NUMSLICES = 29,
	SURFACE_ALIGNMENT = 30,
	CONCURRENT_KERNELS = 31,
	ECC_ENABLED = 32,
	PCI_BUS_ID = 33,
	PCI_DEVICE_ID = 34,
	TCC_DRIVER = 35,
	MEMORY_CLOCK_RATE = 36,
	GLOBAL_MEMORY_BUS_WIDTH = 37,
	L2_CACHE_SIZE = 38,
	MAX_THREADS_PER_MULTIPROCESSOR = 39,
	ASYNC_ENGINE_COUNT = 40,
	UNIFIED_ADDRESSING = 41,
	MAXIMUM_TEXTURE1D_LAYERED_WIDTH = 42,
	MAXIMUM_TEXTURE1D_LAYERED_LAYERS = 43,
	CAN_TEX2D_GATHER = 44,
	MAXIMUM_TEXTURE2D_GATHER_WIDTH = 45,
	MAXIMUM_TEXTURE2D_GATHER_HEIGHT = 46,
	MAXIMUM_TEXTURE3D_WIDTH_ALTERNATE = 47,
	MAXIMUM_TEXTURE3D_HEIGHT_ALTERNATE = 48,
	MAXIMUM_TEXTURE3D_DEPTH_ALTERNATE = 49,
	PCI_DOMAIN_ID = 50,
	TEXTURE_PITCH_ALIGNMENT = 51,
	MAXIMUM_TEXTURECUBEMAP_WIDTH = 52,
	MAXIMUM_TEXTURECUBEMAP_LAYERED_WIDTH = 53,
	MAXIMUM_TEXTURECUBEMAP_LAYERED_LAYERS = 54,
	MAXIMUM_SURFACE1D_WIDTH = 55,
	MAXIMUM_SURFACE2D_WIDTH = 56,
	MAXIMUM_SURFACE2D_HEIGHT = 57,
	MAXIMUM_SURFACE3D_WIDTH = 58,
	MAXIMUM_SURFACE3D_HEIGHT = 59,
	MAXIMUM_SURFACE3D_DEPTH = 60,
	MAXIMUM_SURFACE1D_LAYERED_WIDTH = 61,
	MAXIMUM_SURFACE1D_LAYERED_LAYERS = 62,
	MAXIMUM_SURFACE2D_LAYERED_WIDTH = 63,
	MAXIMUM_SURFACE2D_LAYERED_HEIGHT = 64,
	MAXIMUM_SURFACE2D_LAYERED_LAYERS = 65,
	MAXIMUM_SURFACECUBEMAP_WIDTH = 66,
	MAXIMUM_SURFACECUBEMAP_LAYERED_WIDTH = 67,
	MAXIMUM_SURFACECUBEMAP_LAYERED_LAYERS = 68,
	MAXIMUM_TEXTURE1D_LINEAR_WIDTH = 69,
	MAXIMUM_TEXTURE2D_LINEAR_WIDTH = 70,
	MAXIMUM_TEXTURE2D_LINEAR_HEIGHT = 71,
	MAXIMUM_TEXTURE2D_LINEAR_PITCH = 72,
	MAXIMUM_TEXTURE2D_MIPMAPPED_WIDTH = 73,
	MAXIMUM_TEXTURE2D_MIPMAPPED_HEIGHT = 74,
	COMPUTE_CAPABILITY_MAJOR = 75,
	COMPUTE_CAPABILITY_MINOR = 76,
	MAXIMUM_TEXTURE1D_MIPMAPPED_WIDTH = 77,
	STREAM_PRIORITIES_SUPPORTED = 78,
	GLOBAL_L1_CACHE_SUPPORTED = 79,
	LOCAL_L1_CACHE_SUPPORTED = 80,
	MAX_SHARED_MEMORY_PER_MULTIPROCESSOR = 81,
	MAX_REGISTERS_PER_MULTIPROCESSOR = 82,
	MANAGED_MEMORY = 83,
	MULTI_GPU_BOARD = 84,
	MULTI_GPU_BOARD_GROUP_ID = 85
};
enum class CU_JIT_OPTION : uint32_t {
	MAX_REGISTERS = 0,
	THREADS_PER_BLOCK,
	WALL_TIME,
	INFO_LOG_BUFFER,
	INFO_LOG_BUFFER_SIZE_BYTES,
	ERROR_LOG_BUFFER,
	ERROR_LOG_BUFFER_SIZE_BYTES,
	OPTIMIZATION_LEVEL,
	TARGET_FROM_CUCONTEXT,
	TARGET,
	FALLBACK_STRATEGY,
	GENERATE_DEBUG_INFO,
	LOG_VERBOSE,
	GENERATE_LINE_INFO,
	CACHE_MODE
};
enum class CU_JIT_INPUT_TYPE : uint32_t {
	CUBIN = 0,
	PTX,
	FATBINARY,
	OBJECT,
	LIBRARY
};
enum class CU_LIMIT : uint32_t {
	STACK_SIZE = 0,
	PRINTF_FIFO_SIZE = 1,
	MALLOC_HEAP_SIZE = 2,
	DEV_RUNTIME_SYNC_DEPTH = 3,
	DEV_RUNTIME_PENDING_LAUNCH_COUNT = 4
};

enum class CU_ARRAY_FORMAT : uint32_t {
	UNSIGNED_INT8 = 0x01,
	UNSIGNED_INT16 = 0x02,
	UNSIGNED_INT32 = 0x03,
	SIGNED_INT8 = 0x08,
	SIGNED_INT16 = 0x09,
	SIGNED_INT32 = 0x0a,
	HALF = 0x10,
	FLOAT = 0x20,
};

enum class CU_MEMORY_TYPE : uint32_t {
	HOST = 1,
	DEVICE = 2,
	ARRAY = 3,
	UNIFIED = 4
};

enum class CU_ADDRESS_MODE : uint32_t {
	WRAP = 0,
	CLAMP = 1,
	MIRROR = 2,
	BORDER = 3
};

enum class CU_FILTER_MODE : uint32_t {
	NEAREST = 0,
	LINEAR = 1
};

enum class CU_RESOURCE_TYPE : uint32_t {
	ARRAY = 0,
	MIP_MAPPED_ARRAY = 1,
	LINEAR = 2,
	PITCH_2D = 3
};

enum class CU_RESOURCE_VIEW_FORMAT : uint32_t {
	NONE = 0,
	UINT_1X8 = 1,
	UINT_2X8 = 2,
	UINT_4X8 = 3,
	SINT_1X8 = 4,
	SINT_2X8 = 5,
	SINT_4X8 = 6,
	UINT_1X16 = 7,
	UINT_2X16 = 8,
	UINT_4X16 = 9,
	SINT_1X16 = 10,
	SINT_2X16 = 11,
	SINT_4X16 = 12,
	UINT_1X32 = 13,
	UINT_2X32 = 14,
	UINT_4X32 = 15,
	SINT_1X32 = 16,
	SINT_2X32 = 17,
	SINT_4X32 = 18,
	FLOAT_1X16 = 19,
	FLOAT_2X16 = 20,
	FLOAT_4X16 = 21,
	FLOAT_1X32 = 22,
	FLOAT_2X32 = 23,
	FLOAT_4X32 = 24,
	UNSIGNED_BC1 = 25,
	UNSIGNED_BC2 = 26,
	UNSIGNED_BC3 = 27,
	UNSIGNED_BC4 = 28,
	SIGNED_BC4 = 29,
	UNSIGNED_BC5 = 30,
	SIGNED_BC5 = 31,
	UNSIGNED_BC6H = 32,
	SIGNED_BC6H = 33,
	UNSIGNED_BC7 = 34
};

enum class CU_MEM_HOST_ALLOC : uint32_t {
	PORTABLE = 1,
	DEVICE_MAP = 2,
	WRITE_COMBINED = 4
};
floor_global_enum_ext(CU_MEM_HOST_ALLOC)

enum class CU_MEM_HOST_REGISTER : uint32_t {
	PORTABLE = 1,
	DEVICE_MAP = 2,
	IO_MEMORY = 4
};
floor_global_enum_ext(CU_MEM_HOST_REGISTER)

enum class CU_GRAPHICS_REGISTER_FLAGS : uint32_t {
	NONE = 0,
	READ_ONLY = 1,
	WRITE_DISCARD = 2,
	SURFACE_LOAD_STORE = 4,
	TEXTURE_GATHER = 8
};
floor_global_enum_ext(CU_GRAPHICS_REGISTER_FLAGS)

enum class CU_CONTEXT_FLAGS : uint32_t {
	SCHEDULE_AUTO = 0,
	SCHEDULE_SPIN = 1,
	SCHEDULE_YIELD = 2,
	SCHEDULE_BLOCKING_SYNC = 4,
	MAP_HOST = 8,
	LMEM_RESIZE_TO_MAX = 16,
};
floor_global_enum_ext(CU_CONTEXT_FLAGS)

enum class CU_STREAM_FLAGS : uint32_t {
	DEFAULT = 0,
	NON_BLOCKING = 1
};

enum class CUDA_ARRAY_3D_FLAGS : uint32_t {
	NONE = 0,
	LAYERED = 1,
	SURFACE_LOAD_STORE = 2,
	CUBE_MAP = 4,
	TEXTURE_GATHER = 8,
	DEPTH_TEXTURE = 16
};
floor_global_enum_ext(CUDA_ARRAY_3D_FLAGS)

// these are all external opaque types
typedef struct _cu_array* cu_array;
typedef struct _cu_mip_mapped_array* cu_mip_mapped_array;
typedef struct _cu_context* cu_context;
typedef struct _cu_stream* cu_stream;
typedef struct _cu_module* cu_module;
typedef struct _cu_function* cu_function;
typedef struct _cu_graphics_resource* cu_graphics_resource;
typedef struct _cu_link_state* cu_link_state;

typedef int32_t cu_device;
typedef size_t cu_device_ptr;
typedef uint64_t cu_surf_object;
typedef uint64_t cu_tex_object;
typedef size_t (*cu_occupancy_b2d_size)(int block_size);

// structs that can actually be filled by the user
struct cu_array_3d_descriptor {
	size3 dim;
	CU_ARRAY_FORMAT format;
	uint32_t channel_count;
	CUDA_ARRAY_3D_FLAGS flags;
};

struct cu_memcpy_3d_descriptor {
	struct info {
		size_t x_in_bytes;
		size_t y;
		size_t z;
		size_t lod;
		CU_MEMORY_TYPE memory_type;
		const void* host_ptr;
		cu_device_ptr device_ptr;
		cu_array array;
		void* _reserved;
		size_t pitch;
		size_t height;
	};
	info src;
	info dst;
	
	size_t width_in_bytes;
	size_t height;
	size_t depth;
};

struct cu_resource_descriptor {
	CU_RESOURCE_TYPE type;
	
	union {
		struct {
			cu_array array;
		};
		struct {
			cu_mip_mapped_array mip_mapped_array;
		};
		struct {
			cu_device_ptr device_ptr;
			CU_ARRAY_FORMAT format;
			uint32_t channel_count;
			size_t size_in_bytes;
		} linear;
		struct {
			cu_device_ptr device_ptr;
			CU_ARRAY_FORMAT format;
			uint32_t channel_count;
			size_t width;
			size_t height;
			size_t pitch_in_bytes;
		} pitch_2d;
		struct {
			int _reserved[32];
		};
	};
	
	uint32_t _flags { 0 }; // must always be zero
};

struct cu_resource_view_descriptor {
	CU_RESOURCE_VIEW_FORMAT format;
	size3 dim;
	uint32_t first_mip_map_level;
	uint32_t last_mip_map_level;
	uint32_t first_layer;
	uint32_t last_layer;
	uint32_t _reserved[16];
};

struct cu_texture_descriptor {
	CU_ADDRESS_MODE address_mode[3];
	CU_FILTER_MODE filter_mode;
	uint32_t flags;
	uint32_t max_anisotropy;
	CU_FILTER_MODE mip_map_filter_mode;
	float mip_map_level_bias;
	float min_mip_map_level_clamp;
	float max_mip_map_level_clamp;
	int _reserved[16];
};

// actual cuda api function pointers
struct cuda_api_ptrs {
	CU_RESULT (*array_3d_create)(cu_array* p_handle, const cu_array_3d_descriptor* p_allocate_array);
	CU_RESULT (*array_destroy)(cu_array h_array);
	CU_RESULT (*ctx_create)(cu_context* pctx, CU_CONTEXT_FLAGS flags, cu_device dev);
	CU_RESULT (*ctx_get_limit)(size_t* pvalue, CU_LIMIT limit);
	CU_RESULT (*ctx_set_current)(cu_context ctx);
	CU_RESULT (*device_compute_capability)(int32_t* major, int32_t* minor, cu_device dev);
	CU_RESULT (*device_get)(cu_device* device, int32_t ordinal);
	CU_RESULT (*device_get_attribute)(int32_t* pi, CU_DEVICE_ATTRIBUTE attrib, cu_device dev);
	CU_RESULT (*device_get_count)(int32_t* count);
	CU_RESULT (*device_get_name)(char* name, int32_t len, cu_device dev);
	CU_RESULT (*device_total_mem)(size_t* bytes, cu_device dev);
	CU_RESULT (*driver_get_version)(int32_t* driver_version);
	CU_RESULT (*get_error_name)(CU_RESULT error, const char** p_str);
	CU_RESULT (*get_error_string)(CU_RESULT error, const char** p_str);
	CU_RESULT (*graphics_gl_register_buffer)(cu_graphics_resource* p_cuda_resource, GLuint buffer, CU_GRAPHICS_REGISTER_FLAGS flags);
	CU_RESULT (*graphics_gl_register_image)(cu_graphics_resource* p_cuda_resource, GLuint image, GLenum target, CU_GRAPHICS_REGISTER_FLAGS flags);
	CU_RESULT (*graphics_map_resources)(uint32_t count, cu_graphics_resource* resources, cu_stream h_stream);
	CU_RESULT (*graphics_resource_get_mapped_pointer)(cu_device_ptr* p_dev_ptr, size_t* p_size, cu_graphics_resource resource);
	CU_RESULT (*graphics_sub_resource_get_mapped_array)(cu_array* p_array, cu_graphics_resource resource, uint32_t array_index, uint32_t mip_level);
	CU_RESULT (*graphics_unmap_resources)(uint32_t count, cu_graphics_resource* resources, cu_stream h_stream);
	CU_RESULT (*init)(uint32_t flags);
	CU_RESULT (*launch_kernel)(cu_function f, uint32_t grid_dim_x, uint32_t grid_dim_y, uint32_t grid_dim_z, uint32_t block_dim_x, uint32_t block_dim_y, uint32_t block_dim_z, uint32_t shared_mem_bytes, cu_stream h_stream, void** kernel_params, void** extra);
	CU_RESULT (*link_add_data)(cu_link_state state, CU_JIT_INPUT_TYPE type, void* data, size_t size, const char* name, uint32_t num_options, CU_JIT_OPTION* options, void** option_values);
	CU_RESULT (*link_complete)(cu_link_state state, void** cubin_out, size_t* size_out);
	CU_RESULT (*link_create)(uint32_t num_options, CU_JIT_OPTION* options, void** option_values, cu_link_state* state_out);
	CU_RESULT (*link_destroy)(cu_link_state state);
	CU_RESULT (*mem_alloc)(cu_device_ptr* dptr, size_t bytesize);
	CU_RESULT (*mem_free)(cu_device_ptr dptr);
	CU_RESULT (*mem_host_get_device_pointer)(cu_device_ptr* pdptr, void* p, uint32_t flags);
	CU_RESULT (*mem_host_register)(void* p, size_t bytesize, CU_MEM_HOST_REGISTER flags);
	CU_RESULT (*mem_host_unregister)(void* p);
	CU_RESULT (*memcpy_3d)(const cu_memcpy_3d_descriptor* p_copy);
	CU_RESULT (*memcpy_3d_async)(const cu_memcpy_3d_descriptor* p_copy, cu_stream h_stream);
	CU_RESULT (*memcpy_dtod)(cu_device_ptr dst_device, cu_device_ptr src_device, size_t byte_count);
	CU_RESULT (*memcpy_dtod_async)(cu_device_ptr dst_device, cu_device_ptr src_device, size_t byte_count, cu_stream h_stream);
	CU_RESULT (*memcpy_dtoh)(void* dst_host, cu_device_ptr src_device, size_t byte_count);
	CU_RESULT (*memcpy_dtoh_async)(void* dst_host, cu_device_ptr src_device, size_t byte_count, cu_stream h_stream);
	CU_RESULT (*memcpy_htod)(cu_device_ptr dst_device, const void* src_host, size_t byte_count);
	CU_RESULT (*memcpy_htod_async)(cu_device_ptr dst_device, const void* src_host, size_t byte_count, cu_stream h_stream);
	CU_RESULT (*memset_d16_async)(cu_device_ptr dst_device, uint16_t us, size_t n, cu_stream h_stream);
	CU_RESULT (*memset_d32_async)(cu_device_ptr dst_device, uint32_t ui, size_t n, cu_stream h_stream);
	CU_RESULT (*memset_d8_async)(cu_device_ptr dst_device, unsigned char uc, size_t n, cu_stream h_stream);
	CU_RESULT (*module_get_function)(cu_function* hfunc, cu_module hmod, const char* name);
	CU_RESULT (*module_load_data)(cu_module* module, const void* image);
	CU_RESULT (*module_load_data_ex)(cu_module* module, const void* image, uint32_t num_options, CU_JIT_OPTION* options, void** option_values);
	CU_RESULT (*occupancy_max_potential_block_size)(int32_t* min_grid_size, int32_t* block_size, cu_function func, cu_occupancy_b2d_size block_size_to_dynamic_s_mem_size, size_t dynamic_s_mem_size, int32_t block_size_limit);
	CU_RESULT (*stream_create)(cu_stream* ph_stream, CU_STREAM_FLAGS flags);
	CU_RESULT (*stream_synchronize)(cu_stream h_stream);
	CU_RESULT (*surf_object_create)(cu_surf_object* p_surf_object, const cu_resource_descriptor* p_res_desc);
	CU_RESULT (*surf_object_destroy)(cu_surf_object surf_object);
	CU_RESULT (*tex_object_create)(cu_tex_object* p_tex_object, const cu_resource_descriptor* p_res_desc, const cu_texture_descriptor* p_tex_desc, const cu_resource_view_descriptor* p_res_view_desc);
	CU_RESULT (*tex_object_destroy)(cu_tex_object tex_object);
};
extern cuda_api_ptrs cuda_api;
extern bool cuda_api_init();

#define cu_array_3d_create cuda_api.array_3d_create
#define cu_array_destroy cuda_api.array_destroy
#define cu_ctx_create cuda_api.ctx_create
#define cu_ctx_get_limit cuda_api.ctx_get_limit
#define cu_ctx_set_current cuda_api.ctx_set_current
#define cu_device_compute_capability cuda_api.device_compute_capability
#define cu_device_get cuda_api.device_get
#define cu_device_get_attribute cuda_api.device_get_attribute
#define cu_device_get_count cuda_api.device_get_count
#define cu_device_get_name cuda_api.device_get_name
#define cu_device_total_mem cuda_api.device_total_mem
#define cu_driver_get_version cuda_api.driver_get_version
#define cu_get_error_name cuda_api.get_error_name
#define cu_get_error_string cuda_api.get_error_string
#define cu_graphics_gl_register_buffer cuda_api.graphics_gl_register_buffer
#define cu_graphics_gl_register_image cuda_api.graphics_gl_register_image
#define cu_graphics_map_resources cuda_api.graphics_map_resources
#define cu_graphics_resource_get_mapped_pointer cuda_api.graphics_resource_get_mapped_pointer
#define cu_graphics_sub_resource_get_mapped_array cuda_api.graphics_sub_resource_get_mapped_array
#define cu_graphics_unmap_resources cuda_api.graphics_unmap_resources
#define cu_init cuda_api.init
#define cu_launch_kernel cuda_api.launch_kernel
#define cu_link_add_data cuda_api.link_add_data
#define cu_link_complete cuda_api.link_complete
#define cu_link_create cuda_api.link_create
#define cu_link_destroy cuda_api.link_destroy
#define cu_mem_alloc cuda_api.mem_alloc
#define cu_mem_free cuda_api.mem_free
#define cu_mem_host_get_device_pointer cuda_api.mem_host_get_device_pointer
#define cu_mem_host_register cuda_api.mem_host_register
#define cu_mem_host_unregister cuda_api.mem_host_unregister
#define cu_memcpy_3d cuda_api.memcpy_3d
#define cu_memcpy_3d_async cuda_api.memcpy_3d_async
#define cu_memcpy_dtod cuda_api.memcpy_dtod
#define cu_memcpy_dtod_async cuda_api.memcpy_dtod_async
#define cu_memcpy_dtoh cuda_api.memcpy_dtoh
#define cu_memcpy_dtoh_async cuda_api.memcpy_dtoh_async
#define cu_memcpy_htod cuda_api.memcpy_htod
#define cu_memcpy_htod_async cuda_api.memcpy_htod_async
#define cu_memset_d16_async cuda_api.memset_d16_async
#define cu_memset_d32_async cuda_api.memset_d32_async
#define cu_memset_d8_async cuda_api.memset_d8_async
#define cu_module_get_function cuda_api.module_get_function
#define cu_module_load_data cuda_api.module_load_data
#define cu_module_load_data_ex cuda_api.module_load_data_ex
#define cu_occupancy_max_potential_block_size cuda_api.occupancy_max_potential_block_size
#define cu_stream_create cuda_api.stream_create
#define cu_stream_synchronize cuda_api.stream_synchronize
#define cu_surf_object_create cuda_api.surf_object_create
#define cu_surf_object_destroy cuda_api.surf_object_destroy
#define cu_tex_object_create cuda_api.tex_object_create
#define cu_tex_object_destroy cuda_api.tex_object_destroy

#endif