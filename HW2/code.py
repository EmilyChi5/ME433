import time
import board
import pwmio

SERVO_PIN = board.GP16
MIN_DUTY = 0.015   # 1.5%
MAX_DUTY = 0.12    # 12%

pwm = pwmio.PWMOut(SERVO_PIN, frequency=50, duty_cycle=0)

def set_servo(angle):
    angle = max(0, min(180, angle))
    duty = MIN_DUTY + (angle / 180) * (MAX_DUTY - MIN_DUTY)
    pwm.duty_cycle = int(duty * 65535)

while True:
    for angle in range(0, 181, 2):
        set_servo(angle)
        time.sleep(0.02)

    for angle in range(180, -1, -2):
        set_servo(angle)
        time.sleep(0.02)
