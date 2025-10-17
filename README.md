# Audio Frequency Meter for Arduino UNO (up to 20 kHz)

Monitor sound frequencies in real time (by detecting sound zero crossings) using inbuilt Arduino UNO Analog Comparator (pin 6/7).
To make it robust against noise, a small external circuit (four resistors) is used to create hysteresis in the detection threshold programatically. 

A moving average of the detected sound periods can computed to smooth the signal (default window size = 16 periods), but this can also be turned off (when a very quick response time is needed).

The default configuration detects frequencies between 16Hz - 20kHz within 0.1% accuracy (if the tone is pure). The frequency range and resolution can be configured (esp. useful if lower frequencies need to be detected, as for higher frequencies code execution times start to become a bottleneck).

# External components and wiring

<img width="1549" height="1106" alt="AudioFrequencyMeterCircuit" src="https://github.com/user-attachments/assets/c7c0a5ce-b432-4df7-a922-b8723455042f" />

- Pin 6 (AIN0): Audio signal, or other signal (centered at 2.5V)
- Pin 7 (AIN1): Reference voltage (crossing detection threshold)
- Pin 5 (Digital OUTPUT): Feedback line for creating hysteresis in threshold voltage. In the above configuration the hysteresis is about (10k/220k) * 0.5 * Vcc = 0.1 V

Here is an example of the signals at AIN0 (yellow) and AIN1 (pink) for a 247 Hz tone of about 2 Vpp. The pink signal shows the hysteresis in the threshold created by toggling output pin 5 HIGH/LOW when a downwards/upwards crossing  is detected.

![AIN_0_1_Scope](https://github.com/user-attachments/assets/aa041b9f-6608-4fb6-bded-50c4f0215e60)

# Examples frequency detection (without moving average)

Playing five strings of my guitar (5th string down to the 1st string). After each string I damped the sound, since the meter can only detect a single frequency at a time. Here I used a delay of 0.1 s between the printed samples. As a microphone I used my phone (with a "microphone amplifier" app) connected to the circuit via its headphone audio jack (and a small decoupling circuit, capacitor + resistors).

<img width="866" height="912" alt="guitar_notes" src="https://github.com/user-attachments/assets/8bcf33b1-a1a9-4481-a922-416e8bad3a0a" />

Sound from a cheap dog whistle. Gentle blowing gives a frequency of about 3200 Hz, blowing more firmly generates an overtone at about 7500 Hz.

<img width="732" height="735" alt="dogwhistle_two_tone" src="https://github.com/user-attachments/assets/0d3375d7-b1a4-408c-92d8-8e423982fa87" />

Playing pure notes (1kHz, 5kHz, 10kHz, 15kHz, 20kHz, 24kHz) with a [tone generator app](https://phyphox.org/) on my phone. The phone's headphone output was directly connected to the Arduino via a decoupling circuit.

<img width="859" height="809" alt="Screenshot 2025-10-12 at 09 53 46" src="https://github.com/user-attachments/assets/d2365e1d-8007-43de-b840-20b1ee57c30b" />

Using the tone generator app to play a 5ms "sonar" tjirp (frequency smoothly increasing from 0-7000Hz in a timespan of 5ms). To capture the fast signal in the Serial Plotter a high baud rate (1M) was used and the loop() was freely running (no delay()).

<img width="859" height="598" alt="Screenshot 2025-10-12 at 09 57 12" src="https://github.com/user-attachments/assets/616711b9-f203-4df9-8666-27410d4206e5" />
