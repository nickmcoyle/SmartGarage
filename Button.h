#pragma once;

// use a state machine to move between the four button states
// when button pressed moves from NOBUTTON_PRESSED to BUTTON_DEBOUNCE
// after debounce time moves to BUTTON_PRESSED
// if held for certain time moves to BUTTON_HOLD
class Button {
  private:
    const int DEBOUNCE = 200;      // milliseconds for debounce counter
    const int HOLD_COUNT = 20000;   // milliseconds for hold counter
    int sw1State = NOBUTTON_PRESSED;
    int SwPin = 0;
    int oldSwState = -1;
    int oldReadSw = 0, mSecCounter = 0;

    // Define Button States
  public:
    typedef enum SW_STATES {
      NOBUTTON_PRESSED,
      BUTTON_DEBOUNCE,
      BUTTON_PRESSED,
      BUTTON_HOLD
    };

    Button(int sw) {
      SwPin = sw;  //constructor
      pinMode(sw, INPUT_PULLUP);
    }

    // switch call back function - state machine for button
    void buttonCallBack(void)
    {
      int readSw = digitalRead(SwPin);   // read the switch button
      if (readSw == 1)                    // if button not pressed
        sw1State = NOBUTTON_PRESSED;  // current state not pressed
      else
        switch (sw1State)                 // switch on current state
        {
          case NOBUTTON_PRESSED:
            sw1State = BUTTON_DEBOUNCE;         // in debounce
            oldReadSw = readSw;
            mSecCounter = 0;
            break;
          case BUTTON_DEBOUNCE:
            if (mSecCounter++ > DEBOUNCE)
              sw1State = BUTTON_PRESSED;    // Button still Pressed
            break;
          case BUTTON_PRESSED:
            if (mSecCounter++ > HOLD_COUNT)
              sw1State = BUTTON_HOLD;       // Button in hold
            break;
          case BUTTON_HOLD:
            break;
        }
    }

    // if switch has changed state return its state otherwise return -1
    int getState(void)
    {
      this->buttonCallBack();
      char text[20];
      String s(itoa( SwPin, text, 10));
      if (oldSwState != sw1State)
      {
        oldSwState = sw1State;
        return oldSwState;
      }
      return -1;
    }

    // debugging info
    void debug(void)
    {
      this->buttonCallBack();
      char text[20];
      String s(itoa( SwPin, text, 10));
      if (oldSwState != sw1State)
      {
        oldSwState = sw1State;
        switch (oldSwState)
        {
          case NOBUTTON_PRESSED: Serial.println("BUTTON_" + s + "_NOT_PRESSED"); break;
          case BUTTON_DEBOUNCE: Serial.println("BUTTON_" + s + "_DEBOUNCE " ); break;
          case BUTTON_PRESSED: Serial.println("BUTTON_" + s + "_PRESSED"); break;
          case BUTTON_HOLD: Serial.println("BUTTON_" + s + "_HOLD"); break;
        }
      }
    }
};
