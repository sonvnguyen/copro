#if defined(PNG_READ_FILLER_SUPPORTED) || defined(PNG_WRITE_FILLER_SUPPORTED)
#define PNG_FILLER_BEFORE 0
#define PNG_FILLER_AFTER 1
/* Add a filler byte to rgb images. */
extern void png_set_filler PNGARG((png_structp png_ptr, int filler,
   int flags));

/* old ways of doing this, still supported through 1.x for backwards
   compatability, but not suggested */

/* Add a filler byte to rgb images after the colors. */
extern void png_set_rgbx PNGARG((png_structp png_ptr));

/* Add a filler byte to rgb images before the colors. */
extern void png_set_xrgb PNGARG((png_structp png_ptr));
#endif