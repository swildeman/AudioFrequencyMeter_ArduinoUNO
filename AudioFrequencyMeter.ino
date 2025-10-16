// AudioFrequencyMeter.ino
//
//  Uses inbuilt Arduino (UNO) Analog Comparator (AC, pin 6/7) to detect sound frequencies in real time (by detecting sound zero crossings).
//  An small external circuit (three resitors) is needed to create hysteresis in the detection threshold, making it robust against noise.
//  External circuit: 
//    - Provide a 2.5V reference signal on Pin 7 of Arduino UNO (AIN1-) using a 10k/10k voltage divider; 
//    - Connect Pin 5 to Pin 7 via a 100k resistor (the resulting threshold hysteresis will be roughly 0.5 * (10k/100k) * 2.5V = 0.1V)
//    - Connect Audio signal to Pin 6 (AIN0+); make sure the signal is centered on 2.5V (e.g. using decoupling circuit)
//  The frequency range and resolution can be configured (see below). The default configuration detects frequencies between 16 Hz - 24 kHz within 0.1% accuracy.
//  For smoothing the signal, a moving avarage of the detected sound period can be calculated by setting p > 0 for a window size of N = 2^p 
//  (by default an avg over 16 periods is taken, i.e. p = 4)
//  Disabling the moving average (p = 0) leads to a fast response to changes in freqency at the expense of a somewhat more noisy signal
//
// Created by Sander Wildeman 2025

// threshPin should be connected through resistor (typ. 100k) to the Analog Comparator reference voltage (typ. from a 10k/10k voltage divider) 
// to create hysteresis (for detection thresholding + noise robustness)
uint8_t threshPin = 5; 
uint8_t threshState = HIGH;

// The inbuilt led is toggled at detected sound wave crossings (for testing purposes)
// (usually to fast to see, unless circuit it tested with generated low frequencies)
uint8_t ledState = LOW;

// Timer1 is used to measure the time between sound wave threshold crossings (i.e. the sound period)
// This period is converted to the sound frequency reading (freq = 1/period)
//
// The timer prescaler determines the theoretical frequency resolution
// Frequency resolution df ~ timerPrescaler * frequency^2 / F_CPU  (depends on sound frequency)
// Example: prescaler 1
// Resolution @ 100 Hz:   df = 0.0006 Hz; df/f = 0.0006%
// Resolution @ 10000 Hz: df = 6 Hz; df/f = 0.06%
//
const uint16_t timerPrescaler = 1;  // Set to 1, 8, 64, 256, or 1024;  Timer tick frequency = F_CPU / timerPrescaler
uint32_t Noverflows = 0; // keep track of timer1 overflows for proper timekeeping and resetting
const uint8_t maxOverflows = 16; // maxOverflows determines the minimum frequency that can be detected
                                 // Fmin = F_CPU  / (timerPrescaler * 2^16 * maxOverflows)
                                 // After 1/Fmin seconds, detection is reset/stalled until a new sound is detected.
                                 // maxOverflows * N (window size circular buffer, see below) should < 2^16 to prevent overflow of uint32_t used for storing NperiodTicks;

// State variables to keep track of the sound wave's threshold crossings
bool lookForFirstCrossing = true;  // If true, no crossings have been detected (in a window of maxOverflows*timerPrescaler*2^16/F_CPU seconds)
                                   //    false, one ore more crossings have already been detected, so we can compute a period
volatile uint32_t periodTicks = 0; // Detected sound period (in units of counter ticks: Prescaler/F_CPU); 
                                   // 0 = no sound period established yet

// Circular buffer for computing a moving average
const uint8_t p = 4;  // window size N = 2^p, modify p as needed for application (note: a power of 2 is chosen so that the division operation can be optimized, allowing higher frequencies to be resolved)
const uint8_t N = 1 << p; // compute N = 2^p using a bitshift
uint32_t circbuf[N];
uint8_t bufCount = 0; // number of valid samples in the buffer
uint8_t bufHead = 0;  // index where next sample will be placed in buffer 
uint32_t NperiodsTicks;  // keep track of total time taken by N periods so that AvgPeriod = NperiodsTicks / N

