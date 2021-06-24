#pragma once
#include "Allocator.h"

namespace Core
{
	namespace Memory
	{
		class LinearAllocator: public Allocator
		{
			public:
				explicit LinearAllocator(const std::size_t totalSize);
				
				void Init() override;

				/*
					@param size
					@param allignement
					@return true or false if given size with given alignement can be fitted
				*/
				bool Fits(const std::size_t size, const std::size_t allignement = 8) override;

				/*
					@param size
					@param allignement
					@param offset
					@retun ptr to allocated else nullptr if out of memory
				*/
				void* Allocate(const std::size_t size, const std::size_t allignment = 8) override;

				/*
					@param ptr
				*/
				void Free(void* ptr) override;

				void Reset();

			private:
				void* m_StartPtr = nullptr;
				std::size_t m_Offset;
		};
	}
}