#include "Allocator.h"
#include <cassert>

namespace Core
{
	namespace Memory
	{
		Allocator::Allocator(): m_TotalSize(0), m_UsedMemory(0), m_NumAllocations(0)
		{
			//assert(m_TotalSize > 0);
		}

		Allocator::~Allocator()
		{
			assert(m_NumAllocations == 0 && m_UsedMemory == 0);
		}

		std::size_t Allocator::GetSize() const
		{
			return m_TotalSize;
		}

		std::size_t Allocator::GetUsedMemory() const
		{
			return m_UsedMemory;
		}

		std::size_t Allocator::GetNumAllocations() const
		{
			return m_NumAllocations;
		}
	}
}