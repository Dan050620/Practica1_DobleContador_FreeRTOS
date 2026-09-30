#include "system_state.hpp"
#include "app_config.hpp"

namespace dual_counter
{

void initSystemState(AppContext& c)
{
    /* Estado global */
    c.system.runState        = RunState::Paused;
    c.system.masterDirection = CountDirection::Up;
    c.system.couplingMode    = CouplingMode::Opposite;
    c.system.speedMode       = SpeedMode::Slow;

    /* Counter1: empieza en 0 contando hacia arriba */
    c.counter1.value     = 0U;
    c.counter1.direction = CountDirection::Up;
    c.counter1.periodMs  = config::kSlowPeriodMs;
    c.counter1.displayId = 1U;

    /* Counter2: empieza en 9 contando hacia abajo (modo OPPOSITE) */
    c.counter2.value     = 9U;
    c.counter2.direction = CountDirection::Down;
    c.counter2.periodMs  = config::kSlowPeriodMs;
    c.counter2.displayId = 2U;

    /* Eventos de botones */
    c.startPauseEvent.pending = false;
    c.startPauseEvent.id      = ButtonId::StartPause;

    c.directionEvent.pending = false;
    c.directionEvent.id      = ButtonId::Direction;

    c.speedEvent.pending = false;
    c.speedEvent.id      = ButtonId::Speed;

    c.modeEvent.pending = false;
    c.modeEvent.id      = ButtonId::Mode;

    /* Handles: se llenan en createApplication() */
    c.counter1Handle = nullptr;
    c.counter2Handle = nullptr;
    c.managerHandle  = nullptr;
}

const char* toString(CountDirection direction)
{
    return (direction == CountDirection::Up) ? "UP" : "DOWN";
}

const char* toString(CouplingMode mode)
{
    return (mode == CouplingMode::Same) ? "SAME" : "OPPOSITE";
}

const char* toString(RunState state)
{
    return (state == RunState::Running) ? "RUNNING" : "PAUSED";
}

const char* toString(SpeedMode speed)
{
    return (speed == SpeedMode::Fast) ? "FAST" : "SLOW";
}

} // namespace dual_counter
