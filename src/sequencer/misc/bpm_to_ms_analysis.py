#! /usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

bpms = np.arange(30, 300, 0.1)
milliseconds = 1000.0 * 60 / bpms
milliseconds_int = np.rint(milliseconds)

error_ms = milliseconds_int - milliseconds
max_ms_error = np.max(np.abs(error_ms))

bpms_actual = 1000.0 * 60 / milliseconds_int
error_bpm = bpms_actual - bpms
max_bpm_error = np.max(np.abs(error_bpm))
bpm_of_max_bpm_error = bpms[np.argmax(np.abs(error_bpm))]

print(f"max BPM error: {max_bpm_error} bpm at {bpm_of_max_bpm_error} bpm")

plt.figure()
plt.plot(bpms, milliseconds, label="continuous")
plt.plot(bpms, milliseconds_int, label="integer milliseconds")
plt.xlabel("BPM")
plt.ylabel("period (ms)")
plt.title("period (ms) vs BPM")
plt.legend()

plt.figure()
plt.plot(bpms, bpms_actual)
plt.xlabel("BPM")
plt.ylabel("period error (ms)")
plt.title("effective BPM vs desired BPM")

plt.figure()
plt.plot(bpms, error_bpm)
plt.xlabel("BPM")
plt.ylabel("BPM error")
plt.title("BPM error vs BPM")

plt.show()
