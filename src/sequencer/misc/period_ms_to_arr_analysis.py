#! /usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt

# Two options for term ordering:
# SYSCLK_FREQ_KHZ / prescaler * period_ms
# SYSCLK_FREQ_KHZ * period_ms / prescaler
# period_ms / prescaler * SYSCLK_FREQ_KHZ

# UINT32_MAX / SYSCLK_FREQ_KHZ * prescaler
# UINT32_MAX / (SYSCLK_FREQ_MHZ * 1000) * prescaler

# actually, just graph UINT32_MAX / SYSCLK_FREQ_KHZ * prescaler vs prescaler
# for fun we can compare with UINT32_MAX / (SYSCLK_FREQ_KHZ / prescaler)

SYSCLK_FREQ_MHZ = float(48)
SYSCLK_FREQ_KHZ = SYSCLK_FREQ_MHZ * 1000
UINT32_MAX = 0xFFFFFFFF
PSC_MAX = 0xFFFF
prescaler_max = PSC_MAX + 1
prescalers = np.arange(0, prescaler_max + 1, 1);
continuous = UINT32_MAX / SYSCLK_FREQ_KHZ * prescalers
trunc1 = int(UINT32_MAX / SYSCLK_FREQ_KHZ) * prescalers
trunc2 = np.rint(UINT32_MAX / np.rint(SYSCLK_FREQ_KHZ / prescalers))

plt.figure()
plt.plot(prescalers, continuous, label="no rounding")
plt.plot(prescalers, trunc1, label="UINT32_MAX / SYSCLK_FREQ_KHZ * prescaler")
plt.plot(prescalers, trunc2, label="UINT32_MAX / (SYSCLK_FREQ_KHZ / prescaler)")
plt.legend()
plt.title("max ARR vs. prescaler for different rounding strategies")
plt.xlabel("prescaler")
plt.ylabel("max ARR without overflow")
plt.show()
