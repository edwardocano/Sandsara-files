#include "Bluetooth.h"
#include <Update.h>
#include <SdFiles.h>
#include <Preferences.h>

#include "NimBLEDevice.h"
Preferences preferences;    
//#include <BLEDevice.h>
//#include <BLEUtils.h>
//#include <BLEServer.h>
//#include <BLE2902.h>
//#include <BLEAdvertisedDevice.h>

#include "Motors.h"
#include <EEPROM.h>
#include <FastLED.h>
#include "RTClib.h"

#include <ESP32Time.h>

//===============Variables RTC Time==============
ESP32Time rtcBLE;

//===============================================

extern SdFat SD;
extern bool readingSDFile;

//Definiciones para la transmicion por ota
#include "esp_ota_ops.h"
bool updateFlag = false;
esp_ota_handle_t otaHandler = 0;
#define FULL_PACKET 512

bool timeTicToc = false;
unsigned long endTime;
unsigned long startTime;
unsigned long totalTime;

int countTimes = 0;
int sumTime = 0;
int avTotalTime = 0;
bool bandAvgTime = false;

String playListNameG = "";


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

int MAX_SPEED_MOTOR = String(String(dataS[34]) + String(dataS[35]) + String(dataS[36])).toFloat();
//int MIN_SPEED_MOTOR = String(String(dataS[38])).toFloat();
int MIN_SPEED_MOTOR = 25;

int MAX_PERIOD_LED = String(String(dataS[40]) + String(dataS[41]) + String(dataS[42])).toFloat();
int MIN_PERIOD_LED = String(String(dataS[44]) + String(dataS[45])).toFloat();

int MAX_PALLETE = String(String(dataS[47]) + String(dataS[48])).toFloat();
int MIN_PALLETE = String(dataS[50]).toFloat();

int MAX_CHARACTERS_BTNAME = 20;

int MIN_REPRODUCTION_MODE = String(dataS[55]).toFloat();
int MAX_REPRODUCTION_MODE = String(dataS[57]).toFloat();

int MAX_POSITIONLIST = String(String(dataS[59]) + String(dataS[60]) + String(dataS[61]) + String(dataS[62])).toFloat();

#ifdef rtcExterno
RTC_PCF8563 rtcBle;

  
#endif

extern  int     periodLedsGlobal;
extern  int     delayLeds;
extern  int     romSetPlaylist(String );
extern  String  romGetPlaylist();
extern  int     romSetOrderMode(int );
extern  int     romGetOrderMode();
extern  int     romSetPallete(int );
extern  int     romGetPallete();
extern  int     romSetSpeedMotor(int );
extern  int     romGetSpeedMotor();
extern  int     romSetPeriodLed(int );
extern  int     romGetPeriodLed();
extern  int     romSetCustomPallete(uint8_t* ,uint8_t* , uint8_t* ,uint8_t*, int);
extern  int     romSetBluetoothName(String );


extern  int     romSetBluetoothTimePaths(String);
extern  int     romSetBluetoothTimeOffOn(String);


extern  String  romGetBluetoothName();


extern  String  romGetBluetoothTimePaths();
extern  String  romGetBluetoothTimeOffOn();


extern  int     romSetIntermediateCalibration(bool );
extern  bool    romGetIntermediateCalibration();
extern  int     romSetPositionList(int );
extern  int     romGetPositionList();
extern  bool    romSetIncrementIndexPallete(bool );
extern  int     romGetIncrementIndexPallete();
extern  bool    romSetLedsDirection(bool );
extern  bool    romGetLedsDirection();
extern  int     romSetBrightness(uint8_t );
extern  int     romGetBrightness();
extern  bool    incrementIndexGlobal;
extern  bool    ledsDirection;
extern  int     ledModeGlobal;
extern  void    changePalette(int );
extern  int     stringToArray(String , uint8_t* , int );
extern  bool    changePositionList;
extern  bool    changeProgram;
extern  bool    sdExists(String );
extern  String  playListGlobal;
extern  SdFat   SD;
extern  bool     sdRemove(String );
extern  bool     sdExists(String );
extern  bool     readingSDFile;
extern  int      orderModeGlobal;
extern  bool     rewindPlaylist;
extern  bool     pauseModeGlobal;
extern  String  bluetoothNameGlobal;
extern String  bluetoothTimePaths;
extern String  bluetoothTimeOffOn;
extern  bool    suspensionModeGlobal;
extern  Motors Sandsara;
extern  bool    speedChangedMain;
extern  bool    intermediateCalibration;

extern  bool     ledsOffGlobal;

extern int     speedMotorGlobal;
extern int pathPercentage;

//extern int posListTemp;

//====variables====
extern  String      changedProgram;
extern  int         changedPosition;
bool        receiveFlag = false;
bool        sendFlag = false;
char        buffer[10000];
char        *pointerB;
int         bufferSize;
File        fileReceive;
File        fileSend;

//====Prototypes====
int performUpdate(Stream &, size_t );
int updateFromFS(SdFat &, String );
int programming(String );
void rebootWithMessage(String );
int stringToArray(String , uint8_t* , int );

String obtenerTerminacionArchivo(int); 

//====BLE Characteristics======
//=============================

//Led strip config
BLECharacteristic *ledCharacteristic_speed;
BLECharacteristic *ledCharacteristic_update;
BLECharacteristic *ledCharacteristic_cycleMode;
BLECharacteristic *ledCharacteristic_direction;
BLECharacteristic *ledCharacteristic_brightness;
BLECharacteristic *ledCharacteristic_amountColors;
BLECharacteristic *ledCharacteristic_positions;
BLECharacteristic *ledCharacteristic_red;
BLECharacteristic *ledCharacteristic_green;
BLECharacteristic *ledCharacteristic_blue;
BLECharacteristic *ledCharacteristic_errorMsg;
BLECharacteristic *ledCharacteristic_indexPalette;
BLECharacteristic *ledCharacteristic_offLeds;
BLECharacteristic *ledCharacteristic_onLeds;

//Testing
BLECharacteristic *testingCharacteristic_name;

//playlist config
BLECharacteristic *playlistCharacteristic_name;
BLECharacteristic *playlistCharacteristic_pathAmount;
BLECharacteristic *playlistCharacteristic_pathName;
BLECharacteristic *playlistCharacteristic_addPathNamePlaylist;
BLECharacteristic *playlistCharacteristic_pathNameNotify;
BLECharacteristic *playlistCharacteristic_pathPosition;
BLECharacteristic *playlistCharacteristic_readPlaylistFlag;
BLECharacteristic *playlistCharacteristic_readPath;
BLECharacteristic *playlistCharacteristic_addPath;
BLECharacteristic *playlistCharacteristic_mode;
BLECharacteristic *playlistCharacteristic_progress;
BLECharacteristic *playlistCharacteristic_errorMsg;

BLECharacteristic *playlistCharacteristic_nameFiles;
BLECharacteristic *playlistCharacteristic_shortPlaylist;
BLECharacteristic *playlistCharacteristic_selectPlaylist;
BLECharacteristic *playlistCharacteristic_playNow;

//Files
BLECharacteristic *fileCharacteristic_receiveFlag;
BLECharacteristic *fileCharacteristic_receive;
BLECharacteristic *fileCharacteristic_receiveOta;
BLECharacteristic *fileCharacteristic_exists;
BLECharacteristic *fileCharacteristic_delete;
BLECharacteristic *fileCharacteristic_sendFlag;
BLECharacteristic *fileCharacteristic_send;
BLECharacteristic *fileCharacteristic_errorMsg;  
BLECharacteristic *fileCharacteristic_receiveFlagPatterns;
BLECharacteristic *fileCharacteristic_indexCustomPattern;  
BLECharacteristic *fileCharacteristic_customPatterns;  

//====General====
BLECharacteristic *generalCharacteristic_version;
BLECharacteristic *generalCharacteristic_name;
BLECharacteristic *generalCharacteristic_status;
BLECharacteristic *generalCharacteristic_pause;
BLECharacteristic *generalCharacteristic_play;
BLECharacteristic *generalCharacteristic_sleep;
BLECharacteristic *generalCharacteristic_speed;
BLECharacteristic *generalCharacteristic_restart;
BLECharacteristic *generalCharacteristic_factoryReset;
BLECharacteristic *generalCharacteristic_errorMsg;
BLECharacteristic *generalCharacteristic_calibration;

BLECharacteristic *generalCharacteristic_timePaths;
BLECharacteristic *generalCharacteristic_timeOffOn;
BLECharacteristic *generalCharacteristic_appTime;

BLECharacteristic *generalCharacteristic_SimpleVersion;
BLECharacteristic *generalCharacteristic_TracksForCalib;

//Declaracion de la caracteristica NewCharacteristic (1)
BLECharacteristic *generalCharacteristic_model;
BLECharacteristic *generalCharacteristic_battery;

//================================Callbacks=============================
//======================================================================


//Testing Callback
class testingCallbacks_desfase : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE testResult: ");
            Serial.println(value);
        #endif


        //characteristic->setValue(String(position).c_str());
        characteristic->setValue(value);             //Cambio para la libreria NimBLE
        
       
       
        String dataBLE = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE);
        playlistCharacteristic_errorMsg->notify();
        
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ testResult: ");
            Serial.println(value);
        }
    #endif
};

void Bluetooth::setTestResult(String tResult){

    testingCharacteristic_name->setValue(tResult);                 //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET test Result: ");
        Serial.println(tResult);
    #endif
}


//======================================================================

/**
 * @brief **[BLE]** this class is used for debugging
 * 
 * we use this class to know when a client connects with the device.
 */
class bleServerCallback : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer){
            Serial.println("BLE Server connected");

        Sandsara.setSpeed(SPEED_WHEN_IS_CONNECTED_TO_BLE);
        Serial.println("Dentro de funcion onCoonect");
        Serial.print("Speed: ");
        Serial.println(SPEED_WHEN_IS_CONNECTED_TO_BLE); 
        romSetSpeedMotor(SPEED_WHEN_IS_CONNECTED_TO_BLE);
        speedChangedMain = true;

        int actualPathPlaying;
        actualPathPlaying = romGetPositionList();

        Serial.println("El patron que se esta reproduciendo en este momento esta en la posicion:");
        Serial.println(actualPathPlaying);

        int numbFiles;
        int Code = 0;
        int pListFileG;
        String fileNameB = "";
        int numberOfFiles = 0;

        playListGlobal = romGetPlaylist();
        Code = SdFiles::getLineNumber(actualPathPlaying, playListGlobal, fileNameB);
        Serial.println("Archivo seleccionado con pathPosition");
        Serial.println(fileNameB);


        //===Obtener los ultimos 20 caracteres del nombre dell patron

        String last20Chars = fileNameB.substring(fileNameB.length() - 20);


        playlistCharacteristic_pathNameNotify->setValue(last20Chars);            
        playlistCharacteristic_pathNameNotify->notify();

        playlistCharacteristic_pathName->setValue(fileNameB);
        Bluetooth::setPathPosition(actualPathPlaying);

        Bluetooth::setPlaylistToggle();
        //bluetoothNameGlobal = romGetBluetoothName();
        //Bluetooth::setName(bluetoothNameGlobal);

        Bluetooth::setTimePaths(bluetoothTimePaths);
        Bluetooth::setTimeOffOn(bluetoothTimeOffOn);
        Bluetooth::setStatus(MODE_PLAY);
        Bluetooth::setRed();
        Bluetooth::setGreen();
        Bluetooth::setBlue();
        Bluetooth::setPositions();
        Bluetooth::setAmountOfColors();
        Bluetooth::setPlaylistName(playListGlobal);

        numberOfFiles = SdFiles::numberOfLines(playListGlobal);
        Bluetooth::setPathAmount(numberOfFiles);

        //Bluetooth::setMotorSpeed(speedMotorGlobal);
        Bluetooth::setPercentage(pathPercentage);

        //Segmento donde vuelvo a leer el archivo de la playlist y genero nuevamente la variable de la playlist en formato corto
        Serial.println("Inicia while de la lectura del archivo de la playlist on Connnect");
        File fileT = SD.open("/playlist.playlist");
        String fileContent = "-";
        
        std::vector<int> numeros;

    while (fileT.available())
    {
            // Leer el nombre del archivo
            String nombreArchivo = fileT.readStringUntil('\n');

            Serial.println(nombreArchivo);

            // Extraer el número de la cadena eliminando los caracteres no numéricos
            String numberString = nombreArchivo.substring(nombreArchivo.lastIndexOf('-') + 1, nombreArchivo.lastIndexOf('.'));
            numberString.trim(); // Eliminar espacios en blanco adicionales

            // Convertir el número en un entero
            int number = numberString.toInt();

            // Imprimir el resultado
            Serial.println(number);

            numeros.push_back(number);
    }

    for (size_t i = 0; i < numeros.size(); i++)
    {
            Serial.println(numeros[i]);
    }

    // Generar el String con los números separados por guiones

    for (size_t i = 0; i < numeros.size(); i++)
    {
            fileContent += String(numeros[i]);
            if (i < numeros.size() - 1)
            {
                fileContent += "-";
            }
    }

    // Close the file
    fileT.close();
    Serial.println("Content playlist set Value On connect: ");
    Serial.println(fileContent);
    playlistCharacteristic_nameFiles->setValue(fileContent);
    Serial.println("Autside Onconnect");
    }

    void onDisconnect(BLEServer *pServer){
        #ifdef DEBUGGING_BLUETOOTH
            Serial.println("BLE Server disconnected");
        #endif
        //Set motor speed
        int speed = String(generalCharacteristic_speed->getValue().c_str()).toInt();
        Serial.println("Despues de generalCharacteristic_speed");
        Serial.print("Speed: ");
        Serial.println(speed);
        speed = map(speed,MIN_SLIDER_MSPEED,MAX_SLIDER_MSPEED,MIN_SPEED_MOTOR,MAX_SPEED_MOTOR);
        Serial.println("Despues de map(");
        Serial.print("Speed: ");
        Serial.println(speed);
        if (speed > MAX_SPEED_MOTOR || speed < MIN_SPEED_MOTOR){
            speed = SPEED_MOTOR_DEFAULT;
        }
        Sandsara.setSpeed(speed);
        romSetSpeedMotor(speed);
        speedChangedMain = true;
        //END
        //recieving and sending process
        if (receiveFlag)
        {
            char namePointer [100];
            fileReceive.getName(namePointer, 100);
            String name  = namePointer;
            fileReceive.close();
            if (sdRemove(name)){
                #ifdef DEBUGGING_BLUETOOTH
                    Serial.print("receiving transmision was canceled, file: ");
                    Serial.print(name);
                    Serial.println(" was deleted");
                #endif
            }
            else
            {
                #ifdef DEBUGGING_BLUETOOTH
                Serial.print("receiving transmision was canceled, but file: ");
                Serial.print(name);
                Serial.println(" was not deleted");
                #endif
            }
            pauseModeGlobal = false;
            receiveFlag = false;
        }
        if (sendFlag){
            char namePointer [100];
            fileSend.getName(namePointer, 100);
            String name  = namePointer;
            fileSend.close();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("sending transmision was canceled, file: ");
                Serial.print(name);
                Serial.println(" was no sent");
            #endif
            pauseModeGlobal = false;
            sendFlag = false;
        }
        

    }
};

/**
 * @brief **[BLE Command]** This function is called when you write or read the ledSpeed characteristic
 * 
 * **Properties:**  
 * - Write
 * - Read
 * 
 * **UUID: 1a9a7b7e-2305-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * you can write this characteristic to set the speed of the LEDs. In the same way, you can read it to know the speed of the LEDs
 * 
 * **Range:**  
 * accepted values go from 1 to 100, where 1 is the slowest speed and 100 is the fastest speed.
 * 
 * @note when you write to this characteristic you will be notified by the ledCharacteristic_errorMsg with some of these values:
 * - "ok" means the value was set successfully.
 * - "error = -70" the values is out of range.
 */
class speedLedCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        int periodLed = value.toInt();


        //characteristic->setValue(String(periodLed).c_str());
        characteristic->setValue(String(periodLed));           //Cambio para la libreria NimBLE
        
        periodLed = map(periodLed, MIN_SLIDER_LEDSPEED,MAX_SLIDER_LEDSPEED,MAX_PERIOD_LED,MIN_PERIOD_LED);
        if(periodLed < MIN_PERIOD_LED || periodLed > MAX_PERIOD_LED){

            String dataBLE = "error = -70";
            ledCharacteristic_errorMsg->setValue(dataBLE);
            ledCharacteristic_errorMsg->notify();
            return;
        }
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE ledSpeed: ");
            Serial.println(periodLed);
        #endif
        periodLedsGlobal = periodLed;
        delayLeds = periodLed;
        romSetPeriodLed(periodLedsGlobal);
        String dataBLE = "ok";
        ledCharacteristic_errorMsg->setValue(dataBLE);
        ledCharacteristic_errorMsg->notify();
    } //onWrite
    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ ledSpeed: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This function is called when you write or read the cycleMode characteristic
 * 
 * the cycle mode is used to control the sequence of the LEDs and there is 2 mode, cycle and solid.  
 * - **cycle mode** means that the LED strip will show all the colors from the selected palette at the same tame.  
 * - **solid mode** means that the LED strip will set the same color chosen from the selected palette for all the LEDs in the LED strip.    
 * 
 * **Properties:**  
 * - Write
 * - Read
 * 
 * **UUID: 1a9a7dea-2305-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * you can write to this characteristic to set the cycle mode. In the same way, you can read it to know the cycle mode
 * 
 * **Range:**  
 * accepted values are 0 or any other diffeent than 0.
 * - 0 to set cycle mode.
 * - any other number to set solid mode.  
 * 
 * @note when you write to this characteristic you will be notified by the ledCharacteristic_errorMsg with some of these values:
 * - "ok" means the value was set successfully.
 * - "error = -70" the values is out of range.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class cycleModeCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        int cycleMode = value.toInt();
        if (cycleMode > 0){
            romSetIncrementIndexPallete(true);
        }
        else{
            romSetIncrementIndexPallete(false);
        }
        incrementIndexGlobal = romGetIncrementIndexPallete();
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE cycleMode: ");
            Serial.println(incrementIndexGlobal);
        #endif


        //characteristic->setValue(String(incrementIndexGlobal).c_str());
        characteristic->setValue(String(incrementIndexGlobal));        //Cambio para la libreria NimBLE
        
        String dataBLE = "ok";
        ledCharacteristic_errorMsg->setValue(dataBLE);
        ledCharacteristic_errorMsg->notify();
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ cycleMode: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write the ledDirection characteristic.
 * 
 * the led strip has 2 directions, clockwise and counter-clockwise, you can choose one changing the characteristic.
 * 
 * **Properties:**  
 * - Write
 * - Read
 * 
 * **UUID: 1a9a8042-2305-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * you can write to this characteristic to set the direction of the led strip, or in the same way you can read it to know it.
 * 
 * **Range:**  
 * accepted values are 0 or any other diffeent than 0.
 * - 1 for clockwise direction.
 * - 0 for counter clockwise direction.  
 * 
 * @note when you write to this characteristic you will be notified by the ledCharacteristic_errorMsg with some of these values:
 * - "ok" means the value was set successfully.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class directionCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        //retorna ponteiro para o registrador contendo o valor atual da caracteristica
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        int direction = value.toInt();
        if (direction != 0){
            romSetLedsDirection(true);
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("WRITE direction: ");
                Serial.println("true");
            #endif
            String dataBLE1 = "1";
            characteristic->setValue(dataBLE1);
        }
        else{
            romSetLedsDirection(false);
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("WRITE direction: ");
                Serial.println("false");
            #endif

            String dataBLE2 = "0";
            characteristic->setValue(dataBLE2);
        }
        
        ledsDirection = romGetLedsDirection();

        String dataBLE3 = "ok";
        ledCharacteristic_errorMsg->setValue(dataBLE3);
        ledCharacteristic_errorMsg->notify();
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ ledDirection: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write the ledBrightness characteristic.
 * 
 * you can change the brightness of the led strip just by modifying this characteristic.
 * 
 * **Properties:**  
 * - Write
 * - Read
 * 
 * **UUID: 1a9a8948-2305-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * you have to write to this characteristic the percentage of the desired brightness or you can read it to know it.
 * 
 * **Range:**  
 * - the brightness characteristic accepts values between 0 and 100. 
 * 
 * **posible reponses:**
 * - "ok" the desired brightness was set.
 * - "error= -1" the brightness is our of range.
 * 
 * @note when you write to this characteristic you will be notified by the ledErrorMsg characteristic with the response.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class setBrightnessCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        //retorna ponteiro para o registrador contendo o valor atual da caracteristica
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        int brightness = value.toInt();
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE brightness: ");
            Serial.println(brightness);
        #endif

        //characteristic->setValue(String(brightness).c_str());
        characteristic->setValue(String(brightness));        //Cambio para la libreria NimBLE
        
        brightness = map(brightness,MIN_SLIDER_BRIGHTNESS,MAX_SLIDER_BRIGHTNESS,48,255);
        
        if(brightness < 0 || brightness > 255){


            //characteristic->setValue(String(romGetBrightness()).c_str());
            characteristic->setValue(String(romGetBrightness()));   //Cambio para la libreria NimBLE

            String dataBLE1 = "error = -1";
            ledCharacteristic_errorMsg->setValue(dataBLE1);
            ledCharacteristic_errorMsg->notify();
            return;
        }
        //FastLED.setBrightness(brightness);
        romSetBrightness(brightness);
        
        String dataBLE2 = "ok";
        ledCharacteristic_errorMsg->setValue(dataBLE2);
        ledCharacteristic_errorMsg->notify();
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ brightness: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create and object to be called when you write the selectedPalette characteristic.
 * 
 * **this characteristic is deprecated**  
 * This characteristic used to be used to change between different preset palettes hardcoded, but this characteristic is deprecated because
 * it is possible we delete the hardcoded palettes.
 * 
 * **range: **  
 * "0" to "15" to select a preset palette.  
 * "16" to select the custom palette.
 */
class selectedPaletteCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        int valueInt = value.toInt();
        if(valueInt < MIN_PALLETE || valueInt > MAX_PALLETE){

            String dataBLE = "error = -31";
            ledCharacteristic_errorMsg->setValue(dataBLE);
            ledCharacteristic_errorMsg->notify();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("WRITE presetPalette ERROR");
            #endif
            return;
        }
        
        ledModeGlobal = valueInt;
        changePalette(ledModeGlobal);
        romSetPallete(ledModeGlobal);
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE presetPalete: ");
            Serial.println(ledModeGlobal);
        #endif
        String dataBLE2 = "ok";
        ledCharacteristic_errorMsg->setValue(dataBLE2);
        ledCharacteristic_errorMsg->notify();
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ presetPalette: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This function is called when you write the UpdatePalette characteristic
 * 
 * You can define a custom palette to be shown in the led strip and you only need to send the “main colors” of the palette
 * ( as low as 2 and up to 16) which Sandsara internally interpolates to a 256 color gradient.
 * 
 * Properties:  
 * - Write
 * 
 * **UUID: 1a9a87b8-2305-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * To define the custom palette, your need the next information: Amount of Colors, Positions, Red, Green, and Blue.
 * only write to this characteristic the number of colors, positions, reds, greens, and blues values. **Every value must be a Byte**.
 * 
 * **Range:**  
 * - amount of colors: goes from 2 to 16.  
 * - positions: every position goes from 0 to 255.  
 * - reds: every red value goes from 0 to 255.  
 * - greens: every green value goes from 0 to 255.  
 * - blues: every blue value goes from 0 to 255.  
 * 
 * In this image you can see more clearly how to send a palette.
 * \image html updatePaletteExample.png
 * 
 * **posible reponses:**  
 * - "ok" the custom palette was updated successfully.
 * - "error= -1" there is no data to update.
 * - "error= -2 the number of colors is out of range"
 * - "error= -3 the data is incompleted"  
 * 
 * if you want to know what is the current palette you need to read the next characteristics
 * - amount of colors characteristic (UUID: 1a9a820e-2305-11eb-adc1-0242ac120002)
 * - positions characteristic (UUID: 1a9a82d6-2305-11eb-adc1-0242ac120002)
 * - red characteristic (UUID: 1a9a83a8-2305-11eb-adc1-0242ac120002)
 * - green characteristic (UUID: 1a9a8466-2305-11eb-adc1-0242ac120002)
 * - blue characteristic (UUID: 1a9a852e-2305-11eb-adc1-0242ac120002)
 * @note when you write to this characteristic you will be notified by the ledErrorMsg characteristic with the response.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class CallbacksToUpdate : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        
        //=====
        int sizeData = rxValue.length();

        #ifdef DEBUGGING_DATA
            Serial.print("data size: ");
            Serial.println(sizeData);
        #endif
        if (sizeData <= 0){
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("Write null data for palette");
            #endif
            String dataBLE = "error= -1";
            ledCharacteristic_errorMsg->setValue(dataBLE); //null data
            ledCharacteristic_errorMsg->notify();
            return;
        }
        char data[sizeData+1];
        rxValue.copy(data,sizeData,0);
        Serial.print("data 0x ");
        for (int i = 0; i < sizeData; i++){
            Serial.print(data[i],HEX);
            Serial.print("-");
        }
        Serial.println("");
        int n = data[0]; //amount of colors
        //check if the amuont of colors is valid
        if (n < 2 || n > 16){
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("not a valid number of colors");
            #endif
            String dataBLE2 = "error= -2";
            ledCharacteristic_errorMsg->setValue(dataBLE2); //not a valid number of colors
            ledCharacteristic_errorMsg->notify();
            return;}
        // check for incomplete data
        if (n*4 != sizeData - 1){
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("WRITE incomplete data");
            #endif
            String dataBLE3 = "error= -3";
            ledCharacteristic_errorMsg->setValue(dataBLE3); //incomplete data
            ledCharacteristic_errorMsg->notify();
            return;
        }

        uint8_t positions[n];
        uint8_t red[n];
        uint8_t green[n];
        uint8_t blue[n];
        
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("Positions: ");
        #endif
        for(int i=0; i<n; i++){
            positions[i] = data[i + 1];
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print(positions[i]);
                Serial.print(",");
            #endif
        }
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("\nred: ");
        #endif
        for(int i=0; i<n; i++){
            red[i] = data[n + i + 1];
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print(red[i]);
                Serial.print(",");
            #endif
        }
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("\ngreen: ");
        #endif
        for(int i=0; i<n; i++){
            green[i] = data[2*n + i + 1];
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print(green[i]);
                Serial.print(",");
            #endif
        }
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("\nblue: ");
        #endif
        for(int i=0; i<n; i++){
            blue[i] = data[3*n + i + 1];
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print(blue[i]);
                Serial.print(",");
            #endif
        }
        #ifdef DEBUGGING_BLUETOOTH
            Serial.println("");
        #endif

        romSetCustomPallete(positions, red, green, blue, n);
        Bluetooth::setRed();
        Bluetooth::setGreen();
        Bluetooth::setBlue();
        Bluetooth::setPositions();
        Bluetooth::setAmountOfColors();

        ledModeGlobal = 16;
        changePalette(ledModeGlobal);
        romSetPallete(ledModeGlobal);
        ledCharacteristic_errorMsg->notify();
        #ifdef DEBUGGING_BLUETOOTH
            Serial.println("Custom palette was updated");
        #endif
        String dataBLE4 = "ok";
        ledCharacteristic_errorMsg->setValue(dataBLE4);
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ toUpdate: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE]** we use this class for debugging
 * 
 * we use this class to know when a client communicate with some characteristics.
 */
class genericCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        //retorna ponteiro para o registrador contendo o valor atual da caracteristica
        std::string rxValue = characteristic->getValue();
        std::string uuid = characteristic->getUUID().toString();
        String value = rxValue.c_str();
        #ifdef DEBUGGING_BLUETOOTH
            for (int i = 0; i < uuid.length(); i++)
            {
                Serial.print(uuid[i]);
            }
        #endif
        //verifica se existe dados (tamanho maior que zero)
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print(" Value was changed to: ");
            for (int i = 0; i < rxValue.length(); i++)
            {
                Serial.print(rxValue[i]);
            }
            Serial.println();
        #endif
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string uuid = characteristic->getUUID().toString();
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            // Serial.print("READ ");
            String uuidString = characteristic->getUUID().toString().c_str();
            if (uuidString.equals(CHARACTERISTIC_UUID_AMOUNTCOLORS)){
                Serial.print("READ amountColors: ");
                Serial.println(value);
            }
            else if(uuidString.equals(CHARACTERISTIC_UUID_POSITIONS)){
                Serial.print("READ positions: ");
                Serial.println(value);
            }
            else if(uuidString.equals(CHARACTERISTIC_UUID_RED)){
                Serial.print("READ red: ");
                Serial.println(value);
            }
            else if(uuidString.equals(CHARACTERISTIC_UUID_GREEN)){
                Serial.print("READ green: ");
                Serial.println(value);
            }
            else if(uuidString.equals(CHARACTERISTIC_UUID_BLUE)){
                Serial.print("READ blue: ");
                Serial.println(value);
            }
            else{
                Serial.print("READ ");
                Serial.print(uuidString);
                Serial.print(": ");
                Serial.println(value);
            }
        }
    #endif
};



class offLedsCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        ledsOffGlobal = true;
        
            Serial.print("WRITE off Leds");

    }

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ off Leds: ");
            Serial.println(value);
        }
    #endif
};

class onLedsCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        ledsOffGlobal = false;
            Serial.print("WRITE on Leds");
       
    
    }

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ on Leds: ");
            Serial.println(value);
        }
    #endif
};

//====================callbacks for playlist config==============================================
//==========================================================================================================

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write or read the playlistName characteristic.
 * 
 * Sandsara is always playing back a playlist that contains certain paths. This characteristic is used to know or change the current playlist.
 * all the playlists are store in a text file but with .playlist extension and this contains the file name of the path to be reproduced.
 * For example, a generic playlist file will looks like this:  
 * ~~~
 * path1.txt
 * path2.txt
 * path3.txt
 * path4.txt
 * ~~~
 * 
 * **Properties:**  
 * - Write
 * - Read
 * 
 * **UUID: 9b12a048-2c6e-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * you can write to this characteristic to change to a playlist, or in the same way you can read it to know the current playlist.
 * To change a playlist just write to this characteristic the name of the playlist you want to playback (ignoring its extension, 
 * for example, if you want to change to “geometry.playlist” just write “geometry”).
 * Be sure you have sent the playlist to Sandsara before or make sure it already exists on the SD card.
 * 
 * 
 * **Range:**  
 * accepted values are 0 or any other diffeent than 0.
 * - 1 for clockwise direction.
 * - 0 for counter clockwise direction.  
 * 
 * **responses: **  
 * - "ok" means the playlist was changed successfully.
 * - "-1" means the playlist you try to change does not exist.
 * 
 * @note when you write to this characteristic you will be notified by the playlistCharacteristic_errorMsg characteristic for a response.
 * 
 * @see How to send a file? --> FilesCallbacks_receive, [How to read the response of a characteristic](@ref BLECommunication)
 */

//Declaracion de los Callbacks NewCharacteristic (5)

class playlistCallbacks_name : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        String playList = value + ".playlist";
        if (!sdExists(playList)){
            String dataBLE = "error= -1";
            playlistCharacteristic_errorMsg->setValue(dataBLE);
            playlistCharacteristic_errorMsg->notify();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("WRITE playlist not exists in playlistCallbacks_name: ");
                Serial.println(playList);
            #endif
            Bluetooth::setPlaylistName(playListGlobal);
            return;
        }
        playListGlobal = "/" + playList;
        romSetPlaylist(playListGlobal);
        orderModeGlobal = 1;
        romSetOrderMode(orderModeGlobal);
        rewindPlaylist = true;
        
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE playlist: ");
            Serial.println(playListGlobal);
        #endif
        /*
        if (!playListGlobal.equals(DEFAULTPLAYLIST)) {
            sdRemove(DEFAULTPLAYLIST);
        }
        */
        String dataBLE2 = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE2);
        playlistCharacteristic_errorMsg->notify();
    } //onWrite 

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ playlistName: ");
            Serial.println(value);
        }
    #endif
};

