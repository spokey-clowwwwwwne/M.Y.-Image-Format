#ifndef MY_IMAGE
#define MY_IMAGE

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef NO_REDEF
typedef uint8_t  ui8;
typedef uint16_t ui16;
typedef uint32_t ui32;
typedef uint64_t ui64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
#endif

#ifndef BOOL
#define BOOL ui8
#endif // !BOOL

#ifndef TRUE
#define TRUE ((BOOL)1)
#endif // !TRUE

#ifndef FALSE
#define FALSE ((BOOL)0)
#endif // !FALSE

typedef struct
{
	ui32 m_Width;
	ui32 m_Height;
	ui64 m_Stride;
	ui16 m_Channels;
	ui64 m_TypeSize;
	ui64 m_NumChunks;
}Header_t;

typedef struct
{
	ui32 m_Width;
	ui32 m_Height;
	ui64 m_Stride;
	ui16 m_Channels;
	ui64 m_TypeSize;

	void* m_Data;
	BOOL m_OwnsData;
}Image_t;

typedef enum
{
	NONE = 0,
	HEADER,
	UNCOMPRESSED_DATA,
	RLE
}ChunkType_e;

typedef struct
{
	ChunkType_e m_ChunkType;
	ui32 m_DataLenght;
	ui64 m_TypeSize;

	void* m_Data;
}Chunk_t;

static inline void ImageInit(Image_t* Image)
{
	memset(Image, 0, sizeof(Image_t));
}

static inline BOOL ImageCreate(Image_t* Image, ui32 Width, ui32 Height, ui16 Channels, ui64 TypeSize)
{
	if (!Image || Width == 0 || Height == 0 || Channels == 0 || TypeSize == 0) return FALSE;
	if (Width == 0 || Height == 0 || Channels == 0 || TypeSize == 0) return FALSE;

	if (Image->m_OwnsData && Image->m_Data)
	{
		free(Image->m_Data);
		Image->m_Data = NULL;
	}

	Image->m_Width = Width;
	Image->m_Height = Height;
	Image->m_Channels = Channels;
	Image->m_TypeSize = TypeSize;

	Image->m_Stride = (ui64)Width * Channels * TypeSize;
	Image->m_Data = malloc(Height * Image->m_Stride);
	if (!Image->m_Data)
	{
		return FALSE;
	}

	Image->m_OwnsData = TRUE;

	return TRUE;
}

static inline void ImageCreateView(Image_t* Image, ui32 Width, ui32 Height, ui16 Channels, ui64 TypeSize, void* Data)
{
	if (!Image || !Data || Width == 0 || Height == 0 || Channels == 0 || TypeSize == 0) return;

	if (Image->m_OwnsData && Image->m_Data)
	{
		free(Image->m_Data);
		Image->m_Data = NULL;
	}

	Image->m_Width = Width;
	Image->m_Height = Height;
	Image->m_Channels = Channels;
	Image->m_TypeSize = TypeSize;

	Image->m_Stride = (ui64)Width * Channels * TypeSize;
	Image->m_Data = Data;

	Image->m_OwnsData = FALSE;
}

static inline void ImageCreateMove(Image_t* Image, ui32 Width, ui32 Height, ui16 Channels, ui64 TypeSize, void** Data)
{
	if (!Image || !Data) return;
	if (Width == 0 || Height == 0 || Channels == 0 || TypeSize == 0) return;

	if (Image->m_OwnsData && Image->m_Data)
	{
		free(Image->m_Data);
		Image->m_Data = NULL;
	}

	Image->m_Width = Width;
	Image->m_Height = Height;
	Image->m_Channels = Channels;
	Image->m_TypeSize = TypeSize;

	Image->m_Stride = (ui64)Width * Channels * TypeSize;
	Image->m_Data = *Data;

	Image->m_OwnsData = TRUE;
	*Data = NULL;
}

static inline BOOL ImageCreateCopy(Image_t* Image, ui32 Width, ui32 Height, ui16 Channels, ui64 TypeSize, void* Data)
{
	if (!Image || !Data || Width == 0 || Height == 0 || Channels == 0 || TypeSize == 0) return FALSE;

	if (Image->m_OwnsData && Image->m_Data)
	{
		free(Image->m_Data);
		Image->m_Data = NULL;
	}

	Image->m_Width = Width;
	Image->m_Height = Height;
	Image->m_Channels = Channels;
	Image->m_TypeSize = TypeSize;

	Image->m_Stride = (ui64)Width * Channels * TypeSize;
	Image->m_Data = malloc(Height * Image->m_Stride);
	if (!Image->m_Data)
	{
		return FALSE;
	}

	memcpy(Image->m_Data, Data, (size_t)Image->m_Stride * Image->m_Height);

	Image->m_OwnsData = TRUE;

	return TRUE;
}

static inline void ImageDelete(Image_t* Image)
{
	if (!Image) return;
	if (Image->m_OwnsData && Image->m_Data)
	{
		free(Image->m_Data);
	}

	memset(Image, 0, sizeof(Image_t));
}

static inline void* ImageGetAt(Image_t* Image, ui32 x, ui32 y)
{
	if (!Image || !Image->m_Data) return NULL;
	if (x >= Image->m_Width || y >= Image->m_Height) return NULL;

	ui8* Data = (ui8*)Image->m_Data;
	ui64 BytesPerPixel = (ui64)Image->m_Channels * Image->m_TypeSize;
	ui64 ByteOffset =
		(ui64)y * Image->m_Stride + x * BytesPerPixel;

	return Data + ByteOffset;
}

static inline BOOL ImageSetAt(Image_t* Image, ui32 x, ui32 y, ui64 Count, const void* Val)
{
	if (!Image || !Image->m_Data || !Val) return FALSE;
	if (x >= Image->m_Width || y >= Image->m_Height) return FALSE;

	ui8* Data = (ui8*)Image->m_Data;
	ui64 BytesPerPixel = (ui64)Image->m_Channels * Image->m_TypeSize;
	ui64 ByteOffset =
		(ui64)y * Image->m_Stride + x * BytesPerPixel;

	ui64 BytesToCopy = Count * BytesPerPixel;

	memcpy(Data + ByteOffset, Val, BytesToCopy);

	return TRUE;
}


#endif // MY_IMAGE