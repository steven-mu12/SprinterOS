# SprinterOS
A lightweight Real-Time OS and Bootloader for Cortex-M7, completely bare-metal

<img src="./sprinter_banner.png"></img>
##### SprinterOS Project Wiki: https://stevenmu.dev/wiki <br>
##### Trello Board: https://trello.com/b/7di7a92U/sprinteros-development <br>

## What this project is
The goal is to bring up a CORTEX-M7 based STM32F767ZI chip, with no help from HAL or any external libraries, purely by reading reference manuals & datasheets. At the end:
- Fully customizable bootloader with multiple modes and flexible settings for all peripherals
- A full functioning real-time kernel executive with scheduling, memory management etc.
- User facing CLI that resembles linux, but used for real-time tasks
- An HDMI GUI interface for the CLI (HORIZON PROJECT - https://github.com/steven-mu12/Horizon)
- Power efficient but at the same time fast for real-time tasks

With all the features, we're stuck with only 2MB of FLASH, around half we need for user. Also, we only have 512KB of memory.
As a result, the OS is written with extreme care for the size and resource constraints

## Supported Hardware
- ARM CORTEX-M7 Based Hardware (STM32F767ZI used as dev chip)

## Contact
- For any inquiries, please reach out to xsmu0922@gmail.com
- If you would like to contribute, feel free to open an MR
- I'm also looking for potential work! Please hit me up if you have any opportunities open!