class playlistCallbacks_nameFiles : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
       
        
        
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE list paths: ");
            Serial.println(value);
        #endif
        
        String dataBLE2 = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE2);
        playlistCharacteristic_errorMsg->notify();
    } //onWrite 

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ list paths: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write or read the pathName characteristic.
 * 
 * all the paths have a unique name and the sandsara tracks are named starting with "Sandsara-trackNumber-", for example, "Sandsara-trackNumber-0001.thr".
 * you can change or know the current path with this characteristic.
 * 
 * **Properties:**  
 * - Write
 * - Read
 * 
 * **UUID: 9b12a534-2c6e-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * you can write to this characteristic to change the current path, or in the same way, you can read it to know it.
 * 
 * **Range:**  
 * accepted values are any name, for example "my track.thr", "path x.txt", etc.
 * 
 * **responses: **  
 * - "ok" means the path was changed successfully.
 * - "-1" means the playlist you try to change does not exist.
 * 
 * This characteristic is used to "test" a path, this is the behavior when you change a path using this characteristic.
 * 1. Sandsara is playing the Geometries playlist and it is playing the 15 position track.
 * 2. you change to "my track.txt" using this characteristic.
 * 3. sandara will play "my track.txt"
 * 4. when it finishes, sandsara will go back to the 15 position track of the Geometries playlist.
 * 
 * @note when you write to this characteristic you will be notified by the playlistCharacteristic_errorMsg characteristic for a response.
 * @warning we recommend using playlistCallbacks_pathPosition to change a path.
 * @see How to send a file? --> FilesCallbacks_receive, [How to read the response of a characteristic](@ref BLECommunication)
 */
class playlistCallbacks_pathName : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String name = rxValue.c_str();
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE pathName: ");
            Serial.println(name);
        #endif
        if (SdFiles::getType(name) < 0){

            String dataBLE = "error= -2";
            playlistCharacteristic_errorMsg->setValue(dataBLE);
            playlistCharacteristic_errorMsg->notify();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("Nombre incorrecto");
            #endif
            return;
        }
        if (!sdExists(name)){
            String dataBLE2 = "error= -3";
            playlistCharacteristic_errorMsg->setValue(dataBLE2);
            playlistCharacteristic_errorMsg->notify();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("no existe el archivo");
            #endif
            return;
        }
        changedProgram = name;
        String dataBLE3 = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE3);
        playlistCharacteristic_errorMsg->notify();
        changeProgram = true;
        changePositionList = false;
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ pathName: ");
            Serial.println(value);
        }
    #endif
};

class playlistCallbacks_addPathNamePlaylist : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String pathName = rxValue.c_str();
        while (readingSDFile){
            delay(1);
        }
        readingSDFile = true;
        Serial.println("playListNameG");
        File file = SD.open(playListNameG, FILE_WRITE);
        if (!file){
            String dataBLE = "error= -4";
            playlistCharacteristic_errorMsg->setValue(dataBLE);
            playlistCharacteristic_errorMsg->notify();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("no existe la playlist en la SD");
            #endif
            return; 
        }
        file.print(pathName + "\r\n");
        file.close();
        readingSDFile = false;
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("path name added to playlist: ");
            Serial.println(pathName);
        #endif
        String dataBLE2 = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE2);
        playlistCharacteristic_errorMsg->notify();

        Serial.println("Inicia while de la lectura del archivo de la playlist");
        File fileT = SD.open(playListNameG);
        String fileContent = "-";
    

    std::vector<int> numeros;

    while (fileT.available())
    {
            // Leer el nombre del archivo
            String nombreArchivo = fileT.readStringUntil('\n');

            Serial.println(nombreArchivo);

            // Extraer el número de la cadena eliminando los caracteres no numéricos
            String numberString = nombreArchivo.substring(nombreArchivo.lastIndexOf('-') + 1, nombreArchivo.lastIndexOf('.'));
            numberString.trim(); // Eliminar espacios en blanco adicionales

            // Convertir el número en un entero
            int number = numberString.toInt();

            // Imprimir el resultado
            Serial.println(number);

            numeros.push_back(number);
    }

    for (size_t i = 0; i < numeros.size(); i++)
    {
            Serial.println(numeros[i]);
    }

    // Generar el String con los números separados por guiones

    for (size_t i = 0; i < numeros.size(); i++)
    {
            fileContent += String(numeros[i]);
            if (i < numeros.size() - 1)
            {
                fileContent += "-";
            }
    }


    // Close the file
    fileT.close();
    Serial.println("Content playlist: ");
    Serial.println(fileContent);
    playlistCharacteristic_nameFiles->setValue(fileContent); 



    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ addPath: ");
            Serial.println(value);
        }
    #endif
};




class playlistCallbacks_pathNameNotify : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String name = rxValue.c_str();
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE pathName: ");
            Serial.println(name);
        #endif
        
        String dataBLE3 = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE3);
        playlistCharacteristic_errorMsg->notify();
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ pathName Notify: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write or read the pathPosition characteristic.
 * 
 * Sandsara always are playing a playlist and every path has an index starting at 1, you can change the current path by its index in the playlist.
 * 
 * **Properties:**  
 * - Write
 * - Read
 * 
 * **UUID: 9b12a62e-2c6e-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * you can change the current path just by writing to this characteristic the index of the desired path, or in the same way, you can read it to know the current path index.
 * 
 * 
 * **Range:**  
 * accepted values go from 1 to the number of paths in the current playlist.
 * 
 * **Responses:**  
 * - "ok" means the path was changed successfully.
 * 
 * this is the behavior when you change a path using this characteristic.
 * 1. sandsara is playing the Geometries playlist and it is playing the 15 position track.
 * 2. you change to the third position path using this characteristic.
 * 3. sandara will play the third position path.
 * 4. when it finishes, sandsara will continue with the fourth index path.
 * 
 * @note when you write to this characteristic you will be notified by the playlistCharacteristic_errorMsg characteristic for a response.
 * @warning make sure to change to a valid index
 * @see How to send a file? --> FilesCallbacks_receive, [How to read the response of a characteristic](@ref BLECommunication)
 */
class playlistCallbacks_pathPosition : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        int position = value.toInt();
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE pathPosition: ");
            Serial.println(position);
        #endif
        
//============================================================================
        int numbFiles;
        int Code = 0;
        int pListFileG;
        String fileNameB = "";

        //Code = SdFiles::getLineNumber(pListFileG, playListGlobal, fileNameB);
        Code = SdFiles::getLineNumber(position, playListGlobal, fileNameB);
        Serial.println("Archivo seleccionado con pathPosition");
        Serial.println(fileNameB);


        //===Obtener los ultimos 20 caracteres del nombre dell patron

        String last20Chars = fileNameB.substring(fileNameB.length() - 20);


        playlistCharacteristic_pathNameNotify->setValue(last20Chars);            
        playlistCharacteristic_pathNameNotify->notify();

        playlistCharacteristic_pathName->setValue(fileNameB);
//============================================================================
        //characteristic->setValue(String(position).c_str());
        //characteristic->setValue(String(position));             //Cambio para la libreria NimBLE
        
        changedPosition = position;
        Bluetooth::setPercentage(0);
        String dataBLE = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE);
        playlistCharacteristic_errorMsg->notify();
        changePositionList = true;
        changeProgram = false;
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ pathPosition: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** Deprecated.
 */
class playlistCallbacks_addPath : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String pathName = rxValue.c_str();
        while (readingSDFile){
            delay(1);
        }
        readingSDFile = true;
        File file = SD.open(playListGlobal, FILE_WRITE);
        if (!file){
            String dataBLE = "error= -4";
            playlistCharacteristic_errorMsg->setValue(dataBLE);
            playlistCharacteristic_errorMsg->notify();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("no existe el path en la SD");
            #endif
            return; 
        }
        pathName = "\r\n" + pathName;
        file.print(pathName);
        file.close();
        readingSDFile = false;
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("path name added: ");
            Serial.println(pathName);
        #endif
        String dataBLE2 = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE2);
        playlistCharacteristic_errorMsg->notify();
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ addPath: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** Deprecated.
 */
class playlistCallbacks_mode : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        int mode = value.toInt();
        if(mode < MIN_REPRODUCTION_MODE || mode > MAX_REPRODUCTION_MODE){


            //characteristic->setValue(String(orderModeGlobal).c_str());
            characteristic->setValue(String(orderModeGlobal));              //Cambio para la libreria NimBLE

            String dataBLE = "error= -5";
            playlistCharacteristic_errorMsg->setValue(dataBLE);
            playlistCharacteristic_errorMsg->notify();
            return;
        }
        orderModeGlobal = mode;
        romSetOrderMode(orderModeGlobal);
        rewindPlaylist = true;
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE playlistMode: ");
            Serial.println(mode);
        #endif
        String dataBLE2 = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE2);
        playlistCharacteristic_errorMsg->notify();
    } //onWrite

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ playlistMode: ");
            Serial.println(value);
        }
    #endif
};

//Caracteristica para recibir el contenido de la playlist actual en el formato corto

class playlistCallbacks_shortPlaylist : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        //String playList = value + ".playlist";
        String playList = value;

        Serial.println("El siguiente String es que que recibo desde la app en la carcateristica shortPlaylist");
        Serial.println(value);

        std::vector<int> vector;
        char *cadena = (char *)playList.c_str(); // Convierte el String en una cadena de caracteres
        char *token = strtok(cadena, "-");      // Divide la cadena en subcadenas separadas por guiones

        while (token != NULL)
        {
            vector.push_back(atoi(token)); // Convierte cada subcadena en un entero y lo agrega al vector
            token = strtok(NULL, "-");     // Continúa dividiendo la cadena en subcadenas
        }
        //===========================================================================================
        int shortVectorSize = vector.size();
        int matrizTerminacion[shortVectorSize];

        String terminacionFile = "";
        for (int i = 0; i < vector.size(); i++)
        {
            // Inicio de la medición del tiempo
            unsigned long tiempoInicial = millis();
            terminacionFile = obtenerTerminacionArchivo(vector[i]);
            // Fin de la medición del tiempo
            unsigned long tiempoFinal = millis();

            // Cálculo del tiempo transcurrido
             unsigned long tiempoTranscurrido = tiempoFinal - tiempoInicial;

            // Impresión del resultado
            //Serial.print("Tiempo transcurrido (ms): ");
            //Serial.println(tiempoTranscurrido);
            if(terminacionFile == ".bin")
            {
                matrizTerminacion[i] = 1;
            }
            if(terminacionFile == ".thr")
            {
                matrizTerminacion[i] = 2;
            }
            if(terminacionFile == ".txt")
            {
                matrizTerminacion[i] = 3;
            }
        }
        /*
        Serial.println("Termino de llenar el vector de extensiones de los archivos");
        for (int i = 0; i < vector.size(); i++)
        {
            Serial.print(i);
            Serial.print(" -> ");
            Serial.println(matrizTerminacion[i]);
        }
        */
        /*
        // Segmento de codigo donde hare una copia del archivo playlist.playlist hacia el archivo llamado copyPlaylist.playlist
        // Abrir el archivo de origen en modo lectura
        File srcFile = SD.open("playlist.playlist", FILE_READ);
        if (!srcFile)
        {
            Serial.println("Error al abrir el archivo de origen");
            return;
        }
        SD.remove("copyPlaylist.playlist");
        // Crear un nuevo archivo de destino en modo escritura
        File dstFile = SD.open("copyPlaylist.playlist", FILE_WRITE);
        if (!dstFile)
        {
            Serial.println("Error al crear el archivo de destino");
            srcFile.close();
            return;
        }

        // Copiar el contenido del archivo de origen al archivo de destino
        while (srcFile.available())
        {
            dstFile.write(srcFile.read());
        }

        // Cerrar los archivos
        srcFile.close();
        dstFile.close();

        Serial.println("Copia de archivo completada");
        */
        //=================================================================
        // Segmento de codigo donde se crea ell archivo pllaylist en la SD
        while (readingSDFile)
        {
            delay(1);
    }
    readingSDFile = true;
    File file, root, fileObj;
    int numberOfFiles = 0;
    root = SD.open("/");
    sdRemove("/playlist.playlist");
    file = SD.open("/playlist.playlist", FILE_WRITE);
    if (!file)
    {
        readingSDFile = false;
        Serial.println("File coudn't be created"); //file coudn't be created.
    }
    else
    {
        Serial.println("Esta parte solo ocurre si existe el archivo playlist.playlist");
//Posteriormente despues de convertir la Playlist de formato corto a numeros
///Se proceedera a escribirlos en el archivvo playlist con el nombre completo de los paths
//=================================================================
        Serial.println("Comienza la escritura del contenido en la playlist.playlist");

        String namePathNewPlaylist = "";
        String numberPathWithZeros = "";
        for (int i = 0; i < vector.size(); i++)
        {

            /*
            String terminacionFile = "";
            // Inicio de la medición del tiempo
            unsigned long tiempoInicial = millis();
            terminacionFile = obtenerTerminacionArchivo(vector[i]);
            // Fin de la medición del tiempo
            unsigned long tiempoFinal = millis();
            // Cálculo del tiempo transcurrido
            unsigned long tiempoTranscurrido = tiempoFinal - tiempoInicial;
            // Impresión del resultado
            Serial.print("Tiempo transcurrido (ms): ");
            Serial.println(tiempoTranscurrido);
            Serial.println("Terminacion File");
            Serial.println(terminacionFile);
            */
           if(matrizTerminacion[i] == 1)
           {
                terminacionFile = ".bin";
           }
           if(matrizTerminacion[i] == 2)
           {
                terminacionFile = ".thr";
           }
           if(matrizTerminacion[i] == 3)
           {
                terminacionFile = ".txt";
           }

            root = SD.open("/");
            file = SD.open("/playlist.playlist", FILE_WRITE);
            //Serial.println(vector[i]);
            if(vector[i] < 10 && vector[i] >= 0)
            {
                numberPathWithZeros = String(vector[i]);
                namePathNewPlaylist = "Sandsara-trackNumber-000" + numberPathWithZeros + terminacionFile;
                //Agrega 3 ceros
            }
            if(vector[i] < 100 && vector[i] >= 10)
            {
                numberPathWithZeros = String(vector[i]);
                namePathNewPlaylist = "Sandsara-trackNumber-00" + numberPathWithZeros + terminacionFile;
                //Agrega 2 ceros
            }
            if(vector[i] < 1000 && vector[i] >= 100)
            {
                numberPathWithZeros = String(vector[i]);
                namePathNewPlaylist = "Sandsara-trackNumber-0" + numberPathWithZeros + terminacionFile;
                //Agrega 1 cero
            }
            if(vector[i] >= 1000)
            {
                numberPathWithZeros = String(vector[i]);
                namePathNewPlaylist = "Sandsara-trackNumber-" + numberPathWithZeros + terminacionFile;
                //Agrega 0 ceros
            }

            Serial.println(namePathNewPlaylist);
            file.print(namePathNewPlaylist + "\r\n");
            file.close();
        }
        Serial.println("Hasta esta parte se supone que logro escribir todos los nombres de los patrones");
        readingSDFile = false;


    // Close the file
    //fileT.close();
    Serial.println("Content playlist: ");
    Serial.println(value);
    playlistCharacteristic_nameFiles->setValue(value); 
    



//=======================================================================================================================
        
        #ifdef DEBUGGING_BLUETOOTH
            //Serial.print("WRITE playlist content short Format: ");
            Serial.print("Se escribio desde la esp32 el siguiente formato corto: ");
            Serial.println(value);
            //Serial.println(playList);
        #endif

        Serial.println("Aqui termina el proceso de la caracteristica shortPlaylist");
        
    }
    } 

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            //Serial.print("READ playlist Content Short Format: ");
            Serial.print("La app solicito la lectura del formato corto, actualmente contiene: ");
            Serial.println(value);
        }
    #endif
};

