import math
import pygame
import pgzero
import pgzrun
import serial

ser = serial.Serial('COM8', 115200, timeout=0.01)
print("Opening port:", ser.name)

WIDTH = 600
HEIGHT = 400

sensor_value = 0

def update():
    global sensor_value

    line = ser.readline().decode(errors="ignore").strip()

    if line.startswith("L:"):
        try:
            sensor_value = int(line[2:])
        except:
            pass

def draw():
    screen.fill((0, 0, 0))

    # convert 0–65535 sensor value to 0–500 bar width
    bar_width = int(sensor_value / 65535 * 500)

    screen.draw.text(
        "Pico Sensor Visualizer",
        (170, 40),
        fontsize=40,
        color="white"
    )

    screen.draw.text(
        "A0 value: " + str(sensor_value),
        (220, 100),
        fontsize=30,
        color="white"
    )

    # outline box
    screen.draw.rect(Rect((50, 180), (500, 60)), "white")

    # filled bar
    screen.draw.filled_rect(Rect((50, 180), (bar_width, 60)), "green")

    screen.draw.text(
        "Turn potentiometer",
        (120, 280),
        fontsize=28,
        color="white"
    )

pgzrun.go()