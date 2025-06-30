// Project1_LookupTableGenerator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <cmath>

constexpr float       pi_f = 3.1415927f;

float raiseCosine(float tau, float x) {
    if (abs(x) > tau)
    {
        return 0.0f;
    }
    return 0.5 + 0.5*cos(pi_f*x/tau);
}

float scaledShifted_sinc(float alpha, float k) {
    if (k == 0)
    {
        return 1.0f;
    }
    float result = sin(pi_f*k/alpha)/(pi_f*k /alpha);//(1/powf(alpha,2))
    return result;

}
int main(void)
{
    printf("Lookup Table Generating");
    FILE* file = fopen("SincWindowedLut.h", "w");
    fprintf(file, "\//////// Lookup Table Generated for ELEC4622 Project 1 TASK2 //////////\n");
    fprintf(file, "\/*\n");
    fprintf(file, "\*\tThis File was produced by c code Thomas Gosman (z5425064) wrote \n\*\ton 29/06/25 to produce a table of sampled and raise cosined \n\*\twindowed sinc values for a given region of support with 0<= H <= 15\n");
    fprintf(file, "\*/\n");
    float answer = raiseCosine(2.0f,1.0f);
    printf("\n");
    for (int h = 15; h >= 0; h--)
    {
        fprintf(file, "const float sincAdd1_3_%d[%d] ={", h,2*h + 1);
        float tmp = 0.0;
        float windval = 0.0;
        for (int i = 0; i < (2 * h) + 1; i++)
        {
            float TAPS = (float)(2 * h) + 1;
            float res1 = scaledShifted_sinc(3, (float) 3*(i - h) + 1);
            windval = raiseCosine(TAPS/2,(float) (i-h) );
            if (i == 2 * h)
            {
                fprintf(file, "%f", windval * res1);
            }
            else
            {
                fprintf(file, "%f,", windval * res1);
            }
            
            printf("Window[%d]:\t%f\n",i-h, windval);
            printf("Sinc[%d]:\t%.6f\n", i - h, res1);
        }
        fprintf(file,"};\n");
    }
    for (int h = 15; h >= 0; h--)
    {
        fprintf(file, "const float sincSub1_3_%d[%d] ={", h, 2 * h + 1);
        float tmp = 0.0;
        float windval = 0.0;
        for (int i = 0; i < (2 * h) + 1; i++)
        {
            float TAPS = (float)(2 * h) + 1;
            float res1 = scaledShifted_sinc(3, (float)3*(i - h) - 1);
            windval = raiseCosine(TAPS / 2, (float)(i - h));
            if (i == 2 * h)
            {
                fprintf(file, "%f", windval * res1);
            }
            else
            {
                fprintf(file, "%f,", windval * res1);
            }

            printf("Window[%d]:\t%f\n", i - h, windval);
            printf("Sinc[%d]:\t%.6f\n", i - h, res1);
        }
        fprintf(file, "};\n");
    }
    fclose(file);
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
