# Real-time Audio Frequency Meter (Arduino UNO)

Uses inbuilt Arduino (UNO) Analog Comparator (pin 6/7) to detect sound frequencies in real time (by detecting sound zero crossings).
A small external circuit (three resitors) is needed to create hysteresis in the detection threshold, making it robust against noise.

The frequency range and resolution can be configured. The default configuration detects frequencies between 30Hz - 20000Hz within 1% accuracy (if the tone is pure).

# External components and wiring

<img width="1544" height="1090" alt="AudioFrequencyMeterCircuit" src="https://github.com/user-attachments/assets/12c6586d-8424-491e-bc3e-b76987850b00" />

- Pin 6 (AIN0): Audio signal, or other signal (centered at 2.5V)
- Pin 7 (AIN1): Reference voltage (crossing detection threshold)
- Pin 5 (Digital OUTPUT): Feedback line for creating hysteresis in threshold voltage

# Example signal

Playing five strings of my guitar (5th string down to the 1st string). After each string I damped the sound, since the meter can only detect a single frequency at a time. Here I used a delay of 0.1 s between the printed samples. As a microphone I used my phone (with a microphone amplifier app), connected to the circuit via its audio jack (and a small decoupling circuit, capacitor + resistors).

<img width="866" height="912" alt="guitar_notes" src="https://github.com/user-attachments/assets/8bcf33b1-a1a9-4481-a922-416e8bad3a0a" />
