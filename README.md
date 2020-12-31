# stm32-can-obd
## Wires
```
  STM32           TJA1050            OBD
+-------+       +---------+       +-------+
|       |       |         |       |       |
|   CANRX <---> RX    CAN_H <---> CAN_H   |
|       |       |         |       |       |
|   CANTX <---> TX    CAN_L <---> CAN_L   |
|       |       |         |       |       |
|     GND <---> GND     GND <---> GND     |
|       |       |         |       |       |
+-------+       +---------+       +-------+
```
