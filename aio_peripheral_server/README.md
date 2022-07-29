[![Laird Connectivity](../images/laird_connectivity_logo.jpg)](https://www.lairdconnect.com/)
# Bluetooth low energy Automation Input/Output peripheral server sample using LED and Button
[![Lyra-P & Lyra-S](../images/lyra_p_and_lyra_s_render.jpg)](https://www.lairdconnect.com/wireless-modules/bluetooth-modules/bluetooth-5-modules/lyra-series-bluetooth-53-modules)
[![Silabs](../images/silabs_logo.jpg)](https://www.silabs.com)
[![Gecko SDK](../images/gecko_sdk_logo.jpg)](https://www.silabs.com/developers/gecko-software-development-kit)
[![Simplicity Studio](../images/simplicity_studio_logo.jpg)](https://www.silabs.com/developers/simplicity-studio)

## Introduction 

This example implements an automation input/output service with two instances of digital characteristic - input for button and output for LED, where the  input and output digital data is accessible by a GATT client manually. The peripheral server can be configured to send notification to the central client every time the button's state is changed.  In addition, output digital data of the LED can be written via Bluetooth low energy central client device to control the state of the LED.

## Gecko SDK version

v4.0.0

## Hardware requirement

- [A Lyra Development Kit board](https://www.lairdconnect.com/wireless-modules/bluetooth-modules)

## Connections requirement

The Lyra DVK can be connected to the PC USB slot using a microUSB cable. The USB connection is used as a 5V power source for the  hardware setup, as well as it support COM port serial interface. The exported COM port number on Windows can be found using [**Device Manager**] under [**Ports (COM & LPT)**] with device name [**JLink CDC UART Port (COMYY)**]. Once the COM port was identified you can use a terminal emulator program of your choice to establish serial connection with the device.         

<img src="images/Image_LyraP.png" alt="Laird Connectivity" style="zoom:150%;" />

## Software requirement

This application is implemented using Simplicity Studio v5. The SOC-Empty project template is used as a starting point and additional functions to support the LED and button, GATT automation IO service are added to the provided template. 

1. Select the Lyra DVK Board from the "Debug Adapters" on the left, establish connection with the IDE by pushing **Start** button.

   <img src="images/ImageDebugAdapterConnect.png" alt="Laird Connectivity" style="zoom:150%;" />

2. Configure on-board J-Link debugger, if this is a new workspace, by pressing **Configure**, otherwise skip this step and continue with step **#11**:   

   <img src="images/ImageConfigureJLinkAdapter.png" alt="Laird Connectivity" style="zoom:150%;" />

3.  Set target device by changing to **Device hardware** tab and typing the device name in the **Target path** [for Lyra S use **BGM220SC22HNA**, for Lyra P use **BGM220PC22HNA**], Press **OK** to complete the operation.   

   <img src="images/ImageSetTargetDevice.png" alt="Laird Connectivity" style="zoom:150%;" />

4. Make sure the latest secure FW version is used by reading it.

   <img src="images/ImageReadFwVersionDevSelect.png" alt="Laird Connectivity" style="zoom:150%;" />

5. Select, **Yes** for the Warning dialog to Continue.

   <img src="images/ImageReadFwVersionDevSelectWarning.png" alt="Laird Connectivity" style="zoom:150%;" />

6. Update the Secure FW by clicking the Update button, the update button is prompted if the firmware on the SoC is behind the latest version in IDE.

     <img src="images/ImageUpdFwVersionDevSelect.png" alt="Laird Connectivity" style="zoom:150%;" />

7. Select Crypto profile

    <img src="images/ImageUpdFwVersionDevSelectCryptoProfile.png" alt="Laird Connectivity" style="zoom:150%;" />

8. Allow secure element firmware update to continue by clicking **Yes**

   <img src="images/ImageUpdFwVersionDevSelectWarning.png" alt="Laird Connectivity" style="zoom:150%;" />

9. Validate the secure element firmware update. The latest version should be printed as shown.

     <img src="images/ImageUpdFwVersionDevSelectReadVersionBack.png" alt="Laird Connectivity" style="zoom:150%;" /> 

10. Select SDK for the project.

   <img src="images/ImageSelectSDK.png" alt="Laird Connectivity" style="zoom:150%;" />

11. Create a **Bluetooth - SoC Empty** project for the "Lyra DVK Board" using Simplicity Studio v5. Use the default project settings. 

   <img src="images/ImageLaunchSocEmpty.png" alt="Laird Connectivity" style="zoom:150%;" />

12.  Assign project name, example: *aio_peripheral_server* and select "*Finish*". Once the project is create in the Simplicity Studio workspace, please follow the instruction's session [**Testing the SOC-Empty Application**] in the *readme.html* file of the project to test the firmware. After testing the firmware and if is running as expected we can start with additional modifications to support automation IO service.  

   <img src="images/ImageProjectName.png" alt="Laird Connectivity" style="zoom:150%;" />

13. Open the .slcp file in the project, change to "**Software Components**" tab  and extend "**Advanced Configurators**"  by clicking the arrow, Select [**Bluetooth GATT configurator**] from [**Advanced Configurators**] and click "**Open**" button in the right top corner

   <img src="images/ImageGATTOpen.png" alt="Laird Connectivity" style="zoom:150%;" />

14. Assign device name -  1) select [**Device Name**] from [**Generic Access**] service; 2) Set device name; 3) Set device name size; 4) Save the changes to *.btconf file. BT configurations file can be closed at this point.

   <img src="images/ImageDeviceName.png" alt="Laird Connectivity" style="zoom:150%;" />

15. Install [**Simple timer service**] from [**Application**] -> [**Service**]. 1) Select the [**Simple timer service**] and click "**Install**" button in the top right corner. 

   <img src="images/ImagetimerInstall.png" alt="Laird Connectivity" style="zoom:150%;" />

16. Install [**IO Stream USART**] from [**Services**].  1) Select the [**IO Stream USART**] and click "**Install**" button in the top right corner. 2) Assign "**vcom**" to instance name. 3) Click "**Done**" to complete the installation process of IO stream USART component. 

   <img src="images/ImageIOStreamUSART.png" alt="Laird Connectivity" style="zoom:150%;" />

