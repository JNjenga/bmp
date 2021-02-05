#pragma once
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <algorithm>

#define PIXEL_INDEX(w, x, y) x + w * y;

namespace bmp
{
	struct bmp32_t 
	{
		unsigned int data_offset;
		unsigned int width;
		unsigned int height;

		uint32_t * pixels;
	};

	static char * read_file(const char * path)
	{
		char * data;

		std::ifstream in(path, std::ifstream::binary);

		if(in)
		{
			in.seekg(0, in.end);
			auto length = in.tellg();
			in.seekg(0, in.beg);

			data = new char[length];
			in.read(data, length);
			in.close();
		}

		return data;
	}

	static bmp32_t * read_bmp32(char * data)
	{
		uint16_t bsize;
		uint32_t data_offset;
		uint32_t dib_header_size;
		uint32_t width, height;
		uint16_t depth;
		uint32_t * pixels;

		// Check magic number
		if(data[0] != 'B' && data[1] != 'M')
		{
			return nullptr;
		}

		bsize = *(uint16_t*)(data + 2);
		data_offset = *(uint32_t*)(data + 0x0a);

		dib_header_size = *(uint32_t*)(data + 14);
		width = *(uint32_t*)(data + 18);
		height = *(uint32_t*)(data + 22);
		depth = *(uint16_t*)(data + 28);

		pixels = new uint32_t[width * height];

		for(int i = 0; i < (width * height) ; i ++)
		{
			pixels[i] = *(uint32_t*)(data + data_offset + (i * 4));
		}

		bmp32_t * bmp = new bmp32_t();
		bmp->data_offset = data_offset;
		bmp->width = width;
		bmp->height = height;
		bmp->pixels = pixels;

		return bmp;
	}

	static void print_bmp(bmp32_t * bmp)
	{
		std::cout << "Data offset : " << bmp->data_offset<< "\n";
		std::cout << "Width : " << bmp->width << "\n";
		std::cout << "Height : " << bmp->height << "\n";

		for(int i = 0; i < (bmp->width * bmp->height); i ++)
		{
			std::cout << "r : " << ((unsigned char*) &bmp->pixels[i])[2]
				<< " g : " <<((unsigned char*) &bmp->pixels[i])[1] 
				<< " b : " <<((unsigned char*) &bmp->pixels[i])[0] 
				<< " a : " <<((unsigned char*) &bmp->pixels[i])[3] << std::endl; 
		}
	}

	static int write_bmp32(const char * file_name, bmp32_t * b)
	{
		std::fstream out;

		out.open(file_name, std::ios::binary | std::ios::out);

		if(!out.is_open())
		{
			return -1;
		}
		else
		{
			b->data_offset = 0x36;
			unsigned int file_size = 0x36 + (b->width * b->height * 4);

			char * buffer = new char[file_size];
			buffer[0] = 'B';
			buffer[1] = 'M';

			unsigned int * loc = (unsigned int *)(&buffer[2]);
			*loc = file_size;

			*(uint16_t*)&buffer[6] = 0;
			*(uint16_t*)&buffer[8] = 0;

			*(uint32_t*)&buffer[10] = b->data_offset;
			*(uint32_t*)&buffer[0xe] = 40;
			*(uint32_t*)&buffer[0x12] = b->width;
			*(uint32_t*)&buffer[0x16] = b->height;
			*(uint16_t*)&buffer[0x1a] = 1;
			*(uint16_t*)&buffer[0x1c] = 32;
			*(uint32_t*)&buffer[0x1e] = 0;
			*(uint32_t*)&buffer[0x22] = 32;
			*(uint32_t*)&buffer[0x26] = 2835;
			*(uint32_t*)&buffer[0x2a] = 2835;
			*(uint32_t*)&buffer[0x2e] = 0;
			*(uint32_t*)&buffer[0x32] = 0;
			
			for(int i = 0; i < (b->width * b->height); i ++)
			{
				*(uint32_t*)&buffer[i * 4 + 0x36] = b->pixels[i];
			}

			out.write(buffer , file_size);
			out.close();
			delete buffer;
		}

		return 0;
	}

	static bmp32_t * create_bmp32(unsigned int width, unsigned int height)
	{
		uint32_t * pixels = new uint32_t[width * height];

		for(int i = 0; i < (width * height); i ++)
		{
			pixels[i] = 0x0fffffff;
		}

		bmp32_t * bmp = new bmp32_t();
		bmp->data_offset = 0x36;
		bmp->width = width;
		bmp->height = height;
		bmp->pixels = pixels;

		return bmp;
	}

	static void get_pixel_values(uint32_t & pixel, unsigned char & r, unsigned char & g, unsigned char & b, unsigned char & a)
	{
		b = pixel >> 24;
		g = (pixel << 8) >> 24;
		r = (pixel << 16) >> 24;
		a = (pixel << 24) >> 24;
	}

	static void write_pixel(uint32_t & pixel, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		pixel = b;
		pixel = (pixel << 8);
		pixel += g;
		pixel = (pixel << 8);
		pixel += r;
		pixel = (pixel << 8);
		pixel += a;
	}

}
