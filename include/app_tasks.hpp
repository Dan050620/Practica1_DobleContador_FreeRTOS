#pragma once

#include "system_state.hpp"

namespace dual_counter
{

/* Unico responsable de interpretar los botones y gobernar los contadores. */
void taskManager(void* pvParameters);

/* Crea todas las tareas de la aplicacion y guarda los handles en el contexto. */
void createApplication(AppContext& context);

} // namespace dual_counter
