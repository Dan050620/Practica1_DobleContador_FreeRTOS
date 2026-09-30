#pragma once

#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace dual_counter
{

enum class CountDirection : uint8_t
{
    Up = 0,
    Down
};

enum class CouplingMode : uint8_t
{
    Opposite = 0, /* MODE = 0 */
    Same          /* MODE = 1 */
};

enum class RunState : uint8_t
{
    Paused = 0,
    Running
};

enum class SpeedMode : uint8_t
{
    Slow = 0, /* 500 ms */
    Fast      /* 250 ms */
};

enum class ButtonId : uint8_t
{
    StartPause = 0,
    Direction,
    Speed,
    Mode
};

/* Configuracion de un contador: es lo que recibe counterTask() por pvParameters. */
struct CounterConfig
{
    volatile uint8_t value;
    volatile CountDirection direction;
    volatile uint32_t periodMs;
    uint8_t displayId;
};

/* Flag simple publicado por una tarea de boton y consumido por el TaskManager. */
struct ButtonEvent
{
    volatile bool pending;
    ButtonId id;
};

/* Estado global del sistema. Solo el TaskManager lo modifica. */
struct SystemState
{
    volatile RunState runState;
    volatile CountDirection masterDirection;
    volatile CouplingMode couplingMode;
    volatile SpeedMode speedMode;
};

/* Contexto compartido de toda la aplicacion. */
struct AppContext
{
    SystemState system;

    CounterConfig counter1;
    CounterConfig counter2;

    ButtonEvent startPauseEvent;
    ButtonEvent directionEvent;
    ButtonEvent speedEvent;
    ButtonEvent modeEvent;

    TaskHandle_t counter1Handle;
    TaskHandle_t counter2Handle;
    TaskHandle_t managerHandle;
};

void initSystemState(AppContext& context);

const char* toString(CountDirection direction);
const char* toString(CouplingMode mode);
const char* toString(RunState state);
const char* toString(SpeedMode speed);

} // namespace dual_counter
