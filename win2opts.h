/*---------------------------------------------------------------------
| win2opts.h
|
|	Define global options for the WIN2 2bpp greyscale library
|
|	(c) 1997-1998 ScumbySoft - wesc@ricochet.net
-----------------------------------------------------------------------*/


/*---------------------------------------------------------------------
| Global options for the Win2 library.  Undefine these to elminate
| APIs which you don't call to minimize code size
-----------------------------------------------------------------------*/
//#define WIN2DRAWBITMAP 1
#define WIN2DRAWCHARS  1
#define WIN2FILLRECT   1
#define WIN2DRAWLINE   1 // #defining WIN2FILLRECT too makes horiz lines much faster

/*---------------------------------------------------------------------
| Grey screen buffer Allocation options.  Define 1 of the following
-----------------------------------------------------------------------*/
// Create a database record (recommended over WIN2LOCKEDCHUNK)
//#define WIN2ALLOCDATABASE 1

// Allocate using locked chunks -- could cause mem leaks if you crash or program incorrectly
//#define WIN2USELOCKEDCHUNK 1

// Use MemPtrNew -- safest and smallest, but won't work if you have already used a lot of the local heap
#define WIN2ALLOCPTR 1

