#include "LinearAllocator.h"
#include <cassert>
#include <iostream>

namespace Core
{ 
	namespace Memory
	{ 
		LinearAllocator::LinearAllocator(const std::size_t totalSize) : Allocator(totalSize)
		{

		}


		void LinearAllocator::Init()
		{
			if (m_StartPtr != nullptr) 
			{
				free(m_StartPtr);
			}

			m_StartPtr = malloc(m_TotalSize);
			m_Offset = 0;
		}

		bool LinearAllocator::Fits(std::size_t size, std::size_t allignement)
		{
			std::size_t padding = 0;
			std::size_t paddedAddress = 0;
			const std::size_t currentAddress = (std::size_t)m_StartPtr + m_Offset;

			if (allignement != 0 && m_Offset % allignement != 0)
			{
				// Alignment is required. Find the next aligned memory address and update offset
				padding = CalculatePadding(currentAddress, allignement);
			}

			if (m_Offset + padding + size > m_TotalSize)
			{
				return false;
			}

			return true;
		}

		void* LinearAllocator::Allocate(std::size_t size, std::size_t allignement)
		{
			std::size_t padding = 0;
			std::size_t paddedAddress = 0;
			const std::size_t currentAddress = (std::size_t)m_StartPtr + m_Offset;

			if (allignement != 0 && m_Offset % allignement != 0) 
			{
				// Alignment is required. Find the next aligned memory address and update offset
				padding = CalculatePadding(currentAddress, allignement);
			}

			if (m_Offset + padding + size > m_TotalSize)
			{
				return nullptr;
			}

			m_Offset += padding;
			const std::size_t nextAddress = currentAddress + padding;
			m_Offset += size;

#ifdef _DEBUG
			std::cout << "A" << "\t@C " << (void*)currentAddress << "\t@R " << (void*)nextAddress << "\tO " << m_Offset << "\tP " << padding << std::endl;
#endif

			m_UsedMemory = m_Offset;

			return (void*)nextAddress;
		}

		void LinearAllocator::Free(void* ptr)
		{
			assert(false && "Use Reset() method");
		}

		void LinearAllocator::Reset()
		{
			m_Offset = 0;
			m_UsedMemory = 0;
			m_NumAllocations = 0;
		}
	}
}