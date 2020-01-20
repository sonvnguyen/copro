#if defined(PNG_INTERNAL)
#define PNG_FLAG_FILLER_AFTER             0x0100
#define PNG_BGR                0x0001
#define PNG_FILLER             0x8000
#define PNG_PACK               0x0004
#endif
//pngrtran.c
#if defined(PNG_READ_FILLER_SUPPORTED)
/* add filler byte */
void png_do_read_filler(png_row_infop row_info, png_bytep row,
   png_byte filler, png_byte flags)
{
   png_bytep sp, dp;
   png_uint_32 i;
   if (row && row_info && row_info->color_type == 2 &&
      row_info->bit_depth == 8)
   {
      if (flags & PNG_FLAG_FILLER_AFTER)
      {
         for (i = 1, sp = row + (png_size_t)row_info->width * 3,
            dp = row + (png_size_t)row_info->width * 4;
            i < row_info->width;
            i++)
         {
            *(--dp) = filler;
            *(--dp) = *(--sp);
            *(--dp) = *(--sp);
            *(--dp) = *(--sp);
         }
         *(--dp) = filler;
         row_info->channels = 4;
         row_info->pixel_depth = 32;
         row_info->rowbytes = row_info->width * 4;
      }
      else
      {
         for (i = 0, sp = row + (png_size_t)row_info->width * 3,
            dp = row + (png_size_t)row_info->width * 4;
            i < row_info->width;
            i++)
         {
            *(--dp) = *(--sp);
            *(--dp) = *(--sp);
            *(--dp) = *(--sp);
            *(--dp) = filler;
         }
         row_info->channels = 4;
         row_info->pixel_depth = 32;
         row_info->rowbytes = row_info->width * 4;
      }
   }
}
#endif
//pngtrans.c
#if defined(PNG_READ_FILLER_SUPPORTED) || defined(PNG_WRITE_FILLER_SUPPORTED)
void png_set_filler(png_structp png_ptr, int filler, int filler_loc)
{
   png_ptr->transformations |= PNG_FILLER;
   png_ptr->filler = (png_byte)filler;
   if (filler_loc == PNG_FILLER_AFTER)
      png_ptr->flags |= PNG_FLAG_FILLER_AFTER;
   else
      png_ptr->flags &= ~PNG_FLAG_FILLER_AFTER;

   if (png_ptr->color_type == PNG_COLOR_TYPE_RGB &&
      png_ptr->bit_depth == 8)
      png_ptr->usr_channels = 4;
}

/* old functions kept around for compatability purposes */
void png_set_rgbx(png_structp png_ptr)
{
   png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
}

void png_set_xrgb(png_structp png_ptr)
{
   png_set_filler(png_ptr, 0xff, PNG_FILLER_BEFORE);
}
#endif
//pngwtran.c
#ifdef PNG_WRITE_FILLER_SUPPORTED
/* remove filler byte */
void png_do_write_filler(png_row_infop row_info, png_bytep row,
   png_byte flags)
{
   if (row && row_info && row_info->color_type == PNG_COLOR_TYPE_RGB &&
      row_info->bit_depth == 8)
   {
      if (flags & PNG_FLAG_FILLER_AFTER)
      {
         png_bytep sp, dp;

         png_uint_32 i;

         for (i = 1, sp = row + 4, dp = row + 3;
            i < row_info->width;
            i++)
         {
            *dp++ = *sp++;
            *dp++ = *sp++;
            *dp++ = *sp++;
            sp++;
         }
         row_info->channels = 3;
         row_info->pixel_depth = 24;
         row_info->rowbytes = row_info->width * 3;
      }
      else
      {
         png_bytep sp, dp;
         png_uint_32 i;

         for (i = 0, sp = row, dp = row;
            i < row_info->width;
            i++)
         {
            sp++;
            *dp++ = *sp++;
            *dp++ = *sp++;
            *dp++ = *sp++;
         }
         row_info->channels = 3;
         row_info->pixel_depth = 24;
         row_info->rowbytes = row_info->width * 3;
      }
   }
}
#endif

