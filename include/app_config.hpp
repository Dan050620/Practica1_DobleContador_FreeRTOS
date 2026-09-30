#pragma once

#include <cstdint>
#include "driver/gpio.h"

/*
 * Configuracion de hardware y tiempos.
 * Si cambia el cableado, solo hay que tocar este archivo y la clase
 * SevenSegmentDisplay.
 */
namespace dual_counter
{
namespace config
{

/* Segmentos A..G compartidos por ambos displays (multiplexados). */
constexpr gpio_num_t kSegmentPins[7] =
{
    GPIO_NUM_4,  /* A */
    GPIO_NUM_5,  /* B */
    GPIO_NUM_6,  /* C */
    GPIO_NUM_7,  /* D */
    GPIO_NUM_15, /* E */
    GPIO_NUM_16, /* F */
    GPIO_NUM_17  /* G */
};

/* Habilitacion (catodo comun) de cada display. */
constexpr gpio_num_t kDisplay1Enable = GPIO_NUM_18;
constexpr gpio_num_t kDisplay2Enable = GPIO_NUM_8;

/*
 * Nivel de las lineas de habilitacion con el catodo comun conectado directo
 * al GPIO: catodo en 0 -> display encendido, catodo en 1 -> display apagado.
 * Con un NPN en cada catodo se invierten (On = 1, Off = 0).
 */
constexpr uint32_t kEnableOn  = 0U;
constexpr uint32_t kEnableOff = 1U;

/* Botones a GND (pull-up interno). */
constexpr gpio_num_t kButtonStartPause = GPIO_NUM_9;
constexpr gpio_num_t kButtonDirection  = GPIO_NUM_10;
constexpr gpio_num_t kButtonSpeed      = GPIO_NUM_11;
constexpr gpio_num_t kButtonMode       = GPIO_NUM_12;

/* Tiempos. */
constexpr uint32_t kSlowPeriodMs    = 500U;
constexpr uint32_t kFastPeriodMs    = 250U;
constexpr uint32_t kButtonPeriodMs  = 20U;
constexpr uint32_t kManagerPeriodMs = 10U;
constexpr uint32_t kMuxPeriodMs     = 5U;

} // namespace config
} // namespace dual_counter
