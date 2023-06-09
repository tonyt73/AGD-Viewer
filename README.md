# AGD-Viewer
An application to view Arcade Game Designer .agd text files

[![license](https://img.shields.io/github/license/tonyt73/AGD-Viewer?style=for-the-badge)](./LICENSE.txt)
[![release](https://img.shields.io/github/v/release/tonyt73/AGD-Viewer?include_prereleases&style=for-the-badge)](https://github.com/tonyt73/AGD-Viewer/releases)
[![downloads](https://img.shields.io/github/downloads/tonyt73/AGD-Viewer/total?style=for-the-badge)](https://github.com/tonyt73/AGD-Viewer/releases)

![stars](https://img.shields.io/github/stars/tonyt73/AGD-Viewer?style=social)
![forks](https://img.shields.io/github/forks/tonyt73/AGD-Viewer?style=social)
![watchers](https://img.shields.io/github/watchers/tonyt73/AGD-Viewer?style=social)

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
  * https://www.embarcadero.com/products/cbuilder/starter
  * Register if you need to (its free)
* Install the 32 bit and 64 bit Windows compilers (OSX, iOS & Android are optional)
* Launch the Rad-Studio IDE
  * Open up the location you cloned this repo to
  * Open the AGD Viewer.cproj file
* Press F9 to build and run the application
