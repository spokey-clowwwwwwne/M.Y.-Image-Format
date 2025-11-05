#include "my_image.h"

int main(int argc, char** argv)
{
	Image_t Image;
	ImageInit(&Image);
	
	unsigned int Width = 255;
	unsigned int Height = 255;
	unsigned short Channels = 3; // RGB

	ImageCreate(&Image, Width, Height, Channels, sizeof(unsigned char));

	// Use the image
	// Example

	for (unsigned int y = 0; y < Height; ++y)
	{
		for (unsigned int x = 0; x < Width; ++x)
		{
			unsigned char Pixel[3];
			Pixel[0] = x;
			Pixel[1] = y;
			Pixel[2] = 0;

			ImageSetAt(&Image, x, y, 1, Pixel);
		}
	}

	for (unsigned int y = 0; y < Height; ++y)
	{
		for (unsigned int x = 0; x < Width; ++x)
		{
			unsigned char* Pixel;

			Pixel = (unsigned char*)ImageGetAt(&Image, x, y);

			printf("R: %d, G: %d, B: %d\n", Pixel[0], Pixel[1], Pixel[2]);
		}
	}

	ImageDelete(&Image);
}