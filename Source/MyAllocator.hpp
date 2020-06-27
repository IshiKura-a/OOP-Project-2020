// MyAllocator.cpp
// All the objects in this file should be used
// in the namespace my.
// The schema refers to GNU2.9, written by Zihao Tang.

#include<iostream>
#include<cassert>
#pragma once

#define _DEBUG 0
namespace my {

// Parameters used to observe the status of alloc.
#if _DEBUG
	static int cnt_big = 0;
	static int cnt_small = 0;
#endif

	// Basic parameters of the allocator.
	// ALIGN & ALIGN_BIT denote the increament of the
	// size of Free_List.
	// MAX_BYTES denotes the maximum size of free_list.
	// N_FREELIST denotes the number of free_list, which
	// is decided by ALIGN & MAX_BYTES.
	// N_CHUNK denotes the number allocated blocks of each new alloc
	const size_t ALIGN = 8;
	const size_t ALIGN_BIT = 3;
	const size_t MAX_BYTES = 1024;
	const size_t N_FREELIST = MAX_BYTES / ALIGN;
	const size_t N_CHUNK = 16;

	// We use the struct Obj to link the free_list.
	typedef struct node
	{
		struct node* Next;
	}Obj;

	// The template of customized allocator.
	template <class _Ty>
	class MyAlloc
	{
	public:

		// The typename required by a std allocator.
		typedef void _Not_user_specialized;
		typedef _Ty value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;
		typedef std::true_type propagate_on_container_move_assignment;
		typedef std::true_type is_always_equal;

		// A necessary class require by a std allocator,
		// used to substitute for different types.
		template<class T>
		struct rebind { typedef MyAlloc<T> other; };

		// The start and end of memory pool, struct Obj
		// is embeded in the pool.
		// alloc_size
		// free_list records the free space remained for
		// incoming memory allocation.
		inline static char* start_pool = NULL;
		inline static char* end_pool = NULL;
		inline static size_type alloc_size = 0;
		inline static Obj* free_list[N_FREELIST] = { NULL };

		// Since the class allocator shares the same memory pool,
		// ctors & copy ctors are not in need.
		MyAlloc() noexcept {}

		// Round bytes up to the nearest multiples of ALIGN that
		// no less than bytes.
		// e.g. ALIGN=8, 7->8, 8->8, 10->8
		static inline size_type ROUND_UP(size_type bytes)
		{
			return (bytes + ALIGN - 1) & ~(ALIGN - 1);
		}
		// Get the index of the free_list 0-N_FREELIST-1,
		// 0-ALIGN-1 -> 0, ALIGN-2ALIGN-1 ->1, ...
		// Bit operation is used to accelerate calculations.
		static inline size_type FREELIST_INDEX(size_type bytes)
		{
			return ((bytes + ALIGN - 1) >> ALIGN_BIT) - 1;
		}
		static void* refill(size_type _Count)
		{
			// Default to allocate N_CHUNK objects.
			int n_obj = N_CHUNK;
			char* chunk = chunk_alloc(_Count, n_obj);
			Obj** cur_free_list;
			Obj* dst;
			Obj* cur_obj, * next_obj;
			int i;

			// If chunk_alloc returns only one object, return.
			if (n_obj == 1) return chunk;
			cur_free_list = free_list + FREELIST_INDEX(_Count);

			dst = (Obj*)chunk;
			*cur_free_list = next_obj = (Obj*)(chunk + _Count);
			for (i = 1; i < n_obj; i++)
			{
				// Build the link.
				cur_obj = next_obj;
				next_obj = (Obj*)((char*)next_obj + _Count);
				if (i < n_obj - 1)
				{
					cur_obj->Next = next_obj;
				}
				else
				{
					cur_obj->Next = NULL;
				}
			}

			return dst;
		}
		// Alloc a chunk of space
		static char* chunk_alloc(size_type size, int& n_obj)
		{
			char* dst;
			// Calculate the bytes to allocate and
			// the bytes left.
			size_type total_bytes = size * n_obj;
			size_type bytes_left = end_pool - start_pool;

			// If pool has enough space, return it.
			if (bytes_left >= total_bytes)
			{
				dst = start_pool;
				start_pool += total_bytes;
				return dst;
			}
			// If it's not enough for a chunk, but enough for
			// several tuples, return it.
			else if (bytes_left >= size)
			{
				dst = start_pool;
				// Number of tuples we allocate.
				n_obj = bytes_left / size;
				total_bytes = n_obj * size;
				start_pool += total_bytes;
				return dst;
			}
			// If the pool is not enough for one tuple
			// malloc new space.
			else
			{
				// Allocated spaced is the doubled bytes required
				// together with some margin decided by alloc_size:
				// the amount we have allocated already.
				size_type bytes_alloc = (total_bytes << 1) +
					ROUND_UP(alloc_size >> 4);
				Obj** cur_free_list;

				// If the pool is not empty, add it to free_list.
				if (bytes_left)
				{
					cur_free_list = free_list + FREELIST_INDEX(bytes_left);
					((Obj*)start_pool)->Next = *cur_free_list;
					*cur_free_list = (Obj*)start_pool;
				}

				start_pool = (char*)std::malloc(bytes_alloc);
				// We should test whether malloc successes.
				// If it does, recall chunk_alloc.
				if (start_pool)
				{
					end_pool = start_pool + bytes_alloc;
					alloc_size += bytes_alloc;
					return chunk_alloc(size, n_obj);
				}
				else
				{
					throw "Cannot allocate enough space!";
				}
			}
			return NULL;
		}

