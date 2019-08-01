# Protocol Document

**Prerequisites:**

The Arduino code can be found at:

[https://github.com/Aum19/Arduino\_BME28](https://github.com/Aum19/Arduino_BME280_BLE_GPS)[0\_BLE\_GPS](https://github.com/Aum19/Arduino_BME280_BLE_GPS)

Compile and Upload file Team\_Dexter.ino on the Arduino Uno

To test the Arduino using commands on BLE app: https://play.google.com/store/apps/details?id=com.macdom.ble.blescanner&amp;hl=en\_US

**Setup steps:**

1. **1.**** Arduino and sensor hardware connections.**
2. **2.**** Protocol overview**
3. **3.**** Commands/Requests**
4. **4.**** BLE Scanner App steps**

# Arduino and sensor hardware connections.

The connections here are based on the pin layout of Arduino Uno. Based on certain Arduino types the pin number may change. After uploading the Arduino code setup, the connections as follows:

Pin Layout Arduino reference: [https://components101.com/microcontrollers/arduino-uno](https://components101.com/microcontrollers/arduino-uno)

## BMP 280: Sensor 1

| **Sensor** | **Arduino** | **Sensor** | **Arduino** |
| --- | --- | --- | --- |
| Vin (Voltage In) | D2 | SDA(Serial Data) | A4 |
| SCL (Serial Clock) | A5 | Gnd (Ground) | Gnd |

## SD card reader: Sensor 2

| **Sensor** | **Arduino** | **Sensor** | **Arduino** |
| --- | --- | --- | --- |
| Vin (Voltage In) | 5V | MOSI | D11 |
| MISO | D12 | CLK | D13 |
| CS | D4 | Gnd (Ground) | Gnd |

## BLE HM10:

| **Sensor** | **Arduino** | **Sensor** | **Arduino** |
| --- | --- | --- | --- |
| Vin (Voltage In) | 3.3V | RX | TX D1 |
| TX | RX D0 | Gnd (Ground) | Gnd |

## Neo GPS: Sensor 3

| **Sensor** | **Arduino** | **Sensor** | **Arduino** |
| --- | --- | --- | --- |
| Vin (Voltage In) | 5V | RX | D |
| TX | D | Gnd (Ground) | Gnd |

# Protocol Overview:

## Scan and connect:

From the device application the BLE API can be used to connect to Arduino.

## Get Status:

There are two different ways Arduino can send the status:

1. Upon status request from Application.
2. After successful completion of any requests from application.

## Requests and responses:

1. **Turn ON/OFF sensors:** Sensors can be turned on and off using the Application (Except BLE sensor). After every successful execution the Arduino will send the new status. It is recommended to turn on all the sensors before executing any Read/Write operations.
2. **Change Parameters:**

Parameters available for change are:

1.
  1. Sampling Rate: Sets the interval at which the sensor readings will be written to the memory.
  2. High Temperature Alert: Temperature at which the High Temperature alert light will turn on. Can be set in Celsius.
2. **Write data to SD memory:** At this point all the sensors should be turned on. At specified sampling rate or if not specified at default sampling rate i.e. every 1s(1000ms) sensor readings will be written to the file on the Sd card mounted to the Arduino. With read command an optional comment can be appended that can be used to mark the start of the write or for any other use.
3. **Read data from SD to application:** As soon as a read is requested the write will be stopped if in progress and Arduino will send the data in the Sd&#39;s file via BLE. The receiving application must be ready to receive the data and store it.
4. **Add comments:** At any moment the SD card is turned on we can add Comments in between the readings which can be later easily identified. Arduino will prepend &quot;\\&quot; before the comment data in the file.

# Commands/Requests:

The commands are logically divided into 2 parts and are passed by Application to Arduino via BLE as strings. The separator used here is &quot;;&quot; so only restriction in the parameters passed should not contain &quot;;&quot; in its content.

## Sensor Digit &amp; Function Digit:

1. The first part is sensor digit which are allocated as:

| **Sensor** | Digit |
| --- | --- |
| BMP 280 | 1 |
| SD | 2 |
| GPS | 3 |

## Parameters {max length: 20 chars(mutable)}:

 Sensor 1 = BMP280, Sensor 2 = SD card reader, Sensor 3 = GPS

| **Code to Select function** | **Value to pass** |
| --- | --- |
| 99 : Get Status | Ex: &quot;99;&quot; |
| 10 : Sensor 1 power ON/OFF | Ex: &quot;10;0&quot; |
| 11 : Sensor 1 sample rate | Ex: &quot;11;2000&quot; |
| 12 : Sensor 1 Temperature Alert | Ex: &quot;12;27.45&quot; in Celsius |
| 20 : Sensor 2 power ON/OFF | Ex: &quot;20;1&quot; |
| 21 : Sensor 2 append time stamp, start        writing to SD card | Ex: &quot;21; comment&quot; |
| 22 : Sensor 2 STOP writing and read file | Ex: &quot;22;comment&quot; |
| 23 : Write comment before data | Ex: &quot;23;comment&quot; |
| 24 : Write to a new file | Ex: &quot;24;filename&quot; |
| 30 : Sensor 3 power ON/OFF | Ex: &quot;30;0&quot; |

## Example:

Initially all the sensors except BLE are in OFF state by default.

**Application Request:**

Here we are turning ON BMP280:

Sent: 10;1

**Arduino will turn on the sensor 1 on pin D8**

**Arduino will reply with status:**

BME\_power:         1

SD\_power:          0

GPS\_power:         0

Rate:              2000

Temp\_alert:        26.45

Last\_write:        timestamp







# BLE Scanner App Steps:
