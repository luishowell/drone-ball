/*
 *****************************************************************************
 *                                                                           *
 *                          Platform Independent                             *
 *                    Bitmap Image Reader Writer Library                     *
 *                                                                           *
 * Author: Arash Partow - 2002                                               *
 * URL: http://partow.net/programming/bitmap/index.html                      *
 *                                                                           *
 * Note: This library only supports 24-bits per pixel bitmap format files.   *
 *                                                                           *
 * Copyright notice:                                                         *
 * Free use of the Platform Independent Bitmap Image Reader Writer Library   *
 * is permitted under the guidelines and in accordance with the most current *
 * version of the MIT License.                                               *
 * http://www.opensource.org/licenses/MIT                                    *
 *                                                                           *
 *****************************************************************************
*/


#ifndef INCLUDE_BITMAP_IMAGE_HPP
#define INCLUDE_BITMAP_IMAGE_HPP

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <vector>


class bitmap_image
{
public:

   enum channel_mode {
                        rgb_mode = 0,
                        bgr_mode = 1
                     };

   enum color_plane {
                       blue_plane  = 0,
                       green_plane = 1,
                       red_plane   = 2
                    };

   struct rgb_t
   {
      unsigned char   red;
      unsigned char green;
      unsigned char  blue;
   };

   bitmap_image()
   : file_name_(""),
     width_          (0),
     height_         (0),
     row_increment_  (0),
     bytes_per_pixel_(3),
     channel_mode_(bgr_mode)
   {}

   bitmap_image(const std::string& filename)
   : file_name_(filename),
     width_          (0),
     height_         (0),
     row_increment_  (0),
     bytes_per_pixel_(0),
     channel_mode_(bgr_mode)
   {
      load_bitmap();
   }

   bitmap_image(const unsigned int width, const unsigned int height)
   : file_name_(""),
     width_ (width ),
     height_(height),
     row_increment_  (0),
     bytes_per_pixel_(3),
     channel_mode_(bgr_mode)
   {
      create_bitmap();
   }

   bitmap_image(const bitmap_image& image)
   : file_name_(image.file_name_),
     width_    (image.width_    ),
     height_   (image.height_   ),
     row_increment_  (0),
     bytes_per_pixel_(3),
     channel_mode_(bgr_mode)
   {
      create_bitmap();
      data_ = image.data_;
   }

   bitmap_image& operator=(const bitmap_image& image)
   {
      if (this != &image)
      {
         file_name_       = image.file_name_;
         bytes_per_pixel_ = image.bytes_per_pixel_;
         width_           = image.width_;
         height_          = image.height_;
         row_increment_   = 0;
         channel_mode_    = image.channel_mode_;
         create_bitmap();
         data_ = image.data_;
      }

      return *this;
   }

   inline bool operator!()
   {
      return (data_.size()   == 0) ||
             (width_         == 0) ||
             (height_        == 0) ||
             (row_increment_ == 0);
   }

   inline void clear(const unsigned char v = 0x00)
   {
      std::fill(data_.begin(), data_.end(), v);
   }

   inline unsigned char red_channel(const unsigned int x, const unsigned int y) const
   {
      return data_[(y * row_increment_) + (x * bytes_per_pixel_ + 2)];
   }

   inline unsigned char green_channel(const unsigned int x, const unsigned int y) const
   {
      return data_[(y * row_increment_) + (x * bytes_per_pixel_ + 1)];
   }

   inline unsigned char blue_channel (const unsigned int x, const unsigned int y) const
   {
      return data_[(y * row_increment_) + (x * bytes_per_pixel_ + 0)];
   }

   inline void red_channel(const unsigned int x, const unsigned int y, const unsigned char value)
   {
      data_[(y * row_increment_) + (x * bytes_per_pixel_ + 2)] = value;
   }

   inline void green_channel(const unsigned int x, const unsigned int y, const unsigned char value)
   {
      data_[(y * row_increment_) + (x * bytes_per_pixel_ + 1)] = value;
   }

   inline void blue_channel (const unsigned int x, const unsigned int y, const unsigned char value)
   {
      data_[(y * row_increment_) + (x * bytes_per_pixel_ + 0)] = value;
   }

   inline unsigned char* row(unsigned int row_index) const
   {
      return const_cast<unsigned char*>(&data_[(row_index * row_increment_)]);
   }

   inline void get_pixel(const unsigned int x, const unsigned int y,
                         unsigned char& red,
                         unsigned char& green,
                         unsigned char& blue) const
   {
      const unsigned int y_offset = y * row_increment_;
      const unsigned int x_offset = x * bytes_per_pixel_;
      const unsigned int offset   = y_offset + x_offset;

      blue  = data_[offset + 0];
      green = data_[offset + 1];
      red   = data_[offset + 2];
   }

   template <typename RGB>
   inline void get_pixel(const unsigned int x, const unsigned int y, RGB& colour) const
   {
      get_pixel(x, y, colour.red, colour.green, colour.blue);
   }

   inline rgb_t get_pixel(const unsigned int x, const unsigned int y) const
   {
      rgb_t colour;
      get_pixel(x, y, colour.red, colour.green, colour.blue);
      return colour;
   }

   inline void set_pixel(const unsigned int x, const unsigned int y,
                         const unsigned char red,
                         const unsigned char green,
                         const unsigned char blue)
   {
      const unsigned int y_offset = y * row_increment_;
      const unsigned int x_offset = x * bytes_per_pixel_;
      const unsigned int offset   = y_offset + x_offset;

      data_[offset + 0] = blue;
      data_[offset + 1] = green;
      data_[offset + 2] = red;
   }

   template <typename RGB>
   inline void set_pixel(const unsigned int x, const unsigned int y, const RGB& colour)
   {
      set_pixel(x, y, colour.red, colour.green, colour.blue);
   }

   inline bool copy_from(const bitmap_image& image)
   {
      if (
           (image.height_ != height_) ||
           (image.width_  != width_ )
         )
      {
         return false;
      }

      data_ = image.data_;

      return true;
   }

   inline bool copy_from(const bitmap_image& source_image,
                         const unsigned int& x_offset,
                         const unsigned int& y_offset)
   {
      if ((x_offset + source_image.width_ ) > width_ ) { return false; }
      if ((y_offset + source_image.height_) > height_) { return false; }

      for (unsigned int y = 0; y < source_image.height_; ++y)
      {
         unsigned char* itr1           = row(y + y_offset) + x_offset * bytes_per_pixel_;
         const unsigned char* itr2     = source_image.row(y);
         const unsigned char* itr2_end = itr2 + source_image.width_ * bytes_per_pixel_;

         std::copy(itr2, itr2_end, itr1);
      }

      return true;
   }

   inline bool region(const unsigned int& x     ,
                      const unsigned int& y     ,
                      const unsigned int& width ,
                      const unsigned int& height,
                      bitmap_image& dest_image  ) const
   {
      if ((x + width ) > width_ ) { return false; }
      if ((y + height) > height_) { return false; }

      if (
           (dest_image.width_  < width_ ) ||
           (dest_image.height_ < height_)
         )
      {
         dest_image.setwidth_height(width,height);
      }

      for (unsigned int r = 0; r < height; ++r)
      {
         unsigned char* itr1     = row(r + y) + x * bytes_per_pixel_;
         unsigned char* itr1_end = itr1 + (width * bytes_per_pixel_);
         unsigned char* itr2     = dest_image.row(r);

         std::copy(itr1, itr1_end, itr2);
      }

      return true;
   }

