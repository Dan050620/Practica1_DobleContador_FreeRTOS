#pragma once

#include <cstdint>
#include "driver/gpio.h"

namespace dual_counter
{

/*
 * Dos displays de 7 segmentos (catodo comun) con segmentos compartidos y
 * una habilitacion por display.
 * Responsabilidad: convertir un digito 0-9 en niveles de GPIO.
 */
class SevenSegmentDisplay
{
public:
    void init();
    void showDigit(uint8_t displayId, uint8_t digit);
    void blank(uint8_t displayId);

private:
    void disableAll();
    void writeSegments(uint8_t pattern);
    bool enablePin(uint8_t displayId, gpio_num_t* pin) const;
};

/* Tarea de refresco (multiplexado) de ambos displays. Recibe AppContext*. */
void displayRefreshTask(void* pvParameters);

} // namespace dual_counter
