/*
 *  Flo's Open libRary (floor)
 *  Copyright (C) 2004 - 2018 Florian Ziesche
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

#include <floor/compute/metal/metal_buffer.hpp>

#if !defined(FLOOR_NO_METAL)

#include <floor/core/logger.hpp>
#include <floor/compute/metal/metal_queue.hpp>
#include <floor/compute/metal/metal_device.hpp>
#include <floor/compute/metal/metal_compute.hpp>
#include <floor/darwin/darwin_helper.hpp>

// TODO: proper error (return) value handling everywhere

metal_buffer::metal_buffer(const bool is_staging_buffer_,
						   metal_device* device,
						   const size_t& size_,
						   void* host_ptr_,
						   const COMPUTE_MEMORY_FLAG flags_,
						   const uint32_t opengl_type_,
						   const uint32_t external_gl_object_) :
compute_buffer(device, size_, host_ptr_, flags_, opengl_type_, external_gl_object_), is_staging_buffer(is_staging_buffer_) {
	if(size < min_multiple()) return;
	
	// no special COMPUTE_MEMORY_FLAG::READ_WRITE handling for metal, buffers are always read/write
	
	switch(flags & COMPUTE_MEMORY_FLAG::HOST_READ_WRITE) {
		case COMPUTE_MEMORY_FLAG::HOST_READ:
		case COMPUTE_MEMORY_FLAG::HOST_READ_WRITE:
			// keep the default MTLCPUCacheModeDefaultCache
			break;
		case COMPUTE_MEMORY_FLAG::NONE:
		case COMPUTE_MEMORY_FLAG::HOST_WRITE:
			// host will only write or not read/write at all -> can use write combined
			options = MTLCPUCacheModeWriteCombined;
			break;
		// all possible cases handled
		default: floor_unreachable();
	}
	
	if((flags & COMPUTE_MEMORY_FLAG::HOST_READ_WRITE) == COMPUTE_MEMORY_FLAG::NONE) {
		// if buffer is not accessed by the host at all, use private storage
		// note that this disables pretty much all functionality of this class!
		options |= MTLResourceStorageModePrivate;
	}
	else {
#if !defined(FLOOR_IOS)
		if (!is_staging_buffer && !has_flag<COMPUTE_MEMORY_FLAG::USE_HOST_MEMORY>(flags)) {
			// for performance reasons, still use private storage here, but also create a host-accessible staging buffer
			// that we'll use to copy memory to and from the private storage buffer
			options |= MTLResourceStorageModePrivate;
			staging_buffer = unique_ptr<metal_buffer>(new metal_buffer(true /* staging */, device, size, nullptr,
																	   COMPUTE_MEMORY_FLAG::READ_WRITE |
																	   (flags & COMPUTE_MEMORY_FLAG::HOST_READ_WRITE), 0, 0));
		}
		else {
			// use managed storage for the staging buffer or host memory backed buffer
			// note that this requires us to perform explicit sync operations
			options |= MTLResourceStorageModeManaged;
		}
#else
		// iOS only knows private and shared storage modes
		options |= MTLResourceStorageModeShared;
#endif
	}
	
	// TODO: handle the remaining flags + host ptr
	
	// actually create the buffer
	if(!create_internal(true)) {
		return; // can't do much else
	}
}

metal_buffer::metal_buffer(shared_ptr<compute_device> device,
						   id <MTLBuffer> external_buffer,
						   void* host_ptr_,
						   const COMPUTE_MEMORY_FLAG flags_) :
compute_buffer(device.get(), [external_buffer length], host_ptr_, flags_, 0, 0), buffer(external_buffer), is_external(true) {
	// size _has_ to match and be valid/compatible (compute_buffer will try to fix the size, but it's obviously an external object)
	// -> detect size mismatch and bail out
	if(size != [external_buffer length]) {
		log_error("can't wrap a buffer that has an incompatible size/length!");
		return;
	}
	if(size < min_multiple()) return;
	
	// device must match
	if(((metal_device*)device.get())->device != [external_buffer device]) {
		log_error("specified metal device does not match the device set in the external buffer");
		return;
	}
	
	// copy existing options
	options = [buffer cpuCacheMode];
	
#if defined(FLOOR_IOS)
	FLOOR_PUSH_WARNINGS()
	FLOOR_IGNORE_WARNING(switch) // MTLStorageModeManaged can't be handled on iOS
#endif
	
	switch([buffer storageMode]) {
		default:
		case MTLStorageModeShared:
			options |= MTLResourceStorageModeShared;
			break;
		case MTLStorageModePrivate:
			options |= MTLResourceStorageModePrivate;
			break;
#if !defined(FLOOR_IOS)
		case MTLStorageModeManaged:
			options |= MTLResourceStorageModeManaged;
			break;
#endif
	}
	
#if defined(FLOOR_IOS)
FLOOR_POP_WARNINGS()
#endif
}

