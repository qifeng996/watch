//
// Created by wind on 25-3-23.
//

#ifndef AUDIO_H
#define AUDIO_H
#include <inttypes.h>
#include "audio_player.h"

#ifdef __cplusplus
class Audio {

public:
    Audio();
    ~Audio();
    QueueHandle_t audio_event_queue_handle;
    void init();

    static void player_cb(audio_player_cb_ctx_t *ctx);
    void play();

    void pause();

    void stop();

    void resume();

};
extern Audio *audio;
#endif

#endif //AUDIO_H
