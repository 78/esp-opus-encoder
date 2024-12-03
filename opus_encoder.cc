#include "opus_encoder.h"
#include <esp_log.h>

#define TAG "OpusEncoderWrapper"

OpusEncoderWrapper::OpusEncoderWrapper(int sample_rate, int channels, int duration_ms) {
    int error;
    audio_enc_ = opus_encoder_create(sample_rate, channels, OPUS_APPLICATION_VOIP, &error);
    if (audio_enc_ == nullptr) {
        ESP_LOGE(TAG, "Failed to create audio encoder, error code: %d", error);
        return;
    }

    // Default DTX enabled
    SetDtx(true);
    // Complexity 5 almost uses up all CPU of ESP32C3
    SetComplexity(5);

    frame_size_ = sample_rate / 1000 * channels * duration_ms;
}

OpusEncoderWrapper::~OpusEncoderWrapper() {
    if (audio_enc_ != nullptr) {
        opus_encoder_destroy(audio_enc_);
    }
}

void OpusEncoderWrapper::Encode(std::vector<int16_t>&& pcm, std::function<void(std::vector<uint8_t>&& opus)> handler) {
    if (audio_enc_ == nullptr) {
        ESP_LOGE(TAG, "Audio encoder is not configured");
        return;
    }

    if (in_buffer_.empty()) {
        in_buffer_ = std::move(pcm);
    } else {
        in_buffer_.insert(in_buffer_.end(), pcm.begin(), pcm.end());
    }

    while (in_buffer_.size() >= frame_size_) {
        std::vector<uint8_t> opus(MAX_OPUS_PACKET_SIZE);
        auto ret = opus_encode(audio_enc_, in_buffer_.data(), frame_size_, opus.data(), opus.size());
        if (ret < 0) {
            ESP_LOGE(TAG, "Failed to encode audio, error code: %ld", ret);
            return;
        }
        opus.resize(ret);

        if (handler != nullptr) {
            handler(std::move(opus));
        }

        in_buffer_.erase(in_buffer_.begin(), in_buffer_.begin() + frame_size_);
    }
}

void OpusEncoderWrapper::ResetState() {
    if (audio_enc_ != nullptr) {
        opus_encoder_ctl(audio_enc_, OPUS_RESET_STATE);
        in_buffer_.clear();
    }
}

void OpusEncoderWrapper::SetDtx(bool enable) {
    if (audio_enc_ != nullptr) {
        opus_encoder_ctl(audio_enc_, OPUS_SET_DTX(enable ? 1 : 0));
    }
}

void OpusEncoderWrapper::SetComplexity(int complexity) {
    if (audio_enc_ != nullptr) {
        opus_encoder_ctl(audio_enc_, OPUS_SET_COMPLEXITY(complexity));
    }
}