bool metal_buffer::create_internal(const bool copy_host_data) {
	// should not be called under that condition, but just to be safe
	if(is_external) {
		log_error("buffer is external!");
		return false;
	}
	
	// -> use host memory
	if(has_flag<COMPUTE_MEMORY_FLAG::USE_HOST_MEMORY>(flags)) {
		buffer = [((metal_device*)dev)->device newBufferWithBytesNoCopy:host_ptr length:size options:options
															deallocator:^(void*, NSUInteger) { /* nop */ }];
	}
	// -> alloc and use device memory
	else {
		// copy host memory to device if it is non-null and NO_INITIAL_COPY is not specified
		if(copy_host_data &&
		   host_ptr != nullptr &&
		   !has_flag<COMPUTE_MEMORY_FLAG::NO_INITIAL_COPY>(flags)) {
			// can't use "newBufferWithBytes" with private storage memory
			if((options & MTLResourceStorageModeMask) == MTLResourceStorageModePrivate) {
				auto cqueue = ((metal_compute*)dev->context)->get_device_internal_queue(dev);
				
				// -> create the uninitialized private storage buffer and a host memory buffer (or use the staging buffer
				//    if available), then blit from the host memory buffer
				buffer = [((metal_device*)dev)->device newBufferWithLength:size options:options];
				if (staging_buffer == nullptr) {
					auto buffer_with_host_mem = [((metal_device*)dev)->device newBufferWithBytes:host_ptr length:size
																						 options:(MTLResourceStorageModeManaged |
																								  MTLCPUCacheModeWriteCombined)];
					
					id <MTLCommandBuffer> cmd_buffer = ((metal_queue*)cqueue.get())->make_command_buffer();
					id <MTLBlitCommandEncoder> blit_encoder = [cmd_buffer blitCommandEncoder];
					[blit_encoder copyFromBuffer:buffer_with_host_mem
									sourceOffset:0
										toBuffer:buffer
							   destinationOffset:0
											size:size];
					[blit_encoder endEncoding];
					[cmd_buffer commit];
					[cmd_buffer waitUntilCompleted];
				}
				else {
					staging_buffer->write(cqueue, host_ptr, size);
					copy(cqueue, *staging_buffer);
				}
			}
			else {
				// all other storage modes can just use it
				buffer = [((metal_device*)dev)->device newBufferWithBytes:host_ptr length:size options:options];
			}
		}
		// else: just create a buffer of the specified size
		else {
			buffer = [((metal_device*)dev)->device newBufferWithLength:size options:options];
		}
	}
	return true;
}

metal_buffer::~metal_buffer() {
	// kill the buffer / auto-release
	buffer = nil;
}

void metal_buffer::read(shared_ptr<compute_queue> cqueue, const size_t size_, const size_t offset) {
	read(cqueue, host_ptr, size_, offset);
}

void metal_buffer::read(shared_ptr<compute_queue> cqueue floor_unused_on_ios, void* dst, const size_t size_, const size_t offset) {
	if(buffer == nil) return;
	
	const size_t read_size = (size_ == 0 ? size : size_);
	if(!read_check(size, read_size, offset, flags)) return;
	
#if !defined(FLOOR_IOS)
	if (staging_buffer != nullptr) {
		staging_buffer->copy(cqueue, *this, read_size, offset, offset);
		staging_buffer->read(cqueue, dst, read_size, offset);
		return;
	}
	
	if((options & MTLResourceStorageModeMask) == MTLResourceStorageModeManaged) {
		sync_metal_resource(cqueue, buffer);
	}
#endif
	
	GUARD(lock);
	memcpy(dst, (uint8_t*)[buffer contents] + offset, read_size);
}

void metal_buffer::write(shared_ptr<compute_queue> cqueue, const size_t size_, const size_t offset) {
	write(cqueue, host_ptr, size_, offset);
}