class playlistCallbacks_selectPlaylist : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        characteristic->setValue(rxValue);
        String playList = "";
        playlistCharacteristic_pathPosition->setValue("1");             //Cambio para la libreria NimBLE
        if( value == "1")
        {
            playList = "DEFAULT.playlist";
            
            EEPROM.write(ADDRESSPLAYLISTMODE, 1);
            EEPROM.commit();
        }
        if(value == "0")
        {
            playList = "playlist.playlist";
            EEPROM.write(ADDRESSPLAYLISTMODE, 0);
            EEPROM.commit();
        }
        playListGlobal = "/" + playList;
        //============================================================================
        int numbFiles;
        int Code = 0;
        int pListFileG;
        String fileNameB = "";

        //Code = SdFiles::getLineNumber(pListFileG, playListGlobal, fileNameB);
        Code = SdFiles::getLineNumber(1, playListGlobal, fileNameB);
        Serial.println("Archivo seleccionado con pathPosition");
        Serial.println(fileNameB);


        //===Obtener los ultimos 20 caracteres del nombre dell patron

        String last20Chars = fileNameB.substring(fileNameB.length() - 20);


        playlistCharacteristic_pathNameNotify->setValue(last20Chars);            
        playlistCharacteristic_pathNameNotify->notify();

        playlistCharacteristic_pathName->setValue(fileNameB);
        //============================================================================
        //characteristic->setValue(String(position).c_str());
        
        romSetPositionList(1);
        changedPosition = 1;
        Bluetooth::setPercentage(0);
        changePositionList = true;
        changeProgram = false;
        //======================================================================
        
        if (!sdExists(playList)){
            String dataBLE = "error= -1";
            playlistCharacteristic_errorMsg->setValue(dataBLE);
            playlistCharacteristic_errorMsg->notify();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("WRITE playlist not exists playlistCallbacks_selectPlaylist : ");
                Serial.println(playList);
            #endif
            Bluetooth::setPlaylistName(playListGlobal);
            return;
        }
        
        romSetPlaylist(playListGlobal);
        orderModeGlobal = 1;
        romSetOrderMode(orderModeGlobal);
        rewindPlaylist = true;
        
        
        


        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE playlist selection: ");
            Serial.println(playListGlobal);
        #endif
       
        String dataBLE2 = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE2);
        playlistCharacteristic_errorMsg->notify();
    } //onWrite 

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ playlist Selection: ");
            Serial.println(value);
        }
    #endif
};

void Bluetooth::setPlaylistToggle(){

    int toggle = 0;
    toggle = EEPROM.read(ADDRESSPLAYLISTMODE);

if(toggle == 1)
{
    //playlistCharacteristic_name->setValue(playlistName.c_str());
    playlistCharacteristic_selectPlaylist->setValue("1");            
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET Playlist toggle 1  => Default.playlist ");
        //Serial.println(playlistName.c_str());
    #endif
}
if(toggle == 0)
{
    //playlistCharacteristic_name->setValue(playlistName.c_str());
    playlistCharacteristic_selectPlaylist->setValue("0");            
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET Playlist toggle 0  => playlist.playlist ");
        //Serial.println(playlistName.c_str());
    #endif
}
    
}


class playlistCallbacks_playNow : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        /*
//============================================================
//Genera el archivo temp y guarda la pista que se va a reproducir
//=================================================================
//Segmento de codigo donde se crea ell archivo pllaylist en la SD
String namePathNewPlaylist = "";
String numberPathWithZeros = "";
posListTemp = romGetPositionList();
Serial.println("posicion final antes de entrar a pllayyNow:");
Serial.println(posListTemp);
while (readingSDFile){
        delay(1);
    }
    readingSDFile = true;
    File file, root, fileObj;
    int numberOfFiles = 0;
    //root = SD.open("/");
    sdRemove("/temp.playlist");
    file = SD.open("/temp.playlist", FILE_WRITE);
    if (!file)
    {
        readingSDFile = false;
        Serial.println("File coudn't be created temp file"); //file coudn't be created.
    }
    else
    {
//Posteriormente despues de convertir la Playlist de formato corto a numeros
///Se proceedera a escribirlos en el archivvo playlist con el nombre completo de los paths
//=================================================================

        

        int miEntero = (value.toInt())-1;
        
            //root = SD.open("/");
            file = SD.open("/temp.playlist", FILE_WRITE);
            //Serial.println(vector[i]);
            if(miEntero < 10 && miEntero >= 0)
            {
                numberPathWithZeros = String(miEntero);
                namePathNewPlaylist = "Sandsara-trackNumber-000" + numberPathWithZeros + ".bin";
                //Agrega 3 ceros
            }
            if(miEntero < 100 && miEntero >= 10)
            {
                numberPathWithZeros = String(miEntero);
                namePathNewPlaylist = "Sandsara-trackNumber-00" + numberPathWithZeros + ".bin";
                //Agrega 2 ceros
            }
            if(miEntero < 1000 && miEntero >= 100)
            {
                numberPathWithZeros = String(miEntero);
                namePathNewPlaylist = "Sandsara-trackNumber-0" + numberPathWithZeros + ".bin";
                //Agrega 1 cero
            }
            if(miEntero < 10000 && miEntero >= 1000)
            {
                numberPathWithZeros = String(miEntero);
                namePathNewPlaylist = "Sandsara-trackNumber-" + numberPathWithZeros + ".bin";
                //Agrega 0 ceros
            }
            Serial.println("Pista a reproducir ahora : ");
            Serial.println(namePathNewPlaylist);
            file.print(namePathNewPlaylist + "\r\n");
            file.close();
        
        readingSDFile = false;
        
    

    //===Obtener los ultimos 20 caracteres del nombre dell patron

        String last20Chars = namePathNewPlaylist.substring(namePathNewPlaylist.length() - 20);


        playlistCharacteristic_pathNameNotify->setValue(last20Chars);            
        playlistCharacteristic_pathNameNotify->notify();

        playlistCharacteristic_pathName->setValue(namePathNewPlaylist);

//=================================================================

        String playList = "";
        playList = "temp.playlist";
        
        if (!sdExists(playList)){
            String dataBLE = "error= -1";
            playlistCharacteristic_errorMsg->setValue(dataBLE);
            playlistCharacteristic_errorMsg->notify();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("WRITE playlist not exists playlistCallbacks_playNow : ");
                Serial.println(playList);
            #endif
            Bluetooth::setPlaylistName(playListGlobal);
            return;
        }
        playListGlobal = "/" + playList;
        romSetPlaylist(playListGlobal);
        orderModeGlobal = 1;
        romSetOrderMode(orderModeGlobal);
        rewindPlaylist = true;
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE play now: ");
            Serial.println(playListGlobal);
        #endif
       
        String dataBLE2 = "ok";
        playlistCharacteristic_errorMsg->setValue(dataBLE2);
        playlistCharacteristic_errorMsg->notify();
    }
    */
    } //onWrite 

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            /*
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ playlist Selection: ");
            Serial.println(value);
            */
        }
    #endif
};

//================================Sending files=====================================
//==================================================================================

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write the receiveFlag characteristic.
 * 
 * receiveFlag characteristic is used to start the transfer of a file from the BLE client to this ESP32.
 * 
 * **Properties:**  
 * - Write
 * 
 * **UUID: fcbff68e-2af1-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * in order to transfer a file to this device you need to follow the next steps:
 * 1. Activate notifications of the FilesCallbacks_send characteristic associated with this class.
 * 2. write to this characteristic the name of the file you want to transfer.
 * 3. write to the FilesCallbacks_send the bytes of the file in chunks of 512 bytes.
 * 4. wait to be notified from theFilesCallbacks_send.
 * 5. repeat steps 3 and 4 until you have sent all the bytes of the file
 * 6. write to this characteristic any message to finish the transfer
 * 7. you will be notified by FileErrorMsg characteristic with "done"
 * 
 * **Range:**  
 * accepted values are a name, for example, "New Path.thr" to start the transmission of a file and any other character to finish it.  
 * 
 * @note you may be notified by fileErrorMsg characteristic with some of this massage:
 * - "ok" means the file was created and is waiting for the bytes of the file.
 * - "error= -2" means the file couldn't be created maybe because there is no sd card inserted.
 * @warning if you try to send a file named equals than other inside the sd card it will be deleted to accept this new file, so if you're
 * not sure if the name of your file name already exists in the sd card, Use the FilesCallbacks_checkFile characteristic.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class FilesCallbacks_receiveFlag : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        if (!receiveFlag){
            std::string rxData = characteristic->getValue();
            String name = rxData.c_str();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("Inicia recepcion de archivo: ");
                Serial.println(name);
            #endif
            while (readingSDFile){
                delay(1);
            }
            readingSDFile = true;
            if (sdExists(name)){
                //fileCharacteristic_errorMsg->setValue("error= -1"); //archivo ya existe
                //fileCharacteristic_errorMsg->notify();
                sdRemove(name);
                //readingSDFile = false;
                #ifdef DEBUGGING_BLUETOOTH
                    Serial.println("se borro el archivo para poder ser recibido");
                #endif
            }
            fileReceive = SD.open(name, FILE_WRITE);
            if (!fileReceive)
            {
                String dataBLE = "error= -2";
                fileCharacteristic_errorMsg->setValue(dataBLE); //file cannot be opened 
                fileCharacteristic_errorMsg->notify();
                #ifdef DEBUGGING_BLUETOOTH
                    Serial.println("no se pudo abrir el archivo"); //file cannot be opened
                #endif
                readingSDFile = false;
                return;
            }
            #ifdef DEBUGGING_BLUETOOTH
                //Serial.println("recibiendo bytes...");
            #endif
            pointerB = buffer;
            bufferSize = 0;
            receiveFlag = true;
            pauseModeGlobal = true;
            readingSDFile = false;
            String dataBLE2 = "ok";
            fileCharacteristic_errorMsg->setValue(dataBLE2);
            fileCharacteristic_errorMsg->notify();
        }
        else{
            pointerB = buffer;
            while (readingSDFile){
                delay(1);
            }
            readingSDFile = true;
            fileReceive.write(buffer, bufferSize);
            readingSDFile = false;
            #ifdef DEBUGGING_BLUETOOTH
                //Serial.print("ultimo buffer size: ");
                //Serial.println(bufferSize);
            #endif
            bufferSize = 0;
            fileReceive.close();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("termino archivo");
            #endif
            receiveFlag = false;
            pauseModeGlobal = false;
            String dataBLE3 = "done";
            fileCharacteristic_errorMsg->setValue(dataBLE3);
            fileCharacteristic_errorMsg->notify();
        }
    }
};

class FilesCallbacks_indexCustomPatterns : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxData = characteristic->getValue();
        String index = rxData.c_str();
        Serial.print("Index custom pattern written");
        preferences.begin("myApp", false);
        preferences.putInt("indexCPattern",index.toInt() );
        preferences.end();
    }
      void onRead(BLECharacteristic *characteristic)
    {
        preferences.begin("myApp", false);
        int indexCustomPattern = preferences.getInt("indexCPattern",300);
        preferences.end();
        characteristic->setValue(String(indexCustomPattern));
        Serial.print("Read from Cellphone indexcustompattern ");
        Serial.println(indexCustomPattern);
    }
};

class FilesCallbacks_customPatterns : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxData = characteristic->getValue();
        String custompatterns = rxData.c_str();
        preferences.begin("myApp", false);
        preferences.putString("customPatterns", custompatterns);
        preferences.end();
    }
    void onRead(BLECharacteristic *characteristic)
    {
        std::string rxValue = characteristic->getValue();
        String value = rxValue.c_str();
        Serial.print("Read from Cellphone Actual Mode: ");
        Serial.println(value);
    }
};
void Bluetooth::setCustomPatterns(String _version)
{
    fileCharacteristic_customPatterns->setValue(_version);
    Serial.print("Initialize customPatterns: ");
    Serial.println(String(_version).c_str());
}
void Bluetooth::setindexCustomPatterns(String _version)
{
    fileCharacteristic_indexCustomPattern->setValue(_version);
    Serial.print("Initialize indexCustomPatterns: ");
    Serial.println(String(_version).c_str());
}
class FilesCallbacks_receiveFlagPatterns : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        if (!receiveFlag){
            std::string rxData = characteristic->getValue();
            String name = rxData.c_str();
            while (readingSDFile){
                delay(1);
            }
            readingSDFile = true;
            if (sdExists(name)){
                sdRemove(name);
            }
            fileReceive = SD.open(name, FILE_WRITE);
            if (!fileReceive)
            {
                readingSDFile = false;
                return;
            }

            pointerB = buffer;
            bufferSize = 0;
            receiveFlag = true;
            pauseModeGlobal = true;
            readingSDFile = false;
            String dataBLE2 = "ok";
            fileCharacteristic_errorMsg->setValue(dataBLE2);
            fileCharacteristic_errorMsg->notify();
        }
        else{
            pointerB = buffer;
            while (readingSDFile){
                delay(1);
            }
            readingSDFile = true;
            fileReceive.write(buffer, bufferSize);
            readingSDFile = false;
            bufferSize = 0;
            fileReceive.close();
            receiveFlag = false;
            pauseModeGlobal = false;
            String dataBLE3 = "done";
            fileCharacteristic_errorMsg->setValue(dataBLE3);
            fileCharacteristic_errorMsg->notify();
        }
    }
};


/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write the FileReceive characteristic.
 * 
 * receive characteristic is used to receive the bytes of the file you want to transfer.
 * 
 * **Properties:**  
 * - Write
 * - notify
 * 
 * **UUID: fcbffa44-2af1-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * in order to transfer a file to this device you need to follow the next steps:
 * 1. Activate notifications of this characteristic.
 * 2. write to FilesCallbacks_receiveFlag the name of the file you want to transfer.
 * 3. write to this characteristic the bytes of the file in chunks of 512 bytes.
 * 4. wait to be notified from this characteristic.
 * 5. repeat steps 3 and 4 until you have sent all the bytes of the file
 * 6. write to FilesCallbacks_receiveFlag characteristic any message to finish the transfer
 * 7. you will be notified by FileErrorMsg characteristic with "done"
 * 
 * **Range:**  
 * accepted values are a string of bytes of no more than 512 bytes length  
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class FilesCallbacks_receive : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        if (receiveFlag)
        {
            std::string rxData = characteristic->getValue();
            int len = rxData.length();
            if (len > 0)
            {
                size_t lens = rxData.copy(pointerB, len, 0);
                pointerB = pointerB + lens;
                bufferSize += lens;
                if (bufferSize >= 9000){
                    pointerB = buffer;
                    while (readingSDFile){
                        delay(1);
                    }
                    readingSDFile = true;
                    fileReceive.write(buffer, bufferSize);
                    readingSDFile = false;
                    bufferSize = 0;
                }
                String dataBLE = "1";
                characteristic->setValue(dataBLE);
                characteristic->notify();
            }
        }
    }
};


