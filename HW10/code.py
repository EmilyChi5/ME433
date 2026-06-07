import time
import board
import analogio

sensor = analogio.AnalogIn(board.A0)

while True:
    print("L:" + str(sensor.value))
    time.sleep(1/30)