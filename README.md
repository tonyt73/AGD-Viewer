# AGD-Viewer
An application to view Arcade Game Designer .agd text files

Load a .agd file and see it in all it's glory.


**Please DON'T violate** the original game authors rights by converting games that you haven't personally written or have the express permission of the author to use.


UI:
It's pretty simple.

* Folder -> Load a file
* Magnify buttons -> Zooming
* Play/Stop -> Animate the sprites (image page only)
* Checkboard -> Toggle between white/black and lime/black for the monochrome images

Ctrl+Mouse wheel -> Zoom

Mouse wheel -> Scroll Vertically

Supports:
* ZX Spectrum 8x8 attribute mode
* ZX Spectrum Next/Timex 8x1 attribute mode
* Amstrad CPC mode 0


## Build Instructions

* Clone this repo on to your PC
* Download the Embarcardero Rad-Studio C++ Community Edition
** https://www.embarcadero.com/products/cbuilder/starter

** Register if you need to (its free)
* Install the 32 bit and 64 bit Windows compilers (OSX, iOS & Android are optional)
* Launch the Rad-Studio IDE
** Open up the location you cloned this repo to
** Open the AGDx Viewer.cproj file
* Press F9 to build and run the application
