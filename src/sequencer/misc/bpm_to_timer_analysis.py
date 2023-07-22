#! /usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt

SYSCLK_FREQ_HZ = 48_000_000
MAX_BPM_TENTHS = 3000
MIN_BPM_TENTHS = 300

if __name__ == "__main__":
    tenths_of_bpm = np.arange(MIN_BPM_TENTHS, MAX_BPM_TENTHS + 1)

    # this is a translation of the breakdown from C
    # see tempo_to_period_and_prescaler()
    prescalers = np.full_like(tenths_of_bpm, 2400)
    prescalers[tenths_of_bpm > 500] = 1200
    prescalers[tenths_of_bpm > 1000] = 600
    prescalers[tenths_of_bpm > 2000] = 300

    prescaled_clocks = SYSCLK_FREQ_HZ // prescalers
    periods = (prescaled_clocks * 60 * 10) // tenths_of_bpm

    actual_tenths_of_bpm = prescaled_clocks * 60.0 * 10 / periods
    tempo_error = actual_tenths_of_bpm - tenths_of_bpm
    tempo_error_relative = tempo_error / tenths_of_bpm * 100

    fig, ax1 = plt.subplots()
    ax1_col = "tab:red"
    ax1.set_xlabel("tempo (tenths of BPM)")
    ax1.set_ylabel("register value", color=ax1_col)
    ax1.set_title("Timer Settings")
    ax1.tick_params(axis="y", labelcolor=ax1_col)
    ax1.plot(tenths_of_bpm, prescalers, label="timer prescaler (PSC)", color=ax1_col)
    ax1.plot(tenths_of_bpm, periods, label="timer period (ARR)", color=ax1_col)

    ax2 = ax1.twinx()
    ax2_col = "tab:blue"
    ax2.set_ylabel("frequency (Hz)", color=ax2_col)
    ax2.tick_params(axis="y", labelcolor=ax2_col)
    ax2.plot(tenths_of_bpm, prescaled_clocks, label="prescaled clock frequency", color=ax2_col)

    h1, l1 = ax1.get_legend_handles_labels()
    h2, l2 = ax2.get_legend_handles_labels()
    ax1.legend(h1 + h2, l1 + l2)
    fig.tight_layout()

    fig, ax1 = plt.subplots()

    ax1.set_xlabel("tempo (tenths of BPM)")
    ax1.set_title("Tempo Error")
    ax1.set_ylabel("absolute tempo error (tenths of BPM)", color=ax1_col)
    ax1.tick_params(axis="y", labelcolor=ax1_col)
    ax1.plot(tenths_of_bpm, tenths_of_bpm - actual_tenths_of_bpm, color=ax1_col)

    ax2 = ax1.twinx()
    ax2.set_ylabel("relative tempo error (percent of desired tempo)", color=ax2_col)
    ax2.tick_params(axis="y", labelcolor=ax2_col)
    ax2.plot(tenths_of_bpm, tempo_error_relative, color=ax2_col)
    fig.tight_layout()

    plt.show()

