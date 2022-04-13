//-----------------------------------------------------------------------
// Description: Efficient Sample Rate Converter with the following ratios
//    6/5
//    7/8
//    9/10
//    9/8
//    The SRC consist of up- and down-sampler. 
//    A fir filter is decomposed into polyphase subfilters which makes it possible
//    to compute the down-sample part before the up-sampling part witout loss of bandwith but having less computational complexity.
//
// Author:       Kim Radmacher
//
// Date:         09.04.2022
//
// Copyright (C) 2022 Kim Radmacher
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.
// See the License for the specific language governing permissions
// and limitations under the License.
//----------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

//#define DEBUG

#if defined(_MSC_VER)
#include <getopt.h>
#else
#include <unistd.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "wavreader.h"
#include "wavwriter.h"
#ifdef __cplusplus
}
#endif

#include "src.h"

void usage(const char* name)
{
    fprintf(stderr, "%s in.wav out.wav <conversion index>\n", name);
    fprintf(stderr, "<conversion index> \n");
    fprintf(stderr, " %d: 6/5\n", SRCI_6_TO_5);
    fprintf(stderr, " %d: 7/8\n", SRCI_7_TO_8);
    fprintf(stderr, " %d: 9/10\n", SRCI_9_TO_10);
    fprintf(stderr, " %d: 9/8\n", SRCI_9_TO_8);
}

int main(int argc, char *argv[])
{
    const char *infile, *outfile;
    FILE *out;
    void *wavIn;
    void *wavOut;
    int format, sample_rate, channels, bits_per_sample;
    uint32_t data_length;
    int input_size;
    uint8_t* input_buf;
    int16_t* convert_buf;
    int16_t* output;
    HANDLE_SRC hSrc = NULL;
    
    if (argc - optind < 3)
    {
        fprintf(stderr, "Error: not enough parameter provided\n");
        usage(argv[0]);
        return 1;
    }
    
    infile = argv[optind];
    outfile = argv[optind + 1];
    SAMPLE_RATE_CONVERSION_INDEX srci = (SAMPLE_RATE_CONVERSION_INDEX)atoi(argv[optind + 2]);
    
    wavIn = wav_read_open(infile);
    if (!wavIn)
    {
        fprintf(stderr, "Unable to open wav file %s\n", infile);
        return 1;
    }
    if (!wav_get_header(wavIn, &format, &channels, &sample_rate, &bits_per_sample, &data_length))
    {
        fprintf(stderr, "Bad wav file %s\n", infile);
        return 1;
    }
    if (format != 1)
    {
        fprintf(stderr, "Unsupported WAV format %d\n", format);
        return 1;
    }

    if (bits_per_sample != 16)
    {
        fprintf(stderr, "%d bits are currently not supported, only int16\n", bits_per_sample);
        return 1;
    }

    hSrc = (HANDLE_SRC)srcInit(sample_rate, srci);

    if (hSrc == NULL)
    {
        fprintf(stderr, "ERROR: initialization of SRC failed\n");
        return 1;
    }

    wavOut = wav_write_open(outfile, hSrc->srOut, bits_per_sample, channels);

    if (!wavOut)
    {
        fprintf(stderr, "Unable to open wav file for writing %s\n", infile);
        return 1;
    }

    input_size = data_length;
    input_buf = (uint8_t*) malloc(input_size);
    convert_buf = (int16_t*) malloc(input_size);
    output = (int16_t*) malloc(input_size * hSrc->upRatio / hSrc->dnRatio + 1);

    if (input_buf == NULL || convert_buf == NULL || output == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for buffer\n");
        return 1;
    }

    int read = wav_read_data(wavIn, input_buf, input_size);

    printf("data_length = %d\tread = %d\tinput_size = %d \n", data_length, read, input_size);
    printf("sample rate in = %d Hz\tbits_per_sample = %d\tchannels = %d \n", sample_rate, bits_per_sample, channels);
    printf("upRatio = %d\tdnRadio = %d\tsample rate out = %d Hz\n", hSrc->upRatio, hSrc->dnRatio, hSrc->srOut);

    int numSamplesIn = read/2;
    for(unsigned int n = 0; n < numSamplesIn; n++)
    {
        const uint8_t* in = &input_buf[2*n];
        convert_buf[n] = in[0] | (in[1] << 8);
    }
    
    int numSamplesOut = 0;
    for (int ch=0; ch < channels; ch++)
    {
        numSamplesOut += hSrc->srcMLfunc(hSrc, convert_buf, &output[ch], numSamplesIn/channels, channels);
    }
    
    // iterate over samples and write output
    for(unsigned int n = 0; n < numSamplesOut; n+=channels)
    {
        int16_t oL = output[n];
        wav_write_data(wavOut, (unsigned char*)&oL, 2);
        if (channels > 1)
        {
            int16_t oR = output[n+1];
            wav_write_data(wavOut, (unsigned char*)&oR, 2);
        }
    }    

    free(output);
    free(convert_buf);
    free(input_buf);

    srcClose(hSrc);

    wav_write_close(wavOut);
    wav_read_close(wavIn);

    return 0;
}