// Interrupt routine of Analog Comparator where most of the magic (edge detection) is happening
ISR(ANALOG_COMP_vect) {
  static uint16_t t1, t2; // threshold crossing times for computing the period

  if (threshState == HIGH) {  // detecting a rising edge of sound wave passing the threshold

    uint16_t curTCNT = TCNT1;  // store current timer state once for further processing
    
    if (lookForFirstCrossing) { // this is the first crossing (after reset), no period can be computed yet
      lookForFirstCrossing = false;
      t1 = curTCNT;
    } 
    else { // fill/update the buffer and compute the average period
      t2 = curTCNT;
      if (bufCount < N) { // the buffer is filling up for the first time (after reset)
        circbuf[bufHead] = (Noverflows << 16) + t2 - t1; // period = Noverflows * 2^16 + t2 - t1;
        NperiodsTicks += circbuf[bufHead];
        bufCount++;
      } else { // the buffer is full, slide moving avarage forward by dropping the oldest sample in circular buffer
        NperiodsTicks -= circbuf[bufHead];
        circbuf[bufHead] = (Noverflows << 16) + t2 - t1;
        NperiodsTicks += circbuf[bufHead];
        periodTicks = NperiodsTicks / N; // compute average only when we have enough samples, if N is a power of 2 the compiler can optimize this step
      }
      t1 = t2;
      Noverflows = 0;
      bufHead = (bufHead + 1) % N; // if N is a power of two the compiler can optimize this modulo operation
    }
    
  }

  // Optional: toggle the builtin led at detected crossings (for testing purposes)
  // ledState = !ledState;
  // digitalWrite(LED_BUILTIN, ledState);

  // Toggle high/low threshold voltage, to create hysteresis in comparator
  threshState = !threshState;
  digitalWrite(threshPin, threshState);

  // Toggle between rising/falling edge detection 
  // (to create robustness against noise we make the sound first go through a trough (low point) 
  //  before looking out for the next upgoing edge (crest))
  ACSR ^= (1 << ACIS0);

  // Clear any pending interrupt to make sure the above settings will properly apply to the next detection
  ACSR |= (1 << ACI);
}

ISR(TIMER1_OVF_vect) {
  if(Noverflows < maxOverflows) {
    Noverflows++;
  }
  else { // Noverflows >= maxOverflows: reset state and look for a "first" crossing again
    periodTicks = 0;
    NperiodsTicks = 0;
    bufCount = 0;
    bufHead = 0;
    Noverflows = 0;
    lookForFirstCrossing = true;
  }
}

// Small helper function to get the Clock Select (CS) bits for a given timer prescaler (used in timer setup)
uint8_t cs_bits(uint16_t prescaler) {
  uint8_t cs = 1;
  while ((prescaler >>= 2) > 1) cs++;
  return cs;
}

void setup() {
  Serial.begin(1000000UL); // Use a high baud-rate to keep track of rapidly changing frequency in the serial plotter
  
  // Setup the output pins
  pinMode(threshPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(threshPin, threshState);
  digitalWrite(LED_BUILTIN, ledState);

  // Setup Timer1: https://wolles-elektronikkiste.de/en/interrupts-part-3-timer-interrupts
  TCCR1A = 0;                        // Normal timer use (we don't use it for PWM etc)
  TCCR1B = cs_bits(timerPrescaler);  // Set timer1 prescaler bits
  TIMSK1 = (1 << TOIE1);             // Enable overflow interrupt (detection is reset if timer overflows before the next sound wave is detected)

  // Setup Analog Comparator Interrupt
  DIDR1 |= (1 << AIN0D) | (1 << AIN1D);  // Disable Digital Inputs at AIN0 and AIN1
  ADCSRB &= ~(1 << ACME);                // Clear ACME bits in ADCSRB to use external input at AIN1 -ve input
  ACSR = (0 << ACD) |                    // Analog Comparator: Enabled
         (0 << ACBG) |                   // Clear ACBG to use external input to AIN0 +ve input
         (0 << ACO) | (1 << ACI) |       // Clear Pending Interrupt by setting the bit
         (1 << ACIE) |                   // Analog Comparator Interrupt Enabled
         (0 << ACIC) |                   // Analog Comparator Input Capture Disabled
         (1 << ACIS1) | (1 << ACIS0);    // Interrupt on RISING output
}

void loop() {

  // Temporarly halt the handling of interrupts to obtain the latest measured period without risk 
  // of the interrupt modifying the value while reading it (leading to undefined results)
  uint32_t curPeriodTicks;
  noInterrupts();
    curPeriodTicks = periodTicks;
    periodTicks = 0; // reset periodTicks to 0 to signal that "last reading has been consumed", 
                     // preventing reporting the same reading multiple times
  interrupts();

  // when a new period reading is obtained: convert it to Hz and print the value
  if (curPeriodTicks > 0) { // 
    float freqHz = (float)F_CPU / ((float)curPeriodTicks * (float)timerPrescaler);
    Serial.print("50 1500 "); // optional: print min/max frequency lines to fixate Serial Plotter y-axis
    Serial.println(freqHz, 2);
  }

  delay(50); // A delay can be added to slow down the reporting of frequencies to the Serial monitor/plotter, this won't affect the measurement
}
