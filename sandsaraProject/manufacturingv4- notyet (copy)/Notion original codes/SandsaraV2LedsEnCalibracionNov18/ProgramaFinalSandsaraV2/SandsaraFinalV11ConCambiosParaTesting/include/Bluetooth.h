#pragma once
//#define BLUECOMMENTS
#include <Arduino.h>
#include "config.h"

#include <Preferences.h>

extern Preferences preferences;


#define SERVICE_UUID1 "fd31a2be-22e7-11eb-adc1-0242ac120002" //for led config
#define SERVICE_UUID2 "fd31a58e-22e7-11eb-adc1-0242ac120002" //for 
#define SERVICE_UUID3 "fd31a688-22e7-11eb-adc1-0242ac120002"
#define SERVICE_UUID4 "fd31a778-22e7-11eb-adc1-0242ac120002" //for playlist config
#define SERVICE_UUID5 "fd31a840-22e7-11eb-adc1-0242ac120002" //for general config
#define SERVICE_UUID6 "fd31abc4-22e7-11eb-adc1-0242ac120002" //for file config


//====Led configuration Characteristics====
#define CHARACTERISTIC_UUID_LEDSPEED        "1a9a7b7e-2305-11eb-adc1-0242ac120002"   //Para cambiar la velocidad de los leds,   Lo que recibe es un vallor entre 0 y 100
#define CHARACTERISTIC_UUID_CYCLEMODE       "1a9a7dea-2305-11eb-adc1-0242ac120002"   //Enviamos "0" para definir el modo de ciclo, Enviamos cualquier otro nnumero para definir el modo fijo
#define CHARACTERISTIC_UUID_DIRECTION       "1a9a8042-2305-11eb-adc1-0242ac120002"   //Enviamos "1" para definir el giro en setido horario, Enviamos "0" para definir el giro antihorario
#define CHARACTERISTIC_UUID_BRIGHTNESS      "1a9a8948-2305-11eb-adc1-0242ac120002"   //Acepta valores entre 0 y 100
#define CHARACTERISTIC_UUID_SELECTEDPALETTE "1a9a813c-2305-11eb-adc1-0242ac120002"   
#define CHARACTERISTIC_UUID_AMOUNTCOLORS    "1a9a820e-2305-11eb-adc1-0242ac120002"   //al leer esta caracteristica devuelve Numero de colores
#define CHARACTERISTIC_UUID_POSITIONS       "1a9a82d6-2305-11eb-adc1-0242ac120002"   //al leer esta caracteristica devuelve posiciones
#define CHARACTERISTIC_UUID_RED             "1a9a83a8-2305-11eb-adc1-0242ac120002"   //al leer esta caracteristica devuelve valores de rojos
#define CHARACTERISTIC_UUID_GREEN           "1a9a8466-2305-11eb-adc1-0242ac120002"   //al leer esta caracteristica devuelve valores de verdes
#define CHARACTERISTIC_UUID_BLUE            "1a9a852e-2305-11eb-adc1-0242ac120002"   //al leer esta caracteristica devuelve valores de azules
#define CHARACTERISTIC_UUID_UPDATECPALETTE  "1a9a87b8-2305-11eb-adc1-0242ac120002"   // Se le envia el array conn lla connfiguracion de colores para los leds, al escribir en la caracteristica se aplica el ccambio en los leds
#define CHARACTERISTIC_UUID_MSGERRORLEDS    "1a9a8880-2305-11eb-adc1-0242ac120002"
#define CHARACTERISTIC_OFFLEDS              "2b19da26-b7b9-11ed-afa1-0242ac120002"     //Escribiendo cualquier caracter sobre esta caracteristica enciende los leds
#define CHARACTERISTIC_ONLEDS               "21dbbc20-b7c0-11ed-afa1-0242ac120002"     //Escribiendo cualquier caracter sobre esta caracteristica apaga los leds
/*
1a9a8a06-2305-11eb-adc1-0242ac120002
1a9a8ac4-2305-11eb-adc1-0242ac120002
1a9a8b8c-2305-11eb-adc1-0242ac120002*/

//====File config====
#define FILE_UUID_RECEIVEFLAG   "fcbff68e-2af1-11eb-adc1-0242ac120002" 
#define FILE_UUID_RECEIVE       "fcbffa44-2af1-11eb-adc1-0242ac120002"        //Esta es la caracteeristica original con la que se envian los archivos a la esp32
#define FILE_UUID_RECEIVE_OTA   "7f8c77b9-2075-421f-9ab0-6c0fe625b626"        //Esta caracteristica es para la actualizacion del ota, envia el firmware a la particion ota
#define FILE_UUID_EXISTS        "fcbffb52-2af1-11eb-adc1-0242ac120002"
#define FILE_UUID_DELETE        "fcbffc24-2af1-11eb-adc1-0242ac120002"
#define FILE_UUID_SENDFLAG      "fcbffdaa-2af1-11eb-adc1-0242ac120002"
#define FILE_UUID_SEND          "fcbffe72-2af1-11eb-adc1-0242ac120002"
#define FILE_UUID_ERRORMSG      "fcbffce2-2af1-11eb-adc1-0242ac120002"