void metal_buffer::write(shared_ptr<compute_queue> cqueue floor_unused_on_ios, const void* src,
						 const size_t size_, const size_t offset) {
	if(buffer == nil) return;
	
	const size_t write_size = (size_ == 0 ? size : size_);
	if(!write_check(size, write_size, offset, flags)) return;
	
	GUARD(lock);
	id <MTLBuffer> write_buffer = (staging_buffer != nullptr ? staging_buffer->get_metal_buffer() : buffer);
	memcpy((uint8_t*)[write_buffer contents] + offset, src, write_size);
	
#if !defined(FLOOR_IOS)
	if((options & MTLResourceStorageModeMask) == MTLResourceStorageModeManaged ||
	   staging_buffer != nullptr) {
		[write_buffer didModifyRange:NSRange { offset, offset + write_size }];
	}
	
	if(staging_buffer != nullptr) {
		copy(cqueue, *staging_buffer, write_size, offset, offset);
	}
#endif
}

void metal_buffer::copy(shared_ptr<compute_queue> cqueue, shared_ptr<compute_buffer> src,
						const size_t size_, const size_t src_offset, const size_t dst_offset) {
	if(buffer == nil || src == nullptr) return;
	copy(cqueue, (metal_buffer&)*src, size_, src_offset, dst_offset);
}

void metal_buffer::copy(shared_ptr<compute_queue> cqueue floor_unused_on_ios, metal_buffer& src,
						const size_t size_, const size_t src_offset, const size_t dst_offset) {
	if(buffer == nil) return;
	
	const size_t src_size = src.get_size();
	const size_t copy_size = (size_ == 0 ? std::min(src_size, size) : size_);
	if(!copy_check(size, src_size, copy_size, dst_offset, src_offset)) return;
	
	GUARD(lock);
	
#if !defined(FLOOR_IOS)
	// if either source or destination uses private storage, we need to perform a blit copy
	if((src.get_metal_resource_options() & MTLResourceStorageModeMask) == MTLResourceStorageModePrivate ||
	   (options & MTLResourceStorageModeMask) == MTLResourceStorageModePrivate) {
		id <MTLCommandBuffer> cmd_buffer = ((metal_queue*)cqueue.get())->make_command_buffer();
		id <MTLBlitCommandEncoder> blit_encoder = [cmd_buffer blitCommandEncoder];
		[blit_encoder copyFromBuffer:src.get_metal_buffer()
						sourceOffset:src_offset
							toBuffer:buffer
				   destinationOffset:dst_offset
								size:copy_size];
		[blit_encoder endEncoding];
		[cmd_buffer commit];
		[cmd_buffer waitUntilCompleted];
		return;
	}
	
	if((src.get_metal_resource_options() & MTLResourceStorageModeMask) == MTLResourceStorageModeManaged) {
		sync_metal_resource(cqueue, src.get_metal_buffer());
	}
	if((options & MTLResourceStorageModeMask) == MTLResourceStorageModeManaged) {
		sync_metal_resource(cqueue, buffer);
	}
#endif
	
	memcpy((uint8_t*)[buffer contents] + dst_offset,
		   (uint8_t*)[src.get_metal_buffer() contents] + src_offset,
		   copy_size);
	
#if !defined(FLOOR_IOS)
	if((options & MTLResourceStorageModeMask) == MTLResourceStorageModeManaged) {
		[buffer didModifyRange:NSRange { dst_offset, dst_offset + copy_size }];
	}
#endif
}