   inline bool roi_from_center(const unsigned int& cx    ,
                               const unsigned int& cy    ,
                               const unsigned int& width ,
                               const unsigned int& height,
                               bitmap_image& dest_image  ) const
   {
      return region(cx - (width / 2), cy - (height / 2),
                    width, height,
                    dest_image);
   }

   inline bool set_region(const unsigned int&  x     ,
                          const unsigned int&  y     ,
                          const unsigned int&  width ,
                          const unsigned int&  height,
                          const unsigned char& value )
   {
      if ((x + width ) > width_ ) { return false; }
      if ((y + height) > height_) { return false; }

      for (unsigned int r = 0; r < height; ++r)
      {
         unsigned char* itr     = row(r + y) + x * bytes_per_pixel_;
         unsigned char* itr_end = itr + (width * bytes_per_pixel_);

         std::fill(itr, itr_end, value);
      }

      return true;
   }

   inline bool set_region(const unsigned int&  x     ,
                          const unsigned int&  y     ,
                          const unsigned int&  width ,
                          const unsigned int&  height,
                          const color_plane    color ,
                          const unsigned char& value )
   {
      if ((x + width ) > width_ ) { return false; }
      if ((y + height) > height_) { return false; }

      const unsigned int color_plane_offset = offset(color);

      for (unsigned int r = 0; r < height; ++r)
      {
         unsigned char* itr     = row(r + y) + x * bytes_per_pixel_ + color_plane_offset;
         unsigned char* itr_end = itr + (width * bytes_per_pixel_);

         while (itr != itr_end)
         {
            *itr  = value;
             itr += bytes_per_pixel_;
         }
      }

      return true;
   }

   inline bool set_region(const unsigned int&  x     ,
                          const unsigned int&  y     ,
                          const unsigned int&  width ,
                          const unsigned int&  height,
                          const unsigned char& red   ,
                          const unsigned char& green ,
                          const unsigned char& blue  )
   {
      if ((x + width ) > width_ ) { return false; }
      if ((y + height) > height_) { return false; }

      for (unsigned int r = 0; r < height; ++r)
      {
         unsigned char* itr     = row(r + y) + x * bytes_per_pixel_;
         unsigned char* itr_end = itr + (width * bytes_per_pixel_);

         while (itr != itr_end)
         {
            *(itr++) =  blue;
            *(itr++) = green;
            *(itr++) =   red;
         }
      }

      return true;
   }

   void reflective_image(bitmap_image& image, const bool include_diagnols = false)
   {
      image.setwidth_height(3 * width_, 3 * height_, true);

      image.copy_from(*this, width_, height_);

      vertical_flip();

      image.copy_from(*this, width_,           0);
      image.copy_from(*this, width_, 2 * height_);

      vertical_flip();
      horizontal_flip();

      image.copy_from(*this,          0, height_);
      image.copy_from(*this, 2 * width_, height_);

      horizontal_flip();

      if (include_diagnols)
      {
         bitmap_image tile = *this;

         tile.vertical_flip();
         tile.horizontal_flip();

         image.copy_from(tile,          0,           0);
         image.copy_from(tile, 2 * width_,           0);
         image.copy_from(tile, 2 * width_, 2 * height_);
         image.copy_from(tile, 0         , 2 * height_);
      }
   }

   inline unsigned int width() const
   {
      return width_;
   }

   inline unsigned int height() const
   {
      return height_;
   }

   inline unsigned int bytes_per_pixel() const
   {
      return bytes_per_pixel_;
   }

   inline unsigned int pixel_count() const
   {
      return width_ *  height_;
   }

   inline void setwidth_height(const unsigned int width,
                               const unsigned int height,
                               const bool clear = false)
   {
      data_.clear();
      width_  = width;
      height_ = height;

      create_bitmap();

      if (clear)
      {
         std::fill(data_.begin(), data_.end(), static_cast<unsigned char>(0x00));
      }
   }

   void save_image(const std::string& file_name) const
   {
      std::ofstream stream(file_name.c_str(),std::ios::binary);

      if (!stream)
      {
         std::cerr << "bitmap_image::save_image(): Error - Could not open file "  << file_name << " for writing!" << std::endl;
         return;
      }

      bitmap_information_header bih;

      bih.width            = width_;
      bih.height           = height_;
      bih.bit_count        = static_cast<unsigned short>(bytes_per_pixel_ << 3);
      bih.clr_important    = 0;
      bih.clr_used         = 0;
      bih.compression      = 0;
      bih.planes           = 1;
      bih.size             = bih.struct_size();
      bih.x_pels_per_meter = 0;
      bih.y_pels_per_meter = 0;
      bih.size_image       = (((bih.width * bytes_per_pixel_) + 3) & 0x0000FFFC) * bih.height;

      bitmap_file_header bfh;

      bfh.type             = 19778;
      bfh.size             = bfh.struct_size() + bih.struct_size() + bih.size_image;
      bfh.reserved1        = 0;
      bfh.reserved2        = 0;
      bfh.off_bits         = bih.struct_size() + bfh.struct_size();

      write_bfh(stream,bfh);
      write_bih(stream,bih);

      unsigned int padding = (4 - ((3 * width_) % 4)) % 4;
      char padding_data[4] = { 0x00, 0x00, 0x00, 0x00 };

      for (unsigned int i = 0; i < height_; ++i)
      {
         const unsigned char* data_ptr = &data_[(row_increment_ * (height_ - i - 1))];

         stream.write(reinterpret_cast<const char*>(data_ptr), sizeof(unsigned char) * bytes_per_pixel_ * width_);
         stream.write(padding_data,padding);
      }

      stream.close();
   }

   inline void set_all_ith_bits_low(const unsigned int bitr_index)
   {
      unsigned char mask = static_cast<unsigned char>(~(1 << bitr_index));

      for (unsigned char* itr = data(); itr != end(); ++itr)
      {
         *itr &= mask;
      }
   }

   inline void set_all_ith_bits_high(const unsigned int bitr_index)
   {
      unsigned char mask = static_cast<unsigned char>(1 << bitr_index);

      for (unsigned char* itr = data(); itr != end(); ++itr)
      {
         *itr |= mask;
      }
   }

   inline void set_all_ith_channels(const unsigned int& channel, const unsigned char& value)
   {
      for (unsigned char* itr = (data() + channel); itr < end(); itr += bytes_per_pixel_)
      {
         *itr = value;
      }
   }

   inline void set_channel(const color_plane color,const unsigned char& value)
   {
      for (unsigned char* itr = (data() + offset(color)); itr < end(); itr += bytes_per_pixel_)
      {
         *itr = value;
      }
   }

   inline void ror_channel(const color_plane color, const unsigned int& ror)
   {
      for (unsigned char* itr = (data() + offset(color)); itr < end(); itr += bytes_per_pixel_)
      {
         *itr = static_cast<unsigned char>(((*itr) >> ror) | ((*itr) << (8 - ror)));
      }
   }

   inline void set_all_channels(const unsigned char& value)
   {
      for (unsigned char* itr = data(); itr < end(); )
      {
         *(itr++) = value;
      }
   }