17. Configure **VCOM**.

   <img src="images/ImageIoStreamUSARTOpenconfig.png" alt="Laird Connectivity" style="zoom:150%;" />

18. Select **USART1**.

   <img src="images/ImageIoStreamUSARTSelectUART1.png" alt="Laird Connectivity" style="zoom:150%;" />

19. Configure **USART1**, assign GPIO/s.

   <img src="images/ImageIoStreamUSARTSConfigUART1.png" alt="Laird Connectivity" style="zoom:150%;" />

20. Install [**Log**] from [**Application**] -> [**Utility**] and click "**Install**" button in the top right corner

   <img src="images/ImageInstallLog.png" alt="Laird Connectivity" style="zoom:150%;" />

21. Install [**Tiny printf**] from [**Third Party**] and click "**Install**" button in the top right corner

   <img src="images/ImageTinyPrintf.png" alt="Laird Connectivity" style="zoom:150%;" />

22. Install Button driver;  [**Platform**] -> [**Driver**] -> [**Button**]; use default name "**btn0**" and click "**Install**" button in the top right corner

	<img src="images/13_InstallBtn.png" alt="Laird Connectivity" style="zoom:150%;" />

23. Configure **Simple Button**.

	<img src="images/ImageSimpleButtonStartConfig.png" alt="Laird Connectivity" style="zoom:150%;" /> 

24. Assign GPIO to **Simple Button**.

	<img src="images/ImageSimpleButtonConfig.png" alt="Laird Connectivity" style="zoom:150%;" /> 

25. Install LED driver;  [**Platform**] -> [**Driver**] -> [**LED**]; use default name "**led0**" and click "**Install**" button in the top right corner

	<img src="images/14_InstallLED.png" alt="Laird Connectivity" style="zoom:150%;" />

26. Configure **Simple LED**

	<img src="images/ImageSimpleLedConfigStart.png" alt="Laird Connectivity" style="zoom:150%;" />

27. Assign GPIO to **Simple LED**

	<img src="images/ImageSimpleLedConfig.png" alt="Laird Connectivity" style="zoom:150%;" />

28. Install [**Automation IO GATT Service**] from [**Bluetooth**] -> [**GATT**]. Click "**Install**" button in the top right corner to complete the installation.

	<img src="images/15_BTGATTAioInstall.png" alt="Laird Connectivity" style="zoom:150%;" />

29. Configure the **Automation IO GATT Service** 

	<img src="images/16_ConfAutoIOGATTSvc.png" alt="Laird Connectivity" style="zoom:150%;" />

30. View source of the **Automation IO GATT Service** 

	<img src="images/17_ViewSource.png" alt="Laird Connectivity" style="zoom:150%;" />

31. Enable advertising of Automation IO **UUID = 0x1815** by setting the "**service advertise**" to "**true**" in the **gatt_service_rht.xml**.  Save the changes. 

	<img src="images/18_AutoIOGATTSvcTRUE.png" alt="Laird Connectivity" style="zoom:150%;" />

32. Delete the original **app.c** source file from early created **soc-empty** template and add to the project the ***[app.c](src/app.c)*** and [***lci_aio_app.c***](src/lci_aio_app.c) source files from this [repository](src).

      <img src="images/19_AddSrcCode.png" alt="Laird Connectivity" style="zoom:150%;" />

