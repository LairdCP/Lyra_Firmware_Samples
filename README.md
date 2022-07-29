[![Laird Connectivity](/images/laird_connectivity_logo.jpg)](https://www.lairdconnect.com/)
# Lyra Firmware Samples
[![Lyra-P & Lyra-S](/images/lyra_p_and_lyra_s_render.jpg)](https://www.lairdconnect.com/wireless-modules/bluetooth-modules/bluetooth-5-modules/lyra-series-bluetooth-53-modules)
[![Silabs](/images/silabs_logo.jpg)](https://www.silabs.com)
[![Gecko SDK](/images/gecko_sdk_logo.jpg)](https://www.silabs.com/developers/gecko-software-development-kit)
[![Simplicity Studio](/images/simplicity_studio_logo.jpg)](https://www.silabs.com/developers/simplicity-studio)

This is the firmware samples repository for the Laird Connectivity [Lyra][Lyra product brief] product family.

The product is available in [P (PCB module)][Lyra P module datasheet] & [S (System In Package)][Lyra S module datasheet] variants.

The [Lyra P Explorer DVK][Lyra P Explorer DVK user guide] and [Lyra S Explorer DVK][Lyra S Explorer DVK user guide] were used during development of these sample applications and offer a convenient means of evaluating the Lyra performance. C code development for the Lyra module family is described further in the [Native C development guide][Native C development guide].

A bootloader is required for use with the sample applications. This is available in binary format from the [Lyra Firmware Github page][Lyra Firmware Page]. Further details of the usage of the bootloader are found in the [Firmware options and upgrade guide][Firmware options and upgrade guide]. 

# Content

The Silicon Labs Bluetooth stack allows for a wide variety applications to be built on its foundation. This repository showcases some example applications built using the Silicon Labs Bluetooth stack.

## Examples

- Lyra DVK BLE Environmental Sensing peripheral server example using I2C bus Si7021 Temperature and Humidity sensor 
- Lyra DVK BLE Automation Input/Output peripheral server example using the LED and push Button
- Lyra DVK BLE Environmental Sensing central client example using Si7021 Temperature and Humidity sensor
- Lyra DVK Bootloader example with FOTA support using UART interface 

## Documentation

Official documentation can be found at our [Developer Documentation](https://www.lairdconnect.com/wireless-modules/bluetooth-modules) page.

## Reporting Bugs/Issues and Posting Questions and Comments

To report bugs in the Application Examples projects, please contact [Customer Support team](https://www.lairdconnect.com/contact) 

## Disclaimer

Unless otherwise specified in the specific directory, all examples are considered to be EXPERIMENTAL QUALITY which implies that the code provided in the repos has not been formally tested and is provided as-is. It is not suitable for production environments. In addition, this code will not be maintained and there may be no bug maintenance planned for these resources. Laird Connectivity may update projects from time to time.

[Lyra product brief]: <https://www.lairdconnect.com/documentation/product-brief-lyra-series>
[Lyra P module datasheet]: <https://www.lairdconnect.com/documentation/datasheet-lyra-p>
[Lyra S module datasheet]: <https://www.lairdconnect.com/documentation/datasheet-lyra-s>
[Lyra P Explorer DVK user guide]: <https://www.lairdconnect.com/documentation/user-guide-lyra-p-development-kit>
[Lyra S Explorer DVK user guide]: <https://www.lairdconnect.com/documentation/user-guide-lyra-s-development-kit>
[Native C development guide]: <https://www.lairdconnect.com/documentation/user-guide-lyra-series-c-code-development>
[Lyra Firmware Page]: <https://github.com/LairdCP/Lyra_Firmware>
[Firmware options and upgrade guide]: <https://www.lairdconnect.com/documentation/user-guide-firmware-options-and-upgrading-lyra-series>