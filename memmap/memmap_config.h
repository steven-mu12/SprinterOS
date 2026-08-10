/* pre-process addresses for the linker to consume during compilation */
#define FLASH_ORIGIN       0x08000000
#define FLASH_SIZE_B       (2048 * 1024)
#define DTCM_ORIGIN        0x20000000
#define DTCM_SIZE_B        (128 * 1024)
#define USERSPACE_ORIGIN   0x20020000
#define USERSPACE_SIZE_B   (368 * 1024)
#define KERNEL_IMG_ORIGIN  0x2007C000
#define KERNEL_IMG_SIZE_B  (16 * 1024)
