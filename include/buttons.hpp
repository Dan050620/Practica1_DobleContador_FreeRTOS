#pragma once

#include "driver/gpio.h"
#include "system_state.hpp"

namespace dual_counter
{

/* Boton normalmente abierto a GND con pull-up interno. */
class PushButton
{
public:
    explicit PushButton(gpio_num_t pin);

    void init();
    bool isPressed() const;

    /* Devuelve true una sola vez por cada pulsacion (flanco). */
    bool pressedEdge();

private:
    gpio_num_t pin_;
    bool previous_;
};

/* Parametros de una tarea de boton (se pasan por pvParameters). */
struct ButtonTaskParams
{
    gpio_num_t pin;
    ButtonEvent* event;
};

/* Misma funcion para los cuatro botones; cambia ButtonTaskParams*. */
void buttonTask(void* pvParameters);

} // namespace dual_counter
