/* FIL compression.hpp
 * Header för kompressionen som sker innan behandlad bild
 * skickas till datorn.
 */
#ifndef compression_h
#define compression_h
#include <zlib.h>

typedef uint8_t byte;

struct compression
{
	byte* data{};
	uLong comp_size{};
	uint sizeX{};
	uint sizeY{};
};

int img_comp_main();
int compress_image(int);

#endif