   inline void set_all_channels(const unsigned char& r_value,
                                const unsigned char& g_value,
                                const unsigned char& b_value)
   {
      for (unsigned char* itr = (data() + 0); itr < end(); itr += bytes_per_pixel_)
      {
         *(itr + 0) = b_value;
         *(itr + 1) = g_value;
         *(itr + 2) = r_value;
      }
   }

   inline void invert_color_planes()
   {
      for (unsigned char* itr = data(); itr < end(); *itr = ~(*itr), ++itr);
   }

   inline void add_to_color_plane(const color_plane color, const unsigned char& value)
   {
      for (unsigned char* itr = (data() + offset(color)); itr < end(); itr += bytes_per_pixel_)
      {
         (*itr) += value;
      }
   }

   inline void convert_to_grayscale()
   {
      double r_scaler = 0.299;
      double g_scaler = 0.587;
      double b_scaler = 0.114;

      if (rgb_mode == channel_mode_)
      {
         std::swap(r_scaler, b_scaler);
      }

      for (unsigned char* itr = data(); itr < end(); )
      {
         unsigned char gray_value = static_cast<unsigned char>
                       (
                         (r_scaler * (*(itr + 2))) +
                         (g_scaler * (*(itr + 1))) +
                         (b_scaler * (*(itr + 0)))
                       );

         *(itr++) = gray_value;
         *(itr++) = gray_value;
         *(itr++) = gray_value;
      }
   }

   inline const unsigned char* data() const
   {
      return data_.data();
   }

   inline unsigned char* data()
   {
      return const_cast<unsigned char*>(data_.data());
   }

   inline void bgr_to_rgb()
   {
      if ((bgr_mode == channel_mode_) && (3 == bytes_per_pixel_))
      {
         reverse_channels();
         channel_mode_ = rgb_mode;
      }
   }

   inline void rgb_to_bgr()
   {
      if ((rgb_mode == channel_mode_) && (3 == bytes_per_pixel_))
      {
         reverse_channels();
         channel_mode_ = bgr_mode;
      }
   }

   inline void reverse()
   {
      unsigned char* itr1 = data();
      unsigned char* itr2 = end() - bytes_per_pixel_;

      while (itr1 < itr2)
      {
         for (std::size_t i = 0; i < bytes_per_pixel_; ++i)
         {
            unsigned char* citr1 = itr1 + i;
            unsigned char* citr2 = itr2 + i;

            std::swap(*citr1,*citr2);
         }

         itr1 += bytes_per_pixel_;
         itr2 -= bytes_per_pixel_;
      }
   }

   inline void horizontal_flip()
   {
      for (unsigned int y = 0; y < height_; ++y)
      {
         unsigned char* itr1 = row(y);
         unsigned char* itr2 = itr1 + row_increment_ - bytes_per_pixel_;

         while (itr1 < itr2)
         {
            for (unsigned int i = 0; i < bytes_per_pixel_; ++i)
            {
               unsigned char* p1 = (itr1 + i);
               unsigned char* p2 = (itr2 + i);

               std::swap(*p1,*p2);
            }

            itr1 += bytes_per_pixel_;
            itr2 -= bytes_per_pixel_;
         }
      }
   }

   inline void vertical_flip()
   {
      for (unsigned int y = 0; y < (height_ / 2); ++y)
      {
         unsigned char* itr1 = row(y);
         unsigned char* itr2 = row(height_ - y - 1);

         for (std::size_t x = 0; x < row_increment_; ++x)
         {
            std::swap(*(itr1 + x),*(itr2 + x));
         }
      }
   }

   inline void export_color_plane(const color_plane color, unsigned char* image)
   {
      for (unsigned char* itr = (data() + offset(color)); itr < end(); ++image, itr += bytes_per_pixel_)
      {
         (*image) = (*itr);
      }
   }

   inline void export_color_plane(const color_plane color, bitmap_image& image)
   {
      if (
           (width_  != image.width_ ) ||
           (height_ != image.height_)
         )
      {
         image.setwidth_height(width_,height_);
      }

      image.clear();

      unsigned char* itr1     = (data() + offset(color));
      unsigned char* itr1_end = end();
      unsigned char* itr2     = (image.data() + offset(color));

      while (itr1 < itr1_end)
      {
         (*itr2) = (*itr1);

         itr1 += bytes_per_pixel_;
         itr2 += bytes_per_pixel_;
      }
   }

   inline void export_response_image(const color_plane color, double* response_image)
   {
      double* resp_itr = response_image;

      for (unsigned char* itr = (data() + offset(color)); itr < end(); ++response_image, itr += bytes_per_pixel_)
      {
         *(resp_itr++) = (1.0 * (*itr)) / 256.0;
      }
   }

   inline void export_gray_scale_response_image(double* response_image) const
   {
      double* resp_itr = response_image;

      for (const unsigned char* itr = data(); itr < end(); itr += bytes_per_pixel_)
      {
         unsigned char gray_value = static_cast<unsigned char>
                       (
                         (0.299 * (*(itr + 2))) +
                         (0.587 * (*(itr + 1))) +
                         (0.114 * (*(itr + 0)))
                       );

         *(resp_itr++) = (1.0 * gray_value) / 256.0;
      }
   }

   inline void export_rgb(double* red, double* green, double* blue) const
   {
      if (bgr_mode != channel_mode_)
         return;

      for (const unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         (*blue ) = (1.0 * (*(itr++))) / 256.0;
         (*green) = (1.0 * (*(itr++))) / 256.0;
         (*red  ) = (1.0 * (*(itr++))) / 256.0;
      }
   }

   inline void export_rgb(float* red, float* green, float* blue) const
   {
      if (bgr_mode != channel_mode_)
         return;

      for (const unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         (*blue ) = (1.0f * (*(itr++))) / 256.0f;
         (*green) = (1.0f * (*(itr++))) / 256.0f;
         (*red  ) = (1.0f * (*(itr++))) / 256.0f;
      }
   }

   inline void export_rgb(unsigned char* red, unsigned char* green, unsigned char* blue) const
   {
      if (bgr_mode != channel_mode_)
         return;

      for (const unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         (*blue ) = *(itr++);
         (*green) = *(itr++);
         (*red  ) = *(itr++);
      }
   }

   inline void export_ycbcr(double* y, double* cb, double* cr) const
   {
      if (bgr_mode != channel_mode_)
         return;

      for (const unsigned char* itr = data(); itr < end(); ++y, ++cb, ++cr)
      {
         const double blue  = (1.0 * (*(itr++)));
         const double green = (1.0 * (*(itr++)));
         const double red   = (1.0 * (*(itr++)));

         ( *y) = clamp<double>( 16.0 + (1.0/256.0) * (  65.738 * red + 129.057 * green +  25.064 * blue),1.0,254);
         (*cb) = clamp<double>(128.0 + (1.0/256.0) * (- 37.945 * red -  74.494 * green + 112.439 * blue),1.0,254);
         (*cr) = clamp<double>(128.0 + (1.0/256.0) * ( 112.439 * red -  94.154 * green -  18.285 * blue),1.0,254);
      }
   }

   inline void export_rgb_normal(double* red, double* green, double* blue) const
   {
      if (bgr_mode != channel_mode_)
         return;

      for (const unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         (*blue ) = (1.0 * (*(itr++)));
         (*green) = (1.0 * (*(itr++)));
         (*red  ) = (1.0 * (*(itr++)));
      }
   }

