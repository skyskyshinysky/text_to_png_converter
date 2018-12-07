# TextToPNGСonverter
Program to translate text into image. The program was developed as part of a laboratory work in which it was necessary to create a Makefile for all libraries and the developed application.
## Requirements
* zlib 1.2.11
* libpng 1.6.36
* freetype-2.9
## Building
Building with make (GNU Make 4.1) for Linux (Tested on Debian 9).

From the root project directory: `make all`. The assembly will be carried out in two modes - static and dynamic. After the build is completed, the static dependencies of the library will be located in **lib/static**. Shared library - **lib/shared**.

The application folder also contains two versions of the program. The dynamic application uses the `dlopen` and `dlsym` functions for working with shared libraries.
## Usage
In cmd/terminal:
 `./app_shared -m (phrase) -t (path to ttf file)`
##### Available options:
- **-h, --help**        <-- help message
- **-t, --ttf-file**    <-- specify the font file in ttf format
- **-m, --message**     <-- specify a phrase to display in the picture
- **-f, --file-output** <-- enter image file name
##### Example
Generate pictures with the phrase "Hello, Pusheen cat!": `./app_shared  -m "Hello, Pusheen cat!" -t Chiller.ttf`
##### Result
<p align="center">
   <img src="https://github.com/skyskyshinysky/text_to_png_converter/raw/develop/example_result/app.png">
</p>
