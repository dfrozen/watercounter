# watercounter
Arduino mini LiquidCrystal_I2C watermeter monitor
### warning 
garanted work with Bounce 2 ver 2.2.0
### ver 0.04
Small changes and optimization after tested
### ver 0.01
The screen for the water meter, saves the measurements in EEPROM. There is a reset mode to "0"
Based arduino mini atmega 168p 16MHz

## HOW USE
## [RU] Для начальной установки счетчиков необходимо
#### define RESET Уствновить в 1
#### unsigned int CounterHighBase[COUNTERS] = {00000,00000}; Записать значение Кубометров   {Холодная,Горячая} 
#### unsigned int CounterLowBase[COUNTERS]  = {180,180};     Записать значение литров   {Холодная,Горячая} 
Загрузить прошивку 
#### define RESET Уствновить в 0
Загрузить прошивку 
## [EN] For the initial installation you need
#### define RESET Set to 1
#### unsigned int CounterHighBase [COUNTERS] = {00000,00000}; Record the value of Cubic meters {Cold, Hot}
#### unsigned int CounterLowBase [COUNTERS] = {180,180}; Record the value of liters {Cold, Hot}
upload firmware
#### define RESET Set to 0
upload firmware

## License 
MIT License

Copyright (c) 2018 Andrey Kuznetsoff

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
