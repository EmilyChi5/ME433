import board
import busio
import time

i2c = busio.I2C(scl=board.GP22, sda=board.GP21)

while True:

    try:

        while not i2c.try_lock():
            pass

        devices = [hex(x) for x in i2c.scan()]

        print("Devices found:", devices)

        i2c.unlock()

    except Exception as e:
        print("Error:", e)

    time.sleep(3)