void metal_buffer::fill(shared_ptr<compute_queue> cqueue floor_unused_on_ios,
						const void* pattern, const size_t& pattern_size,
						const size_t size_, const size_t offset) {
	if(buffer == nil) return;
	
	const size_t fill_size = (size_ == 0 ? size : size_);
	if(!fill_check(size, fill_size, pattern_size, offset)) return;
	
	GUARD(lock);
	
	id <MTLBuffer> fill_buffer = (staging_buffer != nullptr ? staging_buffer->get_metal_buffer() : buffer);
	const size_t pattern_count = fill_size / pattern_size;
	switch(pattern_size) {
		case 1: {
#if !defined(FLOOR_IOS)
			// can use fillBuffer directly on the private storage buffer
			if((options & MTLResourceStorageModeMask) == MTLResourceStorageModePrivate) {
				id <MTLCommandBuffer> cmd_buffer = ((metal_queue*)cqueue.get())->make_command_buffer();
				id <MTLBlitCommandEncoder> blit_encoder = [cmd_buffer blitCommandEncoder];
				[blit_encoder fillBuffer:buffer
								   range:NSRange { offset, offset + fill_size }
								   value:*(const uint8_t*)pattern];
				[blit_encoder endEncoding];
				[cmd_buffer commit];
				[cmd_buffer waitUntilCompleted];
				return;
			}
#endif
			fill_n((uint8_t*)[fill_buffer contents] + offset, pattern_count, *(const uint8_t*)pattern);
			break;
		}
		// TODO: implement 2/4/any for MTLResourceStorageModePrivate
		case 2:
			fill_n((uint16_t*)[fill_buffer contents] + offset / 2u, pattern_count, *(const uint16_t*)pattern);
			break;
		case 4:
			fill_n((uint32_t*)[fill_buffer contents] + offset / 4u, pattern_count, *(const uint32_t*)pattern);
			break;
		default:
			// not a pattern size that allows a fast memset
			uint8_t* write_ptr = ((uint8_t*)[fill_buffer contents]) + offset;
			for(size_t i = 0; i < pattern_count; i++) {
				memcpy(write_ptr, pattern, pattern_size);
				write_ptr += pattern_size;
			}
			break;
	}
	
#if !defined(FLOOR_IOS)
	if((options & MTLResourceStorageModeMask) == MTLResourceStorageModeManaged ||
	   staging_buffer != nullptr) {
		[fill_buffer didModifyRange:NSRange { offset, offset + fill_size }];
	}
	
	if(staging_buffer != nullptr) {
		copy(cqueue, *staging_buffer, fill_size, offset, offset);
	}
#endif
}

void metal_buffer::zero(shared_ptr<compute_queue> cqueue) {
	const uint8_t zero_pattern = 0u;
	fill(cqueue, &zero_pattern, sizeof(zero_pattern), 0, 0);
}

bool metal_buffer::resize(shared_ptr<compute_queue> cqueue floor_unused, const size_t& new_size_ floor_unused,
						  const bool copy_old_data floor_unused, const bool copy_host_data floor_unused,
						  void* new_host_ptr floor_unused) {
	if(buffer == nil) return false;
	
	// TODO: !
	return false;
}

void* __attribute__((aligned(128))) metal_buffer::map(shared_ptr<compute_queue> cqueue floor_unused_on_ios,
													  const COMPUTE_MEMORY_MAP_FLAG flags_,
													  const size_t size_, const size_t offset) NO_THREAD_SAFETY_ANALYSIS {
	if(buffer == nil) return nullptr;
	
	const size_t map_size = (size_ == 0 ? size : size_);
	if(!map_check(size, map_size, flags, flags_, offset)) return nullptr;
	
	bool does_read = false, does_write = false;
	if(has_flag<COMPUTE_MEMORY_MAP_FLAG::WRITE_INVALIDATE>(flags_)) {
		does_write = true;
	}
	else {
		switch(flags_ & COMPUTE_MEMORY_MAP_FLAG::READ_WRITE) {
			case COMPUTE_MEMORY_MAP_FLAG::READ:
				does_read = true;
				break;
			case COMPUTE_MEMORY_MAP_FLAG::WRITE:
				does_write = true;
				break;
			case COMPUTE_MEMORY_MAP_FLAG::READ_WRITE:
				does_read = true;
				does_write = true;
				break;
			case COMPUTE_MEMORY_MAP_FLAG::NONE:
			default:
				log_error("neither read nor write flag set for buffer mapping!");
				return nullptr;
		}
	}
	const bool write_only = (!does_read && does_write);
	const bool read_only = (does_read && !does_write);
	
	// must lock this to make sure all prior work has completed
	_lock();
	
#if !defined(FLOOR_IOS)
	// NOTE: MTLResourceStorageModePrivate handled by map_check (-> no host access is handled)
	if((options & MTLResourceStorageModeMask) == MTLResourceStorageModeManaged) {
		alignas(128) unsigned char* host_buffer = nullptr;
		
		// check if we need to copy the buffer from the device (in case READ was specified)
		if(!write_only) {
			// need to sync buffer (resource) before being able to read it
			sync_metal_resource(cqueue, buffer);
			
			// direct access to the metal buffer
			host_buffer = ((uint8_t*)[buffer contents]) + offset;
		}
		else {
			// if write-only, we don't need to actually access the buffer just yet (no need to sync),
			// so just alloc host memory that can be accessed/used by the user
			host_buffer = new unsigned char[map_size] alignas(128);
		}
		
		// need to remember how much we mapped and where (so the host->device write-back copies the right amount of bytes)
		mappings.emplace(host_buffer, metal_mapping { map_size, offset, flags_, write_only, read_only });
		
		return host_buffer;
	}
	else if(staging_buffer != nullptr) {
		if(!write_only) {
			// read-only or read-write: update staging buffer
			staging_buffer->copy(cqueue, (metal_buffer&)*this, map_size, offset);
		}
		
		// do the mapping using the staging buffer
		auto mapped_ptr = staging_buffer->map(cqueue, flags_, size_, offset);
		
		// remember the mapping so that we can properly unmap again later
		mappings.emplace(mapped_ptr, metal_mapping { map_size, offset, flags_, write_only, read_only });
		
		return mapped_ptr;
	}
	else {
#endif
		// can just return the cpu mapped pointer
		return (void*)(((uint8_t*)[buffer contents]) + offset);
#if !defined(FLOOR_IOS)
	}
#endif
}

