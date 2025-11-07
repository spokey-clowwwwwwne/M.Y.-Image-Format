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

#define HEADER_SIGNATURE ("M.Y.")
#define HEADER_SIGNATURE_SIZE_BYTES (sizeof(HEADER_SIGNATURE) - 1)
// The header struct plus the signature size
#define HEADER_SIZE_BYTES (sizeof(Header_t) + HEADER_SIGNATURE_SIZE_BYTES)

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
	ui64 m_DataLenght;
	ui64 m_TypeSize;

	void* m_Data;
}Chunk_t;

#define CHUNK_SIZE_NO_DATA (sizeof(Chunk_t) - sizeof(void*))

static inline void ImageInit(Image_t* Image)
{
	memset(Image, 0, sizeof(Image_t));
}

static inline BOOL ImageCreate(Image_t* OutImage, ui32 Width, ui32 Height, ui16 Channels, ui64 TypeSize)
{
	if (!OutImage || Width == 0 || Height == 0 || Channels == 0 || TypeSize == 0) return FALSE;

	if (OutImage->m_OwnsData && OutImage->m_Data)
	{
		free(OutImage->m_Data);
		OutImage->m_Data = NULL;
	}

	OutImage->m_Width = Width;
	OutImage->m_Height = Height;
	OutImage->m_Channels = Channels;
	OutImage->m_TypeSize = TypeSize;

	OutImage->m_Stride = (ui64)Width * Channels * TypeSize;
	OutImage->m_Data = malloc(Height * OutImage->m_Stride);
	if (!OutImage->m_Data)
	{
		return FALSE;
	}

	OutImage->m_OwnsData = TRUE;

	return TRUE;
}

static inline void ImageCreateView(Image_t* OutImage, ui32 Width, ui32 Height, ui16 Channels, ui64 TypeSize, void* Data)
{
	if (!OutImage || !Data || Width == 0 || Height == 0 || Channels == 0 || TypeSize == 0) return;

	if (OutImage->m_OwnsData && OutImage->m_Data)
	{
		free(OutImage->m_Data);
		OutImage->m_Data = NULL;
	}

	OutImage->m_Width = Width;
	OutImage->m_Height = Height;
	OutImage->m_Channels = Channels;
	OutImage->m_TypeSize = TypeSize;

	OutImage->m_Stride = (ui64)Width * Channels * TypeSize;
	OutImage->m_Data = Data;

	OutImage->m_OwnsData = FALSE;
}

static inline void ImageCreateMove(Image_t* OutImage, ui32 Width, ui32 Height, ui16 Channels, ui64 TypeSize, void** Data)
{
	if (!OutImage || !Data) return;
	if (Width == 0 || Height == 0 || Channels == 0 || TypeSize == 0) return;

	if (OutImage->m_OwnsData && OutImage->m_Data)
	{
		free(OutImage->m_Data);
		OutImage->m_Data = NULL;
	}

	OutImage->m_Width = Width;
	OutImage->m_Height = Height;
	OutImage->m_Channels = Channels;
	OutImage->m_TypeSize = TypeSize;

	OutImage->m_Stride = (ui64)Width * Channels * TypeSize;
	OutImage->m_Data = *Data;

	OutImage->m_OwnsData = TRUE;
	*Data = NULL;
}

static inline BOOL ImageCreateCopy(Image_t* OutImage, ui32 Width, ui32 Height, ui16 Channels, ui64 TypeSize, void* Data)
{
	if (!OutImage || !Data || Width == 0 || Height == 0 || Channels == 0 || TypeSize == 0) return FALSE;

	if (OutImage->m_OwnsData && OutImage->m_Data)
	{
		free(OutImage->m_Data);
		OutImage->m_Data = NULL;
	}

	OutImage->m_Width = Width;
	OutImage->m_Height = Height;
	OutImage->m_Channels = Channels;
	OutImage->m_TypeSize = TypeSize;

	OutImage->m_Stride = (ui64)Width * Channels * TypeSize;
	OutImage->m_Data = malloc(Height * OutImage->m_Stride);
	if (!OutImage->m_Data)
	{
		return FALSE;
	}

	memcpy(OutImage->m_Data, Data, (size_t)OutImage->m_Stride * OutImage->m_Height);

	OutImage->m_OwnsData = TRUE;

	return TRUE;
}

