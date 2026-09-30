#include "app_tasks.hpp"
#include "app_config.hpp"
#include "buttons.hpp"
#include "counter.hpp"
#include "display.hpp"
#include "esp_log.h"

namespace dual_counter
{
namespace
{

const char* const TAG = "TASK_MANAGER";

constexpr uint32_t kSmallStackBytes   = 2048U;
constexpr uint32_t kManagerStackBytes = 4096U;
constexpr UBaseType_t kWorkerPriority  = 2;
constexpr UBaseType_t kManagerPriority = 3;

CountDirection oppositeDirection(CountDirection direction)
{
    return (direction == CountDirection::Up) ? CountDirection::Down : CountDirection::Up;
}

void applyConfiguration(AppContext& c)
{
    c.counter1.direction = c.system.masterDirection;

    if (c.system.couplingMode == CouplingMode::Same)
    {
        c.counter2.direction = c.system.masterDirection;
    }
    else
    {
        c.counter2.direction = oppositeDirection(c.system.masterDirection);
    }

    const uint32_t period = (c.system.speedMode == SpeedMode::Fast)
                                ? config::kFastPeriodMs
                                : config::kSlowPeriodMs;

    c.counter1.periodMs = period;
    c.counter2.periodMs = period;
}

/* Suspension / reanudacion de Counter1 y Counter2 mediante sus TaskHandle_t. */
void setCountersRunning(AppContext& c, bool running)
{
    if (running)
    {
        vTaskResume(c.counter1Handle);
        vTaskResume(c.counter2Handle);
    }
    else
    {
        vTaskSuspend(c.counter1Handle);
        vTaskSuspend(c.counter2Handle);
    }
}

/*
 * Al pasar de OPPOSITE a SAME ambos displays se igualan:
 *   - direccion DOWN -> al digito de menor valor
 *   - direccion UP   -> al digito de mayor valor
 */
void alignCountersForSameMode(AppContext& c)
{
    const uint8_t v1 = c.counter1.value;
    const uint8_t v2 = c.counter2.value;
    uint8_t target;

    if (c.system.masterDirection == CountDirection::Down)
    {
        target = (v1 < v2) ? v1 : v2;
    }
    else
    {
        target = (v1 > v2) ? v1 : v2;
    }

    c.counter1.value = target;
    c.counter2.value = target;
}

} // namespace

void taskManager(void* pvParameters)
{
    AppContext* ctx = static_cast<AppContext*>(pvParameters);

    if (ctx == nullptr)
    {
        vTaskDelete(nullptr);
        return;
    }

    AppContext& c = *ctx;

    applyConfiguration(c);
    setCountersRunning(c, false);

    for (;;)
    {
        if (c.startPauseEvent.pending)
        {
            c.startPauseEvent.pending = false;

            c.system.runState = (c.system.runState == RunState::Paused)
                                    ? RunState::Running
                                    : RunState::Paused;

            setCountersRunning(c, c.system.runState == RunState::Running);

            ESP_LOGI(TAG, "STATE=%s", toString(c.system.runState));
        }

        /* Direccion, velocidad y modo se ignoran estando en PAUSE. */
        if (c.system.runState == RunState::Running)
        {
            if (c.directionEvent.pending)
            {
                c.directionEvent.pending = false;

                c.system.masterDirection = (c.system.masterDirection == CountDirection::Down)
                                               ? CountDirection::Up
                                               : CountDirection::Down;
                applyConfiguration(c);

                ESP_LOGI(TAG, "DIR=%s", toString(c.system.masterDirection));
            }

            if (c.speedEvent.pending)
            {
                c.speedEvent.pending = false;

                c.system.speedMode = (c.system.speedMode == SpeedMode::Slow)
                                         ? SpeedMode::Fast
                                         : SpeedMode::Slow;
                applyConfiguration(c);

                ESP_LOGI(TAG, "SPEED=%s", toString(c.system.speedMode));
            }

            if (c.modeEvent.pending)
            {
                c.modeEvent.pending = false;

                if (c.system.couplingMode == CouplingMode::Opposite)
                {
                    c.system.couplingMode = CouplingMode::Same;
                    alignCountersForSameMode(c);
                }
                else
                {
                    c.system.couplingMode = CouplingMode::Opposite;
                }

                applyConfiguration(c);

                ESP_LOGI(TAG, "MODE=%s", toString(c.system.couplingMode));
            }
        }
        else
        {
            /* En PAUSE se descartan los eventos de dir/vel/modo. */
            c.directionEvent.pending = false;
            c.speedEvent.pending = false;
            c.modeEvent.pending = false;
        }

        vTaskDelay(pdMS_TO_TICKS(config::kManagerPeriodMs));
    }
}

void createApplication(AppContext& c)
{
    /* static: deben seguir vivos cuando createApplication() regrese. */
    static ButtonTaskParams startParams;
    static ButtonTaskParams directionParams;
    static ButtonTaskParams speedParams;
    static ButtonTaskParams modeParams;

    startParams.pin  = config::kButtonStartPause;
    startParams.event = &c.startPauseEvent;

    directionParams.pin  = config::kButtonDirection;
    directionParams.event = &c.directionEvent;

    speedParams.pin  = config::kButtonSpeed;
    speedParams.event = &c.speedEvent;

    modeParams.pin  = config::kButtonMode;
    modeParams.event = &c.modeEvent;

    /*
     * MISMA funcion counterTask() --> dos instancias distintas.
     * pvParameters decide que CounterConfig gobierna cada tarea.
     * Los handles se guardan para que el TaskManager las suspenda/reanude.
     */
    xTaskCreate(counterTask, "Counter1", kSmallStackBytes, &c.counter1, kWorkerPriority, &c.counter1Handle);
    xTaskCreate(counterTask, "Counter2", kSmallStackBytes, &c.counter2, kWorkerPriority, &c.counter2Handle);

    /* MISMA funcion buttonTask() --> cuatro botones, parametrizada por ButtonTaskParams*. */
    xTaskCreate(buttonTask, "BtnStart",     kSmallStackBytes, &startParams,     kWorkerPriority, nullptr);
    xTaskCreate(buttonTask, "BtnDirection", kSmallStackBytes, &directionParams, kWorkerPriority, nullptr);
    xTaskCreate(buttonTask, "BtnSpeed",     kSmallStackBytes, &speedParams,     kWorkerPriority, nullptr);
    xTaskCreate(buttonTask, "BtnMode",      kSmallStackBytes, &modeParams,      kWorkerPriority, nullptr);

    xTaskCreate(displayRefreshTask, "DisplayRefresh", kSmallStackBytes, &c, kWorkerPriority, nullptr);
    xTaskCreate(taskManager, "TaskManager", kManagerStackBytes, &c, kManagerPriority, &c.managerHandle);
}

} // namespace dual_counter
