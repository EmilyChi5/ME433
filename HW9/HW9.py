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

def moving_average(data, X):
    filtered = []

    for i in range(len(data)):
        if i < X:
            avg = np.mean(data[0:i+1])
        else:
            avg = np.mean(data[i-X+1:i+1])

        filtered.append(avg)

    return np.array(filtered)

def iir_filter(data, A, B):
    filtered = [data[0]]

    for i in range(1, len(data)):
        new_value = A * filtered[i-1] + B * data[i]
        filtered.append(new_value)

    return np.array(filtered)

def fir_filter(data, weights):
    filtered = []

    for i in range(len(data)):
        total = 0

        for j in range(len(weights)):
            if i - j >= 0:
                total += weights[j] * data[i-j]

        filtered.append(total)

    return np.array(filtered)

# -- Weighted Step 7
filename = 'sigD.csv'

# Weights
weights = [
    0.000096734265968441,
    0.000079893284958558,
    0.000061665650631385,
    0.000042179215242215,
    0.000021572900859564,
    0.000000000000000000,
    -0.000022368415985591,
    -0.000045340171044719,
    -0.000068698938714455,
    -0.000092201675432196,
    -0.000115576726591127,
    -0.000138522787525481,
    -0.000160708883789383,
    -0.000181775511282445,
    -0.000201337047432889,
    -0.000218985510390935,
    -0.000234295704773932,
    -0.000246831750834351,
    -0.000256154949999036,
    -0.000261832894650648,
    -0.000263449684954200,
    -0.000260617071676855,
    -0.000252986302523476,
    -0.000240260411712007,
    -0.000222206659492970,
    -0.000198668801151712,
    -0.000169578844691995,
    -0.000134967943726175,
    -0.000094976067777329,
    -0.000049860096743331,
    0.000000000000000000,
    0.000054097216359286,
    0.000111796078442149,
    0.000172333972860101,
    0.000234825428260873,
    0.000298269487006124,
    0.000361560240403592,
    0.000423500531979766,
    0.000482818760273357,
    0.000538188637021932,
    0.000588251680004495,
    0.000631642143880105,
    0.000667014018869279,
    0.000693069657820834,
    0.000708589528831230,
    0.000712462534816516,
    0.000703716294860955,
    0.000681546746228202,
    0.000645346401896315,
    0.000594730587437936,
    0.000529560983848557,
    0.000449965820105035,
    0.000356356091108280,
    0.000249437223225559,
    0.000130215670588441,
    -0.000000000000000001,
    -0.000139603890172795,
    -0.000286703672631031,
    -0.000439137761621858,
    -0.000594498780105894,
    -0.000750163228708207,
    -0.000903327133750850,
    -0.001051047322210335,
    -0.001190287842842693,
    -0.001317970927559997,
    -0.001431031768041246,
    -0.001526476272108699,
    -0.001601440865114016,
    -0.001653253315843600,
    -0.001679493496477621,
    -0.001678052933901441,
    -0.001647191976876998,
    -0.001585593391624306,
    -0.001492411208276259,
    -0.001367313673106573,
    -0.001210519216636156,
    -0.001022824425518419,
    -0.000805623105875338,
    -0.000560915646455428,
    -0.000291308030135310,
    0.000000000000000001,
    0.000309237940764728,
    0.000632098830285042,
    0.000963784790485729,
    0.001299063548595230,
    0.001632335470754199,
    0.001957710436521395,
    0.002269093683046843,
    0.002560279555659482,
    0.002825051921291723,
    0.003057289836130386,
    0.003251076912562412,
    0.003400812706056441,
    0.003501324342964838,
    0.003547976537860242,
    0.003536778106032742,
    0.003464483064800013,
    0.003328684437448439,
    0.003127898926515531,
    0.002861640708769005,
    0.002530482722087066,
    0.002136103963371766,
    0.001681321494935990,
    0.001170106062245550,
    0.000607580455696211,
    -0.000000000000000001,
    -0.000645285176955758,
    -0.001319886162314784,
    -0.002014449339559283,
    -0.002718748014202430,
    -0.003421790770976360,
    -0.004111945460951061,
    -0.004777077438770793,
    -0.005404700407224419,
    -0.005982137983363577,
    -0.006496693881791758,
    -0.006935828420685136,
    -0.007287338898286639,
    -0.007539541265278535,
    -0.007681450434290233,
    -0.007702956523949682,
    -0.007594994332814835,
    -0.007349703379031494,
    -0.006960575924765649,
    -0.006422590529755617,
    -0.005732328844419712,
    -0.004888073557841697,
    -0.003889885656973535,
    -0.002739659427243723,
    -0.001441153927556425,
    0.000000000000000001,
    0.001576317778415439,
    0.003278504433256123,
    0.005095523080383360,
    0.007014688051493389,
    0.009021781774464879,
    0.011101194163548354,
    0.013236082926953815,
    0.015408552872023270,
    0.017599851986273975,
    0.019790581801163758,
    0.021960919309007305,
    0.024090847506079878,
    0.026160391480000510,
    0.028149856849749376,
    0.030040067304191710,
    0.031812597971055824,
    0.033450001383472670,
    0.034936022895171816,
    0.036255802527200516,
    0.037396060406773970,
    0.038345263180017831,
    0.039093769041662223,
    0.039633949322274785,
    0.039960284902866974,
    0.040069436082647784,
    0.039960284902866974,
    0.039633949322274785,
    0.039093769041662223,
    0.038345263180017831,
    0.037396060406773970,


]

