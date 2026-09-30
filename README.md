# Práctica 1: Doble Contador Display con FreeRTOS y Task Manager (ESP32-S3 / C++)

**Asignatura:** Sistemas Embebidos en Tiempo Real  

//INTEGRANTES 
//Daniel Pérez González 10992
//Alejandro Gomez Velazquez 11029
//Jessica Figueroa Álvarez 10080

---

## Cuestionario contestado

1. **¿Por qué Counter1 y Counter2 pueden ejecutar la misma función counterTask() y comportarse diferente?**  
   Porque la función es reentrante e independiente. Cada instancia recibe su propio puntero de contexto mediante `pvParameters`, lo cual permite acceder a variables de configuración (`CounterConfig`) distintas en memoria RAM.

2. **¿Qué información recibe cada tarea mediante pvParameters?**  
   Recibe la dirección de una estructura `CounterConfig` que contiene el identificador de la tarea, la dirección actual de conteo (`UP`/`DOWN`), el retardo o velocidad (`delayMs`) y el valor numérico actual.

3. **¿Qué representa un TaskHandle_t y por qué el Task Manager necesita conservarlo?**  
   Representa un puntero al *Task Control Block* (TCB) de FreeRTOS. El Task Manager lo necesita para invocar funciones del API de la RTOS sobre esa tarea específica, tales como `vTaskSuspend()` y `vTaskResume()`.

4. **¿Qué diferencia existe entre BLOCKED y SUSPENDED en esta práctica?**  
   En `BLOCKED`, la tarea está esperando un evento o temporizador (`vTaskDelay`). En `SUSPENDED`, la tarea ha sido pausada explícitamente por el Planificador vía `vTaskSuspend()` y no volverá a ejecutarse hasta que otra tarea ejecute `vTaskResume()`.

5. **¿Qué ocurre con vTaskDelay() cuando una tarea es suspendida?**  
   El temporizador de la tarea queda congelado. Al reanudarse mediante `vTaskResume()`, la tarea continúa el tiempo que le restaba dentro de `vTaskDelay()` desde el punto donde fue suspendida.

6. **¿Qué ventaja aporta enum class frente a constantes enteras para representar estados?**  
   Proporciona tipado fuerte (*strongly typed*), evitando conversiones implícitas no deseadas entre distintos estados, reduciendo errores en tiempo de compilación y delimitando el ámbito de las constantes.

7. **¿Qué responsabilidad tiene BcdCounter y cuál SevenSegmentDisplay?**  
   `BcdCounter` gestiona únicamente la lógica matemática de incremento, decremento y límites BCD (0-9). `SevenSegmentDisplay` abstrae la capa de hardware y el mapeo físico de los pines para la visualización de los segmentos.

8. **¿Por qué volatile no resuelve por sí solo los problemas de concurrencia?**  
   La palabra reservada `volatile` le indica al compilador no optimizar lecturas/escrituras en caché o registros, obligándolo a ir directamente a RAM. No garantiza atomicidad ni previene *race conditions* cuando dos tareas modifican una variable al mismo tiempo.

9. **¿Qué cambiaría en el diseño cuando posteriormente se permitan queues o semáforos?**  
   Se eliminaría el acceso a variables globales mediadas por *polling* y banderas `volatile`. Las tareas de botones notificarían al Task Manager usando colas (`xQueueSend`), garantizando hilos seguros sin uso ineficiente de CPU.

10. **Si ambas tareas tienen la misma prioridad, ¿cómo interviene el scheduler de FreeRTOS?**  
    Aplica *Round-Robin* impulsado por las interrupciones por *tick* del sistema, repartiendo equitativamente el tiempo de procesador entre ambas tareas activas.

---

## Conclusión de Daniel Pérez González 10992
Dividirnos el trabajo fue clave: me encargué de la programación en C++. El reto técnico mayor fue adaptar el proyecto a nuestra ESP32-S3, ya que al ser un modelo distinto al del profesor, tuvimos que reajustar los GPIOs en la configuración sin romper la lógica del código.

## Conclusión de Alejandro Gomez Velazquez 11029
Jessica y yo nos encargamos de todo el cableado en la protoboard. Lo más enredado fue puentear las 7 líneas de los segmentos entre los dos displays y conectar los pines de habilitación (enable). Tuvimos que cuidar mucho el orden de los cables y las resistencias para que al alternar el encendido no se vieran números "fantasma" ni destellos.

## Conclusión de Jessica Figueroa Álvarez 10080
Acomodar los 4 botones y los displays en la ESP32-S3 fue un reto porque los pines no coincidían con la tarjeta del profe. Tuvimos que revisar el pinout de nuestra S3 para no usar pines con restricciones de la tarjeta y asegurarnos de que las lecturas de los botones llegaran limpias sin falsos disparos.


## Conclusión Técnica y Reflexión

* **¿Qué resolvió pvParameters?** Permitió reutilizar exactamente la misma función de tarea (`counterTask`) para múltiples contadores, asignando parámetros e instancias de datos independientes sin duplicar código.
* **¿Qué resolvió TaskHandle_t?** Le dio al Task Manager un identificador directo sobre las tareas de conteo para controlar su estado de ejecución (pausa y reanudación) mediante `vTaskSuspend()` y `vTaskResume()`.
* **¿Qué riesgo queda al no usar mecanismos de sincronización?** Existe el riesgo de condiciones de carrera (*race conditions*) y lecturas o escrituras inconsistentes al acceder y modificar la configuración compartida mientras las tareas de conteo están en ejecución.