void metal_buffer::unmap(shared_ptr<compute_queue> cqueue floor_unused_on_ios,
						 void* __attribute__((aligned(128))) mapped_ptr) NO_THREAD_SAFETY_ANALYSIS {
	if(buffer == nil) return;
	if(mapped_ptr == nullptr) return;
	
#if !defined(FLOOR_IOS)
	const bool is_managed = ((options & MTLResourceStorageModeMask) == MTLResourceStorageModeManaged);
	const bool has_staging_buffer = (staging_buffer != nullptr);
	if(is_managed || has_staging_buffer) {
		// check if this is actually a mapped pointer (+get the mapped size)
		const auto iter = mappings.find(mapped_ptr);
		if(iter == mappings.end()) {
			log_error("invalid mapped pointer: %X", mapped_ptr);
			return;
		}
		
		if (is_managed) {
			// check if we need to actually copy data back to the device (not the case if read-only mapping)
			if(has_flag<COMPUTE_MEMORY_MAP_FLAG::WRITE>(iter->second.flags) ||
			   has_flag<COMPUTE_MEMORY_MAP_FLAG::WRITE_INVALIDATE>(iter->second.flags)) {
				// if mapping is write-only, mapped_ptr was manually alloc'ed and we need to copy the data
				// to the actual metal buffer
				if(iter->second.write_only) {
					memcpy(((uint8_t*)[buffer contents]) + iter->second.offset, mapped_ptr, iter->second.size);
					
					// and cleanup
					delete [] (unsigned char*)mapped_ptr;
				}
				// else: the user received pointer directly to the metal buffer and nothing needs to be copied
				
				// finally, notify the buffer that we changed its contents
				[buffer didModifyRange:NSRange { iter->second.offset, iter->second.offset + iter->second.size }];
			}
		}
		else if(has_staging_buffer) {
			// perform unmap on the staging buffer, then update this buffer if necessary
			staging_buffer->unmap(cqueue, mapped_ptr);
			if (iter->second.write_only || !iter->second.read_only) {
				copy(cqueue, *staging_buffer, iter->second.size, iter->second.offset);
			}
		}
		
		// remove the mapping
		mappings.erase(mapped_ptr);
	}
#endif
	// else: don't need to do anything for shared host/device memory
	
	_unlock();
}

// NOTE: does not apply to metal - buffer can always/directly be used with graphics pipeline
bool metal_buffer::acquire_opengl_object(shared_ptr<compute_queue> cqueue floor_unused) {
	return true;
}

// NOTE: does not apply to metal - buffer can always/directly be used with graphics pipeline
bool metal_buffer::release_opengl_object(shared_ptr<compute_queue> cqueue floor_unused) {
	return true;
}

void metal_buffer::sync_metal_resource(shared_ptr<compute_queue> cqueue floor_unused_on_ios,
									   id <MTLResource> rsrc floor_unused_on_ios) {
#if !defined(FLOOR_IOS)
	id <MTLCommandBuffer> cmd_buffer = ((metal_queue*)cqueue.get())->make_command_buffer();
	id <MTLBlitCommandEncoder> blit_encoder = [cmd_buffer blitCommandEncoder];
	[blit_encoder synchronizeResource:rsrc];
	[blit_encoder endEncoding];
	[cmd_buffer commit];
	[cmd_buffer waitUntilCompleted];
#else
	// not available on iOS for now
#endif
}

#endif