filter_type = "Low-pass sinc"
cutoff = 8
bandwidth = 4
window = "Hamming"

t, data = read_csv(filename)
filtered = fir_filter(data, weights)

frq, Y, Fs = get_fft(t, data)
frq_filt, Y_filt, Fs = get_fft(t, filtered)

fig, (ax1, ax2) = plt.subplots(2, 1)

ax1.plot(t, data, 'k', label='Unfiltered')
ax1.plot(t, filtered, 'r', label='Filtered')
ax1.set_xlabel('Time [s]')
ax1.set_ylabel('Signal')
ax1.set_title(
    filename + 
    ' FIR: ' + filter_type +
    ', ' + str(len(weights)) + ' weights, cutoff=' + str(cutoff) +
    ' Hz, BW=' + str(bandwidth) + ' Hz, ' + window
)
ax1.legend()

ax2.loglog(frq, Y, 'k', label='Unfiltered FFT')
ax2.loglog(frq_filt, Y_filt, 'r', label='Filtered FFT')
ax2.set_xlabel('Frequency [Hz]')
ax2.set_ylabel('|Y(freq)|')
ax2.legend()

plt.tight_layout()
plt.show()

# --- For the MAF part (Step 5)
# filename = 'sigD.csv'
# X = 50

# t, data = read_csv(filename)
# filtered = moving_average(data, X)

# frq, Y, Fs = get_fft(t, data)
# frq_filt, Y_filt, Fs = get_fft(t, filtered)

# fig, (ax1, ax2) = plt.subplots(2, 1)

# ax1.plot(t, data, 'k', label='Unfiltered')
# ax1.plot(t, filtered, 'r', label='Filtered')
# ax1.set_xlabel('Time [s]')
# ax1.set_ylabel('Signal')
# ax1.set_title(filename + ' Moving Average Filter, X = ' + str(X))
# ax1.legend()

# ax2.loglog(frq, Y, 'k', label='Unfiltered FFT')
# ax2.loglog(frq_filt, Y_filt, 'r', label='Filtered FFT')
# ax2.set_xlabel('Frequency [Hz]')
# ax2.set_ylabel('|Y(freq)|')
# ax2.legend()

# plt.tight_layout()
# plt.show()
# print("Sample rate:", Fs, "Hz")

# --- For the IIR Part (Step 6)
# filename = 'sigA.csv'

# A = .9
# B = .1

# t, data = read_csv(filename)
# filtered = iir_filter(data, A, B)

# frq, Y, Fs = get_fft(t, data)
# frq_filt, Y_filt, Fs = get_fft(t, filtered)

# fig, (ax1, ax2) = plt.subplots(2, 1)

# ax1.plot(t, data, 'k', label='Unfiltered')
# ax1.plot(t, filtered, 'r', label='Filtered')
# ax1.set_xlabel('Time [s]')
# ax1.set_ylabel('Signal')
# ax1.set_title(filename + ' IIR Filter, A = ' + str(A) + ', B = ' + str(B))
# ax1.legend()

# ax2.loglog(frq, Y, 'k', label='Unfiltered FFT')
# ax2.loglog(frq_filt, Y_filt, 'r', label='Filtered FFT')
# ax2.set_xlabel('Frequency [Hz]')
# ax2.set_ylabel('|Y(freq)|')
# ax2.legend()

# plt.tight_layout()
# plt.show()