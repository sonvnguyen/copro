//pngtrans.c
#if defined(PNG_READ_INTERLACING_SUPPORTED) || defined(PNG_WRITE_INTERLACING_SUPPORTED)
int png_set_interlace_handling(png_structp png_ptr)
{
   if (png_ptr->interlaced)
   {
      png_ptr->transformations |= PNG_INTERLACE;
      return 7;
   }

   return 1;
}
#endif

//pngread.c
void png_read_image(png_structp png_ptr, png_bytepp image)
{
   png_uint_32 i;
   int pass, j;
   png_bytepp rp;

   pass = png_set_interlace_handling(png_ptr);

   png_ptr->num_rows = png_ptr->height; /* Make sure this is set correctly */

   for (j = 0; j < pass; j++)
   {
      rp = image;
      for (i = 0; i < png_ptr->height; i++)
      {
         png_read_row(png_ptr, *rp, NULL);
         rp++;
      }
   }
}
//pngwrite.c
void png_write_image(png_structp png_ptr, png_bytepp image)
{
   png_uint_32 i; /* row index */
   int pass, num_pass; /* pass variables */
   png_bytepp rp; /* points to current row */

   /* intialize interlace handling.  If image is not interlaced,
      this will set pass to 1 */
   num_pass = png_set_interlace_handling(png_ptr);
   /* loop through passes */
   for (pass = 0; pass < num_pass; pass++)
   {
      /* loop through image */
      for (i = 0, rp = image; i < png_ptr->height; i++, rp++)
      {
         png_write_row(png_ptr, *rp);
      }
   }
}