#define INDEXCUSTOMPATTERN      "b77be920-2bbf-4a15-9422-f827abd47dfd"

#define CUSTOMPATTERNS            "0882e1f1-f117-4294-94a5-657088e90127"

#define FILE_UUID_RECEIVEFLAGPATTERNS "3cf5d7e1-058c-4b9e-aaca-27f4822a5b3d"

/*
fcc0012e-2af1-11eb-adc1-0242ac120002
fcc0020a-2af1-11eb-adc1-0242ac120002
fcc002c8-2af1-11eb-adc1-0242ac120002
*/

//====Playlist Config====
#define PLAYLIST_UUID_NAME              "9b12a048-2c6e-11eb-adc1-0242ac120002"      //Si se escribe el nnombre de una playlist existente, inincia la reproduccion de esa playylist, por ejemplo “geometry.playlist”  
#define PLAYLIST_UUID_PATHAMOUNT        "9b12a26e-2c6e-11eb-adc1-0242ac120002"      //Muestra el numero de archivos que contiene la playlist
#define PLAYLIST_UUID_PATHNAME          "9b12a534-2c6e-11eb-adc1-0242ac120002"      //regreso a su forma anterior como caraccteristica read y write
#define PLAYLIST_UUID_ADDPATHNAMEPLAYLIST "8e50e409-b7da-481d-8dd2-610dfd7bcfbe"    //Nueva caracteristica para agregar un path a la playlist se le envia el nnombre completo, por ejemplo "Sandsara-trackNumber-0006.bin"
#define PLAYLIST_UUID_PATHNAMENOTIFY    "4e600a8c-ba11-11ed-afa1-0242ac120002"      //El nombre del patron actual se muestra como notificacion, solo muestra los ultimos 20 caracteres del nombre
#define PLAYLIST_UUID_PATHPOSITION      "9b12a62e-2c6e-11eb-adc1-0242ac120002"      //Sirve para seleccionar el patha reproducir de una playlist, en la playlist "2-5-27-58-90" , al enviar el numero 2 se reproducie el path 5 porque llos index comienzan a parttir del 1
#define PLAYLIST_UUID_ADDPATH           "9b12a7be-2c6e-11eb-adc1-0242ac120002"      //obsoleta
#define PLAYLIST_UUID_MODE              "9b12a886-2c6e-11eb-adc1-0242ac120002"
#define PLAYLIST_UUID_PATHPROGRESS      "9b12a944-2c6e-11eb-adc1-0242ac120002"      ///Muestra el porcentaje de avance en la ejecucion dell patrton
#define PLAYLIST_UUID_ERRORMSG          "9b12aa02-2c6e-11eb-adc1-0242ac120002"

#define PLAYLIST_UUID_NAMEFILES         "132998e2-b88c-11ed-afa1-0242ac120002"     //Se lee esta caracteristica para conocer el contenido de la playlist actual en el formatto corto

#define PLAYLIST_UUID_SHORT_PLAYLIST    "7f65a4f1-dbb7-4759-a476-b90e9659c0cb"     //Caracteristica para recibir el contenido de la playlist actual en el formato corto "1-3-45-78-90" , y lo sobre escribe en el archivvo de la playlist actual

#define PLAYLIST_UUID_SELECT_PLAYLIST   "338ab3f6-a15b-4e8e-aa9d-b0518ec697c3"     //Seleciona la playlist a reproducir, si se envia 1 reproduce todos llos patrones, si se le envia 0 reproduce la playlist custom

#define PLAYLIST_UUID_PLAY_NOW          "4429830d-744e-425a-a880-b388f8c8212b"     //Se envia el numero del path que se quiere reproducir en ese momento



/*
9b12ac28-2c6e-11eb-adc1-0242ac120002
9b12acfa-2c6e-11eb-adc1-0242ac120002
9b12adb8-2c6e-11eb-adc1-0242ac120002
*/






//====General Config====
#define GENERAL_UUID_TEST "a4756e7a-b2d3-11ed-afa1-0242ac120002"                    //Muestra los ultimos resultados del archivo llog.txt

