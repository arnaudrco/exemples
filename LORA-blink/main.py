from machine import Pin, I2C
import ahtx0
from time import sleep
from bmp280 import BMP280


from machine import UART, Pin
import time
led=Pin(8,Pin.OUT)

# UART1 pour ESP32-C3 : TX=GPIO21, RX=GPIO20 par défaut
# E220-900T : TX module -> RX ESP32, RX module -> TX ESP32
uart = UART(1, baudrate=9600, tx=21, rx=20)

i2c = I2C(0, sda=Pin(8), scl=Pin(9), freq=400000)
sensor = ahtx0.AHT20(i2c)
# ESP32-C3 : broches I2C par défaut (GPIO 8 = SDA, GPIO 9 = SCL)
i2c = I2C(0, sda=Pin(8), scl=Pin(9), freq=400000)
bmp = BMP280(i2c, addr=0x77)  # 0x76 si le pin SD du module est à la masse


time.sleep(1)  # Laisser le module démarrer

while True:
    uart.write('ANUMBY\n')
    print('Envoyé: Hello')
    print(f"Température : {sensor.temperature:.1f} °C")
    print(f"Humidité    : {sensor.relative_humidity:.1f} %")
    print(f"Pression : {bmp.pressure:.1f} Pa")  # en Pascals
    time.sleep(1)
    led.value(0)
    uart.write('Bonjour\n')
    time.sleep(1)
    led.value(1)
