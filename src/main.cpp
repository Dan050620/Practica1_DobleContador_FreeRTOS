#include "app_tasks.hpp"
#include "system_state.hpp"

extern "C" void app_main(void)
{
    static dual_counter::AppContext context;

    dual_counter::initSystemState(context);
    dual_counter::createApplication(context);
}
