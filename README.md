# Project-Pacman-better
Pacman but with better embedded engineers.

# Project Description
Our project aims to use the STM to construct timer and game logic to create an interactive Pacman replica with input via a joystick and keypad, and display show on an LED matrix.

# Objectives
- **Aahan**: An ability to output audio synchronized to game events using a DAC.
- **Alex**: An ability to read from and store high score data into our kit’s serial EEPROM chip (Microchip AA2432AF) across game instances using the I2C protocol.
- **Omkar**: An ability to use ADC peripherals on the STM microcontroller to convert the analog position of a 2-axis joystick into a digital value that can be used by the microcontroller to interpret the joystick position.
- **Zoey**: An ability to utilize STM timers and GPIO Peripherals to control changing game logic, scan for inputs from peripherals, and update the LED matrix display.


               
# Main Features: 
- **Peripherals**: GPIO, Timers, DAC
- **Movement**: Joystick
- **Game control**: Keypad or Pushbuttons
- **Display**: LED Matrix Display
- **Game control**: Timers


# Additional Parts Purchased:
- **Joystick:** https://nam04.safelinks.protection.outlook.com/?url=https%3A%2F%2Fwww.adafruit.com%2Fproduct%2F512&data=05%7C02%7Cmehta295%40purdue.edu%7Ca02189e7d93e4299a28c08dd55d2c002%7C4130bd397c53419cb1e58758d6d63f21%7C0%7C0%7C638761085843781052%7CUnknown%7CTWFpbGZsb3d8eyJFbXB0eU1hcGkiOnRydWUsIlYiOiIwLjAuMDAwMCIsIlAiOiJXaW4zMiIsIkFOIjoiTWFpbCIsIldUIjoyfQ%3D%3D%7C0%7C%7C%7C&sdata=BsiAAMqPYT3qgTyyWpZ5JxV0d3g3xbroM4o6UWwSwKA%3D&reserved=0
- **LED Matrix Display:** https://nam04.safelinks.protection.outlook.com/?url=https%3A%2F%2Fwww.adafruit.com%2Fproduct%2F2278&data=05%7C02%7Cmehta295%40purdue.edu%7Ca02189e7d93e4299a28c08dd55d2c002%7C4130bd397c53419cb1e58758d6d63f21%7C0%7C0%7C638761085843797849%7CUnknown%7CTWFpbGZsb3d8eyJFbXB0eU1hcGkiOnRydWUsIlYiOiIwLjAuMDAwMCIsIlAiOiJXaW4zMiIsIkFOIjoiTWFpbCIsIldUIjoyfQ%3D%3D%7C0%7C%7C%7C&sdata=29kGtGGOcy9DcFBA68U0BwolFBkDP9z3qVx1mVtx0SQ%3D&reserved=0

               
# External Interfaces
[TBD]
               
# Internal Peripherals
[TBD]

               
# Timeline 
## Due date 1 : 3/2/2025
- Research and order all the specific parts needed for the project.
- Build a basic, elementary plan on all the subsystems of the project and how they will work.
- Start building a flow-chart of how these subsystems will work.

## Due date 2 : 3/10/2025
- Finish up the flowchart and turn it into a pseudo-code.
- Use the STM32 resources to understand what specific parts of the microcontroller we will use.
- Initial breadboard testing.

## Due date 3 : 3/18/2025
- Finalize first draft for timer, LED display, and game logic
- Finish and test wiring to ensure all subsystems work separately and together.

## Due date 4 : 3/26/2025
- Start to work on the sub-system code simultaneously.
- Begin to implement code logic with hardware–testing frequently to ease with debugging.

## Final Due date : 4/29/2025
- Follow the engineering method to fix bugs and improve every subsystem.
- If the project timeline has not been delayed, and if significant time remains, then start to design a PCB that achieves the same hardware abilities that are present on our breadboard.


               
# Related Projects 
### 1)
- https://nam04.safelinks.protection.outlook.com/?url=https%3A%2F%2Fyoutu.be%2F6ignbWBmfMw%3Fsi%3D6gDJAv2SGLOrzNAZ&data=05%7C02%7Cmehta295%40purdue.edu%7Ca02189e7d93e4299a28c08dd55d2c002%7C4130bd397c53419cb1e58758d6d63f21%7C0%7C0%7C638761085843805914%7CUnknown%7CTWFpbGZsb3d8eyJFbXB0eU1hcGkiOnRydWUsIlYiOiIwLjAuMDAwMCIsIlAiOiJXaW4zMiIsIkFOIjoiTWFpbCIsIldUIjoyfQ%3D%3D%7C0%7C%7C%7C&sdata=nIVh%2BXzIgfqoAA7mZKp3fIYqJuDzHvJHBx4IVUecwKM%3D&reserved=0
- This project is using an SD Card to import the game logic. We could do something like this. We also really like the PCB in this game, as it replicates a video game control very well.
### 2)
- https://nam04.safelinks.protection.outlook.com/?url=https%3A%2F%2Fyoutu.be%2Fy5O8JRKy1tI%3Fsi%3Dvzc-N45uXxTNpEd1&data=05%7C02%7Cmehta295%40purdue.edu%7Ca02189e7d93e4299a28c08dd55d2c002%7C4130bd397c53419cb1e58758d6d63f21%7C0%7C0%7C638761085843814007%7CUnknown%7CTWFpbGZsb3d8eyJFbXB0eU1hcGkiOnRydWUsIlYiOiIwLjAuMDAwMCIsIlAiOiJXaW4zMiIsIkFOIjoiTWFpbCIsIldUIjoyfQ%3D%3D%7C0%7C%7C%7C&sdata=y6YyciqFMqNrXo0pWfBfyZ3wD1ukze1GAzeSOQeLP%2F8%3D&reserved=0
- This is a great walkthrough of a project that closely resembles what we want to do with our game.

### What makes our project different
A slight difference that we have in our project compared to these projects is that we would like to implement a system that stores the data for each user, such as that particular user’s name, their high score, the time they spent on the game on that day, etc. This would add an extra layer of complexity that the projects above do not have.
