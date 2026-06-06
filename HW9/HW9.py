import matplotlib.pyplot as plt
import numpy as np
import csv

t = [] # column 0
data = [] # column 1

with open('sigA.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data.append(float(row[1])) # second column

for i in range(len(t)):
    # print the data to verify it was read
    print(str(t[i]) + ", " + str(data[i]))

total_time = t[-1] - t[0]
sample_rate = len(t) / total_time
print("Sample rate:", sample_rate, "Hz")

# plot the CSV data
plt.plot(t, data)
plt.xlabel('Time [s]')
plt.ylabel('Signal')
plt.title('sigA Signal vs Time')
plt.show()

