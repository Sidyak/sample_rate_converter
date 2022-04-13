# Description
Efficient Sample Rate Converter with the following ratios
//    6/5
//    7/8
//    9/10
//    9/8
//    The SRC consist of up- and down-sampler. 
//    A fir filter is decomposed into polyphase subfilters which makes it possible
//    to compute the down-sample part before the up-sampling part witout loss of bandwith but having less computational complexity.

# Usage
## Build on x86

make clean; make

## Run

./bin/sample_rate_converter in.wav out.wav 'conversion index'

 conversion index:
 
 0: 6/5
 1: 7/8
 2: 9/10
 3: 9/8