class FilesCallbacks_receiveOta : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        /*
        if(timeTicToc == true)
        {
            endTime = millis();  // End time
            totalTime = endTime - startTime;
            Serial.println(totalTime);
            timeTicToc = false;

            if (bandAvgTime == false)
            {
                countTimes++;
                sumTime = sumTime + totalTime;
                if (countTimes == 4)
                {
                    bandAvgTime = true;
                    avTotalTime = sumTime / 5;
                    Serial.println("Av Time Ota");
                    Serial.println(avTotalTime);
                    if (avTotalTime > 500)
                    {
                        delay(2000);
                        esp_restart();
                    }
                }
            }
        }
        */
        

        
        std::string rxData = characteristic->getValue();
        if (!updateFlag)
        { // If it's the first packet of OTA since bootup, begin OTA
            Serial.println("BeginOTA");
            esp_ota_begin(esp_ota_get_next_update_partition(NULL), OTA_SIZE_UNKNOWN, &otaHandler);
            updateFlag = true;
        }

        if (receiveFlag)
        {
            std::string rxData = characteristic->getValue();
            // int len = rxData.length();
            if (rxData.length() > 0)
            {
                /*
                if (timeTicToc == false)
                {
                    startTime = millis();  // Record the start time
                    timeTicToc = true;
                }
                */
                
                
                esp_ota_write(otaHandler, rxData.c_str(), rxData.length());
                if (rxData.length() != 384)   //FULL_PACKET
                {
                    esp_ota_end(otaHandler);
                    Serial.println("EndOTA");
                    if (ESP_OK == esp_ota_set_boot_partition(esp_ota_get_next_update_partition(NULL)))
                    {
                        delay(2000);
                        esp_restart();
                    }
                    else
                    {
                        Serial.println("Upload Error");
                    }
                }
                String dataBLE = "1";
                characteristic->setValue(dataBLE);
                characteristic->notify();
            }
        } 
    }
};

/**
 * @brief **[BLE Command]** This class is used to create and object to be called when you write the sendFlag characteristic.
 * 
 * sendFlag characteristic is used to start the transfer of a file from the SD card to the BLE Client.
 * 
 * **Properties:**  
 * - Write
 * 
 * **UUID: fcbffdaa-2af1-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * to read a file from the SD card you have to read chunks of bytes corresponding to the desired file, to do it follow the next steps.
 * 1. write to this characteristic the name of the file you want to read.
 * 2. wait to be notified by the fileErrorMsg characteristic and read it (see below what response you can receive).
 * 3. read the FilesCallbacks_send characteristic to get the bytes of the desired file.
 * 4. repeat step 3 until you read less than 512 bytes. when you read less than 512 bytes means that you have read all the bytes of the file.
 * 
 * 
 * **Range:**  
 * accepted values are a name, for example, "Old Path.txt" to start the transmission of this file. 
 * 
 * @note you may be notified by fileErrorMsg characteristic with some of this massage:
 * - "ok" means the file is ready to be read.
 * - "error= -1" the desired file does not exist.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class FilesCallbacks_sendFlag : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        if (!sendFlag){
            std::string rxData = characteristic->getValue();
            
            String name = rxData.c_str();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("Begin sending... ");
                Serial.println(name);
            #endif
            while (readingSDFile){
                delay(1);
            }
            readingSDFile = true;
            if (!sdExists(name)){
                #ifdef DEBUGGING_BLUETOOTH
                    Serial.println("no existe el archivo");
                #endif
                String dataBLE = "error= -1";
                fileCharacteristic_errorMsg->setValue(dataBLE); //archivo no existe
                fileCharacteristic_errorMsg->notify();
                readingSDFile = false;
                return;
            }
            fileSend = SD.open(name, FILE_READ);
            if (!fileSend)
            {
                String dataBLE2 = "error= -2";
                fileCharacteristic_errorMsg->setValue(dataBLE2); //file cannot be opened
                fileCharacteristic_errorMsg->notify();
                #ifdef DEBUGGING_BLUETOOTH
                    Serial.println("error al abrir el archivo"); //file cannot be opened
                #endif
                readingSDFile = false;
                return;
            }
            readingSDFile = false;
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("Enviando archivo...");
            #endif
            /*uint8_t data[CHUNKSIZE];
            int dataSize = fileSend.read(data, CHUNKSIZE);
            readingSDFile = false;
            fileCharacteristic_send->setValue(data,dataSize);
            //fileCharacteristic_send->notify();*/

            sendFlag = true;
            //pauseModeGlobal = true;
            
            String dataBLE3 = "ok";
            fileCharacteristic_errorMsg->setValue(dataBLE3);
            fileCharacteristic_errorMsg->notify();
        }
        else{
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("sendflag puesta a false");
            #endif
            sendFlag = false;
            fileSend.close();
        }
    }
};

/**
 * @brief **[BLE Command]** This class is used to create and object to be called when you write the sendFlag characteristic.
 * 
 * sendFlag characteristic is used to start the transfer of a file from the SD card to the BLE Client.
 * 
 * **Properties:**  
 * - Write
 * - notify
 * 
 * **UUID: fcbffe72-2af1-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * to read a file from the SD card you have to read chunks of bytes corresponding to the desired file, to do it follow the next steps.
 * 1. write to FilesCallbacks_sendFlag characteristic the name of the file you want to read.
 * 2. wait to be notified by the fileErrorMsg characteristic and read it.
 * 3. read this characteristic to get the bytes of the desired file.
 * 4. repeat step 3 until you read less than 512 bytes. when you read less than 512 bytes means that you have read all the bytes of the file.
 * 
 * 
 * **Range:**  
 * accepted values are a name, for example, "Old Path.txt" to start the transmission of this file.  
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class FilesCallbacks_send : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *characteristic)
    {
        if (sendFlag)
        {
            uint8_t data[CHUNKSIZE];
            while (readingSDFile){
                delay(1);
            }
            readingSDFile = true;
            int dataSize = fileSend.read(data, CHUNKSIZE);
            readingSDFile = false;
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("data sent: ");
                Serial.print(dataSize);
                Serial.println(" bytes");
            #endif
            if (dataSize == 0){
                String dataBLE = "";
                characteristic->setValue(dataBLE);
            }else {
                characteristic->setValue(data,dataSize);
            }
            if(dataSize < CHUNKSIZE){
                fileSend.close();
                sendFlag = false;
                #ifdef DEBUGGING_BLUETOOTH
                    Serial.println("done");
                #endif
            }
        }
    }

};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write to the checkFile characteristic.
 * 
 * checkFile characteristic is used to check if a file exists in the sd card.
 * 
 * **Properties:**  
 * - Write
 * 
 * **UUID: fcbffb52-2af1-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * if you want to check if a file exists in the SD card just write to this characteristic the file name you want to check and wait to be notified by fileErroMsg (fcbffce2-2af1-11eb-adc1-0242ac120002).
 * to know if the file exists read fileErroMsg characteristic and if the response is 1 the file exists and if it is 0 file doesn’t exist.
 * 
 * **Range:**  
 * accepted values are a name, for example, "Old Path.txt", "dogs.txt", "geometries.playlist" and so on.
 * 
 * **Response:**  
 * you can receive one of these responses after you write to this characteristic.
 * - "0" means the file does NOT exist
 * - "1" means the file exists*.  
 * 
 * 
 * @note all the responses have to be read from the fileErroMsg characteristic after being notified by it.
 * @warning * if the file name starts with "Sandsara-trackNumber-" you will receive "1" even if your exact filename
 * Does not exist in the sd card but it exists with another extension, for example, if in the sd card exists the file called "Sandsara-trackNumber-011.txt"
 * and you ask for "Sandsara-trackNumber-011.bin" you will receive "1" as a response even if "Sandsara-trackNumber-011.bin" does no exists,
 * but "Sandsara-trackNumber-011.txt" does.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class FilesCallbacks_checkFile : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxData = characteristic->getValue();
        String filename = rxData.c_str();

        if (!sdExists(filename)){
            if (filename.indexOf(FILENAME_BASE) == 0){
                int indexDot = filename.indexOf(".");
                String nameBase = filename.substring(0,indexDot);
                if (indexDot < 0){
                    #ifdef DEBUGGING_BLUETOOTH
                        Serial.print("the file does NOT exist: ");
                        Serial.println(filename);
                    #endif
                    String dataBLE = "0";
                    fileCharacteristic_errorMsg->setValue(dataBLE);
                    fileCharacteristic_errorMsg->notify();
                    return;
                }
                if (sdExists(nameBase + ".bin")){
                    #ifdef DEBUGGING_BLUETOOTH
                        Serial.print("the file exists but with .bin extension: ");
                        Serial.println(filename);
                    #endif
                    String dataBLE2 = "1";
                    fileCharacteristic_errorMsg->setValue(dataBLE2);
                    fileCharacteristic_errorMsg->notify();
                    return;
                }
                if (sdExists(nameBase + ".txt")){
                    #ifdef DEBUGGING_BLUETOOTH
                        Serial.print("the file exists but with .txt extension: ");
                        Serial.println(filename);
                    #endif
                    String dataBLE3 = "1";
                    fileCharacteristic_errorMsg->setValue(dataBLE3);
                    fileCharacteristic_errorMsg->notify();
                    return;
                }
                if (sdExists(nameBase + ".thr")){
                    #ifdef DEBUGGING_BLUETOOTH
                        Serial.print("the file exists but with .thr extension: ");
                        Serial.println(filename);
                    #endif
                    String dataBLE4 = "1";
                    fileCharacteristic_errorMsg->setValue(dataBLE4);
                    fileCharacteristic_errorMsg->notify();
                    return;
                }
            }
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("the file does NOT exist: ");
                Serial.println(filename);
            #endif
            String dataBLE5 = "0";
            fileCharacteristic_errorMsg->setValue(dataBLE5);
            fileCharacteristic_errorMsg->notify();
            return;
        }
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("the file exists: ");
            Serial.println(filename);
        #endif
        String dataBLE6 = "1";
        fileCharacteristic_errorMsg->setValue(dataBLE6);
        fileCharacteristic_errorMsg->notify();
    }

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ checkFile: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write to the deleteFile characteristic.
 * 
 * deleteFile characteristic is used to delete a file in the sd card.
 * 
 * **Properties:**  
 * - Write
 * 
 * **UUID: fcbffc24-2af1-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * if you want to delete a file in the SD card just write to this characteristic the file name you want to delete.
 * to verify if the file was deleted read the fileErrorMsg characteristic for the response.
 * 
 * **Range:**  
 * accepted values are a name, for example, "Old Path.txt", "dogs.txt", "geometries.playlist" and so on.
 * 
 * **Response:**  
 * you can receive one of these responses after you write to this characteristic.
 * - "0" means the file does NOT exist
 * - "1" means the file was deleted.  
 * 
 * 
 * @note all the responses have to be read from the fileErroMsg characteristic after being notified by it.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class FilesCallbacks_deleteFile : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxData = characteristic->getValue();
        String filename = rxData.c_str();

        if (!sdRemove(filename)){
            #ifdef DEBUGGING_BLUETOOTH
                Serial.print("no se pudo eliminar el archivo : ");
                Serial.println(filename);
            #endif
            String dataBLE = "0";
            fileCharacteristic_errorMsg->setValue(dataBLE);
            fileCharacteristic_errorMsg->notify();
            return;
        }
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("se elimino el archivo : ");
            Serial.println(filename);
        #endif
        String dataBLE2 = "1";
        fileCharacteristic_errorMsg->setValue(dataBLE2);
        fileCharacteristic_errorMsg->notify();
    }
    
    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ deleteFile: ");
            Serial.println(value);
        }
    #endif
};

//===========================Callbacks for general config==========================
//=================================================================================

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write to the generalName characteristic.
 * 
 * Sandsara has a Bluetooth name so you can identify it and you can choose your Sandsara's name.
 * 
 * **Properties:**  
 * - Write
 * - Read
 * 
 * **UUID: 7b204548-30c3-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * if you want to change the Bluetooth name of your sandsara just write to this characteristic the desired name. And if
 * you want to know the current name just read the characteristic.
 * 
 * **Range:**  
 * accepted values are a name, for example, "Raul", "living Room", "Maria", etc.
 * The maximum length of the name should be less than 30 characters.
 * 
 * **Response:**  
 * you can receive one of these responses after you write to this characteristic.
 * - "ok" means the name was set successfully.
 * - "-1" the name length is out of range.  
 * 
 * 
 * @note all the responses have to be read from the generalErrorMsg characteristic after being notified by it.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class generalCallbacks_name : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxData = characteristic->getValue();
        String bluetoothName = rxData.c_str();
        if (bluetoothName.length() > MAX_CHARACTERS_BTNAME){
            String dataBLE = "error=  -1";
            generalCharacteristic_errorMsg->setValue(dataBLE);
            generalCharacteristic_errorMsg->notify();
            #ifdef DEBUGGING_BLUETOOTH
                Serial.println("Numero maximo de characteres execedido");
            #endif
            return;
        }

        bluetoothNameGlobal ="Sand " + bluetoothName;
        characteristic->setValue(bluetoothNameGlobal);
        romSetBluetoothName(bluetoothNameGlobal);
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE sandsaraName: ");
            Serial.println(bluetoothName);
        #endif
        String dataBLE2 = "ok";
        generalCharacteristic_errorMsg->setValue(dataBLE2);
        generalCharacteristic_errorMsg->notify();
    }

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ SandsaraName: ");
            Serial.println(value);
        }
    #endif
};



// Correct way to make it. 
class generalCallbacks_timePaths : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxData = characteristic->getValue();
         bluetoothTimePaths = rxData.c_str();
        characteristic->setValue(bluetoothTimePaths);
        romSetBluetoothTimePaths(bluetoothTimePaths);
        
 
    }

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ timePaths: ");
            Serial.println(value);
        }
    #endif
};

class generalCallbacks_timeOffOn : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxData = characteristic->getValue();
         bluetoothTimeOffOn = rxData.c_str();
        characteristic->setValue(bluetoothTimeOffOn);
        romSetBluetoothTimeOffOn(bluetoothTimeOffOn);
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE timeOffOn: ");
            Serial.println(bluetoothTimeOffOn);
        #endif
        String dataBLE2 = "ok";
        generalCharacteristic_errorMsg->setValue(dataBLE2);
        generalCharacteristic_errorMsg->notify();
    }

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ timeOffOn: ");
            Serial.println(value);
        }
    #endif
};

class generalCallbacks_appTime : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxData = characteristic->getValue();
        String bluetoothAppTime = rxData.c_str();
        characteristic->setValue(rxData);
        int NumeroHoras, NumeroMinutos; // Variables donde se almacenarán los números enteros
        
        String strNumeroHoras = bluetoothAppTime.substring(0, 2);
        String strNumeroMinutos = bluetoothAppTime.substring(2, 4);
        NumeroHoras = strNumeroHoras.toInt();
        NumeroMinutos = strNumeroMinutos.toInt();
        Serial.println("//////Horas dentro de generalCallbacks_appTime:");
        Serial.println(NumeroHoras);
        Serial.println("//////Horas dentro de generalCallbacks_appTime:");
        Serial.println(NumeroMinutos);

#ifdef rtcInterno
        rtcBLE.setTime(00, NumeroMinutos, NumeroHoras, 17, 1, 2021); // 17th Jan 2021 00:00:30
#endif

#ifdef rtcExterno
        Wire.begin(I2C_SDA, I2C_SCL);
        rtcBle.begin();
        DateTime startTimeRTC = DateTime(2021, 1, 17, NumeroHoras, NumeroMinutos, 0);
        rtcBle.adjust(startTimeRTC);
        Serial.print("Hour to external rtc updated, current hour: ");
        DateTime now = rtcBle.now();
        int actualHour = now.hour();
        int actualMinute = now.minute();
        Serial.print("rtc Externo: ");
        Serial.print(actualHour);
        Serial.print(" ");
        Serial.println(actualMinute);
#endif
        //romSetBluetoothTimeOffOn(bluetoothAppTime);
    
     
    }


        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ App Time: ");
            Serial.println(value);
        }

};

class generalCallbacks_tracksForCalib : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxData = characteristic->getValue();
        String bluetoothTracksForCalib = rxData.c_str();
        characteristic->setValue(rxData);
        int tracksForCalib = bluetoothTracksForCalib.toInt();
        
        EEPROM.write(ADDRESSTRACKSCALIB, tracksForCalib);
        EEPROM.commit();
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE tracks for calibration: ");
            Serial.println(bluetoothTracksForCalib);
        #endif
        String dataBLE2 = "ok";
        generalCharacteristic_errorMsg->setValue(dataBLE2);
        generalCharacteristic_errorMsg->notify();
    }

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ tracks for calibration: ");
            Serial.println(value);
        }
    #endif
};

class generalCallbacks_battery : public BLECharacteristicCallbacks
{

    void onRead(BLECharacteristic *characteristic)
    {
        float voltajeReferencia = 3.3;
        float voltajeBateriaReal = 14.0;
        float factorConversion = voltajeBateriaReal / voltajeReferencia;
        int valueADC = 0;
        valueADC = analogRead(ADC2_PIN);
        bool valuePowerAdapter;
        valuePowerAdapter = digitalRead(ADC1_PIN);

        float voltajeBateria = (valueADC / 4095.0) * voltajeReferencia * factorConversion;
        String voltajeBateriaStr = String(voltajeBateria, 2);
        // BluetoothSand.setBattery(voltajeBateriaStr + "," + String(valuePowerAdapter));
        String valueToSend = voltajeBateriaStr + "," + String(valuePowerAdapter);
        generalCharacteristic_battery->setValue(valueToSend);
        //std::string rxValue = characteristic->getValue();
        //String value = rxValue.c_str();

        Serial.print("Read battery value from BLE: ");
        Serial.println(valueToSend);
    }
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write to the generalPause characteristic.
 * 
 * Sandsara has 3 movement modes, play, pause, and sleep modes. You can enter in Pause mode using this characteristic.
 * Pause mode means that the mechanism will stop in the current state and the led strip will continue to be working.
 * 
 * **Properties:**  
 * - Write
 * 
 * **UUID: 7b20473c-30c3-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * if you want to pause your sandsara just write to this characteristic any character.
 * 
 * **Range:**  
 * whatever character will be understood as if you want to pause Sandsara.
 * 
 * **Response:**  
 * you can receive one of these responses after you write to this characteristic.
 * - "ok" means Sandsara is in pause mode.
 * 
 * @note all the responses have to be read from the generalErrorMsg characteristic after being notified by it.
 * @see how to know the status of sandsara? --> generalCallbacks_status, 
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class generalCallbacks_pause : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        pauseModeGlobal = true;
        Bluetooth::setStatus(MODE_PAUSE);
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE pause");
        #endif
        String dataBLE = "ok";
        generalCharacteristic_errorMsg->setValue(dataBLE);
        generalCharacteristic_errorMsg->notify();
    }

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ pause: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write to the generalPlay characteristic.
 * 
 * Sandsara has 3 movement modes, play, pause, and sleep modes. You can enter in Play mode using this characteristic.
 * Play mode means that the mechanism will start moving, if Sandsara is in pause or sleep mode you can use this characteristic to resume the movement.
 * if you use this characteristic when Sandsara is already in play mode, nothing will happen.
 * 
 * **Properties:**  
 * - Write
 * 
 * **UUID: 7b20480e-30c3-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * if you want to resume your sandsara just write to this characteristic any character.
 * 
 * **Range:**  
 * whatever character will be understood as if you want to pause Sandsara.
 * 
 * **Response:**  
 * you can receive one of these responses after you write to this characteristic.
 * - "ok" means Sandsara is in pause mode.
 * 
 * @note all the responses have to be read from the generalErrorMsg characteristic after being notified by it.
 * @see how to know the status of sandsara? --> generalCallbacks_status.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class generalCallbacks_play : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        pauseModeGlobal = false;
        suspensionModeGlobal = false;
        Bluetooth::setStatus(MODE_PLAY);
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE play");
        #endif
        String dataBLE = "ok";
        generalCharacteristic_errorMsg->setValue(dataBLE);
        generalCharacteristic_errorMsg->notify();
    }

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ play: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write to the generalSleep characteristic.
 * 
 * Sandsara has 3 movement modes, play, pause, and sleep modes. You can enter in Sleep mode using this characteristic.
 * Sleep mode means that Sandsara will stop and will go to center or edge position and the led strip will turn off.
 * this means that the current path will be rewind and when you resume sandsara the path will start from the beginning.  
 * 
 * **Properties:**  
 * - Write
 * 
 * **UUID: 7b204a3e-30c3-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * if you want to enter Sandsara in sleep mode just write to this characteristic any character.
 * 
 * **Range:**  
 * whatever character will be understood as if you want to sleep Sandsara.
 * 
 * **Response:**  
 * you can receive one of these responses after you write to this characteristic.
 * - "ok" means Sandsara is in sleep mode.
 * 
 * @note all the responses have to be read from the generalErrorMsg characteristic after being notified by it.
 * @see how to know the status of sandsara? --> generalCallbacks_status.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class generalCallbacks_Sleep : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        suspensionModeGlobal = true;
        pauseModeGlobal = false;
        Bluetooth::setStatus(MODE_PAUSE);
        #ifdef DEBUGGING_BLUETOOTH
            Serial.println("WRITE sleep");
        #endif
        String dataBLE = "ok";
        generalCharacteristic_errorMsg->setValue(dataBLE);
        generalCharacteristic_errorMsg->notify();
    }
    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.println("READ Sleep: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write to the generalSpeed characteristic.
 * 
 * the sphere speed can be changed with this characteristic. the sphere tries to move in a constant speed most of the time, the speed can goes down or up
 * because we implement an algorithm to accelerate or decelerate to prevent abrupt motor movements and the Scara mechanism also tends to move slower in the center.
 * 
 * **Properties:**  
 * - Write
 * - Read
 * 
 * **UUID: 7b204b10-30c3-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * if you want to change the speed of sandsara you can choose you're desired speed between 10 posible values just writing to this characteristic a number
 * between 1 and 10 where 1 is the slowest speed and 10 the highest.
 * 
 * **Range:**  
 * accepted values goes from 1 to 10.
 * 
 * **Response:**  
 * you can receive one of these responses after you write to this characteristic.
 * - "ok" the desired speed was set.
 * - "error= -2" the speed is out of range.
 * 
 * @note all the responses have to be read from the generalErrorMsg characteristic after being notified by it.
 * @see how to know the status of sandsara? --> generalCallbacks_status.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class generalCallbacks_speed : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        std::string rxData = characteristic->getValue();
        String value = rxData.c_str();
        int speed = value.toInt();
        #ifdef DEBUGGING_BLUETOOTH
            Serial.print("WRITE speedball : ");
            Serial.println(speed);
        #endif

  



        //characteristic->setValue(String(speed).c_str());
        characteristic->setValue(String(speed));                //Cambio para libreria NimBLE

        //remap the speed acoording to the range of the ball speed
        speed = map(speed,MIN_SLIDER_MSPEED,MAX_SLIDER_MSPEED,MIN_SPEED_MOTOR,MAX_SPEED_MOTOR);
        if (speed > MAX_SPEED_MOTOR || speed < MIN_SPEED_MOTOR){
            
            return;
        }
        // Sandsara.setSpeed(speed);
        // romSetSpeedMotor(speed);
        // speedChangedMain = true;
  
    }

    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic)
        {
            std::string rxValue = characteristic->getValue();
            String value = rxValue.c_str();
            Serial.print("READ speed: ");
            Serial.println(value);
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write to the generalRestart characteristic.
 * 
 * Sandsara can be restarted using this characteristic. The behavior is the same as if you disconnect Sandsara and then connect it again.
 * 
 * **Properties:**  
 * - Write
 * 
 * **UUID: 7b204bce-30c3-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * if you want to restart Sandsara just write to this characteristic any character.
 * 
 * **Range:**  
 * any character.
 * 
 * **Response:**  
 * you can receive one of these responses after you write to this characteristic.
 * - "ok" means Sandsara will restart.
 * 
 * @note all the responses have to be read from the generalErrorMsg characteristic after being notified by it.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class generalCallbacks_restart : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        String dataBLE = "ok"; 
        generalCharacteristic_errorMsg->setValue(dataBLE);
        generalCharacteristic_errorMsg->notify();
        rebootWithMessage("Reiniciando por medio de callback restart");
    }
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write to the generalFactoryReset characteristic.
 * 
 * Sandara has default values for every parameter like ball speed, ledspeed, name, etc and you can set these values with this characteristic.
 * 
 * **Properties:**  
 * - Write
 * 
 * **UUID: 7b204bce-30c3-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * if you want to make a factory reset to Sandsara just write to this characteristic any character.
 * 
 * **Range:**  
 * any character.
 * 
 * **Response:**  
 * you can receive one of these responses after you write to this characteristic.
 * - "ok" sandsara has reset its values as default.
 * 
 * @note all the reponses has to be read from the generalErrorMsg characteristic after being notified by it.
 * @warning all the parameters of sandara will be set as default if you use this characteristic moreover the sensors hall will be calibrated again.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class generalCallbacks_factoryReset : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *characteristic)
    {
        for (int i = 0; i < 512; i++){
            EEPROM.write(i, -1);
        }
        EEPROM.commit();
        String dataBLE = "ok";
        generalCharacteristic_errorMsg->setValue(dataBLE);
        generalCharacteristic_errorMsg->notify();
        delay(1000);
        rebootWithMessage("Se hiso reset de fabrica, Reiniciando...");
    }
};

/**
 * @brief **[BLE Command]** This class is used to create an object to be called when you write to the generalStatus characteristic.
 * 
 * Sandsara has 5 status, play, pause, sleep, busy, and calibration status. you can see a little description of each status below.
 * | status | name        | description                      |
 * |--------|-------------|----------------------------------|
 * | 1      | calibrating | Sandsara is calibrating          |
 * | 2      | running     | Sandsara is playing a path       |
 * | 3      | pause       | Sandsara is in pause mode        |
 * | 4      | sleep       | Sandsara is in sleep mode        |
 * | 5      | busy        | Sandsara is working on something |
 * 
 * **Properties:**  
 * - Read
 * - notify
 * 
 * **UUID: 7b204660-30c3-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * you can know the status of Sandsara reading this characteristic and use it as you need. you even can activate the notifications and
 * you will be notified every time the status changes.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class generalCallbacks_status: public BLECharacteristicCallbacks {
    #ifdef DEBUGGING_BLUETOOTH
        void onRead(BLECharacteristic *characteristic){
            Serial.print("READ status: ");
            //Serial.println(characteristic->getValue().c_str());
            Serial.println(characteristic->getValue());             //Cambio para la libreria NimBLE
        
        }
    #endif
};

/**
 * @brief **[BLE Command]** This class is used to create and object to be called when you write to the generalCalibraring characteristic.
 * 
 * Sandara can perform a calibration of the robot (finding the robot's home position) every time a path finishes in the center position.
 * to activate or deactivate this feature you need to use this characteristic
 * 
 * **Properties:**  
 * - Write
 * - Read
 * 
 * **UUID: 7b204f84-30c3-11eb-adc1-0242ac120002**
 * 
 * **Usage:**  
 * you can activate or deactivate the calibration between programs writing to this characteristic and in the same way you
 * can know if it is activated or deactivated.
 * 
 * **Range:**  
 * 1 to activate calibration between paths.
 * 0 to deactivate calibration between paths.
 * 
 * @note the calibrations need to make a little movement of the ball so if you activate the calibration you will see that the ball makes a little path in the center of sandara every time a path ends in the center.
 * @see [How to read the response of a characteristic](@ref BLECommunication)
 */
