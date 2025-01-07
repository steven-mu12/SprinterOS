# SprinterOS
A lightweight Real-Time OS and Bootloader for Cortex-M7, completely bare-metal

## Task Board and Documentation
Trello Board: https://trello.com/b/7di7a92U/sprinteros-development<br>
SprinterOS Documentation: https://evergreen-ice-067.notion.site/162599e5ae7c801db960f843eb2ad86b?v=dcf41fc7e83b486eb72e42683e113ad0

Current work is on the bootloader. Check out both links above to see what's up!

## What this project is
The goal is to bring up a CORTEX-M7 based STM32F767ZI chip, with no help from HAL or any external libraries, purely by reading reference manuals & datasheets. At the end:
- Fully customizable bootloader with multiple modes and flexible settings for all peripherals
- A simple kernel with task scheduling, time management, basic memory management etc.
- User facing CLI that resembles linux, but used for real-time tasks
- Power efficient but at the same time fast for real-time tasks

With all the features, we're stuck with only 2MB of FLASH, around half we need for user. Also, we only have 512KB of memory.
As a result, the OS is written with extreme care for the size and resource constraints

## Supported Hardware
- ARM CORTEX-M7 Based Hardware (STM32F767ZI used as dev chip)

## Contact
For any inquiries, please reach out to xsmu0922@gmail.com
I'm also looking for potential work! Please hit me up if you have any opportunities open - would love to join a team of passionate engineers and great people. I would be super grateful for the opportunity.
