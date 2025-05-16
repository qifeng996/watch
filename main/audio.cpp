//
// Created by wind on 25-3-23.
//

#include "audio.h"

#include <esp_log.h>
#include <utilities.h>

#include "audio_player.h"
static i2s_chan_handle_t i2s_tx_chan;
static i2s_chan_handle_t i2s_rx_chan;
Audio* audio = nullptr;

static esp_err_t bsp_i2s_write(void* audio_buffer, size_t len, size_t* bytes_written, uint32_t timeout_ms)
{
    esp_err_t ret = ESP_OK;
    ret = i2s_channel_write(i2s_tx_chan, (char*)audio_buffer, len, bytes_written, timeout_ms);
    return ret;
}

static esp_err_t bsp_i2s_reconfig_clk(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch)
{
    esp_err_t ret = ESP_OK;

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(rate),
        .slot_cfg = {
            .data_bit_width = static_cast<i2s_data_bit_width_t>(bits_cfg),
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
            .slot_mode = ch,
            .slot_mask = I2S_STD_SLOT_BOTH,
            .ws_width = bits_cfg,
            .ws_pol = false,
            .bit_shift = false,
            .left_align = true,
            .big_endian = false,
            .bit_order_lsb = false
        },
        .gpio_cfg = {
            .bclk = static_cast<gpio_num_t>(BOARD_DAC_IIS_BCK),
            .ws = static_cast<gpio_num_t>(BOARD_DAC_IIS_WS),
            .dout = static_cast<gpio_num_t>(BOARD_DAC_IIS_DOUT),
        }
    };
    ret |= i2s_channel_disable(i2s_tx_chan);
    ret |= i2s_channel_reconfig_std_clock(i2s_tx_chan, &std_cfg.clk_cfg);
    ret |= i2s_channel_reconfig_std_slot(i2s_tx_chan, &std_cfg.slot_cfg);
    ret |= i2s_channel_enable(i2s_tx_chan);
    return ret;
}

static esp_err_t audio_mute_function(AUDIO_PLAYER_MUTE_SETTING setting)
{
    ESP_LOGI("Audio", "mute setting %d", setting);
    return ESP_OK;
}

Audio::Audio()
= default;

void Audio::init()
{
    /* Setup I2S peripheral */
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true; // Auto clear the legacy data in the DMA buffer
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &i2s_tx_chan, &i2s_rx_chan));

    /* Setup I2S channels */
    constexpr i2s_std_config_t std_cfg_default = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {

            .bclk = static_cast<gpio_num_t>(BOARD_DAC_IIS_BCK),
            .ws = static_cast<gpio_num_t>(BOARD_DAC_IIS_WS),
            .dout = static_cast<gpio_num_t>(BOARD_DAC_IIS_DOUT),

        }
    };

    if (i2s_tx_chan != nullptr)
    {
        ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_tx_chan, &std_cfg_default));
        ESP_ERROR_CHECK(i2s_channel_enable(i2s_tx_chan));
    }
    if (i2s_rx_chan != nullptr)
    {
        ESP_ERROR_CHECK(i2s_channel_init_std_mode(i2s_rx_chan, &std_cfg_default));
        ESP_ERROR_CHECK(i2s_channel_enable(i2s_rx_chan));
    }
    audio_event_queue_handle = xQueueCreate(5, sizeof(audio_player_callback_event_t));
    audio_player_callback_register(player_cb, this);
    constexpr audio_player_config_t config = {
        .mute_fn = audio_mute_function,
        .clk_set_fn = bsp_i2s_reconfig_clk,
        .write_fn = bsp_i2s_write,
        .priority = 0,
        .coreID = 0,
    };

    audio_player_new(config);
}

void Audio::player_cb(audio_player_cb_ctx_t* ctx)
{
    const auto self = static_cast<Audio*>(ctx->user_ctx);
    xQueueSend(self->audio_event_queue_handle, &(ctx->audio_event), 0);
}

void Audio::play()
{
    audio_player_state_t state = audio_player_get_state();
    if (state == AUDIO_PLAYER_STATE_PLAYING || state == AUDIO_PLAYER_STATE_PAUSE)
    {
        ESP_LOGW("Audio", "Audio is already playing or paused.");
        return;
    }
    extern const char mp3_start[] asm("_binary_gs_16b_1c_44100hz_mp3_start");
    extern const char mp3_end[] asm("_binary_gs_16b_1c_44100hz_mp3_end");
    const size_t mp3_size = (mp3_end - mp3_start) - 1;
    ESP_LOGI("Audio", "mp3_size %zu bytes", mp3_size);
    FILE* fp = fmemopen((void*)(mp3_start), mp3_size, "rb");
    audio_player_play(fp);
}

void Audio::pause()
{
    if (audio_player_get_state() == AUDIO_PLAYER_STATE_PLAYING)
    {
        audio_player_pause();
    }
}

void Audio::stop()
{
    if (audio_player_get_state() == AUDIO_PLAYER_STATE_PLAYING)
    {
        audio_player_stop();
    }
}

void Audio::resume()
{
    if (audio_player_get_state() == AUDIO_PLAYER_STATE_PAUSE)
    {
        audio_player_resume();
    }
}
