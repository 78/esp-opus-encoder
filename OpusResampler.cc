#include "OpusResampler.h"
#include "silk_resampler.h"
#include "esp_log.h"

#define TAG "OpusResampler"

OpusResampler::OpusResampler() {
}

OpusResampler::~OpusResampler() {
}

void OpusResampler::Configure(int input_sample_rate, int output_sample_rate) {
    auto ret = silk_resampler_init(&resampler_state_, input_sample_rate, output_sample_rate, 1);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to initialize resampler");
    }
    input_sample_rate_ = input_sample_rate;
    output_sample_rate_ = output_sample_rate;
    ESP_LOGI(TAG, "Resampler configured with input sample rate %d and output sample rate %d", input_sample_rate_, output_sample_rate_);
}

void OpusResampler::Process(const int16_t *input, int input_size, int16_t *output, int output_size) {
    auto ret = silk_resampler(&resampler_state_, output, input, input_size);
    if (ret != 0) {
        ESP_LOGE(TAG, "Failed to process resampler");
    }
}

