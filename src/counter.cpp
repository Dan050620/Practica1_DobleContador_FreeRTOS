#include "counter.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace dual_counter
{

BcdCounter::BcdCounter(uint8_t initialValue)
    : value_(static_cast<uint8_t>(initialValue % 10U))
{
}

void BcdCounter::set(uint8_t value)
{
    value_ = static_cast<uint8_t>(value % 10U);
}

void BcdCounter::step(CountDirection direction)
{
    if (direction == CountDirection::Up)
    {
        value_ = (value_ >= 9U) ? 0U : static_cast<uint8_t>(value_ + 1U);
    }
    else
    {
        value_ = (value_ == 0U) ? 9U : static_cast<uint8_t>(value_ - 1U);
    }
}

uint8_t BcdCounter::value() const
{
    return value_;
}

void counterTask(void* pvParameters)
{
    CounterConfig* config = static_cast<CounterConfig*>(pvParameters);
    BcdCounter counter;

    if (config == nullptr)
    {
        vTaskDelete(nullptr);
        return;
    }

    for (;;)
    {
        TickType_t periodTicks = pdMS_TO_TICKS(config->periodMs);
        if (periodTicks == 0)
        {
            periodTicks = 1;
        }

        const TickType_t start = xTaskGetTickCount();
        vTaskDelay(periodTicks);

        /*
         * Si el TaskManager suspendio y reanudo esta tarea, vTaskDelay()
         * regresa ANTES de cumplir el periodo. En ese caso no se cuenta:
         * se reinicia el ciclo y el contador continua desde su ultimo valor.
         */
        if ((xTaskGetTickCount() - start) < periodTicks)
        {
            continue;
        }

        /*
         * Se relee config->value porque el TaskManager puede haberlo
         * ajustado (cambio OPPOSITE -> SAME).
         */
        counter.set(config->value);
        counter.step(config->direction);
        config->value = counter.value();
    }
}

} // namespace dual_counter