//pngrtran.c
void png_do_read_transformations(png_structp png_ptr)
{
#if defined(PNG_READ_EXPAND_SUPPORTED)
   if ((png_ptr->transformations & PNG_EXPAND) &&
      png_ptr->row_info.color_type == PNG_COLOR_TYPE_PALETTE)
   {
      png_do_expand_palette(&(png_ptr->row_info), png_ptr->row_buf + 1,
         png_ptr->palette, png_ptr->trans, png_ptr->num_trans);
   }
   else if (png_ptr->transformations & PNG_EXPAND)
   {
      if (png_ptr->num_trans)
         png_do_expand(&(png_ptr->row_info), png_ptr->row_buf + 1,
            &(png_ptr->trans_values));
      else
         png_do_expand(&(png_ptr->row_info), png_ptr->row_buf + 1,
            NULL);
   }
#endif

#if defined(PNG_READ_BACKGROUND_SUPPORTED)
   if (png_ptr->transformations & PNG_BACKGROUND)
      png_do_background(&(png_ptr->row_info), png_ptr->row_buf + 1,
         &(png_ptr->trans_values), &(png_ptr->background),
         &(png_ptr->background_1),
         png_ptr->gamma_table, png_ptr->gamma_from_1,
         png_ptr->gamma_to_1, png_ptr->gamma_16_table,
         png_ptr->gamma_16_from_1, png_ptr->gamma_16_to_1,
         png_ptr->gamma_shift);
#endif

#if defined(PNG_READ_GAMMA_SUPPORTED)
   if ((png_ptr->transformations & PNG_GAMMA) &&
      !(png_ptr->transformations & PNG_BACKGROUND))
      png_do_gamma(&(png_ptr->row_info), png_ptr->row_buf + 1,
         png_ptr->gamma_table, png_ptr->gamma_16_table,
         png_ptr->gamma_shift);
#endif

#if defined(PNG_READ_16_TO_8_SUPPORTED)
   if (png_ptr->transformations & PNG_16_TO_8)
      png_do_chop(&(png_ptr->row_info), png_ptr->row_buf + 1);
#endif

#if defined(PNG_READ_DITHER_SUPPORTED)
   if (png_ptr->transformations & PNG_DITHER)
   {
      png_do_dither((png_row_infop)&(png_ptr->row_info), 
         png_ptr->row_buf + 1,
         png_ptr->palette_lookup,
         png_ptr->dither_index);
   }      
#endif

#if defined(PNG_READ_INVERT_SUPPORTED)
   if (png_ptr->transformations & PNG_INVERT_MONO)
      png_do_invert(&(png_ptr->row_info), png_ptr->row_buf + 1);
#endif

#if defined(PNG_READ_SHIFT_SUPPORTED)
   if (png_ptr->transformations & PNG_SHIFT)
      png_do_unshift(&(png_ptr->row_info), png_ptr->row_buf + 1,
         &(png_ptr->shift));
#endif

#if defined(PNG_READ_PACK_SUPPORTED)
   if (png_ptr->transformations & PNG_PACK)
      png_do_unpack(&(png_ptr->row_info), png_ptr->row_buf + 1);
#endif

#if defined(PNG_READ_BGR_SUPPORTED)
   if (png_ptr->transformations & PNG_BGR)
      png_do_bgr(&(png_ptr->row_info), png_ptr->row_buf + 1);
#endif

#if defined(PNG_READ_GRAY_TO_RGB_SUPPORTED)
   if (png_ptr->transformations & PNG_GRAY_TO_RGB)
      png_do_gray_to_rgb(&(png_ptr->row_info), png_ptr->row_buf + 1);
#endif

#if defined(PNG_READ_SWAP_SUPPORTED)
   if (png_ptr->transformations & PNG_SWAP_BYTES)
      png_do_swap(&(png_ptr->row_info), png_ptr->row_buf + 1);
#endif

#if defined(PNG_READ_FILLER_SUPPORTED)
   if (png_ptr->transformations & PNG_FILLER)
      png_do_read_filler(&(png_ptr->row_info), png_ptr->row_buf + 1,
         png_ptr->filler, png_ptr->flags);
#endif
}