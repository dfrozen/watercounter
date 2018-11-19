#include <Wire.h>                     // Для I2C
#include <LiquidCrystal_I2C.h>        // Для I2C LCD
#include <Bounce2.h>                  // Для обработки замыкания линий со счетчиков
#include <EEPROMex.h>                 // Для работы с EEPROM, расширенная библиотека
//////////////////////////////////////////////////////////////////////////////////////////////

#define DEBUG            1   // Выдача отладочной информации в COM-порт
#define RESET            0   // Установка первоначального "0"
//////////////////////////////////////////////////////////////////////////////////////////////
// Пины для подключения устройств
#define BUTTON_PIN       6    //Пин с конпкой
#define HOT_COUNTER_PIN  4    //Пин счетчика горячей воды
#define COLD_COUNTER_PIN 5    //Пин счетчика холодной воды
#define COUNTERS 2            //Колличество счетчиков в системе
//////////////////////////////////////////////////////////////////////////////////////////////

unsigned int CounterHighBase[COUNTERS] = {0000,0000};     // Если значение отлично от нуля - то пишем его в качестве базового     
unsigned int CounterLowBase[COUNTERS]  = {130,80};     // Если значение отлично от нуля - то пишем его в качестве базового
int counterReadDelay  = 0;                          // Текущая задержка считывания счетчика 
                                                    // (нужна для уверенной отработки переключения счетчика) 
int CounterPin[COUNTERS]         = {COLD_COUNTER_PIN, HOT_COUNTER_PIN};  // Пины 
int CounterHighAddress[COUNTERS] = {0x30, 0x40};     //Адреса EEPROM для младшего слова (кубометры)  4 байта
int CounterLowAddress[COUNTERS]  = {0x38, 0x48};     //Адреса EEPROM для младшего слова (литры) 2 байта
char *CounterName[COUNTERS]      = {"Cold :", "Hot  :"};                 // Названия счетчиков для вывода на экран 
Bounce CounterBouncer[COUNTERS]  = {};               // Формируем для счетчиков Bounce объекты
//////////////////////////////////////////////////////////////////////////////////////////////
#define LED_DELAY        30000                    //Задержка в выключении экрана

int ledDelayCount     =  0;                       // Текущая задержка в выключении экрана     
#define LCD_COL           20                      // Разрешение экрана - колонки
#define LCD_ROW           4                       // Разрешение экрана - строки

LiquidCrystal_I2C lcd(0x3F,LCD_COL,LCD_ROW);      // Устанавливаем дисплей
boolean ledEnabled  = true;                       // включен ли экран?
boolean buttonWasUp = true;                       // была ли кнопка отпущена?