		// Copy ctor is not in need.
		template<class T>
		MyAlloc(const MyAlloc<T>& a) noexcept {}

		// This implement is diffcult to dtor.
		// Leave it to the compiler.
		~MyAlloc() noexcept {}

		// max_size is necessary in a std allocator.
		// Hence, return a  sufficiently large number. 
		static inline size_type max_size() noexcept
		{
			return size_type(UINT_MAX / sizeof(value_type));
		}
		// Since & can be overloaded, use addressof to
		// complete the operation.
		static inline pointer address(reference _Val) noexcept
		{
			return std::addressof(_Val);
		}
		// The const version of &.
		static inline const_pointer address(const_reference _Val) noexcept
		{
			return std::addressof(_Val);
		}
		// Dellocate the memory allocated.
		static inline void deallocate(pointer _Ptr, size_type _Count)
		{
			// Get the size of space.
			_Count *= sizeof(value_type);
			// If the space > MAX_BYTES, we used malloc
			// directly to give it memory. Therefore, free it.
			// Else, we should return it to the free_list.
			if (_Count > MAX_BYTES)
			{
				std::free(_Ptr);
				return;
			}
			else
			{
				// Find the index of free_list
				Obj** cur_free_list = free_list + FREELIST_INDEX(_Count);
				// Append it to the head of the free_list.
				// It saves time to find the tail.
				((Obj*)_Ptr)->Next = *cur_free_list;
				*cur_free_list = (Obj*)_Ptr;
			}
		}
		// allocate _Count value_type objects.
		// In this part we round the space up to power of 2
		// and return the total memory.
		// Obviously, every time we use allocate, only one
		// block in the free_list is used.
		static _DECLSPEC_ALLOCATOR pointer allocate(size_type _Count)
		{
			Obj** cur_free_list;
			Obj* dst;
			int index;

			// Get the real space.
			_Count *= sizeof(value_type);
			if (_Count > MAX_BYTES)
			{
#if _DEBUG		// Record number of big allocs for optimizations.
				++cnt_big;
				if (cnt_big % 1000 == 0)
					printf("cnt_big %d: %d\n", cnt_big, _Count);
#endif //  _DEBUG

				// The space required is quite large.
				// Use malloc directly.
				return (pointer)std::malloc(_Count);
			}

#if _DEBUG	// Record small allocs for optimizations,
			++cnt_small;
			if (cnt_small % 10 == 0)
				printf("cnt_small %d: %d\n", cnt_small, _Count);
#endif // _DEBUG

			// Get the index of the free_list.
			index = FREELIST_INDEX(_Count);
			cur_free_list = free_list + index;
			dst = *cur_free_list;

			// The pool is empty.
			// We need to refill the pool.
			if (!dst)
			{
				return (pointer)refill(ROUND_UP(_Count));
			}

			// Give the first block, and free_list points
			// to the next block.
			*cur_free_list = dst->Next;
			return (pointer)dst;
		}

		// dtor for the object using this allocator.
		template<class _Uty>
		static inline void destroy(_Uty* _Ptr)
		{
			_Ptr->~_Uty();
		}
		// ctor for the object using this allocator.
		// Here PLACEMENT-NEW is used, for we should construct
		// the object with the memory allocated in advance.
		// If we use operation new here, new space will be allocated
		// instead of contruct the object on the space we give it.
		template<class _Objty, class _Types>
		static inline void construct(_Objty* _Ptr, _Types _Args)
		{
			new(_Ptr) _Objty(_Args);
		}
	};
}