   inline void export_rgb_normal(float* red, float* green, float* blue) const
   {
      if (bgr_mode != channel_mode_)
         return;

      for (const unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         (*blue ) = (1.0f * (*(itr++)));
         (*green) = (1.0f * (*(itr++)));
         (*red  ) = (1.0f * (*(itr++)));
      }
   }

   inline void import_rgb(double* red, double* green, double* blue)
   {
      if (bgr_mode != channel_mode_)
         return;

      for (unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(256.0 * (*blue ));
         *(itr++) = static_cast<unsigned char>(256.0 * (*green));
         *(itr++) = static_cast<unsigned char>(256.0 * (*red  ));
      }
   }

   inline void import_rgb(float* red, float* green, float* blue)
   {
      if (bgr_mode != channel_mode_)
         return;

      for (unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(256.0f * (*blue ));
         *(itr++) = static_cast<unsigned char>(256.0f * (*green));
         *(itr++) = static_cast<unsigned char>(256.0f * (*red  ));
      }
   }

   inline void import_rgb(unsigned char* red, unsigned char* green, unsigned char* blue)
   {
      if (bgr_mode != channel_mode_)
         return;

      for (unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         *(itr++) = (*blue );
         *(itr++) = (*green);
         *(itr++) = (*red  );
      }
   }

   inline void import_ycbcr(double* y, double* cb, double* cr)
   {
      if (bgr_mode != channel_mode_)
         return;

      for (unsigned char* itr = data(); itr < end(); ++y, ++cb, ++cr)
      {
         double y_  =  (*y);
         double cb_ = (*cb);
         double cr_ = (*cr);

         *(itr++) = static_cast<unsigned char>(clamp((298.082 * y_ + 516.412 * cb_                 ) / 256.0 - 276.836,0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp((298.082 * y_ - 100.291 * cb_ - 208.120 * cr_ ) / 256.0 + 135.576,0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp((298.082 * y_                 + 408.583 * cr_ ) / 256.0 - 222.921,0.0,255.0));
      }
   }

   inline void import_gray_scale_clamped(double* gray)
   {
      if (bgr_mode != channel_mode_)
         return;

      for (unsigned char* itr = data(); itr < end(); ++gray)
      {
         unsigned char c = static_cast<unsigned char>(clamp<double>(256.0 * (*gray),0.0,255.0));

         *(itr + 0) = c;
         *(itr + 1) = c;
         *(itr + 2) = c;

         itr += 3;
      }
   }

   inline void import_rgb_clamped(double* red, double* green, double* blue)
   {
      if (bgr_mode != channel_mode_)
         return;

      for (unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0 * (*blue ),0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0 * (*green),0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0 * (*red  ),0.0,255.0));
      }
   }

