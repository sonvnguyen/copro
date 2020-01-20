#ifdef PNG_PROGRESSIVE_READ_SUPPORTED
extern void png_push_save_buffer PNGARG((png_structp png_ptr));
extern void png_push_check_crc PNGARG((png_structp png_ptr));
#endif
//pngread.c... A lot of other functions calling png_push_save_buffer
void png_push_read_sig(png_structp png_ptr)
{
   png_byte sig[8];

   if (png_ptr->buffer_size < 8)
   {
      png_push_save_buffer(png_ptr);
      return;
   }

   png_push_fill_buffer(png_ptr, sig, 8);

   if (png_check_sig(sig, 8))
   {
      png_ptr->process_mode = PNG_READ_CHUNK_MODE;
   }
   else
   {
      png_error(png_ptr, "Not a PNG file");
   }
}
//...
#if defined(PNG_READ_gAMA_SUPPORTED)
   else if (!png_memcmp(png_ptr->push_chunk_name, png_gAMA, 4))
   {
      if (png_ptr->push_length + 4 > png_ptr->buffer_size)
      {
         png_push_save_buffer(png_ptr);
         return;
      }

      if (png_ptr->mode != PNG_HAVE_IHDR)
         png_error(png_ptr, "Out of place gAMA");

      png_handle_gAMA(png_ptr, info, png_ptr->push_length);
      png_push_check_crc(png_ptr);
   }
#endif
   //...