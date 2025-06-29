

//Credit Should be inserted for using Prof Taubman's libarires and function code
/*****************************************************************************/
// File: filtering_main.cpp
// Author: David Taubman
// Last Revised: 13 August, 2007
/*****************************************************************************/
// Copyright 2007, David Taubman, The University of New South Wales (UNSW)
/*****************************************************************************/

#include "io_bmp.h"
#include "image_comps.h"
#include "SincWindowedLut.h"

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
    float* first_line = buf;
    for (r = 1; r <= border; r++)
        for (c = 0; c < width; c++)
            first_line[-r * stride + c] = first_line[(r - 1) * stride + c];

    // Now extend downwards
    float* last_line = buf + (height - 1) * stride;
    for (r = 1; r <= border; r++)
        for (c = 0; c < width; c++)
            last_line[r * stride + c] = last_line[(1 - r) * stride + c];

    // Now extend all rows to the left and to the right
    float* left_edge = buf - border * stride;
    float* right_edge = left_edge + width - 1;
    for (r = height + 2 * border; r > 0; r--, left_edge += stride, right_edge += stride)
        for (c = 1; c <= border; c++)
        {
            left_edge[-c] = left_edge[c - 1];
            right_edge[c] = right_edge[1 - c];
        }
}


/* ========================================================================= */
/*                              Global Functions                             */
/* ========================================================================= */




/*****************************************************************************/
/*                                apply_filter                               */
/*****************************************************************************/



void apply_filter(my_image_comp* in, my_image_comp* out, float alpha)
{
#define FILTER_EXTENT 4
#define FILTER_DIM (2*FILTER_EXTENT+1)
#define FILTER_TAPS (FILTER_DIM*FILTER_DIM)

    // Create the filter kernel as a local array on the stack, which can accept
    // row and column indices in the range -FILTER_EXTENT to +FILTER_EXTENT.
    float filter_buf[FILTER_TAPS] =
    { 0.0F, 1.0F / 3, 0.5F, 1.0F / 3, 0.0,
        1.0F / 3, 0.5F, 1.0F, 0.5F, 1.0F / 3,
        0.5F, 1.0F, 1.0F, 1.0F, 0.5F,
        1.0F / 3, 0.5F, 1.0F, 0.5F, 1.0F / 3,
        0.0F, 1.0F / 3, 0.5F, 1.0F / 3, 0.0F };
    float unsharp_filter[FILTER_TAPS] = { 0.0F };

    //mirror image or our sharpening filter; Unsharp == uniform sharpening
    float* mirror_psf_unsharp = unsharp_filter + (FILTER_DIM * FILTER_EXTENT) + FILTER_EXTENT;
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


    float* mirror_psf = filter_buf + (FILTER_DIM * FILTER_EXTENT) + FILTER_EXTENT;
    // `mirror_psf' points to the central tap in the filter
    int r, c;
    float normalization_A = 3.0F / 35;
    float normalization_B = 1.0F / 9.0;
    //so that only one multiply is required
    float SharpeningNorm = normalization_A * alpha;
    for (r = -FILTER_EXTENT; r <= FILTER_EXTENT; r++)
        for (c = -FILTER_EXTENT; c <= FILTER_EXTENT; c++)
            if ((r == 0) && (c == 0))
            {
                mirror_psf_unsharp[r * FILTER_DIM + c] = 1.0F;
            }
            else
            {
                mirror_psf_unsharp[r * FILTER_DIM + c] = -(mirror_psf[r * FILTER_DIM + c]) * SharpeningNorm;

            }

    mirror_psf = mirror_psf_unsharp;
    // Check for consistent dimensions
    assert(in->border >= FILTER_EXTENT);
    assert((out->height <= in->height) && (out->width <= in->width));

    // Perform the convolution
    for (r = 0; r < out->height; r++)
        for (c = 0; c < out->width; c++)
        {
            float* ip = in->buf + r * in->stride + c;
            float* op = out->buf + r * out->stride + c;
            float sum = 0.0F;
            for (int y = -FILTER_EXTENT; y <= FILTER_EXTENT; y++)
                for (int x = -FILTER_EXTENT; x <= FILTER_EXTENT; x++)
                    sum += ip[y * in->stride + x] * mirror_psf[y * FILTER_DIM + x];
            *op = sum;
        }
}