   inline void import_rgb_clamped(float* red, float* green, float* blue)
   {
      if (bgr_mode != channel_mode_)
         return;

      for (unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0f * (*blue ),0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0f * (*green),0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0f * (*red  ),0.0,255.0));
      }
   }

   inline void import_rgb_normal(double* red, double* green, double* blue)
   {
      if (bgr_mode != channel_mode_)
         return;

      for (unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(*blue );
         *(itr++) = static_cast<unsigned char>(*green);
         *(itr++) = static_cast<unsigned char>(*red  );
      }
   }

   inline void import_rgb_normal(float* red, float* green, float* blue)
   {
      if (bgr_mode != channel_mode_)
         return;

      for (unsigned char* itr = data(); itr < end(); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(*blue );
         *(itr++) = static_cast<unsigned char>(*green);
         *(itr++) = static_cast<unsigned char>(*red  );
      }
   }

   inline void subsample(bitmap_image& dest) const
   {
      /*
         Half sub-sample of original image.
      */
      unsigned int w = 0;
      unsigned int h = 0;

      bool odd_width = false;
      bool odd_height = false;

      if (0 == (width_ % 2))
         w = width_ / 2;
      else
      {
         w = 1 + (width_ / 2);
         odd_width = true;
      }

      if (0 == (height_ % 2))
         h = height_ / 2;
      else
      {
         h = 1 + (height_ / 2);
         odd_height = true;
      }

      unsigned int horizontal_upper = (odd_width)  ? (w - 1) : w;
      unsigned int vertical_upper   = (odd_height) ? (h - 1) : h;

      dest.setwidth_height(w,h);
      dest.clear();

            unsigned char* s_itr[3];
      const unsigned char*  itr1[3];
      const unsigned char*  itr2[3];

      s_itr[0] = dest.data() + 0;
      s_itr[1] = dest.data() + 1;
      s_itr[2] = dest.data() + 2;

      itr1[0] = data() + 0;
      itr1[1] = data() + 1;
      itr1[2] = data() + 2;

      itr2[0] = data() + row_increment_ + 0;
      itr2[1] = data() + row_increment_ + 1;
      itr2[2] = data() + row_increment_ + 2;

      unsigned int total = 0;

      for (unsigned int j = 0; j < vertical_upper; ++j)
      {
         for (unsigned int i = 0; i < horizontal_upper; ++i)
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; s_itr[k] += bytes_per_pixel_, ++k)
            {
               total = 0;
               total += *(itr1[k]);
               total += *(itr1[k]);
               total += *(itr2[k]);
               total += *(itr2[k]);

               itr1[k] += bytes_per_pixel_;
               itr1[k] += bytes_per_pixel_;
               itr2[k] += bytes_per_pixel_;
               itr2[k] += bytes_per_pixel_;

               *(s_itr[k]) = static_cast<unsigned char>(total >> 2);
            }
         }

         if (odd_width)
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; s_itr[k] += bytes_per_pixel_, ++k)
            {
               total = 0;
               total += *(itr1[k]);
               total += *(itr2[k]);

               itr1[k] += bytes_per_pixel_;
               itr2[k] += bytes_per_pixel_;

               *(s_itr[k]) = static_cast<unsigned char>(total >> 1);
            }
         }

         for (unsigned int k = 0; k < bytes_per_pixel_; ++k)
         {
            itr1[k] += row_increment_;
         }

         if (j != (vertical_upper - 1))
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; ++k)
            {
               itr2[k] += row_increment_;
            }
         }
      }

      if (odd_height)
      {
         for (unsigned int i = 0; i < horizontal_upper; ++i)
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; s_itr[k] += bytes_per_pixel_, ++k)
            {
               total = 0;
               total += *(itr1[k]);
               total += *(itr2[k]);

               itr1[k] += bytes_per_pixel_;
               itr2[k] += bytes_per_pixel_;

               *(s_itr[k]) = static_cast<unsigned char>(total >> 1);
            }
         }

         if (odd_width)
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; ++k)
            {
               (*(s_itr[k])) = *(itr1[k]);
            }
         }
      }
   }

   inline void upsample(bitmap_image& dest) const
   {
      /*
         2x up-sample of original image.
      */

      dest.setwidth_height(2 * width_ ,2 * height_);
      dest.clear();

      const unsigned char* s_itr[3];
            unsigned char*  itr1[3];
            unsigned char*  itr2[3];

      s_itr[0] = data() + 0;
      s_itr[1] = data() + 1;
      s_itr[2] = data() + 2;

      itr1[0] = dest.data() + 0;
      itr1[1] = dest.data() + 1;
      itr1[2] = dest.data() + 2;

      itr2[0] = dest.data() + dest.row_increment_ + 0;
      itr2[1] = dest.data() + dest.row_increment_ + 1;
      itr2[2] = dest.data() + dest.row_increment_ + 2;

      for (unsigned int j = 0; j < height_; ++j)
      {
         for (unsigned int i = 0; i < width_; ++i)
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; s_itr[k] += bytes_per_pixel_, ++k)
            {
               *(itr1[k]) = *(s_itr[k]); itr1[k] += bytes_per_pixel_;
               *(itr1[k]) = *(s_itr[k]); itr1[k] += bytes_per_pixel_;

               *(itr2[k]) = *(s_itr[k]); itr2[k] += bytes_per_pixel_;
               *(itr2[k]) = *(s_itr[k]); itr2[k] += bytes_per_pixel_;
            }
         }

         for (unsigned int k = 0; k < bytes_per_pixel_; ++k)
         {
            itr1[k] += dest.row_increment_;
            itr2[k] += dest.row_increment_;
         }
      }
   }

   inline void alpha_blend(const double& alpha, const bitmap_image& image)
   {
      if (
           (image.width_  != width_ ) ||
           (image.height_ != height_)
         )
      {
         return;
      }

      if ((alpha < 0.0) || (alpha > 1.0))
      {
         return;
      }

      unsigned char* itr1           = data();
      const unsigned char* itr1_end = end();
      const unsigned char* itr2     = image.data();

      double alpha_compliment = 1.0 - alpha;

      while (itr1 != itr1_end)
      {
         *(itr1) = static_cast<unsigned char>((alpha * (*itr2)) + (alpha_compliment * (*itr1)));
         ++itr1;
         ++itr2;
      }
   }

   inline double psnr(const bitmap_image& image)
   {
      if (
           (image.width_  != width_ ) ||
           (image.height_ != height_)
         )
      {
         return 0.0;
      }

      const unsigned char* itr1 = data();
      const unsigned char* itr2 = image.data();

      double mse = 0.0;

      while (itr1 != end())
      {
         const double v = (static_cast<double>(*itr1) - static_cast<double>(*itr2));

         mse += v * v;
         ++itr1;
         ++itr2;
      }

      if (mse <= 0.0000001)
      {
         return 1000000.0;
      }
      else
      {
         mse /= (3.0 * width_ * height_);

         return 20.0 * std::log10(255.0 / std::sqrt(mse));
      }
   }

   inline double psnr(const unsigned int& x,
                      const unsigned int& y,
                      const bitmap_image& image)
   {
      if ((x + image.width() ) > width_ ) { return 0.0; }
      if ((y + image.height()) > height_) { return 0.0; }

      double mse = 0.0;

      const unsigned int height = image.height();
      const unsigned int width  = image.width();

      for (unsigned int r = 0; r < height; ++r)
      {
         const unsigned char* itr1     = row(r + y) + x * bytes_per_pixel_;
         const unsigned char* itr1_end = itr1 + (width * bytes_per_pixel_);
         const unsigned char* itr2     = image.row(r);

         while (itr1 != itr1_end)
         {
            double v = (static_cast<double>(*itr1) - static_cast<double>(*itr2));
            mse += v * v;
            ++itr1;
            ++itr2;
         }
      }

      if (mse <= 0.0000001)
      {
         return 1000000.0;
      }
      else
      {
         mse /= (3.0 * image.width() * image.height());
         return 20.0 * std::log10(255.0 / std::sqrt(mse));
      }
   }

   inline void histogram(const color_plane color, double hist[256]) const
   {
      std::fill(hist, hist + 256, 0.0);

      for (const unsigned char* itr = (data() + offset(color)); itr < end(); itr += bytes_per_pixel_)
      {
         ++hist[(*itr)];
      }
   }

   inline void histogram_normalized(const color_plane color, double hist[256]) const
   {
      histogram(color,hist);

      double* h_itr = hist;
      const double* h_end = hist + 256;
      const double pixel_count = static_cast<double>(width_ * height_);

      while (h_end != h_itr)
      {
         *(h_itr++) /= pixel_count;
      }
   }

   inline unsigned int offset(const color_plane color) const
   {
      switch (channel_mode_)
      {
         case rgb_mode : {
                            switch (color)
                            {
                               case red_plane   : return 0;
                               case green_plane : return 1;
                               case blue_plane  : return 2;
                               default          : return std::numeric_limits<unsigned int>::max();
                            }
                         }

         case bgr_mode : {
                            switch (color)
                            {
                               case red_plane   : return 2;
                               case green_plane : return 1;
                               case blue_plane  : return 0;
                               default          : return std::numeric_limits<unsigned int>::max();
                            }
                         }

         default       : return std::numeric_limits<unsigned int>::max();
      }
   }

   inline void incremental()
   {
      unsigned char current_color = 0;

      for (unsigned char* itr = data(); itr < end();)
      {
         (*itr++) = (current_color);
         (*itr++) = (current_color);
         (*itr++) = (current_color);

         ++current_color;
      }
   }

   inline void reverse_channels()
   {
      if (3 != bytes_per_pixel_)
         return;

      for (unsigned char* itr = data(); itr < end(); itr += bytes_per_pixel_)
      {
         std::swap(*(itr + 0),*(itr + 2));
      }
   }



   inline const unsigned char* end() const
   {
      return data_.data() + data_.size();
   }

   inline unsigned char* end()
   {
      return const_cast<unsigned char*>(data() + data_.size());
   }

   struct bitmap_file_header
   {
      unsigned short type;
      unsigned int   size;
      unsigned short reserved1;
      unsigned short reserved2;
      unsigned int   off_bits;

      unsigned int struct_size() const
      {
         return sizeof(type     ) +
                sizeof(size     ) +
                sizeof(reserved1) +
                sizeof(reserved2) +
                sizeof(off_bits ) ;
      }

      void clear()
      {
         std::memset(this, 0x00, sizeof(bitmap_file_header));
      }
   };

   struct bitmap_information_header
   {
      unsigned int   size;
      unsigned int   width;
      unsigned int   height;
      unsigned short planes;
      unsigned short bit_count;
      unsigned int   compression;
      unsigned int   size_image;
      unsigned int   x_pels_per_meter;
      unsigned int   y_pels_per_meter;
      unsigned int   clr_used;
      unsigned int   clr_important;

      unsigned int struct_size() const
      {
         return sizeof(size            ) +
                sizeof(width           ) +
                sizeof(height          ) +
                sizeof(planes          ) +
                sizeof(bit_count       ) +
                sizeof(compression     ) +
                sizeof(size_image      ) +
                sizeof(x_pels_per_meter) +
                sizeof(y_pels_per_meter) +
                sizeof(clr_used        ) +
                sizeof(clr_important   ) ;
      }

      void clear()
      {
         std::memset(this, 0x00, sizeof(bitmap_information_header));
      }
   };

   inline bool big_endian() const
   {
      unsigned int v = 0x01;

      return (1 != reinterpret_cast<char*>(&v)[0]);
   }

   inline unsigned short flip(const unsigned short& v) const
   {
      return ((v >> 8) | (v << 8));
   }

   inline unsigned int flip(const unsigned int& v) const
   {
      return (
               ((v & 0xFF000000) >> 0x18) |
               ((v & 0x000000FF) << 0x18) |
               ((v & 0x00FF0000) >> 0x08) |
               ((v & 0x0000FF00) << 0x08)
             );
   }

   template <typename T>
   inline void read_from_stream(std::ifstream& stream,T& t)
   {
      stream.read(reinterpret_cast<char*>(&t),sizeof(T));
   }

   template <typename T>
   inline void write_to_stream(std::ofstream& stream,const T& t) const
   {
      stream.write(reinterpret_cast<const char*>(&t),sizeof(T));
   }

   inline void read_bfh(std::ifstream& stream, bitmap_file_header& bfh)
   {
      read_from_stream(stream,bfh.type     );
      read_from_stream(stream,bfh.size     );
      read_from_stream(stream,bfh.reserved1);
      read_from_stream(stream,bfh.reserved2);
      read_from_stream(stream,bfh.off_bits );

      if (big_endian())
      {
         bfh.type      = flip(bfh.type     );
         bfh.size      = flip(bfh.size     );
         bfh.reserved1 = flip(bfh.reserved1);
         bfh.reserved2 = flip(bfh.reserved2);
         bfh.off_bits  = flip(bfh.off_bits );
      }
   }

   inline void write_bfh(std::ofstream& stream, const bitmap_file_header& bfh) const
   {
      if (big_endian())
      {
         write_to_stream(stream,flip(bfh.type     ));
         write_to_stream(stream,flip(bfh.size     ));
         write_to_stream(stream,flip(bfh.reserved1));
         write_to_stream(stream,flip(bfh.reserved2));
         write_to_stream(stream,flip(bfh.off_bits ));
      }
      else
      {
         write_to_stream(stream,bfh.type     );
         write_to_stream(stream,bfh.size     );
         write_to_stream(stream,bfh.reserved1);
         write_to_stream(stream,bfh.reserved2);
         write_to_stream(stream,bfh.off_bits );
      }
   }

   inline void read_bih(std::ifstream& stream,bitmap_information_header& bih)
   {
      read_from_stream(stream,bih.size            );
      read_from_stream(stream,bih.width           );
      read_from_stream(stream,bih.height          );
      read_from_stream(stream,bih.planes          );
      read_from_stream(stream,bih.bit_count       );
      read_from_stream(stream,bih.compression     );
      read_from_stream(stream,bih.size_image      );
      read_from_stream(stream,bih.x_pels_per_meter);
      read_from_stream(stream,bih.y_pels_per_meter);
      read_from_stream(stream,bih.clr_used        );
      read_from_stream(stream,bih.clr_important   );

      if (big_endian())
      {
         bih.size          = flip(bih.size               );
         bih.width         = flip(bih.width              );
         bih.height        = flip(bih.height             );
         bih.planes        = flip(bih.planes             );
         bih.bit_count     = flip(bih.bit_count          );
         bih.compression   = flip(bih.compression        );
         bih.size_image    = flip(bih.size_image         );
         bih.x_pels_per_meter = flip(bih.x_pels_per_meter);
         bih.y_pels_per_meter = flip(bih.y_pels_per_meter);
         bih.clr_used      = flip(bih.clr_used           );
         bih.clr_important = flip(bih.clr_important      );
      }
   }

   inline void write_bih(std::ofstream& stream, const bitmap_information_header& bih) const
   {
      if (big_endian())
      {
         write_to_stream(stream,flip(bih.size            ));
         write_to_stream(stream,flip(bih.width           ));
         write_to_stream(stream,flip(bih.height          ));
         write_to_stream(stream,flip(bih.planes          ));
         write_to_stream(stream,flip(bih.bit_count       ));
         write_to_stream(stream,flip(bih.compression     ));
         write_to_stream(stream,flip(bih.size_image      ));
         write_to_stream(stream,flip(bih.x_pels_per_meter));
         write_to_stream(stream,flip(bih.y_pels_per_meter));
         write_to_stream(stream,flip(bih.clr_used        ));
         write_to_stream(stream,flip(bih.clr_important   ));
      }
      else
      {
         write_to_stream(stream,bih.size            );
         write_to_stream(stream,bih.width           );
         write_to_stream(stream,bih.height          );
         write_to_stream(stream,bih.planes          );
         write_to_stream(stream,bih.bit_count       );
         write_to_stream(stream,bih.compression     );
         write_to_stream(stream,bih.size_image      );
         write_to_stream(stream,bih.x_pels_per_meter);
         write_to_stream(stream,bih.y_pels_per_meter);
         write_to_stream(stream,bih.clr_used        );
         write_to_stream(stream,bih.clr_important   );
      }
   }

   inline std::size_t file_size(const std::string& file_name) const
   {
      std::ifstream file(file_name.c_str(),std::ios::in | std::ios::binary);
      if (!file) return 0;
      file.seekg (0, std::ios::end);
      return static_cast<std::size_t>(file.tellg());
   }

   void create_bitmap()
   {
      row_increment_ = width_ * bytes_per_pixel_;
      data_.resize(height_ * row_increment_);
   }

   void load_bitmap()
   {
      std::ifstream stream(file_name_.c_str(),std::ios::binary);

      if (!stream)
      {
         std::cerr << "bitmap_image::load_bitmap() ERROR: bitmap_image - file " << file_name_ << " not found!" << std::endl;
         return;
      }

      width_  = 0;
      height_ = 0;

      bitmap_file_header bfh;
      bitmap_information_header bih;

      bfh.clear();
      bih.clear();

      read_bfh(stream,bfh);
      read_bih(stream,bih);

      if (bfh.type != 19778)
      {
         bfh.clear();
         bih.clear();

         stream.close();

         std::cerr << "bitmap_image::load_bitmap() ERROR: bitmap_image - Invalid type value " << bfh.type << " expected 19778." << std::endl;
         return;
      }

      if (bih.bit_count != 24)
      {
         bfh.clear();
         bih.clear();

         stream.close();

         std::cerr << "bitmap_image::load_bitmap() ERROR: bitmap_image - Invalid bit depth " << bih.bit_count << " expected 24." << std::endl;

         return;
      }

      if (bih.size != bih.struct_size())
      {
         bfh.clear();
         bih.clear();

         stream.close();

         std::cerr << "bitmap_image::load_bitmap() ERROR: bitmap_image - Invalid BIH size " << bih.size << " expected " << bih.struct_size() << std::endl;

         return;
      }

      width_  = bih.width;
      height_ = bih.height;

      bytes_per_pixel_ = bih.bit_count >> 3;

      unsigned int padding = (4 - ((3 * width_) % 4)) % 4;
      char padding_data[4] = {0,0,0,0};

      std::size_t bitmap_file_size = file_size(file_name_);

      std::size_t bitmap_logical_size = (height_ * width_ * bytes_per_pixel_) +
                                        (height_ * padding)                   +
                                         bih.struct_size()                    +
                                         bfh.struct_size()                    ;

      if (bitmap_file_size != bitmap_logical_size)
      {
         bfh.clear();
         bih.clear();

         stream.close();

         std::cerr << "bitmap_image::load_bitmap() ERROR: bitmap_image - Mismatch between logical and physical sizes of bitmap. " <<
                      "Logical: "  << bitmap_logical_size << " " <<
                      "Physical: " << bitmap_file_size    << std::endl;

         return;
      }

      create_bitmap();

      for (unsigned int i = 0; i < height_; ++i)
      {
         unsigned char* data_ptr = row(height_ - i - 1); // read in inverted row order

         stream.read(reinterpret_cast<char*>(data_ptr), sizeof(char) * bytes_per_pixel_ * width_);
         stream.read(padding_data,padding);
      }
   }

   template <typename T>
   inline T clamp(const T& v, const T& lower_range, const T& upper_range) const
   {
      if (v < lower_range)
         return lower_range;
      else if (v >  upper_range)
         return upper_range;
      else
         return v;
   }

   std::string  file_name_;
   unsigned int width_;
   unsigned int height_;
   unsigned int row_increment_;
   unsigned int bytes_per_pixel_;
   channel_mode channel_mode_;
   std::vector<unsigned char> data_;
};

