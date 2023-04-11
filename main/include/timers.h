#pragma once

enum {
    FIRST_TIMER_MSG,
    SECOND_TIMER_MSG
};

struct timer_message {
    int timer;
    int count;
    TickType_t sent_at;
};

#define FIRST_TIMER_DELAY CONFIG_FIRST_TIMER_DELAY
#define SECOND_TIMER_DELAY CONFIG_SECOND_TIMER_DELAY

extern QueueHandle_t timers_rxq;

esp_err_t timers_init(void);

