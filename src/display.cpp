#include "display.hpp"
#include "app_config.hpp"
#include "system_state.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace dual_counter
{
namespace
{

/* Bit 0 = segmento A ... bit 6 = segmento G. */
const uint8_t kDigitPatterns[10] =
{
    0x3FU, 0x06U, 0x5BU, 0x4FU, 0x66U,
    0x6DU, 0x7DU, 0x07U, 0x7FU, 0x6FU
};

} // namespace

bool SevenSegmentDisplay::enablePin(uint8_t displayId, gpio_num_t* pin) const
{
    if (displayId == 1U)
    {
        *pin = config::kDisplay1Enable;
        return true;
    }

    if (displayId == 2U)
    {
        *pin = config::kDisplay2Enable;
        return true;
    }

    return false;
}

void SevenSegmentDisplay::disableAll()
{
    gpio_set_level(config::kDisplay1Enable, config::kEnableOff);
    gpio_set_level(config::kDisplay2Enable, config::kEnableOff);
}

void SevenSegmentDisplay::writeSegments(uint8_t pattern)
{
    for (uint8_t i = 0U; i < 7U; ++i)
    {
        gpio_set_level(config::kSegmentPins[i], (pattern >> i) & 0x01U);
    }
}

void SevenSegmentDisplay::init()
{
    for (uint8_t i = 0U; i < 7U; ++i)
    {
        gpio_reset_pin(config::kSegmentPins[i]);
        gpio_set_direction(config::kSegmentPins[i], GPIO_MODE_OUTPUT);
    }

    gpio_reset_pin(config::kDisplay1Enable);
    gpio_set_direction(config::kDisplay1Enable, GPIO_MODE_OUTPUT);

    gpio_reset_pin(config::kDisplay2Enable);
    gpio_set_direction(config::kDisplay2Enable, GPIO_MODE_OUTPUT);

    disableAll();
    writeSegments(0U);
}

void SevenSegmentDisplay::showDigit(uint8_t displayId, uint8_t digit)
{
    gpio_num_t pin;

    if (digit > 9U)
    {
        return;
    }

    if (!enablePin(displayId, &pin))
    {
        return;
    }

    disableAll();
    writeSegments(kDigitPatterns[digit]);
    gpio_set_level(pin, config::kEnableOn);
}

void SevenSegmentDisplay::blank(uint8_t displayId)
{
    gpio_num_t pin;

    if (enablePin(displayId, &pin))
    {
        gpio_set_level(pin, config::kEnableOff);
    }
}

void displayRefreshTask(void* pvParameters)
{
    AppContext* context = static_cast<AppContext*>(pvParameters);
    TickType_t muxTicks = pdMS_TO_TICKS(config::kMuxPeriodMs);
    SevenSegmentDisplay display;

    if (context == nullptr)
    {
        vTaskDelete(nullptr);
        return;
    }

    /* Con tick de 100 Hz, 5 ms da 0 ticks: se fuerza minimo 1 tick. */
    if (muxTicks == 0)
    {
        muxTicks = 1;
    }

    display.init();

    for (;;)
    {
        display.showDigit(1U, context->counter1.value);
        vTaskDelay(muxTicks);

        display.showDigit(2U, context->counter2.value);
        vTaskDelay(muxTicks);
    }
}

} // namespace dual_counter
