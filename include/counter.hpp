#pragma once

#include <cstdint>
#include "system_state.hpp"

namespace dual_counter
{

/*
 * Contador BCD 0-9. No usa FreeRTOS: se puede probar por separado.
 * Responsabilidad: mantener y actualizar el valor (con rollover 9->0 y 0->9).
 */
class BcdCounter
{
public:
    explicit BcdCounter(uint8_t initialValue = 0U);

    void set(uint8_t value);
    void step(CountDirection direction);
    uint8_t value() const;

private:
    uint8_t value_;
};

/*
 * Tarea de conteo. Counter1 y Counter2 usan esta MISMA funcion; cambia
 * el CounterConfig* que reciben por pvParameters.
 */
void counterTask(void* pvParameters);

} // namespace dual_counter
