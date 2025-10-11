# Real-time Audio Frequency Meter (Arduino UNO)

Uses inbuilt Arduino (UNO) Analog Comparator (pin 6/7) to detect sound frequencies in real time (by detecting sound zero crossings).
A small external circuit (three resitors) is needed to create hysteresis in the detection threshold, making it robust against noise.

The frequency range and resolution can be configured. The default configuration detects frequencies between 30Hz - 20000Hz within 1% accuracy.

# External components and wiring

<img width="1544" height="1090" alt="AudioFrequencyMeterCircuit" src="https://github.com/user-attachments/assets/12c6586d-8424-491e-bc3e-b76987850b00" />
