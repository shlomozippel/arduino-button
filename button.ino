#include <Adafruit_MCP23X17.h>

#define BUTTON_MCP_SUPPORT
#include "button.h"

Adafruit_MCP23X17 mcp;

Button normalButton(D3);
McpButton ioexpanderButton(mcp, D3);

void setup()
{
    delay(500);
    Serial.begin(115200);
    Serial.println("Starting...");
    delay(500);
    ioexpanderButton.init();

}

void loop()
{
    ioexpanderButton.readButton(
        [](Button &b) { Serial.println("Button short press");},
        [](Button &b) { Serial.println("Button hold");}
    );

    ioexpanderButton.readSwitch(
        [](Button &b) { Serial.printf("Switch is now: %s\r\n", b.pressed ? "ON" : "OFF");}
    );
}