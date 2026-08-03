# SprinterOS
A lightweight Real-Time OS and Bootloader for Cortex-M7, completely bare-metal

<img src="./sprinter_banner.png"></img>
##### SprinterOS Project Wiki: https://stevenmu.dev/wiki <br>
##### Trello Board: https://trello.com/b/7di7a92U/sprinteros-development <br>

## What this project is
The goal is to bring up a CORTEX-M7 based STM32F767ZI chip, with no help from HAL or any external libraries, purely by reading reference manuals & datasheets. At the end:
- bootloader
- true real-time kernel
- user facing CLI that resembles linux

## Performance & Resource Usage
With all the features, we're stuck with only 2MB of FLASH, around half we need for user. Also, we only have 512KB of memory.
As a result, the OS is written with extreme care for the size and resource constraints

#### Bootloader
| Region | Used      | Size   | Usage |
| :----- | --------: | -----: | ----: |
| RAM    | 2,000 B   | 512 KB | 0.38% |
| FLASH  | 10,904 B  | 2 MB   | 0.52% |

## See it in Action


## Supported Hardware
- ARM CORTEX-M7 Based Hardware (STM32F767ZI used as dev chip)

## Contact
- For any inquiries, please reach out to xsmu0922@gmail.com
