#include "my_image.h"

int main(int argc, char** argv)
{
	// Creating image
	Image_t Image;
	ImageInit(&Image);

	// Size of image
	unsigned int Width = 255;
	unsigned int Height = 255;
	unsigned short Channels = 3; // RGB

	ImageCreate(&Image, Width, Height, Channels, sizeof(unsigned char));

	// Use the image
	// Example
	// Assining values from 0 to 255 to each channel
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

	// Saving the file
	ImageSave(&Image, "C:/Users/Usuario/C++-Programs/M.Y. Image Format/Test.my");

	// Reading it back
	Image_t NewImage;
	ImageInit(&NewImage);
	ImageLoad(&NewImage, "C:/Users/Usuario/C++-Programs/M.Y. Image Format/Test.my");

	// Using the image
	for (unsigned int y = 0; y < Height; ++y)
	{
		for (unsigned int x = 0; x < Width; ++x)
		{
			unsigned char* Pixel;

			Pixel = (unsigned char*)ImageGetAt(&NewImage, x, y);

			printf("R: %d, G: %d, B: %d\n", Pixel[0], Pixel[1], Pixel[2]);
		}
	}

	// Freing memory
	ImageDelete(&NewImage);
	ImageDelete(&Image);
}