#include <limits.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "timers.h"

static void first_timer_task(TimerHandle_t arg);
static void second_timer_task(TimerHandle_t arg);

#define TIMERS_QUEUE_SIZE 10

static const char * const TAG = "timers";

QueueHandle_t timers_rxq = NULL;

static volatile int counter = 0;
static SemaphoreHandle_t mutex = NULL;

esp_err_t timers_init(void) {
    static TimerHandle_t first_timer = NULL;
    static TimerHandle_t second_timer = NULL;

    if (first_timer || second_timer) {
        ESP_LOGE(TAG, "Couldn't be initialized twice");
        return ESP_FAIL;
    }

    if (timers_rxq == NULL) {
        timers_rxq = xQueueCreate(TIMERS_QUEUE_SIZE,
                sizeof(struct timer_message));

        if (timers_rxq == NULL) {
            return ESP_FAIL;
        }
    }

    if (mutex == NULL) {
        mutex = xSemaphoreCreateMutex();

        if (mutex == NULL) {
            return ESP_FAIL;
        }
    }

    first_timer = xTimerCreate("FirstTimer",
            (FIRST_TIMER_DELAY / portTICK_PERIOD_MS),
            pdTRUE, 0, first_timer_task);

    if (first_timer == NULL || xTimerStart(first_timer, 0) != pdPASS) {
        xTimerDelete(first_timer, 0);
        first_timer = NULL;
        return ESP_FAIL;
    }

    second_timer = xTimerCreate("SecondTimer",
            (SECOND_TIMER_DELAY / portTICK_PERIOD_MS),
            pdTRUE, 0, second_timer_task);

    if (second_timer == NULL || xTimerStart(second_timer, 0) != pdPASS) {
        /*
         * Lets make it bug-free
         */
        xTimerDelete(first_timer, 0);
        xTimerDelete(second_timer, 0);
        first_timer = NULL;
        second_timer = NULL;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Started successfully");
    return ESP_OK;
}

static void first_timer_task(TimerHandle_t arg) {
    (void) arg;
    const struct timer_message msg = {
        FIRST_TIMER_MSG,
        counter,
        xTaskGetTickCount()
    };

    xQueueSend(timers_rxq, (void *)&msg, (TickType_t)0);

    /*
     * We start our messages from zero.
     */
    xSemaphoreTake(mutex, portMAX_DELAY);
    if (counter < INT_MAX) {
        counter++;
    }
    xSemaphoreGive(mutex);
}

static void second_timer_task(TimerHandle_t arg) {
    (void) arg;
    const struct timer_message msg = {
        SECOND_TIMER_MSG,
        counter,
        xTaskGetTickCount()
    };

    xQueueSend(timers_rxq, (void *)&msg, (TickType_t)0);

    xSemaphoreTake(mutex, portMAX_DELAY);
    if (counter < INT_MAX) {
        counter++;
    }
    xSemaphoreGive(mutex);
}

