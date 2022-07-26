#pragma once

#include <stdint.h>

#define BUTTON_HOLD_TIME      1500
#define BUTTON_DEBOUNCE_TIME  50

#if defined(BUTTON_MCP_SUPPORT)
#include <Adafruit_MCP23X17.h>
#endif

class Button
{
  public:
    uint8_t pin;
    bool pressed;
    bool held;
    bool changed;
    bool switchstate;
    unsigned long change_time;
    unsigned long ignore_until;
  
    Button(uint8_t pin) : pin(pin) {
        this->pressed = false;
        this->held = false;
        this->changed = false;
        this->change_time = 0;
        this->switchstate = false;
    }

    void init() {
        pinInit();
    }

    void read() {
        changed = false;
        if (this->ignore_until > millis()) {
            return;
        }

        bool current_pressed = (pinRead() == LOW);
        this->changed = current_pressed != this->pressed;
        if (changed) {
            this->ignore_until = millis() + BUTTON_DEBOUNCE_TIME;
            this->pressed = current_pressed;
            this->change_time = millis();
        }
    }

    void readSwitch(void (*changed)(Button &b)=NULL)
    {
        read();
        if (this->switchstate != this->pressed && changed != NULL) {
            this->switchstate = this->pressed;
            changed(*this);
        }
    }

    void readButton(void (*short_press)(Button &b)=NULL, void (*hold)(Button &b)=NULL)
    {
        read();
        bool current_held = this->pressed && this->change_time + BUTTON_HOLD_TIME < millis();
        if (!this->held && current_held) {
            this->held = true;
            if (hold) {
                hold(*this);
            }
        }

        if (this->changed && !this->pressed) {
            if (!this->held && short_press) {
                short_press(*this);
            }
            this->held = false;
        }
    }

  protected:

    virtual void pinInit() {
        pinMode(this->pin, INPUT_PULLUP);
    }
    virtual int pinRead() {
        return digitalRead(this->pin);
    }
};

#if defined(BUTTON_MCP_SUPPORT)
class McpButton : public Button
{

    public:
        Adafruit_MCP23X17 &mcp;
        McpButton(Adafruit_MCP23X17 &mcp, uint8_t pin): Button(pin), mcp(mcp) {}

    protected:
        void pinInit() {
            mcp.pinMode(pin, INPUT_PULLUP);
        }

        int pinRead() {
            return mcp.digitalRead(BUTTON_PIN);
        }
};
#endif