
# https://domoticx.net/docs/lora-communication-advanced/


from lora_e220 import LoRaE220, print_configuration, Configuration
from lora_e220_constants import FixedTransmission, RssiEnableByte
from lora_e220_operation_constant import ResponseStatusCode
from lora_e220 import LoRaE220
from machine import Pin
from machine import UART
import time

uart = UART(1, 9600)
lora = LoRaE220('400T22D', uart, aux_pin=15, m0_pin=21, m1_pin=19)

code = lora.begin()
print("Initialization: {}", ResponseStatusCode.get_description(code))

# Set the configuration to default values and print the updated configuration to the console
# Not needed if already configured
configuration_to_set = Configuration('400T22D')
configuration_to_set.ADDH = 0x00 # Address of this sender
configuration_to_set.ADDL = 0x02 # Address of this sender
configuration_to_set.CHAN = 5 # Channel of this sender
configuration_to_set.TRANSMISSION_MODE.fixedTransmission = FixedTransmission.FIXED_TRANSMISSION
# # To enable RSSI, you must also enable RSSI on receiver
configuration_to_set.TRANSMISSION_MODE.enableRSSI = RssiEnableByte.RSSI_DISABLED
code, confSetted = lora.set_configuration(configuration_to_set)
# 
code, configuration = lora.get_configuration()
print("Retrieve configuration: {}", ResponseStatusCode.get_description(code))
print_configuration(configuration)

counter = 0

while True:
  counter = counter + 1
  print("send message:", counter)
  #lora.send_transparent_message(str(counter))
  #code = lora.send_fixed_message(0, 3, 23, "Hello LoRa")
  lora.send_fixed_message(0, 3, 5, '0-3-5:' + str(counter))
  #data = {'key1': 'value1', 'key2': 'value2'}
  #code = lora.send_transparent_dict(data)
  print("Send message: {}", ResponseStatusCode.get_description(code))
  #time.sleep(.2)
  #lora.send_fixed_message(0, 3, 23, 'pippo0-3-23' + str(counter))
  #print("Send message: {}", ResponseStatusCode.get_description(code))
  time.sleep(2)
