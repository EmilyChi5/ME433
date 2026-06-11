import math
import serial
import pgzrun

ser = serial.Serial('COM8', 115200, timeout=0.01)
print("Opening port:", ser.name)

WIDTH = 600
HEIGHT = 600

angle = 0

def update():
    global angle

    line = ser.readline().decode(errors="ignore").strip()

    if line.startswith("A:"):
        try:
            angle = float(line[2:])
        except:
            pass

def draw():
    screen.fill((0, 0, 0))

    screen.draw.text(
        "AS5600 Encoder Visualization",
        center=(300, 40),
        fontsize=40,
        color="white"
    )

    screen.draw.text(
        f"Angle = {angle:.1f} deg",
        center=(300, 90),
        fontsize=30,
        color="white"
    )

    cx = WIDTH // 2
    cy = HEIGHT // 2 + 50

    length = 180

    x2 = cx + length * math.cos(math.radians(angle))
    y2 = cy - length * math.sin(math.radians(angle))

    # encoder center
    screen.draw.filled_circle((cx, cy), 10, "white")

    # rotating arm
    screen.draw.line((cx, cy), (x2, y2), "green")

    # end point
    screen.draw.filled_circle((x2, y2), 15, "red")

    # reference circle
    screen.draw.circle((cx, cy), length, "white")

pgzrun.go()