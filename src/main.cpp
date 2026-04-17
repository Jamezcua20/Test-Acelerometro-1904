#include <Arduino.h>
#include "SparkFunLIS3DH.h"
#include <Wire.h>

LIS3DH SensorTwo(I2C_MODE, 0x18);
#define ACC_INT_PIN WB_IO3

typedef enum{
	STATE_STATIONARY,
	STATE_MOVING
} SystemState;

SystemState current_state = STATE_STATIONARY;

bool move_flag = false;
unsigned long last_movement = 0;
const unsigned long PARKING_TIMEOUT = 120000;

void onMotionDetected() {
  move_flag = true;
}

/*void lis3dh_read_data()
{
  // Datos en bruto

  Serial.print(" X(g) = ");
  Serial.println(SensorTwo.readFloatAccelX(), 4);
  Serial.print(" Y(g) = ");
  Serial.println(SensorTwo.readFloatAccelY(), 4);
  Serial.print(" Z(g)= ");
  Serial.println(SensorTwo.readFloatAccelZ(), 4);
}*/

void setup()
{
	uint8_t Auto_Coto = 0x0B;
	uint8_t Montacargas = 0x16; // Cambia la sensibilidad para el margen de estados

	unsigned long timeout = millis();
	Serial.begin(115200);
	while (!Serial)
	{
		if ((millis() - timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
	}

	if (SensorTwo.begin() != 0)
	{
		Serial.println("Problem starting the sensor at 0x18.");
	}
	else
	{
		Serial.println("Sensor at 0x18 started.");
		uint8_t data_to_write = 0;
		SensorTwo.readRegister(&data_to_write, LIS3DH_CTRL_REG1);
		data_to_write |= 0x08;
		SensorTwo.writeRegister(LIS3DH_CTRL_REG1, data_to_write);
		data_to_write = 0;
		SensorTwo.readRegister(&data_to_write, 0x1E);
		data_to_write |= 0x90;
		SensorTwo.writeRegister(0x1E, data_to_write);
		data_to_write = 0;
		SensorTwo.readRegister(&data_to_write, 0x21);
		data_to_write |= 0x09;
		SensorTwo.writeRegister(0x21, data_to_write);
		SensorTwo.writeRegister(0x22, 0x40); 
  		SensorTwo.writeRegister(0x30, 0x2A); 
  		SensorTwo.writeRegister(0x32, Auto_Coto); 
  		SensorTwo.writeRegister(0x33, 0x0A); 

		uint8_t dummy = 0;
  		SensorTwo.readRegister(&dummy, 0x31);

  		pinMode(ACC_INT_PIN, INPUT_PULLDOWN); 
  		attachInterrupt(digitalPinToInterrupt(ACC_INT_PIN), onMotionDetected, RISING);
		Serial.println("SYSTEM INITIALIZED");
	}
}

void loop() {

  if (move_flag) {
    move_flag = false; 
    
    uint8_t int_src = 0;
    SensorTwo.readRegister(&int_src, 0x31); 
    
    last_movement = millis();
  }

  switch(current_state) {
      
      case STATE_STATIONARY:
          if (last_movement > 0 && (millis() - last_movement < 100)) {
              Serial.println("Cambiando a STATE_MOVING.");
              // Falta ingresar la parte de LoRaWAN y GPS
              current_state = STATE_MOVING;
          }
          break;

      case STATE_MOVING:
          if (millis() - last_movement > PARKING_TIMEOUT) {
              Serial.println("Cambiando a STATE_STATIONARY.");
              //Falta ingresar la parte de LoRaWAN y GPS
              current_state = STATE_STATIONARY;
          }
          break;
  }
}