#pragma once
#include <cstdint>
#include <cstddef>

namespace Core
{
	namespace Memory
	{
		class Allocator
		{
		public:

			Allocator();

			virtual ~Allocator();

			virtual void Init(const std::size_t totalSize) = 0;

			/*
				@param size
				@param allignement
			*/
			virtual bool Fits(const std::size_t size, const std::size_t allignement = 8) = 0;

			/*
				@param size
				@parma allignement
			*/
			virtual void* Allocate(const std::size_t size, const std::size_t allignment = 8) = 0;

			/*
				@param ptr
			*/
			virtual void Free(void* ptr) = 0;

			/*
				@return
			*/
			std::size_t GetSize() const;

			/*
				@return
			*/
			std::size_t GetUsedMemory() const;

			/*
				@return
			*/
			std::size_t GetNumAllocations() const;

		protected:

			std::size_t m_TotalSize;
			std::size_t m_UsedMemory;
			std::size_t m_NumAllocations;
		};


		static const std::size_t CalculatePadding(const std::size_t baseAddress, const std::size_t alignment) 
		{
			const std::size_t multiplier = (baseAddress / alignment) + 1;
			const std::size_t alignedAddress = multiplier * alignment;
			const std::size_t padding = alignedAddress - baseAddress;
			return padding;
		}

		static const std::size_t CalculatePaddingWithHeader(const std::size_t baseAddress, const std::size_t alignment, const std::size_t headerSize) 
		{
			std::size_t padding = CalculatePadding(baseAddress, alignment);
			std::size_t neededSpace = headerSize;

			if (padding < neededSpace) 
			{
				// Header does not fit - Calculate next aligned address that header fits
				neededSpace -= padding;

				// How many alignments I need to fit the header        
				if (neededSpace % alignment > 0) 
				{
					padding += alignment * (1 + (neededSpace / alignment));
				}
				else 
				{
					padding += alignment * (neededSpace / alignment);
				}
			}

			return padding;
		}
	}
}