#define GENERAL_UUID_VERSION        "7b204278-30c3-11eb-adc1-0242ac120002"          //Muestra la version dell firmware
#define GENERAL_UUID_NAME           "7b204548-30c3-11eb-adc1-0242ac120002"          //Se puede leer el nombre bluetooth del sandsara, o all escribir en la caracteristica se cambia el nombre, el prefijo Sand no cambia, solo la palabra que le sigue por ejemplo al enviar "sandsara1"   el nombre completo es Sand sandsara1  
#define GENERAL_UUID_STATUS         "7b204660-30c3-11eb-adc1-0242ac120002"          // Muesttra el status acttual del sandsara
#define GENERAL_UUID_PAUSE          "7b20473c-30c3-11eb-adc1-0242ac120002"          //Se envia cualquier caracter para hacer pausa
#define GENERAL_UUID_PLAY           "7b20480e-30c3-11eb-adc1-0242ac120002"          //Se envia cualquier caracter para hacer Play
#define GENERAL_UUID_SLEEP          "7b204a3e-30c3-11eb-adc1-0242ac120002"          //Se envia cualquier caracter para enntar al modo sleep
#define GENERAL_UUID_SPEED          "7b204b10-30c3-11eb-adc1-0242ac120002"          //Para connfigurar la velocidad e la bola, acepta valores del 1 al 10
#define GENERAL_UUID_RESTART        "7b204bce-30c3-11eb-adc1-0242ac120002"          //Se envia cualquier caracter para reiniciar
#define GENERAL_UUID_FACTORYRESET   "7b204c8c-30c3-11eb-adc1-0242ac120002"          //Se envia cualquier caracter para hacer factory reset
#define GENERAL_UUID_ERRORMSG       "7b204d4a-30c3-11eb-adc1-0242ac120002"
#define GENERAL_UUID_CALIBRATION    "7b204f84-30c3-11eb-adc1-0242ac120002"

#define GENERAL_UUID_TIMEPATHS      "d06d2fec-ba1a-11ed-afa1-0242ac120002"          //Obtiene el intervalo de espera entre patrones, en minutos ejemplo "5"
#define GENERAL_UUID_TIMEOFFON      "d6097f96-ba1a-11ed-afa1-0242ac120002"          //El formato para los tiempos de enccendido es el siguiente
                                                                                    //"123018002330"
                                                                                    // 12:30     18:00   23:00       hora de enceendido/hora de apagado/hora actual
                                                                                    //Para deesactivar esta funncion hay que enviar 252525252525

#define GENERAL_UUID_APP_TIME       "7114beee-e63d-4743-92cd-8af0fd119837"          //Se envia desde la App la hora actual  enn el formato "1630"  que corresponde a las 16:30


#define GENERAL_UUID_SIMPLE_VERSION    "9e23e02e-8921-4bf9-84f5-f58fa81c726e"       //Muestra el numero de version, solo un numero
#define GENERAL_UUID_PATHS_FOR_CALIB   "99240cad-83ea-4c92-8a3e-197add35fad1"       //Caracteristica para definir el numero de patrones antes de hacer la calibracion intermedia 

//=============== Definicion de los UUID de las Caracteristicas NewCharacteristic (3)
#define GENERAL_UUID_BATTERY            "893441aa-3720-40e6-8229-03f3fcd38dd0" 
#define GENERAL_UUID_MODEL              "caf74386-dd8b-4f54-9dd9-71c75f6d679a" 

/**
 * @class Bluetooth
 * @brief Se encarga de gestionar la comunicacion por bluetooth
 * @param timeOutBt es el tiempo, en milisegundos, que se va a esperar para recibir respuesta del dispositivo bluetooth conectado.
 * @param dataBt es donde se va a almacenar la informacion recibida.
 */
class Bluetooth {
    private:
        
    public:
        Bluetooth(); 
        int init(String = "Sandsara");

        static void setPlaylistName(String );
        static void setPathAmount(int);
        static void setPathName(String);
        static void setAddPathNamePlaylist(String);
        static void setPathNameNotify(String);
        static void setPathPosition(int);
        static void setPlayMode(int);
        static void setPathProgress(int);
        static void setPlaylistNameFiles(String );
        static void setPlaylistToggle();

        static void setLedSpeed(int);
        static void setCycleMode(int);
        static void setLedDirection(int);
        static void setBrightness(uint16_t);
        static void setIndexPalette(int);
        static void setRed();
        static void setGreen();
        static void setBlue();
        static void setPositions();
        static void setAmountOfColors();

        static void setVersion(String);
        static void setModel(String);
        static void setBattery(String);
        static void setSimpleVersion(String);
        static void setName(String);

        static void setTimePaths(String);
        static void setTimeOffOn(String);


        static void setStatus(int);
        static void setMotorSpeed(int);
        static void setPercentage(int);
        static void setCalibrationStatus(bool);

        static void setTestResult(String);

        static void setCustomPatterns(String);
        static void setindexCustomPatterns(String);
};