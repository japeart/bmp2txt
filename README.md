# bmp2txt
Convert a BMP font sheet to ASCII tiles ready for import into your project

Do you program in assembly for old computers such as the ZX Spectrum or Atari 2600? Do you use .bmp fontsheets at all? Have you ever wanted to convert these fontsheets to ASCII for easy import into your retro projects? Well, I have.

To make this process easier, I have created this handy tool. To convert a black and white fontsheet into an ASCII txt file containing a list of your font glyphs, compile and use the following syntax:

Usage: ./bmp2txt input_file output_filename tile_width tile_height

Output is to the executable folder to the txt file specified by {output_filename}.

The SDL2 library is required for compilation.
