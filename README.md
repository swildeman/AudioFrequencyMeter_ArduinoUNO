# Real-time Audio Frequency Meter (Arduino UNO)

Uses inbuilt Arduino (UNO) Analog Comparator (pin 6/7) to detect sound frequencies in real time (by detecting sound zero crossings).
A small external circuit (three resistors) is needed to create hysteresis in the detection threshold, making it robust against noise. 

Two version are available: In one a moving average is computed of the detected period, in the other no additional averaging or filtering is applied (when a very quick response time is needed).

The frequency range and resolution can be configured. The default configuration detects frequencies between 30Hz - 20000Hz within 1% accuracy (if the tone is pure).

# External components and wiring

<img width="1544" height="1090" alt="AudioFrequencyMeterCircuit" src="https://github.com/user-attachments/assets/12c6586d-8424-491e-bc3e-b76987850b00" />

- Pin 6 (AIN0): Audio signal, or other signal (centered at 2.5V)
- Pin 7 (AIN1): Reference voltage (crossing detection threshold)
- Pin 5 (Digital OUTPUT): Feedback line for creating hysteresis in threshold voltage

# Example signals

Playing five strings of my guitar (5th string down to the 1st string). After each string I damped the sound, since the meter can only detect a single frequency at a time. Here I used a delay of 0.1 s between the printed samples. As a microphone I used my phone (with a "microphone amplifier" app) connected to the circuit via its headphone audio jack (and a small decoupling circuit, capacitor + resistors).

<img width="866" height="912" alt="guitar_notes" src="https://github.com/user-attachments/assets/8bcf33b1-a1a9-4481-a922-416e8bad3a0a" />

Playing pure notes (1kHz, 5kHz, 10kHz, 15kHz, 20kHz, 24kHz) with a [tone generator app](https://phyphox.org/) on my phone. The phone's headphone output was directly connected to the Arduino via a decoupling circuit.

<img width="859" height="809" alt="Screenshot 2025-10-12 at 09 53 46" src="https://github.com/user-attachments/assets/d2365e1d-8007-43de-b840-20b1ee57c30b" />

Sound from a cheap dog whitle. Gentle blowing gives a frequency of about 3200 Hz, blowing more firmly generates an overtone at about 7500 Hz.

<img width="732" height="735" alt="dogwhistle_two_tone" src="https://github.com/user-attachments/assets/0d3375d7-b1a4-408c-92d8-8e423982fa87" />

Using the tone generator app to play a 5ms "sonar" tjirp (frequency smoothly increasing from 0-7000Hz in a timespan of 5ms). To capture the fast signal in the Serial Plotter a high baud rate (1M) was used and the loop() was freely running (no delay()).

<img width="859" height="598" alt="Screenshot 2025-10-12 at 09 57 12" src="https://github.com/user-attachments/assets/616711b9-f203-4df9-8666-27410d4206e5" />
