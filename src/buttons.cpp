#include "buttons.hpp"
#include "app_config.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace dual_counter
{

PushButton::PushButton(gpio_num_t pin)
    : pin_(pin), previous_(false)
{
}

void PushButton::init()
{
    gpio_reset_pin(pin_);
    gpio_set_direction(pin_, GPIO_MODE_INPUT);
    gpio_set_pull_mode(pin_, GPIO_PULLUP_ONLY);
}

bool PushButton::isPressed() const
{
    return gpio_get_level(pin_) == 0;
}

bool PushButton::pressedEdge()
{
    const bool current = isPressed();
    const bool edge = current && !previous_;

    previous_ = current;
    return edge;
}

void buttonTask(void* pvParameters)
{
    ButtonTaskParams* params = static_cast<ButtonTaskParams*>(pvParameters);

    if ((params == nullptr) || (params->event == nullptr))
    {
        vTaskDelete(nullptr);
        return;
    }

    PushButton button(params->pin);
    button.init();

    for (;;)
    {
        if (button.pressedEdge())
        {
            params->event->pending = true;
        }

        vTaskDelay(pdMS_TO_TICKS(config::kButtonPeriodMs));
    }
}

} // namespace dual_counter