/*
    Bilinear Interpolation Function:

    Description:
    This function will be used to perform Bilinear Interpolation on a single image
    component using an integer expansion factor as a key measure.
*/
void bilInterp(my_image_comp* in, int expansionFactor) {
    int xWindow_extent = expansionFactor;
    float* XwindowEntry = in->handle;//in->buf +(in->stride) - (in->border);//line up with top left corner of Actual Image then down one row and right across to the border
    //Horizontal Interpolation on select rows
    for (int r = 0; r < in->height + 2 * (in->border); r += expansionFactor)
    {
        float* Xwindow = XwindowEntry + r * (in->stride);
        for (int xshift = 0; xshift <= (in->width); xshift += expansionFactor, Xwindow = XwindowEntry + r * (in->stride) + xshift)
        {
            //Should slide across in blocks
            for (int i = 1; i < expansionFactor; i++)
            {
                Xwindow[i] = (1.0 / expansionFactor) * (Xwindow[expansionFactor] - Xwindow[0]) * i + Xwindow[0];
            }
        }
    }
    //Vertical Interpolation on all columns (processed horizontally)
    for (int r = 0; r <= in->height; r += expansionFactor)
    {
        float* Xwindow = XwindowEntry + r * (in->stride);
        for (int xshift = 0; xshift < (in->width) + (in->border); xshift++, Xwindow = XwindowEntry + r * (in->stride) + xshift)
        {
            //Should slide across in blocks
            for (int i = 1; i < expansionFactor; i++)
            {
                Xwindow[i * (in->stride)] = (1.0 / expansionFactor) * (Xwindow[expansionFactor * (in->stride)] - Xwindow[0]) * i + Xwindow[0];
            }
        }
    }
    return;
}


