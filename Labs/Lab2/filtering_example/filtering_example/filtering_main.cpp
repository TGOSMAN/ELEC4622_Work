/*****************************************************************************/
// File: filtering_main.cpp
// Author: David Taubman
// Last Revised: 13 August, 2007
/*****************************************************************************/
// Copyright 2007, David Taubman, The University of New South Wales (UNSW)
/*****************************************************************************/

#include "io_bmp.h"
#include "image_comps.h"

/* ========================================================================= */
/*                 Implementation of `my_image_comp' functions               */
/* ========================================================================= */

/*****************************************************************************/
/*                  my_image_comp::perform_boundary_extension                */
/*****************************************************************************/

void my_image_comp::perform_boundary_extension()
{
  int r, c;

  // First extend upwards
  float *first_line = buf;
  for (r = 1; r <= border; r++)
      for (c = 0; c < width; c++)
          first_line[-r * stride + c] = first_line[r * stride + c];

  // Now extend downwards
  float *last_line = buf+(height-1)*stride;
  for (r = 1; r <= border; r++)
      for (c = 0; c < width; c++)
          last_line[r * stride + c] = last_line[- r* stride + c];

  // Now extend all rows to the left and to the right
  float *left_edge = buf-border*stride;
  float *right_edge = left_edge + width - 1;
  for (r=height+2*border; r > 0; r--, left_edge+=stride, right_edge+=stride)
    for (c=1; c <= border; c++)
      {
        left_edge[-c] = left_edge[c];
        right_edge[c] = right_edge[-c];
      }
}


/* ========================================================================= */
/*                              Global Functions                             */
/* ========================================================================= */

/*****************************************************************************/
/*                                apply_filter                               */
/*****************************************************************************/

void apply_filter(my_image_comp *in, my_image_comp *out, float alpha)
{
#define FILTER_EXTENT 4
#define FILTER_DIM (2*FILTER_EXTENT+1)
#define FILTER_TAPS (FILTER_DIM*FILTER_DIM)

  // Create the filter kernel as a local array on the stack, which can accept
  // row and column indices in the range -FILTER_EXTENT to +FILTER_EXTENT.
    float filter_buf[FILTER_TAPS] =
    {0.0F, 1.0F / 3, 0.5F, 1.0F / 3, 0.0,
        1.0F / 3, 0.5F, 1.0F, 0.5F, 1.0F / 3,
        0.5F, 1.0F, 1.0F, 1.0F, 0.5F,
        1.0F / 3, 0.5F, 1.0F, 0.5F, 1.0F / 3,
        0.0F, 1.0F / 3, 0.5F, 1.0F / 3, 0.0F};
    float unsharp_filter[FILTER_TAPS] = { 0.0F };
 
    //mirror image or our sharpening filter; Unsharp == uniform sharpening
    float *mirror_psf_unsharp = unsharp_filter +(FILTER_DIM * FILTER_EXTENT) + FILTER_EXTENT;
    //Impulse Value at center

    //All Filter Coefficients are before normalisation
    //H3 Manually Mirrored
    /*{0.25,0.50,0.50,0.25,0.0,0.0,0.0,0.0,0.0,
        0.50,1.00,1.00,0.50,0.0,0.0,0.0,0.0,0.0,
        0.50,1.00,1.00,0.50,0.0,0.0,0.0,0.0,0.0,
        0.25,0.5,0.5,0.25,0.0,0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};*/

        
     //H2 Manually Mirrored for this exercise
    /*{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
        0.0,0.0,0.0,0.0,0.0,0.25,0.50,0.50,0.25,
        0.0,0.0,0.0,0.0,0.0,0.50,1.00,1.00,0.50,
        0.0,0.0,0.0,0.0,0.0,0.50,1.00,1.00,0.50,
        0.0,0.0,0.0,0.0,0.0,0.25,0.5,0.5,0.25};*/
        //H1
        /*{0.0F, 1.0F / 3, 0.5F, 1.0F / 3, 0.0,
        1.0F/3, 0.5F, 1.0F, 0.5F, 1.0F/3, 
        0.5F, 1.0F, 1.0F, 1.0F, 0.5F,
        1.0F/3, 0.5F, 1.0F, 0.5F, 1.0F/3,
        0.0F, 1.0F/3, 0.5F, 1.0F/3, 0.0F};*/


  float *mirror_psf = filter_buf+(FILTER_DIM*FILTER_EXTENT)+FILTER_EXTENT;
          // `mirror_psf' points to the central tap in the filter
  int r, c;
  float normalization_A = 3.0F / 35;
  float normalization_B = 1.0F / 9.0;
  //so that only one multiply is required
  float SharpeningNorm = normalization_A * alpha;
for (r = -FILTER_EXTENT; r <= FILTER_EXTENT; r++)
    for (c = -FILTER_EXTENT; c <= FILTER_EXTENT; c++)
        if ((r == 0)&& (c == 0)) {
            mirror_psf_unsharp[r * FILTER_DIM + c] = 1.0F;
        }
        else {
            mirror_psf_unsharp[r* FILTER_DIM + c] = -(mirror_psf[r * FILTER_DIM + c]) * SharpeningNorm;
        
        }
         
    mirror_psf = mirror_psf_unsharp;
  // Check for consistent dimensions
  assert(in->border >= FILTER_EXTENT);
  assert((out->height <= in->height) && (out->width <= in->width));

  // Perform the convolution
  for (r=0; r < out->height; r++)
    for (c=0; c < out->width; c++)
      {
        float *ip = in->buf + r*in->stride + c;
        float *op = out->buf + r*out->stride + c;
        float sum = 0.0F;
        for (int y=-FILTER_EXTENT; y <= FILTER_EXTENT; y++)
          for (int x=-FILTER_EXTENT; x <= FILTER_EXTENT; x++)
            sum += ip[y*in->stride+x] * mirror_psf[y*FILTER_DIM+x];
        *op = sum;
      }
}