class generalCallbacks_calibrating: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic){
        std::string rxData = characteristic->getValue();
        String value = rxData.c_str();
        int calibrationStatus = value.toInt();

        if (calibrationStatus > 0){
            romSetIntermediateCalibration(true);
            intermediateCalibration = true;
        }
        else {
            romSetIntermediateCalibration(false);
            intermediateCalibration = false;
        }
    }
};

Bluetooth::Bluetooth(){

}

int Bluetooth::init(String name){
    Serial.println(name);
    BLEDevice::init(name.c_str());
    BLEServer *pServer = BLEDevice::createServer();    
    //BLEService *pServiceLedConfig = pServer->createService(BLEUUID(SERVICE_UUID1), 40);
    BLEService *pServiceLedConfig = pServer->createService(BLEUUID(SERVICE_UUID1));

    BLEService *pServiceTesting = pServer->createService(BLEUUID(SERVICE_UUID2));
    
    //BLEService *pServicePlaylist = pServer->createService(BLEUUID(SERVICE_UUID4), 30);
    BLEService *pServicePlaylist = pServer->createService(BLEUUID(SERVICE_UUID4));


    //BLEService *pServiceGeneralConfig = pServer->createService(BLEUUID(SERVICE_UUID5), 30);
    BLEService *pServiceGeneralConfig = pServer->createService(BLEUUID(SERVICE_UUID5));

    //BLEService *pServiceFile = pServer->createService(BLEUUID(SERVICE_UUID6), 30);
    BLEService *pServiceFile = pServer->createService(BLEUUID(SERVICE_UUID6));


    pServer->setCallbacks(new bleServerCallback());
    //====Characteristics for LEDs configuration====
    
    ledCharacteristic_indexPalette = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_SELECTEDPALETTE,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);
    ledCharacteristic_speed = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_LEDSPEED,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);
    ledCharacteristic_cycleMode = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_CYCLEMODE,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);
    ledCharacteristic_direction = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_DIRECTION,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);
    ledCharacteristic_brightness = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_BRIGHTNESS,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);
    ledCharacteristic_amountColors = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_AMOUNTCOLORS,
            NIMBLE_PROPERTY::READ);
    ledCharacteristic_positions = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_POSITIONS,
            NIMBLE_PROPERTY::READ);
    ledCharacteristic_red = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_RED,
            NIMBLE_PROPERTY::READ);
    ledCharacteristic_green = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_GREEN,
            NIMBLE_PROPERTY::READ);
    ledCharacteristic_blue = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_BLUE,
            NIMBLE_PROPERTY::READ);
    ledCharacteristic_update = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_UPDATECPALETTE,
        NIMBLE_PROPERTY::WRITE);
    ledCharacteristic_errorMsg = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_UUID_MSGERRORLEDS,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY);
    //--->ledCharacteristic_errorMsg->addDescriptor(new BLE2902());
    ledCharacteristic_offLeds = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_OFFLEDS,
        NIMBLE_PROPERTY::WRITE);

    ledCharacteristic_onLeds = pServiceLedConfig->createCharacteristic(
        CHARACTERISTIC_ONLEDS,
        NIMBLE_PROPERTY::WRITE);
    
    ledCharacteristic_speed->setCallbacks(new speedLedCallbacks());
    ledCharacteristic_cycleMode->setCallbacks(new cycleModeCallbacks());
    ledCharacteristic_direction->setCallbacks(new directionCallbacks());
    ledCharacteristic_brightness->setCallbacks(new setBrightnessCallbacks());
    ledCharacteristic_amountColors->setCallbacks(new genericCallbacks());
    ledCharacteristic_positions->setCallbacks(new genericCallbacks);
    ledCharacteristic_red->setCallbacks(new genericCallbacks);
    ledCharacteristic_green->setCallbacks(new genericCallbacks);
    ledCharacteristic_blue->setCallbacks(new genericCallbacks);
    ledCharacteristic_update->setCallbacks(new CallbacksToUpdate());
    ledCharacteristic_indexPalette->setCallbacks(new selectedPaletteCallbacks());
    ledCharacteristic_offLeds->setCallbacks(new offLedsCallbacks());
    ledCharacteristic_onLeds->setCallbacks(new onLedsCallbacks());
    setLedSpeed(periodLedsGlobal);
    if(romGetIncrementIndexPallete()){
        String dataBLE = "1";
        ledCharacteristic_cycleMode->setValue(dataBLE);}
    else{
        String dataBLE2 = "0";
        ledCharacteristic_cycleMode->setValue(dataBLE2);}
    if(romGetLedsDirection()){
        String dataBLE3 = "1";
        ledCharacteristic_direction->setValue(dataBLE3);}
    else{
        String dataBLE4 = "0";
        ledCharacteristic_direction->setValue(dataBLE4);}

    
    //ledCharacteristic_indexPalette->setValue(String(romGetPallete()).c_str());
    ledCharacteristic_indexPalette->setValue(String(romGetPallete()));      //Cambio para libreria NimBLE


    setRed();
    setGreen();
    setBlue();
    setPositions();
    setAmountOfColors();

    //====Characteristics for playlist configuration====
    playlistCharacteristic_name = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_NAME,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    playlistCharacteristic_pathAmount = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_PATHAMOUNT,
        NIMBLE_PROPERTY::READ);
    
    playlistCharacteristic_pathName = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_PATHNAME,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);
    playlistCharacteristic_addPathNamePlaylist = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_ADDPATHNAMEPLAYLIST,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    playlistCharacteristic_pathNameNotify = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_PATHNAMENOTIFY,
        NIMBLE_PROPERTY::READ |
              NIMBLE_PROPERTY::NOTIFY);
              //NIMBLE_PROPERTY::WRITE);

    playlistCharacteristic_pathPosition = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_PATHPOSITION,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    playlistCharacteristic_addPath = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_ADDPATH,
            NIMBLE_PROPERTY::WRITE);

    playlistCharacteristic_mode = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_MODE,
            NIMBLE_PROPERTY::WRITE);

    playlistCharacteristic_progress = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_PATHPROGRESS,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY);
    //--->playlistCharacteristic_progress->addDescriptor(new BLE2902());
    
    playlistCharacteristic_errorMsg = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_ERRORMSG,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY);
    //--->playlistCharacteristic_errorMsg->addDescriptor(new BLE2902());

    playlistCharacteristic_nameFiles = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_NAMEFILES,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    playlistCharacteristic_shortPlaylist = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_SHORT_PLAYLIST,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    playlistCharacteristic_selectPlaylist = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_SELECT_PLAYLIST,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);
    playlistCharacteristic_playNow = pServicePlaylist->createCharacteristic(
        PLAYLIST_UUID_PLAY_NOW,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    playlistCharacteristic_name->setCallbacks(new playlistCallbacks_name());
    playlistCharacteristic_pathName->setCallbacks(new playlistCallbacks_pathName());
    playlistCharacteristic_addPathNamePlaylist->setCallbacks(new playlistCallbacks_addPathNamePlaylist());
    playlistCharacteristic_pathNameNotify->setCallbacks(new playlistCallbacks_pathNameNotify());
    playlistCharacteristic_pathPosition->setCallbacks(new playlistCallbacks_pathPosition());
    playlistCharacteristic_addPath->setCallbacks(new playlistCallbacks_addPath());
    playlistCharacteristic_mode->setCallbacks(new playlistCallbacks_mode());
    playlistCharacteristic_nameFiles->setCallbacks(new playlistCallbacks_nameFiles());
    playlistCharacteristic_shortPlaylist->setCallbacks(new playlistCallbacks_shortPlaylist());
    playlistCharacteristic_selectPlaylist->setCallbacks(new playlistCallbacks_selectPlaylist());
    playlistCharacteristic_playNow->setCallbacks(new playlistCallbacks_playNow());

    //=============================================================================
    //=============================Characteristics for Testing=====================

    testingCharacteristic_name = pServiceTesting->createCharacteristic(
        GENERAL_UUID_TEST,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    //--->generalCharacteristic_errorMsg->addDescriptor(new BLE2902());

    testingCharacteristic_name->setCallbacks(new testingCallbacks_desfase());

    //=============================================================================
    //=============================================================================

    //Declaracion de las propiedades de las caracteristicas NewCharacteristic (2)
    generalCharacteristic_battery = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_BATTERY,
        NIMBLE_PROPERTY::READ);

    generalCharacteristic_model = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_MODEL,
        NIMBLE_PROPERTY::READ);

    //====Characteristics for General configuration====
    generalCharacteristic_version = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_VERSION,
        NIMBLE_PROPERTY::READ);

    generalCharacteristic_name = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_NAME,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    generalCharacteristic_status= pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_STATUS,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY);
    //--->generalCharacteristic_status->addDescriptor(new BLE2902());

    generalCharacteristic_pause = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_PAUSE,
            NIMBLE_PROPERTY::WRITE);

    generalCharacteristic_play = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_PLAY,
            NIMBLE_PROPERTY::WRITE);

    generalCharacteristic_sleep = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_SLEEP,
            NIMBLE_PROPERTY::WRITE);

    generalCharacteristic_speed = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_SPEED,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    generalCharacteristic_restart = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_RESTART,
            NIMBLE_PROPERTY::WRITE);

    generalCharacteristic_factoryReset = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_FACTORYRESET,
            NIMBLE_PROPERTY::WRITE);
    
    generalCharacteristic_calibration = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_CALIBRATION,
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::READ);

    generalCharacteristic_errorMsg = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_ERRORMSG,
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY);

    generalCharacteristic_timePaths = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_TIMEPATHS,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    generalCharacteristic_timeOffOn = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_TIMEOFFON,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    generalCharacteristic_appTime = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_APP_TIME,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);

    generalCharacteristic_SimpleVersion = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_SIMPLE_VERSION,
        NIMBLE_PROPERTY::READ);

     generalCharacteristic_TracksForCalib = pServiceGeneralConfig->createCharacteristic(
        GENERAL_UUID_PATHS_FOR_CALIB,
        NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::WRITE);
    //--->generalCharacteristic_errorMsg->addDescriptor(new BLE2902());

    //======Declaracion de los callbacks de las caracteriticas de chess NewCharacteristic (4)====

    generalCharacteristic_name->setCallbacks(new generalCallbacks_name());
    generalCharacteristic_pause->setCallbacks(new generalCallbacks_pause());
    generalCharacteristic_play->setCallbacks(new generalCallbacks_play());
    generalCharacteristic_sleep->setCallbacks(new generalCallbacks_Sleep());
    generalCharacteristic_speed->setCallbacks(new generalCallbacks_speed());
    generalCharacteristic_restart->setCallbacks(new generalCallbacks_restart());
    generalCharacteristic_factoryReset->setCallbacks(new generalCallbacks_factoryReset());
    generalCharacteristic_status->setCallbacks(new generalCallbacks_status());
    generalCharacteristic_calibration->setCallbacks(new generalCallbacks_calibrating());

    generalCharacteristic_timePaths->setCallbacks(new generalCallbacks_timePaths());
    generalCharacteristic_timeOffOn->setCallbacks(new generalCallbacks_timeOffOn());
    generalCharacteristic_appTime->setCallbacks(new generalCallbacks_appTime());

    generalCharacteristic_TracksForCalib->setCallbacks(new generalCallbacks_tracksForCalib());

    generalCharacteristic_battery->setCallbacks(new generalCallbacks_battery());
    //====Characteristics for File configuration====
    fileCharacteristic_receiveFlag = pServiceFile->createCharacteristic(
        FILE_UUID_RECEIVEFLAG,
            NIMBLE_PROPERTY::WRITE);
    fileCharacteristic_receiveFlagPatterns = pServiceFile->createCharacteristic(
        FILE_UUID_RECEIVEFLAGPATTERNS,
            NIMBLE_PROPERTY::WRITE);
           
    fileCharacteristic_indexCustomPattern = pServiceFile->createCharacteristic(
        INDEXCUSTOMPATTERN,
            NIMBLE_PROPERTY::WRITE|
            NIMBLE_PROPERTY::NOTIFY|
            NIMBLE_PROPERTY::READ);

    fileCharacteristic_customPatterns = pServiceFile->createCharacteristic(
        CUSTOMPATTERNS,
            NIMBLE_PROPERTY::WRITE|
            NIMBLE_PROPERTY::NOTIFY|
            NIMBLE_PROPERTY::READ);

    fileCharacteristic_receive = pServiceFile->createCharacteristic(
        FILE_UUID_RECEIVE,
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::NOTIFY);
    //--->fileCharacteristic_receive->addDescriptor(new BLE2902());

    fileCharacteristic_receiveOta = pServiceFile->createCharacteristic(
        FILE_UUID_RECEIVE_OTA,
            NIMBLE_PROPERTY::WRITE |
            NIMBLE_PROPERTY::NOTIFY);

    fileCharacteristic_exists = pServiceFile->createCharacteristic(
        FILE_UUID_EXISTS,
            NIMBLE_PROPERTY::WRITE);

    fileCharacteristic_delete = pServiceFile->createCharacteristic(
        FILE_UUID_DELETE,
            NIMBLE_PROPERTY::WRITE);

    fileCharacteristic_sendFlag = pServiceFile->createCharacteristic(
        FILE_UUID_SENDFLAG,
            NIMBLE_PROPERTY::WRITE);

    fileCharacteristic_send = pServiceFile->createCharacteristic(
        FILE_UUID_SEND,
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY);
    //--->fileCharacteristic_send->addDescriptor(new BLE2902());

    fileCharacteristic_errorMsg = pServiceFile->createCharacteristic(
        FILE_UUID_ERRORMSG,
            NIMBLE_PROPERTY::READ |
            NIMBLE_PROPERTY::NOTIFY);
    //--->fileCharacteristic_errorMsg->addDescriptor(new BLE2902());

    fileCharacteristic_receiveFlag->setCallbacks(new FilesCallbacks_receiveFlag());
    fileCharacteristic_receiveFlagPatterns->setCallbacks(new FilesCallbacks_receiveFlagPatterns());
    fileCharacteristic_indexCustomPattern->setCallbacks(new FilesCallbacks_indexCustomPatterns());
    fileCharacteristic_customPatterns->setCallbacks(new FilesCallbacks_customPatterns());
    fileCharacteristic_receive->setCallbacks(new FilesCallbacks_receive());
    fileCharacteristic_receiveOta->setCallbacks(new FilesCallbacks_receiveOta());
    fileCharacteristic_exists->setCallbacks(new FilesCallbacks_checkFile());
    fileCharacteristic_delete->setCallbacks(new FilesCallbacks_deleteFile());
    fileCharacteristic_send->setCallbacks(new FilesCallbacks_send());
    fileCharacteristic_sendFlag->setCallbacks(new FilesCallbacks_sendFlag());

    //ledCharacteristic_speed->addDescriptor(new BLE2902());
    
    pServiceLedConfig->start();
    pServicePlaylist->start();
    pServiceGeneralConfig->start();
    pServiceFile->start();
    pServiceTesting->start();
    //pService3->start();


    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID1);
    pAdvertising->addServiceUUID(SERVICE_UUID2);
    pAdvertising->addServiceUUID(SERVICE_UUID3);
    pAdvertising->addServiceUUID(SERVICE_UUID4);
    pAdvertising->addServiceUUID(SERVICE_UUID5);
    pAdvertising->addServiceUUID(SERVICE_UUID6);
    //pAdvertising->addServiceUUID(SERVICE_UUID8);

    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    #ifdef DEBUGGING_DATA
        Serial.println("BLE is anable");
    #endif
    
    return 0;
}

