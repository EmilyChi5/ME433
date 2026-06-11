import numpy as np
import matplotlib.pyplot as plt

x = np.linspace(-1, 1, 500)

A = 1.0
sigma = 0.18

# Haptic effects
F_bump = A * np.exp(-(x**2)/(2*sigma**2))
F_dip = -A * np.exp(-(x**2)/(2*sigma**2))
F_toggle = np.tanh(4*x)

print("\nEquations Used:")
print("Bump:   F(x) = exp(-x**2 / (2*sigma**2))")
print("Dip:    F(x) = -exp(-x**2 / (2*sigma**2))")
print("Toggle: F(x) = tanh(4*x)")
print(f"sigma = {sigma}")

print("""
Haptic Force Curve Explanation:
- Bump (red): Creates resistance near the center, simulating a raised bump.
- Dip (blue): Pulls the paddle toward the center, simulating a valley or detent.
- Toggle Switch (purple): Pushes the paddle toward either side, creating two stable positions like a switch.
- Displacement (x) and force (F) are normalized between -1 and +1.
""")

plt.plot(x, F_bump, 'r', linewidth=2, label='Bump')
plt.plot(x, F_dip, 'b', linewidth=2, label='Dip')
plt.plot(x, F_toggle, color='purple', linewidth=2, label='Toggle Switch')
plt.xlabel("Normalized displacement, x")
plt.ylabel("Normalized desired force, F")
plt.title("Haptic Effect Force vs. Displacement Curves")
plt.ylim(-1.1, 1.1)
plt.grid(True)
plt.legend()
plt.show()