/*****************************************************************************/
/*                                    main                                   */
/*****************************************************************************/

int
  main(int argc, char *argv[])
{
  if (argc != 4)
    {
      fprintf(stderr,"Usage: %s <in bmp file> <out bmp file>\n",argv[0]);
      return -1;
    }

  int err_code=0;
  try {
      // Read the input image
      bmp_in in;
      if ((err_code = bmp_in__open(&in,argv[1])) != 0)
        throw err_code;

      int width = in.cols, height = in.rows;
      int n, num_comps = in.num_components;
      my_image_comp *input_comps = new my_image_comp[num_comps];
      for (n=0; n < num_comps; n++)
        input_comps[n].init(height,width,4); // Leave a border of 4
      
      int r; // Declare row index
      io_byte *line = new io_byte[width*num_comps];
      for (r=height-1; r >= 0; r--)
        { // "r" holds the true row index we are reading, since the image is
          // stored upside down in the BMP file.
          if ((err_code = bmp_in__get_line(&in,line)) != 0)
            throw err_code;
          for (n=0; n < num_comps; n++)
            {
              io_byte *src = line+n; // Points to first sample of component n
              float *dst = input_comps[n].buf + r * input_comps[n].stride;
              for (int c=0; c < width; c++, src+=num_comps)
                dst[c] = (float) *src; // The cast to type "float" is not
                      // strictly required here, since bytes can always be
                      // converted to floats without any loss of information.
            }
        }
      bmp_in__close(&in);
      float alpha;
      sscanf(argv[3], "%f", &alpha);
      printf("Alpha is %f\n",alpha);
      // Allocate storage for the filtered output
      my_image_comp *output_comps = new my_image_comp[num_comps];
      for (n=0; n < num_comps; n++)
        output_comps[n].init(height,width,0); // Don't need a border for output

      // Process the image, all in floating point (easy)
      for (n=0; n < num_comps; n++)
        input_comps[n].perform_boundary_extension();
      for (n=0; n < num_comps; n++)
        apply_filter(input_comps+n,output_comps+n,alpha);
      int temp_val;
      // Write the image back out again
      bmp_out out;
      if ((err_code = bmp_out__open(&out,argv[2],width,height,num_comps)) != 0)
        throw err_code;
      for (r=height-1; r >= 0; r--)
        { // "r" holds the true row index we are writing, since the image is
          // written upside down in BMP files.
          for (n=0; n < num_comps; n++)
            {
              io_byte *dst = line+n; // Points to first sample of component n
              float *src = output_comps[n].buf + r * output_comps[n].stride;
              for (int c = 0; c < width; c++, dst += num_comps) {
                  temp_val = (int)(src[c] + 0.5);
                  if (temp_val < 0) {
                      *dst = 0;
                  }
                  else if (temp_val > 255) {
                      *dst = 255;
                  }
                  else {
                      *dst = (io_byte) temp_val;
                  }
              
              }

                //*dst = (io_byte) src[c]; 
                      // The cast to type "io_byte" is
                      // required here, since floats cannot generally be
                      // converted to bytes without loss of information.  The
                      // compiler will warn you of this if you remove the cast.
                      // There is in fact not the best way to do the
                      // conversion.  You should fix it up in the lab.
            }
          bmp_out__put_line(&out,line);
        }
      bmp_out__close(&out);
      delete[] line;
      delete[] input_comps;
      delete[] output_comps;
    }
  catch (int exc) {
      if (exc == IO_ERR_NO_FILE)
        fprintf(stderr,"Cannot open supplied input or output file.\n");
      else if (exc == IO_ERR_FILE_HEADER)
        fprintf(stderr,"Error encountered while parsing BMP file header.\n");
      else if (exc == IO_ERR_UNSUPPORTED)
        fprintf(stderr,"Input uses an unsupported BMP file format.\n  Current "
                "simple example supports only 8-bit and 24-bit data.\n");
      else if (exc == IO_ERR_FILE_TRUNC)
        fprintf(stderr,"Input or output file truncated unexpectedly.\n");
      else if (exc == IO_ERR_FILE_NOT_OPEN)
        fprintf(stderr,"Trying to access a file which is not open!(?)\n");
      return -1;
    }
  return 0;
}
