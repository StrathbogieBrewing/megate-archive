#include <avr/pgmspace.h>

// map of ascii to 7 segment display
static const uint8_t PROGMEM charmap[] =
{
	0x00, // ' '
	0x81, // '!'
	0x44, // '"'
	0xC6, // '#'
	0x00, // '$', No seven-segment conversion for dollar sign
	0xB4, // '%'
	0xBE, // '&'
	0x40, // '''
	0x9C, // '('
	0xF0, // ')'
	0x00, // '*', No seven-segment conversion for asterix
	0x00, // '+', No seven-segment conversion for plus sign
	0x01, // ','
	0x02, // '-'
	0x01, // '.'
	0x4A, // '/'
	0xFC, // '0'
	0x60, // '1'
	0xDA, // '2'
	0xF2, // '3'
	0x66, // '4'
	0xB6, // '5'
	0xBE, // '6'
	0xE0, // '7'
	0xFE, // '8'
	0xF6, // '9'
	0x90, // ':'
	0xB0, // ';'
	0x86, // '<'
	0x12, // '='
	0xC2, // '>'
	0xCA, // '?'
	0xDE, // '@'
	0xEE, // 'A'
	0x3E, // 'B'
	0x9C, // 'C'
	0x7A, // 'D'
	0x9E, // 'E'
	0x8E, // 'F'
	0xBC, // 'G'
	0x6E, // 'H'
	0x60, // 'I'
	0x78, // 'J'
	0x6E, // 'K'
	0x1C, // 'L'
	0xA8, // 'M'
	0x2A, // 'N'
	0x3A, // 'O'
	0xCE, // 'P'
	0xE6, // 'Q'
	0x0A, // 'R'
	0xB6, // 'S'
	0x1E, // 'T'
	0x7C, // 'U'
	0x38, // 'V'
	0x54, // 'W'
	0x6E, // 'X'
	0x76, // 'Y'
	0xDA, // 'Z'
	0x9C, // '['
	0x26, // '\'
	0xF0, // ']'
	0xC4, // '^'
	0x10, // '_'
};

uint8_t seg_getSegments(uint8_t asciicode)
{
	while (asciicode > '_') asciicode -= 32;
	if (asciicode < ' ') asciicode = ' ';
	asciicode -= ' ';
	return pgm_read_byte(charmap + asciicode);
}
