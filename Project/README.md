This project implements the game Tic-Tac-Toe using a Nokia 5110 cell phone LCD display. The players are able to select their token placement using an attached keypad. The LCD display will update to reflect the placed tokens if the placement was valid. If the game is won by a player, a message will be displayed, and a new game will begin. The players may also choose to restart the game by pressing the ‘New Game’ button on the keypad. A potentiometer (to dim the display), voltage regulator, and logic level shifter have been included in the design to interface with the LCD.

##Motivation

This project was inspired by the games included on the Nokia 5110 and 3310 cell phones. We designed and implemented a simple Tic-Tac-Toe game which interfaces with a keypad reminiscent of those on the Nokia 5110/3310. This type of game is ideal for the simple keypad interface and small display size.

##Implementation

To implement our project, we chose to use a Nokia 5110/3310 LCD unit as the user display, and a 3 x 4 matrix keypad as the interface for user input. Interfacing these components with the ATmega88PA and development board required that we also incorporate a 3.3 V Voltage regulator, and High-to-Low logic level shifter. This hardware will be described in further detail in the sections below. 
