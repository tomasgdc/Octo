#include "LinearOffsetAllocator.h"

namespace Core
{
	namespace Memory
	{
		LinearOffsetAllocator::LinearOffsetAllocator()
			:m_sizeInBytes(0), m_currentOffsetInBytes(0), m_initialOffset(0)
		{

		}

		void LinearOffsetAllocator::Init(const std::size_t totalSize)
		{
			m_sizeInBytes = totalSize;
			m_currentOffsetInBytes = 0;
			m_initialOffset = 0;
		}

		bool LinearOffsetAllocator::Fits(const std::size_t size, const std::size_t allignement)
		{
			const uint32_t startOffset = ((m_currentOffsetInBytes + allignement - 1) & ~(allignement - 1)) + size;
			return (startOffset - m_initialOffset) <= m_sizeInBytes;
		}

		std::size_t LinearOffsetAllocator::Allocate(const std::size_t size, const std::size_t allignment)
		{
			const size_t alignedNewOffset = (m_currentOffsetInBytes + allignment - 1) & ~(allignment - 1);
			m_currentOffsetInBytes = alignedNewOffset + size;
			return alignedNewOffset;
		}

		std::size_t LinearOffsetAllocator::GetCurrentOffset() const
		{
			return m_currentOffsetInBytes;
		}

		void LinearOffsetAllocator::Reset()
		{
			m_currentOffsetInBytes = m_initialOffset;
		}
	}
}