33. Build the project. The build process should finish with zero errors and zero warnings. Once is completed, please use debug sessions from Simplicity Studio or SWD to load the firmware executable to the Lyra DVK and at this point we can start with testing the fimrware.     

## How to read digital input/output, control the LED and read button's state

The firmware is based on the Bluetooth Low Energy **soc-empty** template from Simplicity Studio. The example already has the GATT server, advertising, and connection mechanisms. However, to support Automation IO service several additional configurations were added to the template. 

The GATT changes were updating the device name and adding Automation IO service UUID `1815` that has a characteristic UUID `2A56` with Read and Notify properties for the button and second instance of characteristic UUID `2A56` with Read and Write properties for LED. The digital IO characteristics are 1 bit. The bit sets to zero for the LED indicates the "OFF" state and set to one indicates the "ON" state. Similarly, for the button the bit will be set to one when the button is held in the pushed position and will be set to zero otherwise.   

When the AIO peripheral server starts advertising, it will be sending the UUID of the automation IO service in every advertising packet. It provides the central devices in the vicinity the information that this peripheral server supports automation IO service. In addition, the Lyra DVK LED is utilized with the simple timer to indicate the user in regards to the state of the device. The LED will be flashing with 1 second duty cycle during advertising and will be solid upon establishing the connection with the central device. If the connection is closed, the device will go back to advertising state and LED will continue to flashing with 1 second duty cycle. 

The Lyra DVK button is supported by *sl_button_on_change*  button handler function, which runs every time the state of the button is changed. The function is included to demonstrate the integration of the button in the application and can be extended as necessary. Currently, it outputs the button states - [push] & [release] to serial interface.      

To interact with the sensor please follow the below steps:

1. Open the nRFConnect app on your smartphone. The application can be downloaded from [Apple App Store](https://apps.apple.com/ca/app/nrf-connect-for-mobile/id1054362403) or [Google Play](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=en_CA&gl=US)

2. Find your device in the Bluetooth Scanner, advertising as "aio_peripheral"

3. To test the advertising portion of the firmware works correctly, locate "0x1815 - Automation IO" in the "Complete list of 16-bit Service UUIDs" [step # 13 above]  and "aio_peripheral" in "Complete Local Name" [step # 5 above] sections. 

   In addition, the LED on the Lyra DVK should be flashing with 1 second duty cycle. If the serial port is connected to serial emulator software the following message should be observed during board boot up: "***[I] [AIO] Laird Connectivity simple peripheral server demo***" 

   <img src="images/adv.png" alt="Laird Connectivity" style="zoom:150%;" />   

4. Tap "Connect" and find the Automation IO service, expend it, try to read the LED and Button digital characteristics and check the values. The value of the digital input associated with the button is zero and the value of the digital out associated with LED is one. 

5. Try to change the digital value of the button by pushing it on the Lyra DVK; Check the values again, while holding the button. The value should be set to one. Release the button and read the value again. Now the value should be set back to zero. 

   By using the second digital characteristic change the LED state to "OFF" by writing zero, turn the LED "ON" by writing one. The change of the LED state can be observed on Lyra DVK and two states are supported: "ON, and "OFF". In "ON" state the LED is solid ember, otherwise is turned off.  

   <img src="images/connection.png" alt="Laird Connectivity" style="zoom:150%;" />

TeraTerm logs from the virtual COM port:

   <img src="images/ImageTeraTerm.png" alt="Laird Connectivity" style="zoom:150%;" />

## Execute firmware with project binaries

The precompiled and ready to be used binaries of the bootloader [[bootloader-uart-bgapi.bin](bin/bootloader-uart-bgapi.bin)] and application [[aio_peripheral_server.bin](bin/aio_peripheral_server.bin)] are included in the [bin](bin) folder of the repository. The files can be programmed using Simplicity Studio Flash Programmer tool, Simplicity Commander application or [Segger J-Link](https://www.segger.com/products/debug-probes/j-link/). Remember to flash the bootloader at least once. 

To flash the files using [Segger J-Link](https://www.segger.com/products/debug-probes/j-link/), please follow the steps below, issue target reset "***r***" if the command is completed with errors:   

1. *erase 0x6000 0x80000* - to erase the application partition 

2. *erase 0 0x6000* - to erase the bootloader partition 

3. *loadfile [aio_peripheral_server.bin](bin/aio_peripheral_server.bin) 0x6000* - to program the application partition 

4. *loadfile [bootloader-uart-bgapi.bin](bin/bootloader-uart-bgapi.bin) 0* - to program the bootloader partition

 

