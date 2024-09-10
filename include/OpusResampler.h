#ifndef OPUS_RESAMPLER_H
#define OPUS_RESAMPLER_H

#include <cstdint>
#include "opus.h"
#include "resampler_structs.h"

class OpusResampler {
public:
    OpusResampler();
    ~OpusResampler();

    void Configure(int input_sample_rate, int output_sample_rate);
    void Process(const int16_t *input, int input_size, int16_t *output, int output_size);

private:
    silk_resampler_state_struct resampler_state_;
    int input_sample_rate_;
    int output_sample_rate_;
};

#endif


