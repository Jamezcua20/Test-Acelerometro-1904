# LIS3DH Motion Detection System - State Machine

Este repositorio contiene la implementación de una máquina de estados para la gestión de activos basada en el acelerómetro LIS3DH. El sistema utiliza interrupciones por hardware para optimizar el consumo energético y gestionar la transición entre estados de movilidad y reposo.

## Arquitectura del Sistema

El firmware se basa en una máquina de estados finitos que segrega el comportamiento del dispositivo en dos contextos operativos:

1. **STATE_STATIONARY**: Estado de bajo consumo donde el sistema permanece a la espera de un evento de aceleración superior al umbral configurado.
2. **STATE_MOVING**: Estado de actividad donde se registra movimiento continuo. Si la inactividad supera el `PARKING_TIMEOUT` (actualmente 120,000ms), el sistema regresa al estado estacionario.

## Especificaciones Técnicas

### Hardware y Comunicación
* **Sensor**: LIS3DH (Acelerómetro de 3 ejes).
* **Interfaz**: I2C (Dirección de esclavo 0x18).
* **Pin de Interrupción**: WB_IO3.

### Configuración de Registros
El sistema realiza una escritura directa en los registros del LIS3DH para configurar la lógica de detección de movimiento:
* **CTRL_REG1 (0x20)**: Configuración de frecuencia de muestreo y habilitación de ejes.
* **CTRL_REG2 (0x21)**: Configuración del filtro de paso alto para ignorar la gravedad estática.
* **CTRL_REG3 (0x22)**: Mapeo de la interrupción IA1 al pin INT1.
* **INT1_THS (0x32)**: Umbral de sensibilidad (Variables predefinidas para Auto/Montacargas).
* **INT1_DURATION (0x33)**: Tiempo mínimo de persistencia del evento de aceleración.

## Requisitos
* Framework: Arduino.
* Librerías: 
    * `SparkFunLIS3DH.h`
    * `Wire.h`

## Implementación

### Configuración de Sensibilidad
El código incluye dos perfiles de sensibilidad para el margen de estados:
* `Auto_Coto` (0x0B): Optimizado para vehículos ligeros.
* `Montacargas` (0x16): Ajuste de mayor umbral para maquinaria pesada o entornos de alta vibración.

### Flujo de Interrupción
La función `onMotionDetected()` actúa como el Interrupt Service Routine, levantando una bandera (`move_flag`) que es procesada en el ciclo principal para actualizar el cronómetro de último movimiento (`last_movement`).

## Pendientes de Integración
* Implementación de stack LoRaWAN para reporte de estados.
* Adquisición de señales GNSS/GPS durante `STATE_MOVING`.
* Gestión de modos de sueño profundo durante `STATE_STATIONARY`.
