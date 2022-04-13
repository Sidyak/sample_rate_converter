
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
#ifndef SRC_H
#define SRC_H

#define fMultDiv2(x1, x2) (x1*x2/2)
#define N_POLY_2 2
#define N_POLY_3 3

typedef enum
{
    SRCI_6_TO_5,
    SRCI_7_TO_8,
    SRCI_9_TO_10,
    SRCI_9_TO_8,
} SAMPLE_RATE_CONVERSION_INDEX;

struct SRC_DATA
{
    bool useSRC;
    int upRatio;
    int dnRatio;
    int mm;
    float gain;
    float *delays;
    int nDecDelays;
    const float* filterCoeff;
    int* decimationOutput;
    int(*srcMLfunc)(SRC_DATA*, int16_t*, int16_t*, int, int);
    float *states;
    int srIn;
    int srOut;
};

typedef SRC_DATA* HANDLE_SRC;

void* srcInit(int fs, SAMPLE_RATE_CONVERSION_INDEX sampleRateIndex);
void srcClose (HANDLE_SRC hSrc);

int src9to8(  HANDLE_SRC hSrc9to8,
                  int16_t *input,
                  int16_t *output,
                  int     length,
                  int     stride);


int src7to8(  HANDLE_SRC hSrc7to8,
                  int16_t *input,
                  int16_t *output,
                  int     length,
                  int     stride);


int src6to5(  HANDLE_SRC hSrc6to5,
                  int16_t *input,
                  int16_t *output,
                  int     length,
                  int     stride);


int src9to10(  HANDLE_SRC hSrc9to10,
                  int16_t *input,
                  int16_t *output,
                  int     length,
                  int     stride);
#endif
