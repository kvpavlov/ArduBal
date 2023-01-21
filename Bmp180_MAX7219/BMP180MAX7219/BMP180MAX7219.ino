#include <Adafruit_BMP085.h>
//#include <Adafruit_CircuitPlayground.h>
//#include <Adafruit_Circuit_Playground.h>

//We always have to include the library
#include "LedControl.h"
#include <OneWire.h>

/*
  Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
  pin 12 is connected to the DataIn
  pin 11 is connected to the CLK
  pin 10 is connected to LOAD
  We have only a single MAX72XX.
*/
LedControl lc = LedControl(12, 11, 10, 1);

Adafruit_BMP085 bmp;

OneWire  ds(9);  // on pin 9 (a 4.7K resistor is necessary)

/* we always wait a bit between updates of the display */
unsigned long delaytime = 500;

float celsius; //, fahrenheit;

int q;
int w;
int e;
int r;
//int t;
//int y;

//int u;

//int o;
//int p;
//int m;
//int mm;
//int qq;

void setup() {

  Serial.begin(9600);

  //BMP180
  if (!bmp.begin())
  {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  }

  //The MAX72XX is in power-saving mode on startup,
  //we have to do a wakeup call
  lc.shutdown(0, false);
  // Set the brightness to a medium values
  lc.setIntensity(0, 8);
  // and clear the display
  lc.clearDisplay(0);

}

void ForSegm(String str, int num, boolean sign) {
  /*
    отображение на двойном семисегментнике MAX7219
    str - строка с данными
    num - номер блока индикатора
    sign - проверять знак
  */

  boolean fl;
  int k, j, s;

  if (sign)
  { if (str.charAt(0) != '-')
    { str = " " + str;
      j = 3 + 4 * num;
      s = 4;
    }
  }

  j = 3 + 4 * num;
  s = 4;

  k = str.indexOf('.');
  str.remove(k, 1);
  k--;

  for (int i = 0; i < s; i++)
  { if (i == k) {
      fl = true;
    } else {
      fl = false;
    }

    lc.setChar(0, j - i, str.charAt(i), fl);
  }
}


/*
  void BMP180Date() {

  // запросы к модулю BMP180
  // атм давление
  // температура

  String pan1;

  //Serial.print("Temperature = ");
  //Serial.println(bmp.readTemperature());

  //Serial.print("Давление мм рт. ст. = ");
  //Serial.println(bmp.readSealevelPressure()*0.007500637554192);

  pan1=String(bmp.readTemperature());
  ForSegm(pan1,0,true);

  //pan1=String(bmp.readSealevelPressure()*0.007500637554192);
  pan1=String(bmp.readPressure()*0.007500637554192);
  ForSegm(pan1,1,false);

  delay(delaytime);
  }
*/

void DS18S20Date() {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  //float celsius, fahrenheit;

  if ( !ds.search(addr))
  {
    //Serial.println("No more addresses.");
    //Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }

  /*
    Serial.print("ROM =");
    for( i = 0; i < 8; i++)
    {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
    }
  */

  if (OneWire::crc8(addr, 7) != addr[7])
  {
    //Serial.println("CRC is not valid!");
    return;
  }
  //Serial.println();


  // the first ROM byte indicates which chip
  switch (addr[0])
  {
    case 0x10:
      //Serial.println("Chip=DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      //Serial.println("Chip=DS18B20");
      type_s = 0;
      break;
    case 0x22:
      //Serial.println("Chip=DS1822");
      type_s = 0;
      break;
    default:
      //Serial.println("Device is not a DS18x20 family device.");
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad

  //Serial.print("  Data = ");
  //Serial.print(present, HEX);
  //Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  //Serial.print(" CRC=");
  //Serial.print(OneWire::crc8(data, 8), HEX);
  //Serial.println();

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  //fahrenheit = celsius * 1.8 + 32.0;
  //Serial.print("  Temperature = ");
  //Serial.print(celsius);
  //Serial.println(" Celsius, ");

  //Serial.print(fahrenheit);
  //Serial.println(" Fahrenheit");

}

void loop() {
  String pan1;

  // запросы к модулю BMP180
  // температура
  pan1 = String(bmp.readTemperature());
  ForSegm(pan1, 0, true);

  // атм давление
  //pan1=String(bmp.readSealevelPressure()*0.007500637554192);
  pan1 = String(bmp.readPressure() * 0.007500637554192);
  ForSegm(pan1, 1, false);

  delay(4 * delaytime);

  // запросы к датчику DS18S20
  DS18S20Date();

  //celsius=10.33;
  pan1 = String(celsius);
  //Serial.println(pan1);
  //Serial.println(celsius);
  ForSegm(pan1, 0, true);

  delay(4 * delaytime);

}