typedef bitmap_image::rgb_t rgb_t;

inline bool operator==(const rgb_t& c0, const rgb_t& c1)
{
   return (c0.red   == c1  .red) &&
          (c0.green == c1.green) &&
          (c0.blue  == c1 .blue) ;
}

inline bool operator!=(const rgb_t& c0, const rgb_t& c1)
{
   return (c0.red   != c1  .red) ||
          (c0.green != c1.green) ||
          (c0.blue  != c1 .blue) ;
}

inline std::size_t hamming_distance(const rgb_t& c0, const rgb_t& c1)
{
   std::size_t result = 0;

   if (c0.red   != c1  .red) ++result;
   if (c0.green != c1.green) ++result;
   if (c0.blue  != c1 .blue) ++result;

   return result;
}

inline rgb_t make_colour(const unsigned int& red, const unsigned int& green, const unsigned int& blue)
{
   rgb_t result;

   result.red   = static_cast<unsigned char>(red  );
   result.green = static_cast<unsigned char>(green);
   result.blue  = static_cast<unsigned char>(blue );

   return result;
}

template <typename OutputIterator>
inline void generate_colours(const std::size_t& steps, const rgb_t c0, const rgb_t& c1, OutputIterator out)
{
   double dr = ((double)c1.red   -  (double)c0.red   ) / steps;
   double dg = ((double)c1.green -  (double)c0.green ) / steps;
   double db = ((double)c1.blue  -  (double)c0.blue  ) / steps;

   for (std::size_t i = 0; i < steps; ++i)
   {
      rgb_t c;

      c.red   = static_cast<unsigned char>(c0.red   + (i * dr));
      c.green = static_cast<unsigned char>(c0.green + (i * dg));
      c.blue  = static_cast<unsigned char>(c0.blue  + (i * db));

      *(out++) = c;
   }
}

