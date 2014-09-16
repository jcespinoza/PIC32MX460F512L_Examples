How to use MicroB Bootloder:

1. Program your MMB with MicroB.hex.
2. Copie the boot-version of the demos on SD card.
3. Then insert an SD card in the slot.
4. Press the down button (and keeping it pressed down) press and release the reset.
5. The display should go black and MICROB should be written on top.
6. Then with the list of available (boot version) hex files should show below the title.
7. With the joystick you can scroll up/down the list and then select one of the demos.
8. The SD card will flash shortly and the new demo will be loaded and will be running.

 - Repeat from step 4. to load and try another demo!



How to create release for microB:

1. Put the project in Release mode.
2. Copy the special linker script (found in the Apps-Boot directory) into the main project directory
3. Do not include the linker script file in the project (just copy the file in the directory)
4. Do a new Build
5. Save the output .hex file to the SD card (root)