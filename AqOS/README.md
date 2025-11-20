## Requirements :
 - Seeedstudio Lora E5 module with accesible SWD pins
 - STLink debug probe
 - STM32CubeProgrammer
 - Visual Studio code with the Platform.io extension installed

## How to program the module : 
1. Connect the **STLink** to the board.
2. Open **STM32CubeProgrammer** and, while pressing on the reset button, click "connect".
3. In the "OB" tab, go to "Read-out protection" and select "AA".
4. In **VSCode**, open the AqOS project from the Platform.io extension and wait for it to configure the project.