static inline void ImageDelete(Image_t* Image)
{
	if (Image->m_OwnsData && Image->m_Data)
	{
		free(Image->m_Data);
		Image->m_Data = NULL;
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

static inline BOOL SaveToFileChunk(const Chunk_t* Chunk, FILE* Stream, const ui16 Channels)
{
	if (!Chunk || !Stream || Channels == 0) return FALSE;

	switch (Chunk->m_ChunkType)
	{
	case HEADER:
	{
		// The header is implicit that it is the first chunk in the file and
		// size is always known at compile time so no need to save those
		ui8 Buffer[HEADER_SIZE_BYTES] = { 0 };

		memcpy(Buffer, HEADER_SIGNATURE, HEADER_SIGNATURE_SIZE_BYTES);
		memcpy(Buffer + HEADER_SIGNATURE_SIZE_BYTES,
			Chunk->m_Data,
			HEADER_SIZE_BYTES - HEADER_SIGNATURE_SIZE_BYTES);

		size_t BytesWritten = fwrite(Buffer, 1, HEADER_SIZE_BYTES, Stream);
		if (BytesWritten != HEADER_SIZE_BYTES)
		{
			free(Chunk->m_Data);
			return FALSE;
		}
			
		return TRUE;
	}

	case UNCOMPRESSED_DATA:
	{
		size_t dataBytes = (size_t)Chunk->m_DataLenght * (size_t)Chunk->m_TypeSize;
		size_t totalBytes = dataBytes + CHUNK_SIZE_NO_DATA;

		ui8* Buffer = (ui8*)malloc(totalBytes);
		if (!Buffer) return FALSE;

		// Copy the hole struct minus the void* to data

		memcpy(Buffer, Chunk, CHUNK_SIZE_NO_DATA);
		memcpy(Buffer + CHUNK_SIZE_NO_DATA, Chunk->m_Data, dataBytes);

		size_t BytesWritten = fwrite(Buffer, 1, totalBytes, Stream);
		if (BytesWritten != totalBytes)
		{
			free(Buffer);
			return FALSE;
		}
		
		free(Buffer);
		return TRUE;
	}

	case RLE:
	{
		// Allocate enuff for the Chunk and for the one pixel to be repeated
		size_t dataBytes = (size_t)Channels * (size_t)Chunk->m_TypeSize;
		size_t totalBytes = dataBytes + CHUNK_SIZE_NO_DATA;

		ui8* Buffer = (ui8*)malloc(totalBytes);
		if (!Buffer)
		{
			return FALSE;
		}

		memcpy(Buffer, Chunk, CHUNK_SIZE_NO_DATA);
		memcpy(Buffer + CHUNK_SIZE_NO_DATA, Chunk->m_Data, dataBytes);

		size_t BytesWritten = fwrite(Buffer, 1, totalBytes, Stream);
		if (BytesWritten != totalBytes)
		{
			free(Buffer);
			return FALSE;
		}

		free(Buffer);
		return TRUE;
	}

	default:
		return FALSE;
	}
}

static inline BOOL ImageSave(const Image_t* Image, const char* FileName)
{
	if (!Image || !FileName) return FALSE;

	FILE* File = fopen(FileName, "wb");
	if (!File) return FALSE;

	// For now save raw data no RLE compression

	Header_t Header =
	{
		.m_Width = Image->m_Width,
		.m_Height = Image->m_Height,
		.m_Stride = Image->m_Stride,
		.m_Channels = Image->m_Channels,
		.m_TypeSize = Image->m_TypeSize,

		.m_NumChunks = 2 // For now only header and the raw data chunk
	};

	Chunk_t ChunkHeader = 
	{
		.m_ChunkType = HEADER,
		.m_DataLenght = sizeof(Header_t),
		.m_TypeSize = sizeof(ui8)
	};
	

	ui8 HeaderData[sizeof(Header_t)];
	memcpy(HeaderData, &Header, sizeof(Header_t));

	ChunkHeader.m_Data = HeaderData;
	if (!SaveToFileChunk(&ChunkHeader, File, Image->m_Channels))
	{
		fclose(File);
		return FALSE;
	}

	Chunk_t ImageData =
	{
		.m_ChunkType = UNCOMPRESSED_DATA,
		.m_DataLenght = Image->m_Stride * Image->m_Height,
		.m_TypeSize = sizeof(ui8),
		.m_Data = Image->m_Data
	};
	

	if (!SaveToFileChunk(&ImageData, File, Image->m_Channels))
	{
		fclose(File);
		return FALSE;
	}

	fclose(File);
	return TRUE;
}

static inline BOOL ReadHeaderFromFile(Header_t* OutHeader, FILE* Stream)
{
	// Assuming that this function is called at the beging of the file

	// Signature plus Header size
	ui8 Buffer[HEADER_SIZE_BYTES];

	if (fread(Buffer, 1, HEADER_SIZE_BYTES, Stream) != HEADER_SIZE_BYTES)
	{
		return FALSE;
	}

	// Check if signature is pressent "M.Y."
	if (memcmp(Buffer, HEADER_SIGNATURE, HEADER_SIGNATURE_SIZE_BYTES) != 0)
	{
		return FALSE;
	}

	// Not copy the signature "M.Y."
	memcpy(OutHeader, Buffer + HEADER_SIGNATURE_SIZE_BYTES, sizeof(Header_t));
	return TRUE;
}

static inline BOOL ReadChunkFromFile(Chunk_t* OutChunk, FILE* Stream)
{
	if (fread(OutChunk, 1, CHUNK_SIZE_NO_DATA, Stream) != CHUNK_SIZE_NO_DATA)
	{
		return FALSE;
	}

	switch (OutChunk->m_ChunkType)
	{
	case UNCOMPRESSED_DATA:
	{
		OutChunk->m_Data = malloc(OutChunk->m_DataLenght * OutChunk->m_TypeSize);
		if (!OutChunk->m_Data)
		{
			return FALSE;
		}

		if (fread(OutChunk->m_Data, OutChunk->m_TypeSize, OutChunk->m_DataLenght, Stream) != OutChunk->m_DataLenght)
		{
			free(OutChunk->m_Data);
			return FALSE;
		}

		return TRUE;
	}
	// To be added
	case RLE:
	{
		return FALSE;
	}
	default:
	{
		return FALSE;
	}

	}
}

// !!! This is only temporary !!!
static inline BOOL ImageLoad(Image_t* OutImage, const char* FileName)
{
	FILE* ImageFile = fopen(FileName, "rb");
	if (!ImageFile)
	{
		return FALSE;
	}

	Header_t FileHeader;
	if (!ReadHeaderFromFile(&FileHeader, ImageFile))
	{
		fclose(ImageFile);
		return FALSE;
	}

	OutImage->m_Channels = FileHeader.m_Channels;
	OutImage->m_Width = FileHeader.m_Width;
	OutImage->m_Height = FileHeader.m_Height;
	OutImage->m_Stride = FileHeader.m_Stride;
	OutImage->m_TypeSize = FileHeader.m_TypeSize;

	OutImage->m_OwnsData = TRUE;
	
	Chunk_t Chunk;
	if (!ReadChunkFromFile(&Chunk, ImageFile))
	{
		fclose(ImageFile);
		return FALSE;
	}

	OutImage->m_Data = Chunk.m_Data;

	fclose(ImageFile);
	return TRUE;
}

#endif // MY_IMAGE