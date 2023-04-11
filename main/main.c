#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "timers.h"

static void print_message(
        const struct timer_message *msg, const char *text);

#define MAX_QUEUE_DELAY ((SECOND_TIMER_DELAY * 2) / portTICK_PERIOD_MS)
static const char * const TAG = "app_main";

/* {{{ This program continously prints the following:
 * (in case we uncomment vTaskDelay() function call)

    First Timer Event:
            Took Ticks:      200
            Event Time:      6s
            Counter:         101
    First Timer Event:
            Took Ticks:      100
            Event Time:      4s
            Counter:         102
    Second Timer Event:
            Took Ticks:      100
            Event Time:      10s
            Counter:         103
    First Timer Event:
            Took Ticks:      200
            Event Time:      6s
            Counter:         104
    First Timer Event:
            Took Ticks:      100
            Event Time:      4s
            Counter:         105
    Second Timer Event:
            Took Ticks:      100
            Event Time:      10s
            Counter:         106
    First Timer Event:
            Took Ticks:      200
            Event Time:      6s
            Counter:         107
  .......................
 }}} */

void app_main(void) {
    ESP_ERROR_CHECK(timers_init());

    for (;;) {
        struct timer_message msg = { 0 };

        if (xQueueReceive(timers_rxq, &msg, MAX_QUEUE_DELAY)) {

            /*
             * Place the
             * vTaskDelay(100);
             * here to see tick delays in the messages.
             * It will print 100 and 200 correspondingly
             * Please note that tick counter is an extra feature which
             * is not part of the basic requirements and does not
             * intend to work this way, in the demo app.
             */

            switch (msg.timer) {
                case FIRST_TIMER_MSG:
                    print_message(&msg, "First Timer Event");
                    break;
                case SECOND_TIMER_MSG:
                    print_message(&msg, "Second Timer Event");
                    break;
                default:
                    break;
            }
        } else {
            /*
             * Supposed to never happen.
             */
            ESP_LOGE(TAG, "Time out");
        }
    }
}

static void print_message(
        const struct timer_message *msg, const char *text) {

#define TIMERS_NUM 2

    static time_t timers[TIMERS_NUM] = { 0 };
    const uint32_t ticks = xTaskGetTickCount() - msg->sent_at;
    const time_t cur_time = time(NULL);
    const time_t ev_time = cur_time- timers[msg->timer];

    timers[msg->timer] = cur_time;

    printf("%s:\n"
           "\tTook Ticks:\t %u\n"
           "\tEvent Time:\t %lus\n"
           "\tCounter:\t %d\n",
           text, ticks, ev_time, msg->count);
}

