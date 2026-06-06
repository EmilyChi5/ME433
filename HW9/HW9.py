import matplotlib.pyplot as plt
import numpy as np
import csv

def read_csv(filename):
    t = [] # column 0
    data = [] # column 1

    with open(filename) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            data.append(float(row[1])) # second column

    return np.array(t), np.array(data)

def get_fft(t, data):
    # Find Sample Rates
    total_time = t[-1] - t[0]
    Fs = len(t) / total_time
    print("Sample rate:", Fs, "Hz")

    n = len(data) # Length of the signal
    k = np.arange(n)
    T = n/Fs 

    frq = k/T
    frq = frq[range(int(n/2))]

    Y = np.fft.fft(data) / n
    Y = Y[range(int(n/2))]

    return frq, abs(Y), Fs

filename = 'sigD.csv'
t, data = read_csv(filename)

frq, Y, Fs = get_fft(t, data)

fig,(ax1, ax2) = plt.subplots(2,1)

ax1.plot(t, data, 'k')
ax1.set_xlabel('Time [s]')
ax1.set_ylabel('Signal')
ax1.set_title(filename + ' Signal vs Time')

ax2.loglog(frq, Y, 'k')
ax2.set_xlabel('Frequency [Hz]')
ax2.set_ylabel('|Y(freq)|')
ax2.set_title(filename + ' FFT')

plt.tight_layout()
plt.show()

print("Sample rate:", Fs, "Hz")