/////////////////////////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  delay(10);
  #if DEBUG
    Serial.begin(9600);
    Serial.println( "Debug is ON...");
    Serial.println( "------- FLAGS ------");
 
  #endif
   for (int i=0; i<COUNTERS; i++)
  {   
    pinMode(CounterPin[i], INPUT);                              // Инициализируем пин
    digitalWrite(CounterPin[i], HIGH);                          // Включаем подтягивающий резистор
    CounterBouncer[i].attach(CounterPin[i]);                    // Настраиваем Bouncer
    CounterBouncer[i].interval(10);                             // и прописываем ему интервал дребезга
  }

    lcd.init();                                     // Инициализируем дисплей
    lcd.backlight();                                // Включаем подсветку дисплея
    pinMode(BUTTON_PIN, INPUT_PULLUP);              // Инициализация конпки
    countersInit();                                   // Инициализация начальных показаний счетчиков              
    for (int i=0; i<COUNTERS; i++)                  // Выводим на экран начальные значения    
    {
        printPos(0,i,CounterName[i]);
        printHigh(7,i,CounterHighBase[i]);
        printPos(12,i,",");
        printLow(13,i,CounterLowBase[i]);
    }
}
void loop() {
  // put your main code here, to run repeatedly:

delay (1);
 readCounter();                                    // Читаем и обрабатываем значения счетчиков

}


  void printPos(byte col, byte row, char* str)
  {
    lcd.setCursor(col, row);
    lcd.print(str);
  }
  
  //////////////////////////////////////////////////////////////////////////////////////////////
  // Выводит на LCD старший разряд счетчика, начиная с указаных координат
  void printHigh(byte col, byte row, int val)
  {
    char str1[5];
    row=row;
    lcd.setCursor(col, row);
    sprintf(str1,"%0.5u",val);
    lcd.print(str1);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////
  // Выводит на LCD младший разряд счетчика, начиная с указаных координат
  void printLow(byte col, byte row, int val)
  {
    char str1[3];
    row=row;
    lcd.setCursor(col, row);
    sprintf(str1,"%0.3u",val);
    lcd.print(str1);
  }
//////////////////////////////////////////////////////////////////////////////////////////////
void countersInit()
{
  // инит пинов счетчиков
  for (int i=0; i<COUNTERS; i++)
  {
    #if RESET // Установка значений по умолчанию - пишем его в EEPROM
      EEPROM.writeInt(CounterLowAddress[i], CounterLowBase[i]);
      EEPROM.writeInt(CounterHighAddress[i], CounterHighBase[i]);
    #endif  
    CounterHighBase[i] = EEPROM.readInt(CounterHighAddress[i]); // Читаем начальные значения из EEPROM
    CounterLowBase[i]  = EEPROM.readInt(CounterLowAddress[i]);  // Читаем начальные значения из EEPROM
    #if DEBUG
      Serial.print("Read form EEPROM "); Serial.print(i,DEC ); Serial.print(" counter. Name "); Serial.println(CounterName[i]); 
      Serial.print(CounterHighAddress[i] ,HEX); Serial.print(" => "); Serial.println(CounterHighBase[i]);
      Serial.print(CounterLowAddress[i]  ,HEX); Serial.print(" => "); Serial.println(CounterLowBase[i] );
    #endif
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Обрабатываются счетчики
void readCounter()
{
  if (counterReadDelay >= 100) // Если подошло время обработки
  {
    counterReadDelay = 0;
    for (int i=0; i<COUNTERS; i++) 
    {
      boolean changed = CounterBouncer[i].update();
      //Serial.println(changed);
      if ( changed ) {
        int value = CounterBouncer[i].read();
        if ( value == LOW) // Если значение датчика стало ЗАМКНУТО, т.е сработал счетчик
        {
          #if DEBUG
            Serial.print("Counter [ "); Serial.print(CounterName[i]); Serial.print("] switched ");
          #endif
          if (CounterLowBase[i]<990) // если не произошло перехода на кубометры - увеличиваем счетчик литров на 10
          {
            CounterLowBase[i]+=10;  
            printLow(13,i,CounterLowBase[i]);        
            EEPROM.writeInt (CounterLowAddress[i],CounterLowBase[i]);
            #if DEBUG
               Serial.print("Write to EEPROM "); Serial.print(i,DEC ); Serial.print(" counter. Name "); Serial.println(CounterName[i]); 
               Serial.print(CounterHighAddress[i] ,HEX); Serial.print(" => "); Serial.println(CounterHighBase[i]);
               Serial.print(CounterLowAddress[i]  ,HEX); Serial.print(" => "); Serial.println(CounterLowBase[i] );
          #endif
          }
          else  // иначе, если произошел переход - обнуляем счетчик литров и увеличиваем счетчик кубометров на 1
          {
              CounterLowBase[i] = 0;
              CounterHighBase[i]++;
              printHigh(7 ,i ,CounterHighBase[i]);
              printLow(13,i ,CounterLowBase[i] );         
            EEPROM.writeInt( CounterLowAddress[i],  CounterLowBase[i] );
            EEPROM.writeInt( CounterHighAddress[i], CounterHighBase[i]);
            #if DEBUG
               Serial.print("Write to EEPROM "); Serial.print(i,DEC ); Serial.print(" counter. Name "); Serial.println(CounterName[i]); 
               Serial.print(CounterHighAddress[i] ,HEX); Serial.print(" => "); Serial.println(CounterHighBase[i]);
               Serial.print(CounterLowAddress[i]  ,HEX); Serial.print(" => "); Serial.println(CounterLowBase[i] );
            #endif
          }  
        }
      }
    }
  }
  else //Если время обработки еще не истекло
  {    
    counterReadDelay++;
    //println(counterReadDelay); 
  } 
}
