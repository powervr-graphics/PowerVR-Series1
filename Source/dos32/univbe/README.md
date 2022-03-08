Due to licensing concerns with re-distributing "The Universal VESA VBE" libraries and headers, they have been removed. the files removed from this folder are the following:

* `inc/debug.h`
* `inc/pmode.h`
* `inc/pmpro.h`
* `inc/uvbelib.h`
* `lib/pmode.lib`
* `lib/suvbelib.lib`
* `lib/uvbelib.lib`

VBE was use when constructing the SGL port of Tomb Raider. It was only used to set up the framebuffers, get the resolution, stride, pixel format and memory address of the framebuffer. These details where then sent to the PCX hardware so that it knew where and how to write the rendered pixels.