void sincInterp(my_image_comp* in, int h) {
    int xWindow_extent = 2*h+1;
    float* XwindowEntry = in->buf;//in->buf +(in->stride) - (in->border);//line up with top left corner of Actual Image then down one row and right across to the border
    float* ip = in->handle;//input Pointer to keep Track Of values in terms of multiple of 3
    float* op = in->handle;//
    /*float* const float *interpKernF ;
    const float* const float *interpKernB;*/
    //Choose the correct precomputed interpolation kernel
    const float* interpKernF = NULL;
    const float* interpKernB = NULL;
    switch (h) {
        case(1):
        {
            const float *interpKernF = sincAdd1_3_1;
            const float *interpKernB = sincSub1_3_1;
            break;
        }
        case(2):
        {
            const float *interpKernF = sincAdd1_3_2;
            const float *interpKernB = sincSub1_3_2;
            break;
        }
        case(3):
        {
            const float *interpKernF = sincAdd1_3_3;
            const float *interpKernB = sincSub1_3_3;
            break;
        }
        case(4):
        {
            const float *interpKernF = sincAdd1_3_4;
            const float *interpKernB = sincSub1_3_4;
            break;
        }
        case(5):
        {
            const float *interpKernF = sincAdd1_3_5;
            const float *interpKernB = sincSub1_3_5;
            break;
        }
        case(6):
        {
            const float *interpKernF = sincAdd1_3_6;
            const float *interpKernB = sincSub1_3_6;
            break;
        }
        case(7):
        {
            const float *interpKernF = sincAdd1_3_7;
            const float *interpKernB = sincSub1_3_7;
            break;
        }
        case(8):
        {
            const float *interpKernF = sincAdd1_3_8;
            const float *interpKernB = sincSub1_3_8;
            break;
        }
        case(9):
        {
            const float *interpKernF = sincAdd1_3_9;
            const float *interpKernB = sincSub1_3_9;
            break;
        }
        case(10):
        {
            const float *interpKernF = sincAdd1_3_10;
            const float *interpKernB = sincSub1_3_10;
            break;
        }
        case(11):
        {
            const float *interpKernF = sincAdd1_3_11;
            const float *interpKernB = sincSub1_3_11;
            break;
        }
        case(12):
        {
            const float *interpKernF = sincAdd1_3_12;
            const float *interpKernB = sincSub1_3_12;
            break;
        }
        case(13):
        {
            const float *interpKernF = sincAdd1_3_13;
            const float *interpKernB = sincSub1_3_13;
            break;
        }
        case(14):
        {
            const float *interpKernF = sincAdd1_3_14;
            const float *interpKernB = sincSub1_3_14;
            break;
        }
        case(15):
        {
            const float *interpKernF = sincAdd1_3_15;
            const float *interpKernB = sincSub1_3_15;
            break;
        }
        case(0):
        {
            const float *interpKernF = sincAdd1_3_0;
            const float *interpKernB = sincSub1_3_0;
            break;
        }
        default:
        {
            const float* interpKernF = sincAdd1_3_0;
            const float* interpKernB = sincSub1_3_0;
        }
    }
    //Horizontal Interpolation on select rows
    for (int r = 0; r < in->height + 2 * (in->border); r += 3)
    {
        float* Xwindow = XwindowEntry + r * (in->stride);
        for (int xshift = 0; xshift <= (in->width); xshift++, Xwindow = XwindowEntry + r * (in->stride) + xshift)
        {
           //will hit issue regarding the ordering
            //Should slide across in blocks
            for (int i = -h; i < h; i++) {
                Xwindow[2] += ip[-i * 3] * interpKernB[i];
                Xwindow[1] += ip[-i*3]*interpKernF[i];//1.0;//3M + 1 (the first one after a multiple of 3) should be convolving or mirror and inner product using the add_1_3
                //3M - 1 (the first one after a multiple of 3) should be convolving or mirror and inner product using the add_1_3
            
            }
            
        }
    }
    //Vertical Interpolation on all columns (processed horizontally)
    for (int r = 0; r <= in->height; r += 3)
    {
        float* Xwindow = XwindowEntry + r * (in->stride);
        for (int xshift = 0; xshift < (in->width) + (in->border); xshift++, Xwindow = XwindowEntry + r * (in->stride) + xshift)
        {   
            Xwindow[1* (in->stride)] = 1.0;//3M + 1 (the first one after a multiple of 3) should be convolving or mirror and inner product using the add_1_3
            Xwindow[2* (in->stride)] = 1.0;//3M - 1 (the first one after a multiple of 3) should be convolving or mirror and inner product using the add_1_3
        }
    }
    return;
}

/*****************************************************************************/
/*                                    main                                   */
/*****************************************************************************/

