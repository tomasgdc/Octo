#pragma once
#include <cstdint>
#include <cstddef>

namespace Core
{
	namespace Memory
	{
		class LinearOffsetAllocator
		{
		public:
			explicit LinearOffsetAllocator();

			void Init(const std::size_t totalSize);

			/*
				@param size
				@param allignement
				@return true or false if given size with given alignement can be fitted
			*/
			bool Fits(const std::size_t size, const std::size_t allignement = 8);

			/*
				@param size
				@param allignement
				@param offset
				@retun new offset
			*/
			std::size_t Allocate(const std::size_t size, const std::size_t allignment = 8);

			/*
				@return
			*/
			std::size_t GetCurrentOffset() const;

			void Reset();

		private:
			uint32_t m_initialOffset;
			uint32_t m_sizeInBytes;
			uint32_t m_currentOffsetInBytes;
		};
	}
}