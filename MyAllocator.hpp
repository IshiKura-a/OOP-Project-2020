#include<iostream>
#include<cassert>
#pragma once

namespace my {
	const size_t ALIGN = 8;
	const size_t ALIGN_BIT = 3;
	const size_t MAX_BYTES = 128;
	const size_t N_FREELIST = MAX_BYTES / ALIGN;
	const size_t N_CHUNK = 20;

	typedef struct node
	{
		struct node* Next;
	}Obj;

	template <class _Ty>
	class MyAlloc
	{
	public:
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

		template<class T>
		struct rebind { typedef MyAlloc<T> other; };

		inline static char* start_pool = NULL;
		inline static char* end_pool = NULL;
		inline static size_type alloc_size = 0;
		inline static Obj* free_list[N_FREELIST] = { NULL };

		MyAlloc() noexcept {}

		static inline size_type ROUND_UP(size_type bytes)
		{
			return (bytes + ALIGN - 1) & ~(ALIGN - 1);
		}
		static inline size_type FREELIST_INDEX(size_type bytes)
		{
			return (bytes + ALIGN - 1) >> ALIGN_BIT - 1;
		}
		static void* refill(size_type _Count)
		{
			int n_obj = N_CHUNK;
			char* chunk = chunk_alloc(_Count, n_obj);
			Obj** cur_free_list;
			Obj* dst;
			Obj* cur_obj, * next_obj;
			int i;

			if (n_obj == 1) return chunk;
			cur_free_list = free_list + FREELIST_INDEX(_Count);

			dst = (Obj*)chunk;
			*cur_free_list = next_obj = (Obj*)(chunk + _Count);
			for (i = 1; i < n_obj; i++)
			{
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
		static char* chunk_alloc(size_type size, int& n_obj)
		{
			char* dst;
			size_type total_bytes = size * n_obj;
			size_type bytes_left = end_pool - start_pool;

			if (bytes_left >= total_bytes)
			{
				dst = start_pool;
				start_pool += total_bytes;
				return dst;
			}
			else if (bytes_left >= size)
			{
				dst = start_pool;
				n_obj = bytes_left / size;
				total_bytes = n_obj * size;
				start_pool += total_bytes;
				return dst;
			}
			else
			{
				size_type bytes_alloc = total_bytes << 1 +
					ROUND_UP(alloc_size >> 4);
				Obj** cur_free_list;

				if (bytes_left)
				{
					cur_free_list = free_list + FREELIST_INDEX(bytes_left);
					((Obj*)start_pool)->Next = *cur_free_list;
					*cur_free_list = (Obj*)start_pool;
				}

				start_pool = (char*)std::malloc(bytes_alloc);
				if (start_pool)
				{
					end_pool = start_pool + bytes_alloc;
					alloc_size += bytes_alloc;
					return chunk_alloc(size, n_obj);
				}
				else
				{
					int i;
					for (i = FREELIST_INDEX(bytes_left) + 1; i < N_FREELIST; i++)
					{
						cur_free_list = free_list + i;
						if (*cur_free_list)
						{
							start_pool = (char*)(*cur_free_list);
							end_pool = start_pool + (i + 1) * ALIGN;
							*cur_free_list = (*cur_free_list)->Next;
							return chunk_alloc(size, n_obj);
						}
					}
				}
			}
			return NULL;
		}
		template<class T>
		MyAlloc(const MyAlloc<T>& a) noexcept {}

		~MyAlloc() noexcept {}
		static inline size_type max_size() noexcept
		{
			return size_type(UINT_MAX / sizeof(value_type));
		}
		static inline pointer address(reference _Val) noexcept
		{
			return std::addressof(_Val);
		}
		static inline const_pointer address(const_reference _Val) noexcept
		{
			return std::addressof(_Val);
		}
		static inline void deallocate(pointer _Ptr, size_type _Count)
		{
			_Count *= sizeof(value_type);
			if (_Count > MAX_BYTES)
			{
				std::free(_Ptr);
				return;
			}
			else
			{
				Obj** cur_free_list = free_list + FREELIST_INDEX(_Count);
				((Obj*)_Ptr)->Next = *cur_free_list;
				*cur_free_list = (Obj*)_Ptr;
			}
		}
		static _DECLSPEC_ALLOCATOR pointer allocate(size_type _Count)
		{
			Obj** cur_free_list;
			Obj* dst;
			int index;

			_Count *= sizeof(value_type);
			if (_Count > MAX_BYTES)
			{
				return (pointer)std::malloc(_Count);
			}

			index = FREELIST_INDEX(_Count);
			cur_free_list = free_list + index;
			dst = *cur_free_list;

			if (!dst)
			{
				return (pointer)refill(ROUND_UP(_Count));
			}
			*cur_free_list = dst->Next;
			return (pointer)dst;
		}
		template<class _Uty>
		static inline void destroy(_Uty* _Ptr)
		{
			_Ptr->~_Uty();
		}
		template<class _Objty, class _Types>
		static inline void construct(_Objty* _Ptr, _Types _Args)
		{
			new(_Ptr) _Objty(_Args);
		}
	};
}