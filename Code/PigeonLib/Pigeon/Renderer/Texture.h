#pragma once

#include <string>

#include "Pigeon/Core.h"

namespace pig 
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static pig::U_Ptr<Texture2D> Create(const std::string& path);
		static pig::U_Ptr<Texture2D> Create(unsigned int width, unsigned int height, unsigned int channels, const unsigned char* data);
	};
}