/**
 * Las siguientes funciones se encargan de calcular el hash por el metodo de MD5.
 */
typedef union uwb {
    unsigned w;
    unsigned char b[4];
} MD5union;

typedef unsigned DigestArray[4];

static unsigned func0(unsigned abcd[])
{
    return (abcd[1] & abcd[2]) | (~abcd[1] & abcd[3]);
}

static unsigned func1(unsigned abcd[])
{
    return (abcd[3] & abcd[1]) | (~abcd[3] & abcd[2]);
}

static unsigned func2(unsigned abcd[])
{
    return abcd[1] ^ abcd[2] ^ abcd[3];
}

static unsigned func3(unsigned abcd[])
{
    return abcd[2] ^ (abcd[1] | ~abcd[3]);
}

typedef unsigned (*DgstFctn)(unsigned a[]);

static unsigned *calctable(unsigned *k)
{
    double s, pwr;
    int i;

    pwr = pow(2.0, 32);
    for (i = 0; i < 64; i++)
    {
        s = fabs(sin(1.0 + i));
        k[i] = (unsigned)(s * pwr);
    }
    return k;
}

static unsigned rol(unsigned r, short N)
{
    unsigned mask1 = (1 << N) - 1;
    return ((r >> (32 - N)) & mask1) | ((r << N) & ~mask1);
}

static unsigned *MD5Hash(uint8_t *msg, int mlen)
{
    static DigestArray h0 = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476};
    static DgstFctn ff[] = {&func0, &func1, &func2, &func3};
    static short M[] = {1, 5, 3, 7};
    static short O[] = {0, 1, 5, 0};
    static short rot0[] = {7, 12, 17, 22};
    static short rot1[] = {5, 9, 14, 20};
    static short rot2[] = {4, 11, 16, 23};
    static short rot3[] = {6, 10, 15, 21};
    static short *rots[] = {rot0, rot1, rot2, rot3};
    static unsigned kspace[64];
    static unsigned *k;

    static DigestArray h;
    DigestArray abcd;
    DgstFctn fctn;
    short m, o, g;
    unsigned f;
    short *rotn;
    union {
        unsigned w[16];
        char b[64];
    } mm;
    int os = 0;
    int grp, grps, q, p;
    unsigned char *msg2;

    if (k == NULL)
        k = calctable(kspace);

    for (q = 0; q < 4; q++)
        h[q] = h0[q];

    {
        grps = 1 + (mlen + 8) / 64;
        msg2 = (unsigned char *)malloc(64 * grps);
        memcpy(msg2, (unsigned char *)msg, mlen);
        msg2[mlen] = (unsigned char)0x80;
        q = mlen + 1;
        while (q < 64 * grps)
        {
            msg2[q] = 0;
            q++;
        }
        {
            MD5union u;
            u.w = 8 * mlen;
            q -= 8;
            memcpy(msg2 + q, &u.w, 4);
        }
    }

    for (grp = 0; grp < grps; grp++)
    {
        memcpy(mm.b, msg2 + os, 64);
        for (q = 0; q < 4; q++)
            abcd[q] = h[q];
        for (p = 0; p < 4; p++)
        {
            fctn = ff[p];
            rotn = rots[p];
            m = M[p];
            o = O[p];
            for (q = 0; q < 16; q++)
            {
                g = (m * q + o) % 16;
                f = abcd[1] + rol(abcd[0] + fctn(abcd) + k[q + 16 * p] + mm.w[g], rotn[q % 4]);

                abcd[0] = abcd[3];
                abcd[3] = abcd[2];
                abcd[2] = abcd[1];
                abcd[1] = f;
            }
        }
        for (p = 0; p < 4; p++)
            h[p] += abcd[p];
        os += 64;
    }
    free(msg2); //THIS IS IMPORTANT
    return h;
}

String GetMD5String(uint8_t *msg, int mlen)
{
    String str;
    int j;
    unsigned *d = MD5Hash(msg, mlen);
    MD5union u;
    for (j = 0; j < 4; j++)
    {
        u.w = d[j];
        char s[9];
        sprintf(s, "%02x%02x%02x%02x", u.b[0], u.b[1], u.b[2], u.b[3]);
        str += s;
    }
    return str;
}

/**
 * Hasta aqui terminan las funciones para MD5
 */

/**
 * @brief Actualiza el firmware.
 * @return Uno de los siguientes numeros.
 * -4, No se pudo finalizar la actualizacion
 * -5, No hay suficiente espacio para el OTA.
 * -6, Ocurrio un error al actualizar el firmware
 */
int performUpdate(Stream &updateSource, size_t updateSize)
{
    if (Update.begin(updateSize))
    {
        size_t written = Update.writeStream(updateSource);
        if (written == updateSize)
        {
            #ifdef DEBUGGING_DATA
                Serial.println("Written : " + String(written) + " successfully");
            #endif
        }
        else
        {
            #ifdef DEBUGGING_DATA
                Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
            #endif
            
        }
        if (Update.end())
        {
            Serial.println("OTA done!");
            if (Update.isFinished())
            {
                #ifdef DEBUGGING_DATA
                    Serial.println("Update successfully completed. Rebooting.");
                #endif
                
                return 1;
            }
            else
            {
                #ifdef DEBUGGING_DATA
                    Serial.println("Update not finished? Something went wrong!");
                #endif
                return -4;
            }
        }
        else
        {
            #ifdef DEBUGGING_DATA
                Serial.println("Error Occurred. Error #: " + String(Update.getError()));
            #endif
            return -6;
        }
    }
    else
    {
        #ifdef DEBUGGING_DATA
            Serial.println("Not enough space to begin OTA");
        #endif
        return -5;
    }
}

