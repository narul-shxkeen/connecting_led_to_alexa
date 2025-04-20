# Connecting LED with Alexa

## Objective

To be able to switch an LED on and off using alexa and an ESP32 microcontroller.

## Materials Used

1. Breadboard
2. Alexa
3. ESP32 Microcontroller
   -    Model: Seeed Studio XIAO ESP32C3
4. A White LED
5. USB to type C data cable
   - To flash data to the microcontroller


## Process


- Installed Arduino IDE V2

- Acquired a white LED and attached it on the breadboard.

- Attached the ESP32 microcontroller to the breadboard as well (In order to avoid the pins from accidentally touching a metal surface and causing a short circuit.)

- Installed ESP32 board Manager(`esp32` by Espressif Systems) on the IDE to connect to the board.

- IDE detected the board after installing the correct board manager.

>   [!Note]
>   The pins on the provided board are not marked. I have to find out how to find the appropriate labels. Here's the board's guide to figure that out(This is also useful for selecting and installing the libraries for this specific ESP32): https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/#run-your-first-blink-program

- Installed FauxmoESP library to for integrating the ESP32 with alexa.

- Found the pins to be used. Luckily the team has soldered male heads to 5v, GND, 3v, and D10 pin already so soldering is not required.

- The IDE was not able to access the USB port(a linux os issue.) Running the following command helped: `sudo chown username:username /dev/ttyACM0 `
    >[!Warning]
    >On Fedora, the IDE was not able to access the USB ports for uploading the programmes. Running the following command and rebooting afterwards fixed the issue: `sudo usermod -aG dialout username`

- The blinking programme worked successfully!
  
- Uploaded the programme from the provided Github Repo by the team.

- Successfully uploaded the programme to the board. On testing, Alexa was not able to detect the device for some reason.

- Worked successfully! Programme saved in  `Alexa_Led_1`. The commands are "Alexa, turn on the Genius" or "Alexa, turn off the Genius". It can only switch the LED on or off, there is no brightness control or blinking yet.

- Demonstration till this stage is saved in `./Test_Videos/Prototype_1.mp4`

### More Experimentation

Trying to make the LED blink on command as well. However, even though the commands are getting accepted by Alexa the LED is only taking 2 states: On or Off. There is no change in brightness or blinking.

- At this stage I am using ChatGPT and other bots to modify this code so that we can incorporate blinking and dimming in the commands. However there seems to be some problem with the PWM functions. Errors with `ledcAttach`, `ledcSetup`, etc.

- The brightness control and blinking feature has now been implemented successfully. The code is in `Alex_Led_2`.

>   [!NOTE]
>   Key Changes: Added the blinking function as part of a new device named `The Genius Blink`.
>   Switched to `analogWrite` from `digitalWrite`.

- Demonstration of the final work present in `./Test_Videos/Prototype_Final.mp4`