int
main(int argc, char* argv[])
{
    /*CMD Line Args: <input image> <output image> */
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <in bmp file> <out bmp file>\n", argv[0]);
        return -1;
    }

    int err_code = 0;
    try
    {
        // Read the input image
        bmp_in in;
        if ((err_code = bmp_in__open(&in, argv[1])) != 0)
            throw err_code;

        int width = in.cols, height = in.rows;
        int n, num_comps = in.num_components;
        /* IMPLEMENTATION NOTE:
        * The input input image is read in to a larger buffer and spread out.
        * This step is to skip having two buffers in memory that need to be switched.
        * The way we can achieve this is by leveraging the fact that each sample point
        * on the expanded image at points 3*n(vector n) will be exactly x[n(vector)] and therefore
        * these points can be written to directly from the file rather than beind duplicated in memory
        * bilinear interpolation will only operate between the sample point hence this is a better use
        * of memory.
        */
        int h;
        sscanf(argv[3], "%d",&h);
        printf("H VALUE: %d\n",h);
        //NOTE: For later only need to consider a single image component which is green if RGB there should be no direct impact intially however it is just bloat otherwise
        //WARNING FOR LATER: Might need to spread from the center since this could possibly be causing a shift which is no good 
        my_image_comp* input_comps = new my_image_comp[num_comps];
        for (n = 0; n < num_comps; n++)
            input_comps[n].init(3 * height, 3 * width, 2); // Leave a border of 4 -> reevaluate what we need to missor teh next sample
        int r; // Declare row index
        io_byte* line = new io_byte[width * num_comps];
        for (r = 3 * height - 1; r >= 0; r--)
        { // "r" holds the true row index we are reading, since the image is
          // stored upside down in the BMP file.
          // Note: r is decremented by 3 since image is store at intervals of 3 apart and height must be a multiple of 3 (this would not work if it wasn't)
            /*if ((err_code = bmp_in__get_line(&in, line)) != 0)
                throw err_code;
            */
            if (r % 3 == 1)
            {
                bmp_in__get_line(&in, line);
            }


            for (n = 0; n < num_comps; n++)
            {
                io_byte* src = line + n; // Points to first sample of component n
                float* dst = input_comps[n].buf + r * input_comps[n].stride;
                for (int c = 0; c < 3 * width; c++)
                    //note that the src increments by one whilst our destination will increment over by 3
                    if ((r % 3 == 1) && (c % 3 == 1))
                    {
                        dst[c] = (float)*src;
                        src += num_comps;
                    }
                    else
                    {
                        dst[c] = 0.0f;
                    }
                // The cast to type "float" is not
            // strictly required here, since bytes can always be
            // converted to floats without any loss of information.
            }
        }
        bmp_in__close(&in);
        // Allocate storage for the filtered output
        // Process the image, all in floating point (easy)
        for (n = 0; n < num_comps; n++)
            input_comps[n].perform_boundary_extension();
        /*for (n = 0; n < num_comps; n++)
            bilInterp(input_comps + n, 3);*/


        int temp_val;
        io_byte* lineOut = new io_byte[3 * width * num_comps];
        // Write the image back out again
        bmp_out out;
        if ((err_code = bmp_out__open(&out, argv[2], 3 * width, 3 * height, num_comps)) != 0)
            throw err_code;
        for (r = 3 * height - 1; r >= 0; r--)
        { // "r" holds the true row index we are writing, since the image is
          // written upside down in BMP files.
            for (n = 0; n < num_comps; n++)
            {
                io_byte* dst = lineOut + n; // Points to first sample of component n
                float* src = input_comps[n].buf + r * input_comps[n].stride;
                for (int c = 0; c < 3 * width; c++, dst += num_comps)
                {
                    temp_val = (int)(src[c] + 0.5);
                    if (temp_val < 0)
                    {
                        *dst = 0;
                    }
                    else if (temp_val > 255)
                    {
                        *dst = 255;
                    }
                    else
                    {
                        *dst = (io_byte)temp_val;
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
            bmp_out__put_line(&out, lineOut);
        }
        bmp_out__close(&out);
        delete[] lineOut;
        delete[] line;
        delete[] input_comps;
        //delete[] output_comps;
    }
    catch (int exc)
    {
        if (exc == IO_ERR_NO_FILE)
            fprintf(stderr, "Cannot open supplied input or output file.\n");
        else if (exc == IO_ERR_FILE_HEADER)
            fprintf(stderr, "Error encountered while parsing BMP file header.\n");
        else if (exc == IO_ERR_UNSUPPORTED)
            fprintf(stderr, "Input uses an unsupported BMP file format.\n  Current "
                "simple example supports only 8-bit and 24-bit data.\n");
        else if (exc == IO_ERR_FILE_TRUNC)
            fprintf(stderr, "Input or output file truncated unexpectedly.\n");
        else if (exc == IO_ERR_FILE_NOT_OPEN)
            fprintf(stderr, "Trying to access a file which is not open!(?)\n");
        return -1;
    }
    return 0;
}