/**
 * @brief Revisa si el archivo es valido y si lo es actualiza el firmware
 * @return Uno de los siguientes numeros
 *  1, Se actualizo el firmware
 * -1, Es un directorio
 * -2, El archivo esta vacio
 * -3, El archivo no se pudo abrir.
 * -4, No se pudo finalizar la actualizacion
 * -5, No hay suficiente espacio para el OTA.
 * -6, Ocurrio un error al actualizar el firmware
 */
int updateFromFS(SdFat &fs, String name)
{
    int errorCode;
    File updateBin = fs.open(name);
    if (updateBin)
    {
        if (updateBin.isDirectory())
        {
            Serial.println("Error, update.bin is not a file");
            updateBin.close();
            return -1;
        }

        size_t updateSize = updateBin.size();

        if (updateSize > 0)
        {
            Serial.println("Try to start update");
            errorCode = performUpdate(updateBin, updateSize);
            updateBin.close();
            sdRemove(name);
            return errorCode;
        }
        else
        {
            Serial.println("Error, file is empty");
            updateBin.close();
            return -2;
        }

        updateBin.close();
        sdRemove(name);
    }
    else
    {
        Serial.println("Could not load update.bin from sd root");
        return -3;
    }
}

/**
 * @brief Intenta actualizar el firmware
 * @return Un codigo para saber si ocurre un error a la hora de realizar la actualizacion.
 */
int programming(String name){
    int errorCode;
    errorCode = updateFromFS(SD, name);
    return errorCode;
}

/**
 * @brief Reinicia el Esp32 pero antes escribe un mensaje por Serial.
 * @return Un codigo para saber si ocurre un error a la hora de realizar la actualizacion.
 */
void rebootWithMessage(String reason){
    #ifdef DEBUGGING_DATA
        Serial.println(reason);
    #endif
    delay(2000);
    ESP.restart();
}

/**
 * @brief Convierte un string en un array
 * un string de la forma x1,x2,...,xn se convierte en un array [0]=x1, [1]=x1, ...,[n-1]=xn
 * @param str Es el string que se desea convertir.
 * @param array Es el array donde se van a guardar los valores del string
 * @param n Es el numero de elementos que tiene el string
 * @return Uno de los siguientes numeros
 * 0, Todo salio normal.
 * -1, No hay n elementos en el string
 */
int stringToArray(String str, uint8_t* array, int n){
    int i;
    for (i = 0; i<n ; i++){
        if (str.indexOf(",") < 0){
            *(array + i) = str.toInt();
            break;
        }
        *(array + i) = str.substring(0, str.indexOf(",")).toInt();
        str = str.substring(str.indexOf(",") + 1);
    }
    i++;
    if(i != n){
        return -1;
    }
    return 0;
}

void Bluetooth::setPlaylistName(String playlistName){

    playListNameG = playlistName;
    if (playlistName.charAt(0) == '/'){
        playlistName.remove(0,1);
    }
    int index = playlistName.lastIndexOf('.');
    if (index > 0){
        playlistName.remove(index);
    }


    //playlistCharacteristic_name->setValue(playlistName.c_str());
    playlistCharacteristic_name->setValue(playlistName);            //Cambio para libreria NimBLE


    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET Playlist Name: ");
        Serial.println(playlistName.c_str());
    #endif
}

void Bluetooth::setPlaylistNameFiles(String playlistNameFiles){
    //playlistCharacteristic_name->setValue(playlistName.c_str());
    playlistCharacteristic_nameFiles->setValue(playlistNameFiles);            //Cambio para libreria NimBLE


    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET Playlist FILES: ");
        Serial.println(playlistNameFiles.c_str());
    #endif
}


void Bluetooth::setPathAmount(int pathAmount){


    //playlistCharacteristic_pathAmount->setValue(String(pathAmount).c_str());
    playlistCharacteristic_pathAmount->setValue(String(pathAmount));                //Cambio para libreria NimBLE
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET pathAmount: ");
        Serial.println(String(pathAmount).c_str());
    #endif
}

void Bluetooth::setPathName(String pathName){


    //playlistCharacteristic_pathName->setValue(pathName.c_str());
    playlistCharacteristic_pathName->setValue(pathName);                //Cambio para libreria NimBLE
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET pathName: ");
        Serial.println(pathName.c_str());
    #endif
}

void Bluetooth::setAddPathNamePlaylist(String addPathName){

    playlistCharacteristic_addPathNamePlaylist->setValue(addPathName);                //Cambio para libreria NimBLE
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET pathName: ");
        Serial.println(addPathName.c_str());
    #endif
}


void Bluetooth::setPathNameNotify(String pathName){


    //playlistCharacteristic_pathName->setValue(pathName.c_str());
    //===Obtener los ultimos 20 caracteres del nombre dell patron

    String last20Chars = pathName.substring(pathName.length() - 20);

    //============================================================= 
    Serial.println("PathName");
    Serial.println(pathName);


    playlistCharacteristic_pathNameNotify->setValue(last20Chars);                //Cambio para libreria NimBLE
    
    playlistCharacteristic_pathNameNotify->notify();

    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET pathName: ");
        Serial.println(last20Chars.c_str());
    #endif
}


void Bluetooth::setPathPosition(int pathPosition){

    playlistCharacteristic_pathPosition->setValue(String(pathPosition));               //Cambio para libreria NimBLE
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET pathPosition: ");
        Serial.println(String(pathPosition).c_str());
    #endif
}
void Bluetooth::setPlayMode(int mode){


    //playlistCharacteristic_mode->setValue(String(mode).c_str());
    playlistCharacteristic_mode->setValue(String(mode));                //Cambio para libreria NimBLE
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET PlaylistMode: ");
        Serial.println(String(mode).c_str());
    #endif
}
// void Bluetooth::setPathProgress(int progress){
//     playlistCharacteristic_progress->setValue(String(progress).c_str());
//     #ifdef DEBUGGING_BLUETOOTH
//         Serial.print("SET progress: ");
//         Serial.println(String(progress).c_str());
//     #endif
// }


void Bluetooth::setLedSpeed(int speed){
    speed = map(speed,MIN_PERIOD_LED,MAX_PERIOD_LED,MAX_SLIDER_LEDSPEED,MIN_SLIDER_LEDSPEED);
    if (speed > MAX_SLIDER_LEDSPEED){
        speed = MAX_SLIDER_LEDSPEED;
    }
    if (speed < MIN_SLIDER_LEDSPEED){
        speed = MIN_SLIDER_LEDSPEED;
    }


    //ledCharacteristic_speed->setValue(String(speed).c_str());
    ledCharacteristic_speed->setValue(String(speed));       //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET ledSoeed: ");
        Serial.println(String(speed).c_str());
    #endif
}
void Bluetooth::setCycleMode(int cycleMode){


    //ledCharacteristic_cycleMode->setValue(String(cycleMode).c_str());
    ledCharacteristic_cycleMode->setValue(String(cycleMode));              //Cambio para libreria NimBLE
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET cycleMode: ");
        Serial.println(String(cycleMode).c_str());
    #endif
}
void Bluetooth::setLedDirection(int ledDirection){


    //ledCharacteristic_direction->setValue(String(ledDirection).c_str());
    ledCharacteristic_direction->setValue(String(ledDirection));            //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET ledDirection: ");
        Serial.println(String(ledDirection).c_str());
    #endif
}
void Bluetooth::setBrightness(uint16_t brightness){
    brightness = map(brightness,0,255,MIN_SLIDER_BRIGHTNESS,MAX_SLIDER_BRIGHTNESS);
    
    
    //ledCharacteristic_brightness->setValue(String(brightness).c_str());
    ledCharacteristic_brightness->setValue(String(brightness));             //Cambio para libreria NimBLE
    

    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET brightness: ");
        Serial.println(String(brightness).c_str());
    #endif
}
void Bluetooth::setIndexPalette(int indexPalette){


    //ledCharacteristic_indexPalette->setValue(String(indexPalette).c_str());
    ledCharacteristic_indexPalette->setValue(String(indexPalette));             //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET indexPalette: ");
        Serial.println(String(indexPalette).c_str());
    #endif
}

void Bluetooth::setVersion(String version){

    generalCharacteristic_version->setValue(version);               //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET version: ");
        Serial.println(version.c_str());
    #endif
}

void Bluetooth::setSimpleVersion(String version){

    generalCharacteristic_SimpleVersion->setValue(version);               //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET version: ");
        Serial.println(version.c_str());
    #endif
}


void Bluetooth::setName(String name){

    generalCharacteristic_name->setValue(name);                 //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET ble name: ");
        Serial.println(name.c_str());
    #endif
}


void Bluetooth::setTimePaths(String time){

    generalCharacteristic_timePaths->setValue(time);                 //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET ble TimePaths: ");
        Serial.println(time.c_str());
    #endif
}

void Bluetooth::setTimeOffOn(String time){

    generalCharacteristic_timeOffOn->setValue(time);                 //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET ble TimeOffOn: ");
        Serial.println(time.c_str());
    #endif
}





void Bluetooth::setStatus(int status){


    //generalCharacteristic_status->setValue(String(status).c_str());
    generalCharacteristic_status->setValue(String(status));    //Cambio para la libreria NimBLE
    
    
    generalCharacteristic_status->notify();
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET status: ");
        Serial.println(String(status).c_str());
    #endif
}
void Bluetooth::setMotorSpeed(int speed){
    speed = map(speed,MIN_SPEED_MOTOR,MAX_SPEED_MOTOR,MIN_SLIDER_MSPEED,MAX_SLIDER_MSPEED);
    if (speed > MAX_SLIDER_MSPEED){
        speed = MAX_SLIDER_MSPEED;
    }
    if (speed < MIN_SLIDER_MSPEED){
        speed = MIN_SLIDER_MSPEED;
    }


    //generalCharacteristic_speed->setValue(String(speed).c_str());
    generalCharacteristic_speed->setValue(String(speed));               //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET motor Speed: ");
        Serial.println(String(speed).c_str());
    #endif
}
void Bluetooth::setPercentage(int percentage){
    if (percentage < 0 ){
        percentage = 0;
    }
    if (percentage > 100){
        percentage = 100;
    }


    //playlistCharacteristic_progress->setValue(String(percentage).c_str());
    playlistCharacteristic_progress->setValue(String(percentage));              //Cambio para libreria NimBLE
    
    
    playlistCharacteristic_progress->notify();
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET percentage: ");
        Serial.println(String(percentage).c_str());
    #endif
}

void Bluetooth::setRed(){
    uint8_t numberOfColors = EEPROM.read(ADDRESSCUSTOMPALLETE_COLORS);
    if (numberOfColors > MAX_COLORSPERPALLETE || numberOfColors < 2){
        return;
    }
    uint8_t red[numberOfColors];
    String reds = "";
    for (int i = 0; i < numberOfColors; i++){
        red[i] = EEPROM.read(ADDRESSCUSTOMPALLETE_RED + i);
        reds.concat(red[i]);
        reds.concat(",");
    }
    reds.remove(reds.length() - 1);


    //ledCharacteristic_red->setValue(reds.c_str());
    ledCharacteristic_red->setValue(reds);                  //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET red: ");
        Serial.println(reds.c_str());
    #endif
}
void Bluetooth::setGreen(){
    uint8_t numberOfColors = EEPROM.read(ADDRESSCUSTOMPALLETE_COLORS);
    if (numberOfColors > MAX_COLORSPERPALLETE || numberOfColors < 2){
        return;
    }
    uint8_t green[numberOfColors];
    String greens = "";
    for (int i = 0; i < numberOfColors; i++){
        green[i] = EEPROM.read(ADDRESSCUSTOMPALLETE_GREEN + i);
        greens.concat(green[i]);
        greens.concat(",");
    }
    greens.remove(greens.length() - 1);
    Serial.println("Greens fubction setGreen");
    Serial.println(greens);

    //ledCharacteristic_green->setValue(greens.c_str());
    ledCharacteristic_green->setValue(greens);              //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET green: ");
        Serial.println(greens.c_str());
    #endif
}
void Bluetooth::setBlue(){
    uint8_t numberOfColors = EEPROM.read(ADDRESSCUSTOMPALLETE_COLORS);
    if (numberOfColors > MAX_COLORSPERPALLETE || numberOfColors < 2){
        return;
    }
    uint8_t blue[numberOfColors];
    String blues = "";
    for (int i = 0; i < numberOfColors; i++){
        blue[i] = EEPROM.read(ADDRESSCUSTOMPALLETE_BLUE + i);
        blues.concat(blue[i]);
        blues.concat(",");
    }
    blues.remove(blues.length() - 1);


    //ledCharacteristic_blue->setValue(blues.c_str());
    ledCharacteristic_blue->setValue(blues);                //Cambios para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET blue: ");
        Serial.println(blues.c_str());
    #endif
}
void Bluetooth::setPositions(){
    uint8_t numberOfColors = EEPROM.read(ADDRESSCUSTOMPALLETE_COLORS);
    if (numberOfColors > MAX_COLORSPERPALLETE || numberOfColors < 2){
        return;
    }
    uint8_t position[numberOfColors];
    String positions = "";
    for (int i = 0; i < numberOfColors; i++){
        position[i] = EEPROM.read(ADDRESSCUSTOMPALLETE_POSITIONS + i);
        positions.concat(position[i]);
        positions.concat(",");
    }
    positions.remove(positions.length() - 1);


    //ledCharacteristic_positions->setValue(positions.c_str());
    ledCharacteristic_positions->setValue(positions);               //Cambios para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET pathPositions: ");
        Serial.println(positions.c_str());
    #endif
}
void Bluetooth::setAmountOfColors(){
    uint8_t numberOfColors = EEPROM.read(ADDRESSCUSTOMPALLETE_COLORS);
    String amount;
    amount.concat(numberOfColors);


    //ledCharacteristic_amountColors->setValue(amount.c_str());
    ledCharacteristic_amountColors->setValue(amount);               //Cambios para libreria NiBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET amount: ");
        Serial.println(amount.c_str());
    #endif
}

void Bluetooth::setCalibrationStatus(bool calibrationStatus){
    if (calibrationStatus){
        String dataBLE = "1";
        generalCharacteristic_calibration->setValue(dataBLE);
    }
    else{
        String dataBLE2 = "0";
        generalCharacteristic_calibration->setValue(dataBLE2);
    }
}

String obtenerTerminacionArchivo(int numeroBuscado) {
  File archivo;
  String terminacionArchivo = ".bin";

  

  archivo = SD.open("playlist.playlist");  //Antes se hacoa sobre copyPlaylist.playlist
  if (!archivo) {
    return terminacionArchivo;  // Error al abrir el archivo
  }

  while (archivo.available()) {
    String linea = archivo.readStringUntil('\n');
    linea.trim();
    
    int posicionInicial = linea.lastIndexOf('-') + 1;
    int posicionFinal = linea.lastIndexOf('.');
    String numeroString = linea.substring(posicionInicial, posicionFinal);
    int numero = numeroString.toInt();
    if (numero == numeroBuscado) {
      terminacionArchivo = linea.substring(posicionFinal);
      break;
    }
  }

  archivo.close();
  return terminacionArchivo;
}

void Bluetooth::setModel(String version){

    generalCharacteristic_model->setValue(version);               //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET Model: ");
        Serial.println(version.c_str());
    #endif
}


void Bluetooth::setBattery(String version){

    generalCharacteristic_battery->setValue(version);               //Cambio para libreria NimBLE
    
    
    #ifdef DEBUGGING_BLUETOOTH
        Serial.print("SET Battery: ");
        Serial.println(version.c_str());
    #endif
}