template <typename ResponseImage, typename Palette>
inline std::size_t convert_rsp_to_image(const ResponseImage& resp_image, const Palette& palette, bitmap_image& image)
{
   if (
        (resp_image.width () > image.width ()) ||
        (resp_image.height() > image.height())
      )
      return 0;

   for (std::size_t y = 0; y < resp_image.height(); ++y)
   {
      for (std::size_t x = 0; x < resp_image.width(); ++x)
      {
         const double v = resp_image(x,y);

         unsigned int index = static_cast<unsigned int>((v < 0) ? 0 : v > (palette.size()) ? (palette.size() - 1) : v);

         image.set_pixel(x,y,palette[index]);
      }
   }

   return (resp_image.width() * resp_image.height());
}

inline void rgb_to_ycbcr(const unsigned int& length, double* red, double* green, double* blue,
                                                     double* y,   double* cb,    double* cr)
{
   unsigned int i = 0;

   while (i < length)
   {
      ( *y) =   16.0 + (  65.481 * (*red) +  128.553 * (*green) +  24.966 * (*blue));
      (*cb) =  128.0 + ( -37.797 * (*red) +  -74.203 * (*green) + 112.000 * (*blue));
      (*cr) =  128.0 + ( 112.000 * (*red) +  -93.786 * (*green) -  18.214 * (*blue));

      ++i;
      ++red; ++green; ++blue;
      ++y;   ++cb;    ++cr;
   }
}

inline void ycbcr_to_rgb(const unsigned int& length, double* y,   double* cb,    double* cr,
                                                     double* red, double* green, double* blue)
{
   unsigned int i = 0;

   while (i < length)
   {
      double y_  =  (*y) -  16.0;
      double cb_ = (*cb) - 128.0;
      double cr_ = (*cr) - 128.0;

        (*red) = 0.000456621 * y_                    + 0.00625893 * cr_;
      (*green) = 0.000456621 * y_ - 0.00153632 * cb_ - 0.00318811 * cr_;
       (*blue) = 0.000456621 * y_                    + 0.00791071 * cb_;

      ++i;
      ++red; ++green; ++blue;
      ++y;   ++cb;    ++cr;
   }
}

inline void subsample(const unsigned int& width,
                      const unsigned int& height,
                      const double* source,
                      unsigned int& w,
                      unsigned int& h,
                      double*& dest)
{
   /*  Single channel.  */

   w = 0;
   h = 0;

   bool odd_width = false;
   bool odd_height = false;

   if (0 == (width % 2))
      w = width / 2;
   else
   {
      w = 1 + (width / 2);
      odd_width = true;
   }

   if (0 == (height % 2))
      h = height / 2;
   else
   {
      h = 1 + (height / 2);
      odd_height = true;
   }

   unsigned int horizontal_upper = (odd_width)  ? w - 1 : w;
   unsigned int vertical_upper   = (odd_height) ? h - 1 : h;

   dest = new double[w * h];

         double* s_itr = dest;
   const double* itr1  = source;
   const double* itr2  = source + width;

   for (unsigned int j = 0; j < vertical_upper; ++j)
   {
      for (unsigned int i = 0; i < horizontal_upper; ++i, ++s_itr)
      {
          (*s_itr)  = *(itr1++);
          (*s_itr) += *(itr1++);
          (*s_itr) += *(itr2++);
          (*s_itr) += *(itr2++);
          (*s_itr) /=  4.0;
      }

      if (odd_width)
      {
         (*(s_itr++)) = ((*itr1++) + (*itr2++)) / 2.0;
      }

      itr1 += width;

      if (j != (vertical_upper -1))
      {
         itr2 += width;
      }
   }

   if (odd_height)
   {
      for (unsigned int i = 0; i < horizontal_upper; ++i, ++s_itr)
      {
         (*s_itr) += (*(itr1++));
         (*s_itr) += (*(itr1++));
         (*s_itr) /= 2.0;
      }

      if (odd_width)
      {
         (*(s_itr++)) = (*itr1);
      }
   }
}

inline void upsample(const unsigned int& width,
                     const unsigned int& height,
                     const double* source,
                     unsigned int& w,
                     unsigned int& h,
                     double*& dest)
{
   /* Single channel. */

   w = 2 * width;
   h = 2 * height;

   dest = new double[w * h];

   const double* s_itr = source;
         double* itr1  = dest;
         double* itr2  = dest + w;

   for (unsigned int j = 0; j < height; ++j)
   {
      for (unsigned int i = 0; i < width; ++i, ++s_itr)
      {
          *(itr1++) = (*s_itr);
          *(itr1++) = (*s_itr);
          *(itr2++) = (*s_itr);
          *(itr2++) = (*s_itr);
      }

      itr1 += w;
      itr2 += w;
   }
}

inline void checkered_pattern(const unsigned int x_width,
                              const unsigned int y_width,
                              const unsigned char value,
                              const bitmap_image::color_plane color,
                              bitmap_image& image)
{
   if (
        (x_width >= image.width ()) ||
        (y_width >= image.height())
      )
   {
      return;
   }

   bool setter_x = false;
   bool setter_y = true;

   const unsigned int color_plane_offset = image.offset(color);
   const unsigned int height = image.height();
   const unsigned int width  = image.width();

   for (unsigned int y = 0; y < height; ++y)
   {
      if (0 == (y % y_width))
      {
         setter_y = !setter_y;
      }

      unsigned char* row = image.row(y) + color_plane_offset;

      for (unsigned int x = 0; x < width; ++x, row += image.bytes_per_pixel())
      {
         if (0 == (x % x_width))
         {
            setter_x = !setter_x;
         }

         if (setter_x ^ setter_y)
         {
            *row = value;
         }
      }
   }
}

inline void checkered_pattern(const unsigned int x_width,
                              const unsigned int y_width,
                              const unsigned char red,
                              const unsigned char green,
                              const unsigned char blue,
                              bitmap_image& image)
{
   if (
        (x_width >= image.width ()) ||
        (y_width >= image.height())
      )
   {
      return;
   }

   bool setter_x = false;
   bool setter_y = true;

   const unsigned int height = image.height();
   const unsigned int width  = image.width();

   for (unsigned int y = 0; y < height; ++y)
   {
      if (0 == (y % y_width))
      {
         setter_y = !setter_y;
      }

      unsigned char* row = image.row(y);

      for (unsigned int x = 0; x < width; ++x, row += image.bytes_per_pixel())
      {
         if (0 == (x % x_width))
         {
            setter_x = !setter_x;
         }

         if (setter_x ^ setter_y)
         {
            *(row + 0) = blue;
            *(row + 1) = green;
            *(row + 2) = red;
         }
      }
   }
}

inline void plasma(bitmap_image& image,
                   const double& x,     const double& y,
                   const double& width, const double& height,
                   const double& c1,    const double& c2,
                   const double& c3,    const double& c4,
                   const double& roughness  = 3.0,
                   const rgb_t   colormap[] = 0)
{
   // Note: c1,c2,c3,c4 -> [0.0,1.0]

   const double half_width  = ( width / 2.0);
   const double half_height = (height / 2.0);

   if ((width >= 1.0) || (height >= 1.0))
   {
      const double corner1 = (c1 + c2) / 2.0;
      const double corner2 = (c2 + c3) / 2.0;
      const double corner3 = (c3 + c4) / 2.0;
      const double corner4 = (c4 + c1) / 2.0;
            double center  = (c1 + c2 + c3 + c4) / 4.0 +
                             ((1.0 * ::rand() /(1.0 * RAND_MAX))  - 0.5) * // should use a better rng
                             ((1.0 * half_width + half_height) / (image.width() + image.height()) * roughness);

      center = std::min<double>(std::max<double>(0.0,center),1.0);

      plasma(image, x,                            y, half_width, half_height,      c1, corner1,  center, corner4,roughness,colormap);
      plasma(image, x + half_width,               y, half_width, half_height, corner1,      c2, corner2,  center,roughness,colormap);
      plasma(image, x + half_width, y + half_height, half_width, half_height,  center, corner2,      c3, corner3,roughness,colormap);
      plasma(image, x,              y + half_height, half_width, half_height, corner4,  center, corner3,      c4,roughness,colormap);
   }
   else
   {
      rgb_t color = colormap[static_cast<unsigned int>(1000.0 * ((c1 + c2 + c3 + c4) / 4.0)) % 1000];

      image.set_pixel(static_cast<unsigned int>(x),static_cast<unsigned int>(y),color);
   }
}

inline void plasma(bitmap_image& image,
                   const double& c1, const double& c2,
                   const double& c3, const double& c4,
                   const double& roughness  = 3.0,
                   const rgb_t   colormap[] = 0)
{
   plasma
   (
     image, 0, 0, image.width(), image.height(),
     c1, c2, c3, c4,
     roughness, colormap
   );
}

inline double psnr_region(const unsigned int& x,      const unsigned int& y,
                          const unsigned int& width,  const unsigned int& height,
                          const bitmap_image& image1, const bitmap_image& image2)
{
   if (
        (image1.width()  != image2.width ()) ||
        (image1.height() != image2.height())
      )
   {
      return 0.0;
   }

   if ((x + width ) > image1.width() ) { return 0.0; }
   if ((y + height) > image1.height()) { return 0.0; }

   double mse = 0.0;

   for (unsigned int r = 0; r < height; ++r)
   {
      const unsigned char* itr1     = image1.row(r + y) + x * image1.bytes_per_pixel();
      const unsigned char* itr1_end = itr1 + (width * image1.bytes_per_pixel());
      const unsigned char* itr2     = image2.row(r + y) + x * image2.bytes_per_pixel();

      while (itr1 != itr1_end)
      {
         double v = (static_cast<double>(*itr1) - static_cast<double>(*itr2));
         mse += v * v;
         ++itr1;
         ++itr2;
      }
   }

   if (mse <= 0.0000001)
   {
      return 1000000.0;
   }
   else
   {
      mse /= (3.0 * width * height);
      return 20.0 * std::log10(255.0 / std::sqrt(mse));
   }
}

inline void hierarchical_psnr_r(const double& x,     const double& y,
                                const double& width, const double& height,
                                const bitmap_image& image1,
                                      bitmap_image& image2,
                                const double& threshold,
                                const rgb_t colormap[])
{
   if ((width <= 4.0) || (height <= 4.0))
   {
      const double psnr = psnr_region
                          (
                            static_cast<unsigned int>(x),
                            static_cast<unsigned int>(y),
                            static_cast<unsigned int>(width),
                            static_cast<unsigned int>(height),
                            image1, image2
                          );

      if (psnr < threshold)
      {
         rgb_t c = colormap[static_cast<unsigned int>(1000.0 * (1.0 - (psnr / threshold)))];

         image2.set_region
                (
                  static_cast<unsigned int>(x),
                  static_cast<unsigned int>(y),
                  static_cast<unsigned int>(width + 1),
                  static_cast<unsigned int>(height + 1),
                  c.red, c.green, c.blue
                );
      }
   }
   else
   {
      const double half_width  = ( width / 2.0);
      const double half_height = (height / 2.0);

      hierarchical_psnr_r(x             , y              , half_width, half_height, image1, image2, threshold, colormap);
      hierarchical_psnr_r(x + half_width, y              , half_width, half_height, image1, image2, threshold, colormap);
      hierarchical_psnr_r(x + half_width, y + half_height, half_width, half_height, image1, image2, threshold, colormap);
      hierarchical_psnr_r(x             , y + half_height, half_width, half_height, image1, image2, threshold, colormap);
   }
}

inline void hierarchical_psnr(bitmap_image& image1, bitmap_image& image2, const double threshold, const rgb_t colormap[])
{
   if (
        (image1.width()  != image2.width ()) ||
        (image1.height() != image2.height())
      )
   {
      return;
   }

   const double psnr = psnr_region
                       (
                         0, 0, image1.width(), image1.height(),
                         image1, image2
                       );

   if (psnr < threshold)
   {
      hierarchical_psnr_r
      (
        0, 0, image1.width(), image1.height(),
        image1, image2,
        threshold,
        colormap
      );
   }
}

#endif
