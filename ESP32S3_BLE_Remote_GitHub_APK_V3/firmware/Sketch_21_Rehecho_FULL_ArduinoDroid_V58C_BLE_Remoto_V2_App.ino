/*
  Sketch_21_Rehecho_FULL_ArduinoDroid_V34_ArranqueSeguro
  Freenove ESP32-S3 Development Kit + TFT ST7789 240x320 + FT6336U

  Objetivo:
  - Rehacer desde cero una version parecida al Sketch_21 original,
    pero mas compatible con ArduinoDroid.
  - No usa LVGL para evitar los errores de enlazado vistos en Android.
  - No usa la libreria Freenove_WS2812, porque ArduinoDroid encontraba el .h
    pero no enlazaba bien el .cpp.
  - Usa Audio.h si la libreria ESP32-audioI2S esta instalada.
    Si no esta instalada, la pagina Audio avisa y solo hace un beep de diagnostico.

  Funciones incluidas:
  - Pantalla TFT
  - Tactil FT6336U
  - Diagnostico I2C
  - SD_MMC con pines Freenove
  - Camara RGB565 con pines Freenove
  - Camara RGB565 y previsualizacion en pantalla
  - Escaner WiFi
  - LED RGB WS2812 en GPIO48 con neopixelWrite seguro
  - Buzzer en GPIO45
  - MAX30102/MAX30105 si la libreria SparkFun esta instalada
  - Menu tactil directo sin LVGL
  - V5: clave WiFi visible mientras se escribe, sin asteriscos.
  - V5: camara arranca en modo color recomendado y mejora ajustes del sensor.
  - V6: elimina WiFi/Camara repetidos del segundo menu y anade FTP.
  - V6: camara reescrita siguiendo Sketch_15_Lvgl_Camera: RGB565 + swap bytes.
  - V6: fotos BMP a microSD en /picture.
  - V6: reduce parpadeo actualizando solo zonas dinamicas.
  - V6: pitido de arranque desactivado por defecto y con interruptor.
  - V6: reproductor MP3 desde /music con barra de posicion y volumen.
  - V7: corrige error ArduinoDroid ESP32 core 3.x: conflicto byte/std::byte entre Audio.h y SparkFun MAX30105.
  - V9: corrige compilacion SD_MMC en ArduinoDroid, desactiva Audio.h real por defecto y cambia la camara a conversion RGB565 directa para evitar imagen en negativo.
  - V10: anade redes WiFi guardadas y conexion automatica al arrancar. La red MERCUSYS_393E queda guardada con clave visible en el codigo.
  - V11: camara corregida con la misma logica de color del Sketch_15_Lvgl_Camera:
    RGB565 + orden de bytes estilo LVGL + envio a TFT_eSPI con swap activo.
    La foto BMP se guarda desde el mismo buffer corregido que se ve en pantalla.
  - V12: simplifica la camara: Foto = captura y guarda directamente en /picture.
    El boton Guardar pasa a ser Galeria para visualizar los BMP guardados.
  - V13: anade boton Rotar en camara para invertir arriba/abajo, izquierda/derecha o 180 grados.
  - V13: activa Audio.h por defecto, reproduce MP3 desde carpeta seleccionable
    y recuerda la carpeta en /music_folder.txt de la microSD.
  - V14: elimina el acceso duplicado Musica del menu 2, renombra Audio a Musica,
    anade botones Vol-/Vol+ de 5 puntos, retroceso/avance de 5 s y porcentaje en calculadora.
  - V15: cambia los botones de volumen a 10 secciones: -1/+1 seccion por pulsacion,
    mostrando el estado como Volumen 0/10..10/10 y evitando saltos grandes.
  - V16: rehace WS2812 con 4 barras tactiles: brillo, rojo, verde y azul.
  - V16: anade boton Efecto para alternar animaciones tipo arbol de Navidad.
  - V16: anade menu Pantalla para apagar la TFT tras 1-5 minutos sin tocar,
    manteniendo el tactil activo para despertar en el mismo punto.
  - V17: corrige WS2812 para apagar/encender y colores reales en barras.
  - V19: elimina la funcion de Video y refuerza el servidor FTP: PASV/EPSV/PORT,
    STOR mas robusto para subir archivos desde FileZilla y respuestas extra usadas por clientes FTP.
  - V20: corrige FileZilla cuando abre una segunda conexion de control para subir archivos:
    el servidor acepta la nueva conexion y cierra la anterior para no quedarse esperando bienvenida.
  - V24: indicador automatico del WS2812 para actividad de microSD fuera del menu WS2812:
    morado = lectura, amarillo = escritura. No interfiere con las barras manuales.
  - V26: el indicador de microSD funciona en cualquier menu, incluso dentro de WS2812.
  - V27: el indicador de microSD parpadea: morado en lectura, amarillo en escritura.
    Si hay lectura y escritura a la vez, alterna 3 parpadeos morados y 3 amarillos.
  - V27: conexion WiFi con IP estatica 192.168.1.120.
  - V28: programacion OTA por WiFi con ArduinoOTA.
  - V29: OTA Web por navegador para evitar problemas de compilacion de ArduinoDroid con ArduinoOTA.
  - V32: reinicio OTA corregido usando deep sleep corto si ESP.restart() se queda bloqueado.
  - V33: reinicio OTA cambiado a reset forzado por panic/abort para placas que no despiertan con deep sleep
    ni reinician con ESP.restart(); evita depender del boton RESET.
  - V34: Arranque Seguro: espera inicial mas larga, pantalla/backlight inicializados de forma escalonada,
    WiFi/OTA aplazados unos segundos y reinicio seguro desde menu para evitar pantalla negra tras reset corto.

  Librerias necesarias:
  - TFT_eSPI configurada con los setups Freenove
  - Arduino-FT6336U
  - V35: sensor de pulso deshabilitado; conector I2C libre
  - V36: servidor CamWeb integrado dentro del INO para evitar error undefined reference
    a startCameraServer/stopCameraServer cuando ArduinoDroid no compila app_httpd.cpp.
  - V37: la pagina CamWeb se integra en el servidor web principal del puerto 80
    para evitar que el navegador no cargue los puertos 82/83. Stream en puerto 81.
  - Audio.h / ESP32-audioI2S del kit Freenove para reproducir MP3 reales

  - V57B: LED morado de lectura microSD vuelve a parpadear durante reproduccion de musica,
    sin hacer lecturas extra: solo marca actividad mientras Audio.h esta decodificando.
  - V57B: barras de progreso y volumen pasan a ser solo visuales por defecto para evitar
    saltos grandes cuando el tactil conectado por USB-OTG da coordenadas inestables.
    Los botones -5s/+5s y -1/+1 quedan con proteccion anti-repeticion.
  - V57B: base compatible con app Android sencilla de subida OTA: usa el mismo endpoint
    /update?key=CLAVE de la pagina OTA Web actual.

  Configuracion ArduinoDroid recomendada:
  - Board: ESP32S3 Dev Module
  - Flash Size: 8MB (64Mb)
  - PSRAM: OPI PSRAM

  - V52: al entrar en Musica, si hay MP3 sueltos en /music pregunta si deseas ordenar.
  - V53: evita bloqueo/parpadeo al terminar Musica: EOF diferido, indices por pasos y LED SD sin forzado durante reproduccion.
    Si eliges No, bloquea la reproduccion hasta salir y volver a entrar.
    Si eliges Si, reparte canciones en subcarpetas de 50 y genera un indice por carpeta
    para que al seleccionar carpeta cargue la lista al momento.
  - V43: botones y zonas tactiles mas separados. El servidor web/OTA/CamWeb queda apagado
    hasta entrar en Servidor Web y pulsar Activar Web.
  - Partition Scheme: 8M with spiffs (3MB APP/1.5MB SPIFFS) para OTA. No uses esquemas "No OTA" si quieres programar por WiFi.
  - Flash Mode: QIO 80MHz
  - CPU Frequency: 240MHz WiFi
  - Upload Mode: UART0 / Hardware CDC
  - USB CDC On Boot: Disabled si usas USB-UART/CH343
  - Upload Speed: 115200 si falla al subir; 921600 si ya sube bien
*/

#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <Preferences.h>
#include "FT6336U.h"
#include "FS.h"
#include "SD_MMC.h"
#include "WiFi.h"
#include <WebServer.h>
#include <Update.h>
#include <esp_sleep.h>
#include <esp_system.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#if __has_include(<esp_arduino_version.h>)
#include <esp_arduino_version.h>
#endif
#ifndef ESP_ARDUINO_VERSION_MAJOR
#define ESP_ARDUINO_VERSION_MAJOR 3
#endif
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

// V58: control remoto ligero por Bluetooth Low Energy.
#if __has_include(<BLEDevice.h>)
  #include <BLEDevice.h>
  #include <BLEServer.h>
  #include <BLEUtils.h>
  #include <BLE2902.h>
  #define HAVE_BLE_REMOTE 1
#else
  #define HAVE_BLE_REMOTE 0
#endif


// V35: sensor MAX30102/MAX30105 deshabilitado por completo.
// Se liberan sus conectores I2C para usarlos con otros modulos externos.
// No se incluyen las librerias SparkFun para que ArduinoDroid no las compile.
#define HAVE_MAX30105_LIB 0

// Audio SD opcional: usa la libreria ESP32-audioI2S del kit Freenove si esta instalada.
#ifndef ENABLE_SD_AUDIO
#define ENABLE_SD_AUDIO 1   // V13: audio real activado por defecto. Si Audio.h no compila, cambia a 0.
#endif
#if ENABLE_SD_AUDIO && __has_include("Audio.h")
  #include "Audio.h"
  #define HAVE_AUDIO_LIB 1
#else
  #define HAVE_AUDIO_LIB 0
#endif

// -------------------- Pines Freenove --------------------
#ifndef I2C_SDA
#define I2C_SDA 2
#endif
#ifndef I2C_SCL
#define I2C_SCL 1
#endif
#define TOUCH_RST_PIN -1
#define TOUCH_INT_PIN -1

#define PIN_BUZZER 45
#define LEDS_PIN 48
#define I2S_BCLK 42
#define I2S_DOUT 41
#define I2S_LRC  14

#define SD_MMC_CMD 38
#define SD_MMC_CLK 39
#define SD_MMC_D0  40

// V50: medicion de bateria/VIN del kit Freenove.
// El ejemplo oficial ADC_Battery usa GPIO19 y multiplica por 4 porque
// el circuito de la placa entrega al ADC una cuarta parte del voltaje real.
#define BATTERY_ADC_PIN 19

// Camara Freenove / ESP32S3_EYE, tomada del Sketch original
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM  4
#define SIOC_GPIO_NUM  5
#define Y2_GPIO_NUM    11
#define Y3_GPIO_NUM    9
#define Y4_GPIO_NUM    8
#define Y5_GPIO_NUM    10
#define Y6_GPIO_NUM    12
#define Y7_GPIO_NUM    18
#define Y8_GPIO_NUM    17
#define Y9_GPIO_NUM    16
#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM  7
#define PCLK_GPIO_NUM  13

// -------------------- Pantalla / tactil --------------------
static const int SCREEN_W = 240;
static const int SCREEN_H = 320;
static const int TFT_ROTATION = 0;

// Control de apagado automatico de pantalla.
// V49 IMPORTANTE: GPIO2 queda LIBRE para I2C SDA del tactil FT6336U.
// Versiones anteriores usaban GPIO2 como backlight/LED_ON, pero en este kit
// GPIO2 tambien es I2C_SDA. Ponerlo como OUTPUT puede bloquear o desalinear
// el tactil: al principio responde y luego abre botones equivocados o no responde.
// Por eso se desactiva el control por GPIO y solo se usa DISPOFF/DISPON.
static const int TFT_BACKLIGHT_PIN = -1;
static const bool TFT_BACKLIGHT_ON_LEVEL = HIGH;
static const char *PREF_NAMESPACE_UI = "ui";


// Si el tactil no coincide con la pantalla, cambia estos valores.
static const bool TOUCH_SWAP_XY  = false;
static const bool TOUCH_INVERT_X = false;
static const bool TOUCH_INVERT_Y = false;

// V40: encuadre tactil de botones.
// Estos margenes hacen que la zona tactil cubra mejor el boton dibujado
// sin mezclarse con el boton vecino. Si en tu placa el toque queda desplazado,
// ajusta TOUCH_CAL_OFFSET_X / TOUCH_CAL_OFFSET_Y en pasos pequenos (+/- 2 o 3).
static const int16_t TOUCH_CAL_OFFSET_X = 0;
static const int16_t TOUCH_CAL_OFFSET_Y = 0;
// V43: zona tactil estrictamente separada.
// Antes habia margen invisible y podia solaparse con el boton vecino.
// Ahora la zona tactil coincide con el boton dibujado, con un pequeno inset
// opcional para dejar una franja muerta entre botones si hace falta.
static const int16_t TOUCH_BUTTON_MARGIN_X = 0;
static const int16_t TOUCH_BUTTON_MARGIN_Y = 0;
static const int16_t TOUCH_HITBOX_INSET_X = 1;
static const int16_t TOUCH_HITBOX_INSET_Y = 1;
static const bool TOUCH_DRAW_HITBOX = false;
static const uint8_t TOUCH_READ_RETRIES = 1;
static const bool TOUCH_DEBUG_SERIAL = false;

TFT_eSPI tft = TFT_eSPI();
FT6336U ft6336u(I2C_SDA, I2C_SCL, TOUCH_RST_PIN, TOUCH_INT_PIN);

// -------------------- Conector MAX30102 liberado --------------------
// V35: no se usa el sensor de pulso. SDA/SCL quedan disponibles como bus I2C.

// -------------------- Estado general --------------------
enum AppPage : uint8_t {
  PAGE_HOME = 0,
  PAGE_SYSTEM,
  PAGE_DIAG,
  PAGE_CAMERA,
  PAGE_PHOTOS,
  PAGE_SD,
  PAGE_WIFI,
  PAGE_WS2812,
  PAGE_BUZZER,
  PAGE_CAMERA_WEB,
  PAGE_AUDIO,
  PAGE_MUSIC_FOLDER,
  PAGE_MUSIC_LIST,
  PAGE_CLOCK,
  PAGE_CALC,
  PAGE_FTP,
  PAGE_BATTERY,
  PAGE_SCREEN_TIMEOUT
};

enum Action : uint8_t {
  ACT_NONE = 0,
  ACT_SYSTEM,
  ACT_DIAG,
  ACT_CAMERA,
  ACT_SD,
  ACT_WIFI,
  ACT_WS2812,
  ACT_BUZZER,
  ACT_CAMERA_WEB,
  ACT_AUDIO,
  ACT_CLOCK,
  ACT_CALC,
  ACT_FTP,
  ACT_RESTART,
  ACT_BACK,
  ACT_RESCAN_I2C,
  ACT_SD_REFRESH,
  ACT_SD_TEST,
  ACT_WIFI_SCAN,
  ACT_CAM_CAPTURE,
  ACT_CAM_SAVE,
  ACT_LED_RED_PLUS,
  ACT_LED_RED_MINUS,
  ACT_LED_GREEN_PLUS,
  ACT_LED_GREEN_MINUS,
  ACT_LED_BLUE_PLUS,
  ACT_LED_BLUE_MINUS,
  ACT_LED_BRIGHT_PLUS,
  ACT_LED_BRIGHT_MINUS,
  ACT_LED_OFF,
  ACT_BUZZER_BEEP,
  ACT_BUZZER_MELODY,
  ACT_CAMERA_WEB_START,
  ACT_WEB_SERVER_TOGGLE,
  ACT_WEB_CAMERA_STOP,
  ACT_AUDIO_TEST,
  ACT_AUDIO_PREV,
  ACT_AUDIO_PLAY,
  ACT_AUDIO_STOP,
  ACT_AUDIO_NEXT,
  ACT_AUDIO_REFRESH,
  ACT_AUDIO_FOLDER,
  ACT_FOLDER_UP,
  ACT_FOLDER_USE,
  ACT_FOLDER_REFRESH,
  ACT_BUZZER_BOOT_TOGGLE,
  ACT_FTP_TOGGLE,
  ACT_FTP_REFRESH,
  ACT_CAM_COLOR,
  ACT_CAM_ROTATE,
  ACT_PHOTO_VIEWER,
  ACT_PHOTO_PREV,
  ACT_PHOTO_NEXT,
  ACT_PHOTO_REFRESH,
  ACT_WIFI_CONNECT,
  ACT_CLOCK_H_PLUS,
  ACT_CLOCK_M_PLUS,
  ACT_CLOCK_AH_PLUS,
  ACT_CLOCK_AM_PLUS,
  ACT_CLOCK_ALARM_TOGGLE,
  ACT_CLOCK_ALARM_SONG,
  ACT_BATTERY,
  ACT_SCREEN_TIMEOUT,
  ACT_SCREEN_1MIN,
  ACT_SCREEN_2MIN,
  ACT_SCREEN_3MIN,
  ACT_SCREEN_4MIN,
  ACT_SCREEN_5MIN,
  ACT_LED_EFFECT,
  ACT_AUDIO_LIST,
  ACT_AUDIO_REPEAT,
  ACT_AUDIO_SHUFFLE,
  ACT_MUSIC_ORG_YES,
  ACT_MUSIC_ORG_NO,
  ACT_MUSIC_LIST_PREV,
  ACT_MUSIC_LIST_NEXT,
  ACT_MUSIC_LIST_REFRESH
};

struct Button {
  int16_t x;
  int16_t y;
  int16_t w;
  int16_t h;
  const char *label;
  Action action;
};

AppPage currentPage = PAGE_HOME;
unsigned long lastTouchMs = 0;
unsigned long lastRefreshMs = 0;
unsigned long lastCameraMs = 0;
unsigned long lastPulseMs = 0;
int lastX = -1;
int lastY = -1;
uint32_t touchCounter = 0;
bool bootBeepEnabled = false;   // V6: desactivado por defecto

uint8_t i2cFound[20];
uint8_t i2cCount = 0;
bool touchDetected = false;

bool sdTried = false;
bool sdOK = false;
bool cameraTried = false;
bool cameraOK = false;
bool wifiScanned = false;
int wifiCount = -1;
bool cameraWebStarted = false;       // Solo la camara/stream.
bool cameraWebTried = false;
String cameraWebStatusMsg = "Camara parada";
// V43: el servidor web completo (OTA + pestañas + CamWeb) queda apagado
// hasta activarlo manualmente desde la pantalla Servidor Web.
bool webServerEnabled = false;
String webServerStatusMsg = "Web apagada";

int ledR = 0;
int ledG = 0;
int ledB = 80;
int ledBright = 20;
int ledSavedR = 0;
int ledSavedG = 0;
int ledSavedB = 80;
int ledSavedBright = 20;
bool ledPoweredOff = false;
uint8_t ledEffectMode = 0;       // 0 manual, 1 arcoiris, 2 respirar, 3 parpadeo, 4 navidad
uint8_t ledEffectStep = 0;
bool ledBlinkState = false;
unsigned long lastLedEffectMs = 0;

// V27: indicador automatico de actividad microSD usando el WS2812.
// Funciona en cualquier menu.
// 0 = sin indicador, 1 = lectura morado, 2 = escritura amarillo, 3 = lectura+escritura.
uint8_t sdLedShownMode = 0;
uint8_t sdLedBlinkPhaseMode = 1;
uint8_t sdLedBlinkPulseCount = 0;
bool sdLedBlinkOn = false;
unsigned long sdLedReadUntilMs = 0;
unsigned long sdLedWriteUntilMs = 0;
unsigned long sdLedBlinkLastMs = 0;
const unsigned long SD_LED_READ_HOLD_MS = 280;   // V54: evita que el morado quede parpadeando tras lecturas cortas
const unsigned long SD_LED_WRITE_HOLD_MS = 650;  // suficiente para ver escritura real sin quedarse activo demasiado
const unsigned long SD_LED_BLINK_MS = 180;

// V35: indicador de actividad WiFi. Verde parpadeante.
// El parpadeo se acelera segun los bytes por segundo transferidos
// por FTP o por el servidor web de camara.
uint8_t wifiLedShown = 0;
bool wifiLedBlinkOn = false;
unsigned long wifiLedActiveUntilMs = 0;
unsigned long wifiLedLastBlinkMs = 0;
unsigned long wifiLedWindowStartMs = 0;
uint32_t wifiLedWindowBytes = 0;
uint32_t wifiLedBytesPerSec = 0;
uint32_t wifiLedBytesPerSecSmooth = 0;  // V45: velocidad suavizada para LED verde mas estable.
const unsigned long WIFI_LED_HOLD_MS = 1500;
const unsigned long WIFI_LED_WINDOW_MS = 1000;

// V47: arbitro unico del LED de actividad.
// Permite combinar WiFi + microSD sin que una actividad tape a la otra.
// 0 = reposo, 1 = WiFi, 2 = SD lectura, 3 = SD escritura.
uint8_t activityLedShownMode = 0;
uint8_t activityLedGroupMode = 1;
uint8_t activityLedSdAltMode = 1;
uint8_t activityLedPulseCount = 0;
bool activityLedBlinkOn = false;
unsigned long activityLedLastBlinkMs = 0;

Preferences prefs;
uint8_t screenTimeoutMinutes = 1;
bool screenSleeping = false;
bool screenWakeWaitingRelease = false;
unsigned long lastUserActivityMs = 0;

// V41: diagnostico ligero de bloqueo del loop.
unsigned long loopLastTickMs = 0;
unsigned long loopStatsResetMs = 0;
uint16_t loopLastDurationMs = 0;
uint16_t loopWorstDurationMs = 0;

// V50: medicion de bateria/VIN por ADC.
// Ajusta estos dos valores si quieres que el porcentaje sea mas conservador.
// Para una pila rectangular de 9V: llena suele rondar 9.2-9.6V y agotada 6.0-6.5V.
static const float BATTERY_DIVIDER_FACTOR = 4.0f;
static const float BATTERY_ADC_REF_V = 3.3f;
static const float BATTERY_EMPTY_V = 6.2f;
static const float BATTERY_FULL_V = 9.4f;
static const unsigned long BATTERY_READ_INTERVAL_MS = 15000UL;  // V56: actualizar bateria cada 15 segundos
int batteryAdcRaw = 0;
float batteryAdcVoltage = 0.0f;
float batteryVoltage = 0.0f;
int batteryPercent = 0;
bool batteryValid = false;
String batteryStatusMsg = "Sin lectura";
unsigned long batteryLastReadMs = 0;
unsigned long batteryHeaderLastDrawMs = 0;

// -------------------- Botones --------------------
Button homeButtons[] = {
  // V43: dos columnas mas estrechas y con mas separacion horizontal.
  { 10,  48,  94, 32, "Sistema",  ACT_SYSTEM },
  {136,  48,  94, 32, "Diagnost.",ACT_DIAG },
  { 10,  92,  94, 32, "Camara",   ACT_CAMERA },
  {136,  92,  94, 32, "microSD",  ACT_SD },
  { 10, 136,  94, 32, "WiFi",     ACT_WIFI },
  {136, 136,  94, 32, "WS2812",   ACT_WS2812 },
  { 10, 180,  94, 32, "Buzzer",   ACT_BUZZER },
  {136, 180,  94, 32, "Serv/FTP", ACT_CAMERA_WEB },
  { 10, 224,  94, 32, "Musica",   ACT_AUDIO },
  {136, 224,  94, 32, "Reiniciar",ACT_RESTART }
};

Button extraHomeButtons[] = {
  // V56: FTP SD se integra dentro del boton Servidor del menu principal.
  { 10,  52,  94, 34, "Reloj",      ACT_CLOCK },
  {136,  52,  94, 34, "Calcul.",    ACT_CALC },
  { 10, 100,  94, 34, "Bateria",    ACT_BATTERY },
  {136, 100,  94, 34, "Pantalla",   ACT_SCREEN_TIMEOUT },
  { 10, 148,  94, 34, "Menu 1",     ACT_NONE },
  {136, 148,  94, 34, "Reiniciar",  ACT_RESTART }
};

uint8_t homePageIndex = 0;
bool touchActive = false;
bool swipeHandled = false;
bool touchTapConsumed = false;       // V57D: evita doble toque al ejecutar al soltar
bool touchMovedTooMuch = false;      // V57D: distingue toque real de deslizamiento
unsigned long touchStartMs = 0;      // V57D: duracion del toque
const int TOUCH_TAP_MAX_MOVE = 22;   // V57D: margen pequeno para dedo tembloroso
const unsigned long TOUCH_TAP_MAX_MS = 1200;
int swipeStartX = 0;
int swipeStartY = 0;
int swipeLastX = 0;
int swipeLastY = 0;

// Camara V11:
// El Sketch_15_Lvgl_Camera que funciona usa RGB565, intercambia los bytes de cada
// pixel antes de entregarlo a LVGL, y LVGL termina enviando los colores al ST7789
// mediante TFT_eSPI con swap de bytes activado. Aqui se replica esa ruta sin LVGL:
//   1) leer frame RGB565 de esp_camera
//   2) convertir a palabras RGB565 nativas equivalentes al buffer ya intercambiado
//   3) enviar con tft.pushColors(..., true)
//   4) guardar el BMP desde ese mismo buffer para que la foto coincida con pantalla
bool cameraFlipVertical = false;
bool cameraMirrorHorizontal = true;
uint8_t cameraOrientationMode = 0;  // 0 normal, 1 arriba/abajo, 2 izquierda/derecha, 3 180 grados
static uint16_t *cameraTftBuf = nullptr;
static const size_t CAMERA_RGB565_PIXELS = 240UL * 240UL;
static const size_t CAMERA_RGB565_SIZE = CAMERA_RGB565_PIXELS * 2UL;
uint8_t cameraColorMode = 0;  // 0 = Color real estilo Sketch_15_Lvgl_Camera
String lastPhotoPath = "";

#define MAX_PHOTO_FILES 80
String photoFiles[MAX_PHOTO_FILES];
int photoCount = 0;
int photoIndex = 0;
String photoStatusMsg = "";

int wifiSelectedIndex = -1;
String wifiPassword = "";
bool wifiKeyboard = false;
bool wifiUpper = false;
String wifiStatusMsg = "Sin conectar";

// -------------------- WiFi guardado / conexion automatica --------------------
// V10: redes WiFi conocidas. Se conectan automaticamente al arrancar.
// Puedes anadir mas lineas con el mismo formato:
//   { "NOMBRE_WIFI", "CLAVE_WIFI" },
struct KnownWiFiConfig {
  const char *ssid;
  const char *password;
};

const KnownWiFiConfig KNOWN_WIFI[] = {
  { "MERCUSYS_393E", "12345678Israel" },
};
const uint8_t KNOWN_WIFI_COUNT = sizeof(KNOWN_WIFI) / sizeof(KNOWN_WIFI[0]);

// V27/V56: IP estatica preferida para FTP/servicios.
// V56: si la conexion falla con 192.168.1.120, reintenta con una IP aleatoria
// en 192.168.1.121-230 para evitar quedarse bloqueado por una IP ocupada.
const bool WIFI_USE_STATIC_IP = true;
const bool WIFI_RANDOM_IP_IF_STATIC_FAILS = true;
IPAddress WIFI_LOCAL_IP(192, 168, 1, 120);
IPAddress WIFI_GATEWAY(192, 168, 1, 1);
IPAddress WIFI_SUBNET(255, 255, 255, 0);
IPAddress WIFI_DNS1(8, 8, 8, 8);
IPAddress WIFI_DNS2(1, 1, 1, 1);
bool wifiUseRandomStaticIp = false;
IPAddress wifiRuntimeStaticIp(192, 168, 1, 120);

// V29: OTA Web por WiFi. Primera carga siempre por USB; despues puedes subir un .bin desde el navegador.
// No usa ArduinoOTA.h porque en ArduinoDroid puede fallar al compilar bibliotecas internas.
const bool OTA_ENABLED = true;
const char *OTA_HOSTNAME = "esp32s3-freenove";
const char *OTA_PASSWORD = "esp32ota";   // Clave para entrar a la pagina OTA. Cambiala si quieres.
const uint16_t OTA_WEB_PORT = 80;
// V41: WiFi apagado por defecto. Se conecta solo cuando lo pidas desde el menu WiFi.
const bool WIFI_START_MANUAL_ONLY = true;
const char *SPAIN_TZ = "CET-1CEST,M3.5.0/2,M10.5.0/3";
WebServer otaServer(OTA_WEB_PORT);
bool otaReady = false;
bool otaInProgress = false;
uint8_t otaLastPercent = 255;
unsigned long otaLastUiMs = 0;
String otaStatusMsg = "OTA Web sin iniciar";
bool otaRestartPending = false;
unsigned long otaRestartAtMs = 0;

// V34: Arranque Seguro.
// La placa Freenove a veces no arranca bien tras reset muy corto o tras carga USB.
// Este modo da tiempo a que TFT, tactil, SD, WiFi, PSRAM y alimentaciones queden estables.
const bool SAFE_BOOT_ENABLED = true;
const bool SAFE_BOOT_DEFER_WIFI = true;          // Muestra el menu antes de intentar WiFi.
const uint32_t SAFE_BOOT_WIFI_DELAY_MS = 3500;   // WiFi arranca unos segundos despues del menu.
const uint32_t SAFE_BOOT_TFT_POWER_DELAY_MS = 350;
bool safeBootWifiPending = false;
unsigned long safeBootWifiAtMs = 0;
bool safeBootWifiDone = false;


void chooseRandomWiFiStaticIp() {
  // Mantiene la misma red 192.168.1.x usada por tu router, pero evita la .120
  // cuando esa direccion ya este dando problemas.
  uint8_t lastOctet = (uint8_t)random(121, 231);
  wifiRuntimeStaticIp = IPAddress(192, 168, 1, lastOctet);
  wifiUseRandomStaticIp = true;
}

void applyWiFiNetworkConfig() {
  WiFi.setHostname(OTA_HOSTNAME);
  WiFi.setSleep(false);  // V45: evita pausas por ahorro de energia durante CamWeb/FTP.
  if (WIFI_USE_STATIC_IP) {
    IPAddress ip = wifiUseRandomStaticIp ? wifiRuntimeStaticIp : WIFI_LOCAL_IP;
    WiFi.config(ip, WIFI_GATEWAY, WIFI_SUBNET, WIFI_DNS1, WIFI_DNS2);
  }
}

uint32_t clockBaseSeconds = 12UL * 3600UL;
uint32_t clockBaseMillis = 0;
uint8_t alarmHour = 7;
uint8_t alarmMinute = 0;
bool alarmEnabled = false;
bool alarmWasActive = false;
bool clockNtpSynced = false;
bool clockEpochValid = false;
time_t clockBaseEpoch = 0;
String clockDateText = "Fecha sin sincronizar";

// V44: al arrancar conecta una vez a Internet, sincroniza hora de España y apaga WiFi.
const bool STARTUP_SYNC_TIME_ON_BOOT = true;
bool startupTimeSyncDone = false;

// V44: alarma musical. Se puede elegir una cancion de la microSD desde Reloj.
String alarmMusicPath = "";
bool alarmMusicSelectMode = false;
bool alarmMusicActive = false;
String alarmStatusMsg = "Sin cancion alarma";

String calcDisplay = "0";
double calcStored = 0;
char calcOperator = 0;
bool calcNewEntry = true;

// -------------------- Audio SD --------------------
// V21: lista grande de canciones, auto-siguiente, repetir, aleatorio e indice en microSD.
#define MAX_MUSIC_FILES 1024
#define MAX_MUSIC_FOLDERS 64
#define MUSIC_LIST_ROWS 6
#define MUSIC_FOLDER_ROWS 8
#define MUSIC_AUTO_FOLDER_SIZE 50
const char *MUSIC_FOLDER_CONFIG = "/music_folder.txt";
const char *MUSIC_INDEX_FILE = "/music_index.txt";
const char *MUSIC_ROOT_FOLDER = "/music";
const int AUDIO_HW_VOLUME_MAX = 21;      // Audio.h usa volumen 0..21
const int AUDIO_VOLUME_UI_STEPS = 10;    // Botones: 10 secciones, mas fino que saltos de 5
// V57B: en esta placa el tactil puede dar coordenadas inestables cuando se alimenta
// desde USB-OTG de la tablet. Para evitar saltos grandes, las barras son solo visuales
// por defecto. Los ajustes se hacen con botones -5s/+5s y -1/+1.
const bool AUDIO_DIRECT_BAR_TOUCH_ENABLED = false;
const unsigned long AUDIO_SEEK_BUTTON_GUARD_MS = 650;
const unsigned long AUDIO_VOLUME_BUTTON_GUARD_MS = 260;
// V57C: los botones de modo (Repetir/Aleatorio) necesitan una guarda mas larga.
// Si el tactil da pequenos cortes mientras mantienes el dedo, antes podia cambiar
// varias veces el modo y redibujar toda la pantalla, entrecortando la musica.
const unsigned long AUDIO_MODE_BUTTON_GUARD_MS = 900;
const unsigned long MUSIC_SD_READ_LED_PULSE_MS = 180;
String musicFolder = "/music";
String musicBrowserPath = "/";
String musicFiles[MAX_MUSIC_FILES];
int musicCount = 0;
int musicIndex = 0;
int musicListOffset = 0;
bool musicScanned = false;
bool musicIndexSavePending = false;
unsigned long musicIndexSavePendingMs = 0;
bool musicPlaying = false;
bool musicPaused = false;
bool musicShuffle = false;
uint8_t musicRepeatMode = 1;  // 0 normal, 1 repetir lista, 2 repetir una
bool audioChangingTrack = false;
unsigned long lastAudioUiMs = 0;
unsigned long lastAudioStartMs = 0;
unsigned long lastAudioSeekButtonMs = 0;
unsigned long lastAudioVolumeButtonMs = 0;
unsigned long lastAudioRepeatButtonMs = 0;
unsigned long lastAudioShuffleButtonMs = 0;
unsigned long lastMusicSdReadLedMs = 0;
unsigned long lastMusicAutoScanMs = 0;
uint32_t musicFolderFingerprint = 0;
int musicVolume = 10;  // valor interno Audio.h 0..21
String musicFolders[MAX_MUSIC_FOLDERS];
int musicFolderCount = 0;
int musicFolderOffset = 0;
bool musicFolderScanned = false;
bool musicOrganizePending = false;  // V55: nunca ordenar al arrancar; solo tras pulsar Si
bool musicAutoOrganizeChecked = false;
unsigned long musicOrganizePendingMs = 0;
String audioStatusMsg = "";
// V46: modo Musica de alto rendimiento. Al entrar en Musica se paran
// servicios pesados para dar prioridad a Audio.h y a la microSD.
bool musicHighPerfActive = false;
bool musicHighPerfStoppedWiFi = false;
bool musicHighPerfStoppedWeb = false;
bool musicHighPerfStoppedFTP = false;
bool musicHighPerfStoppedCamera = false;
unsigned long musicHighPerfStartedMs = 0;
// V52: flujo de ordenacion musical confirmado por el usuario.
bool musicNeedOrganizeConfirm = false;
bool musicLockedByUnordered = false;
bool musicUserApprovedOrganization = false;  // V55: impide ordenar en segundo plano sin pulsar Si
bool musicIndexBuildPending = false;
uint32_t musicOrganizedMovedTotal = 0;
uint32_t musicIndexBuildLastCount = 0;
String musicOrgStatusMsg = "";

// V53: tareas diferidas para que el tactil no quede bloqueado.
// V54: Musica estable: no reescanea /music en cada refresco, no crea indices
// globales de golpe, y limpia el LED SD al terminar operaciones cortas.
bool musicIndexBuildActive = false;
int musicIndexBuildPos = 0;
unsigned long musicIndexBuildLastMs = 0;
bool musicEofPending = false;
unsigned long musicEofPendingMs = 0;
// V23: evita que un mismo toque mantenido pulse botones muchas veces.
bool audioTouchConsumed = false;
// V23: pausa breve de refrescos automaticos tras tocar controles, para no cortar audio.
unsigned long audioRedrawBlockUntil = 0;
const unsigned long AUDIO_UI_REFRESH_PLAYING_MS = 3000;
const unsigned long AUDIO_UI_REFRESH_IDLE_MS = 5000;
#if HAVE_AUDIO_LIB
Audio audio;
#endif

// -------------------- FTP microSD --------------------
WiFiServer ftpServer(21);
WiFiServer ftpDataServer(50009);
WiFiClient ftpControlClient;
bool ftpEnabled = false;
bool ftpLogged = false;
bool ftpUserOk = false;
String ftpCurrentDir = "/";
String ftpRenameFrom = "";
String ftpControlLine = "";
String ftpStatus = "FTP parado";

// FTP V19: FileZilla puede usar PASV/EPSV o, segun configuracion, PORT/EPRT.
// Antes solo se anunciaba PASV y el canal de datos podia quedarse esperando
// durante STOR, por eso crear carpetas funcionaba pero subir archivos no.
static const uint16_t FTP_PASSIVE_PORT = 50009;
bool ftpActiveReady = false;
IPAddress ftpActiveIp;
uint16_t ftpActivePort = 0;

// -------------------- BLE remoto V58C / App V2 --------------------
// BLE queda pensado como mando ligero: controles, estado, IP, WiFi, OTA, FTP y musica. V2 anade anterior/siguiente y evita restos graficos al controlar volumen desde la app.
// No se usa para enviar camara ni archivos grandes. Para OTA se sigue usando Web OTA por WiFi.
const char *BLE_REMOTE_DEVICE_NAME = "ESP32S3-Freenove-BLE";
const char *BLE_REMOTE_SERVICE_UUID = "b6a00001-7a9b-4d22-93c4-9bb2f2c9a001";
const char *BLE_REMOTE_COMMAND_UUID = "b6a00002-7a9b-4d22-93c4-9bb2f2c9a002";
const char *BLE_REMOTE_STATUS_UUID  = "b6a00003-7a9b-4d22-93c4-9bb2f2c9a003";

#if HAVE_BLE_REMOTE
BLEServer *bleRemoteServer = nullptr;
BLECharacteristic *bleRemoteStatusCharacteristic = nullptr;
bool bleRemoteDeviceConnected = false;
bool bleRemoteOldDeviceConnected = false;
String bleRemotePendingCommand = "";
bool bleRemoteCommandPending = false;
String bleRemoteLastStatus = "BLE iniciado";
unsigned long bleRemoteLastAdvertiseMs = 0;
#endif

// -------------------- Prototipos --------------------
void showSplash();
void drawHeader(const char *title);
void drawFooter(const char *txt);
void drawButton(const Button &b, uint16_t color);
void drawBackButton();
void drawHome();
void drawSystemPage();
void drawDiagPage();
void drawCameraPage();
void drawPhotoViewerPage();
void drawSDPage();
void drawWiFiPage();
void drawWS2812Page();
void drawBuzzerPage();
void drawCameraWebPage();
void drawAudioPage();
void drawMusicFolderPage();
void drawMusicListPage();
void drawFTPPage();
void drawBatteryPage();
void updateBatteryPageDynamic(bool force);
void drawBatteryHeaderWidget();
void drawBatteryHeaderWidget(bool force);  // V57: permite forzar lectura inmediata al arrancar
void drawBatteryIcon(int x, int y, int w, int h, int percent, bool valid);
void batteryInit();
void updateBatteryReading(bool force);
const char *batteryStatusText();
void drawScreenTimeoutPage();
void drawClockPage();
void drawCalcPage();
void drawWifiKeyboard();
void handleWifiKeyboardTouch(int x, int y);
void connectToSelectedWiFi();
void autoConnectKnownWiFi(bool showOnTft);
int knownWiFiIndexForSSID(const String &ssid);
void otaStartIfPossible();
void otaHandle();
void otaForceRebootNow();
uint32_t safeBootDelayForResetReason();
void safeBootStage(const char *msg, uint16_t color);
void safeBootService();
void safeRestartNow();
void handleClockTouch(int x, int y);
void handleScreenTimeoutTouch(int x, int y);
void handleCalcTouch(int x, int y);
void handleAudioTouch(int x, int y);
void handleMusicFolderTouch(int x, int y);
void handleMusicListTouch(int x, int y);
void handleFTPTouch(int x, int y);
void updateSystemPageDynamic(bool force);
void updateClockPageDynamic(bool force);
void updateAudioPageDynamic(bool force);
void drawAudioProgressOnly();
void drawAudioVolumeOnly();
void drawAudioVolumeControlsOnly();
void drawAudioStatusOnly();
void drawAudioRepeatButtonOnly();
void drawAudioShuffleButtonOnly();
void calcPress(const char *key);
String formatDouble(double value);
uint16_t swapRedBlue565(uint16_t c);
uint32_t getClockSeconds();
void alarmCheck();
void drawAnalogClock(int cx, int cy, int r, int hh, int mm, int ss);
void drawStatusLine(int y, const char *name, const char *value, uint16_t color);
void drawBar(int x, int y, int w, int h, int percent, uint16_t color);
void scanI2C();
bool readTouchPoint(int &x, int &y);
void handleTouch(int x, int y);
void drawButtonTouchFrame(const Button &b);
bool pointInButton(int x, int y, const Button &b);
String bytesToText(uint64_t bytes);

bool initSD();
void createSDTestFile();
void listSDFiles(const char *path, int yStart);

bool initCamera();
void deinitCamera();
bool ensureCameraTftBuffer();
bool prepareCameraFrameForTFT(camera_fb_t *fb);
void drawCameraFrame();
const char *cameraColorModeLabel();
void cameraNextColorMode();
const char *cameraOrientationLabel();
void cameraNextOrientationMode();
File sdOpenCompat(const char *path);
File sdOpenCompat(const char *path, const char *mode);
bool sdExistsCompat(const char *path);
bool sdMkdirCompat(const char *path);
bool sdRemoveCompat(const char *path);
bool sdRmdirCompat(const char *path);
bool sdRenameCompat(const char *from, const char *to);
bool saveCameraBMP(const char *path);
bool captureAndSavePhoto();
String nextPhotoPath();
void ensurePictureFolder();
void scanPhotoFiles();
bool drawBmpPhoto(const char *path, int x, int y);
bool isBmpFileName(const String &name);
void handlePhotoViewerTouch(int x, int y);

void scanWiFiAndDraw();

void ws2812Init();
void ws2812SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
uint16_t ws2812UiColor565(uint8_t r, uint8_t g, uint8_t b);
bool ledHasVisibleValue();
void saveLedBeforeOff();
void restoreLedAfterOff();
void drawSliderControl(int x, int y, int w, const char *label, int value, uint16_t color);
bool sliderTouchToValue(int x, int y, int sx, int sy, int sw, int &value);
const char *ledEffectName();
void updateWs2812Effect(bool force);
void activityLedResetPattern(uint8_t mode);
void clearSdActivityLedNow();
void wheelColor(uint8_t pos, uint8_t &r, uint8_t &g, uint8_t &b);
bool sdModeIsWrite(const char *mode);
void sdLedMarkActivity(uint8_t mode);
void sdLedMarkRead();
void sdLedMarkWrite();
uint8_t sdLedDesiredMode();
bool sdLedIndicatorActive();
void ws2812RestoreNormalState();
void updateSdLedIndicator();
void loadScreenTimeoutConfig();
void saveScreenTimeoutConfig();
void screenSetOn(bool on);
void handleScreenSleep();

void buzzerBeep(int freq, int durationMs);
void buzzerMelody();

void scanMusicFiles(bool forceFullScan = false);
bool loadMusicIndexFile();
void saveMusicIndexFile();
uint32_t computeMusicFolderFingerprint();
void musicAutoRescanIfChanged();
bool isMusicFileName(const String &name);
bool isMusicRootPath(const String &path);
bool isDirectChildOfMusicRoot(const String &path);
void organizeMusicRootFiles();
void invalidateMusicListIfPathLooksMusic(const String &path);
void audioStartCurrent();
void audioStopPlayback();
void audioPauseResume();
void audioNextTrack(bool fromEof);
void audioPreviousTrack();
void audioToggleRepeatMode();
void audioToggleShuffle();
const char *audioRepeatLabel();
void audioMonitorAutoNext();
void musicHandleEofPending();
void audioSetSeekPercent(int percent);
void audioSeekRelativeSeconds(int deltaSeconds);
int audioVolumeUiLevel();
void audioSetVolumeUiLevel(int level);
void audioSetVolumeFromX(int x);
void audioChangeVolume(int deltaSections);
void enterMusicHighPerformanceMode();
void leaveMusicHighPerformanceMode();
void prepareMusicFoldersForMusicMode();
bool musicRootHasLooseFiles();
void musicStartConfirmedOrganization();
void musicCancelOrganizationForNow();
void drawMusicOrganizePrompt();
void enterMusicPageFromHome();
String musicIndexPathForFolder(const String &folder);
void saveMusicIndexFileForFolder(const String &folder);
void buildAllMusicFolderIndexes();
void musicStartIndexBuildDeferred();
void musicIndexBuildTick();

void ftpStartStop();
void ftpHandle();
void ftpWriteRaw(WiFiClient &client, const char *text);
void ftpReply(int code, const char *message);

bool initCameraWebMode();
void drawCameraWebPage();
void startCameraServer();
void stopCameraServer();
String camWebBuildPage();
String webPortalPage(const String &activeTab, const String &msg);
void wifiLedMarkBytes(uint32_t bytes);
void wifiLedMarkActivity();
bool wifiLedIndicatorActive();
void updateWifiLedIndicator();
void updateActivityLedIndicator();
bool syncSpainTimeFromInternet(bool showOnTft);
void wifiManualOff();
void startupSyncTimeAndDisconnect();
void updateClockDateTextFromEpoch();
void adjustClockBySeconds(int32_t delta);
void drawAlarmSongPage();
void alarmStartSelectedMusic();
void alarmStopMusicIfActive();
void scrollMusicFolderList(int deltaRows);
void bleRemoteBegin();
void bleRemoteHandle();
void bleRemoteNotify(const String &msg);
void bleRemoteSendStatus();
void bleRemoteSendFolders();
void bleRemoteProcessCommand(String cmd);
void bleRemoteSetClockFromEpoch(time_t epoch);
bool bleRemoteEnsureWifiConnected();
void bleRemoteSetWeb(bool on);
void bleRemoteSetFtp(bool on);
void bleRemoteSetCamera(bool on);


// -------------------- BLE remoto V58 implementacion --------------------
#if HAVE_BLE_REMOTE
class BleRemoteServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *server) override {
    bleRemoteDeviceConnected = true;
    bleRemoteLastStatus = "BLE conectado";
  }

  void onDisconnect(BLEServer *server) override {
    bleRemoteDeviceConnected = false;
    bleRemoteLastStatus = "BLE desconectado";
  }
};

class BleRemoteCommandCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *characteristic) override {
    String cmd = String(characteristic->getValue().c_str());
    cmd.trim();
    if (cmd.length() <= 0) return;
    // No ejecutar acciones pesadas desde el callback BLE. Se procesan en loop().
    bleRemotePendingCommand = cmd;
    bleRemoteCommandPending = true;
  }
};
#endif

void bleRemoteBegin() {
#if HAVE_BLE_REMOTE
  BLEDevice::init(BLE_REMOTE_DEVICE_NAME);
  BLEDevice::setPower(ESP_PWR_LVL_P6);

  bleRemoteServer = BLEDevice::createServer();
  bleRemoteServer->setCallbacks(new BleRemoteServerCallbacks());

  BLEService *service = bleRemoteServer->createService(BLE_REMOTE_SERVICE_UUID);

  BLECharacteristic *commandCharacteristic = service->createCharacteristic(
    BLE_REMOTE_COMMAND_UUID,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
  );
  commandCharacteristic->setCallbacks(new BleRemoteCommandCallbacks());

  bleRemoteStatusCharacteristic = service->createCharacteristic(
    BLE_REMOTE_STATUS_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  bleRemoteStatusCharacteristic->addDescriptor(new BLE2902());
  bleRemoteStatusCharacteristic->setValue("BLE listo");

  service->start();

  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(BLE_REMOTE_SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->setMinPreferred(0x06);
  advertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  bleRemoteLastStatus = String("BLE: ") + BLE_REMOTE_DEVICE_NAME;
#else
  Serial.println("BLE remoto no disponible en este core/libreria");
#endif
}

void bleRemoteHandle() {
#if HAVE_BLE_REMOTE
  if (bleRemoteCommandPending) {
    String cmd = bleRemotePendingCommand;
    bleRemotePendingCommand = "";
    bleRemoteCommandPending = false;
    bleRemoteProcessCommand(cmd);
  }

  // Al desconectar, reanuncia para que la app pueda volver a conectar.
  if (!bleRemoteDeviceConnected && bleRemoteOldDeviceConnected) {
    delay(40);
    BLEDevice::startAdvertising();
    bleRemoteOldDeviceConnected = bleRemoteDeviceConnected;
    bleRemoteLastAdvertiseMs = millis();
  }
  if (bleRemoteDeviceConnected && !bleRemoteOldDeviceConnected) {
    bleRemoteOldDeviceConnected = bleRemoteDeviceConnected;
    bleRemoteNotify("CONNECTED");
    bleRemoteSendStatus();
  }
#endif
}

void bleRemoteNotify(const String &msg) {
#if HAVE_BLE_REMOTE
  if (!bleRemoteStatusCharacteristic) return;
  bleRemoteStatusCharacteristic->setValue(msg.c_str());
  if (bleRemoteDeviceConnected) {
    bleRemoteStatusCharacteristic->notify();
    delay(12);
  }
  bleRemoteLastStatus = msg;
#else
  (void)msg;
#endif
}

bool bleRemoteEnsureWifiConnected() {
  if (WiFi.status() == WL_CONNECTED) return true;
  if (KNOWN_WIFI_COUNT <= 0) {
    bleRemoteNotify("ERR|No hay redes WiFi guardadas");
    return false;
  }
  bleRemoteNotify("INFO|Conectando WiFi...");
  bool ok = connectKnownWiFiByIndex(0, false, 15000);
  if (ok) {
    bleRemoteNotify(String("WIFI|ON|") + WiFi.localIP().toString());
    return true;
  }
  bleRemoteNotify("ERR|No se pudo conectar WiFi");
  return false;
}

void bleRemoteSetWeb(bool on) {
  if (on) {
    if (!bleRemoteEnsureWifiConnected()) return;
    webServerEnabled = true;
    webServerStatusMsg = "Web activa por BLE";
    otaStartIfPossible();
    bleRemoteNotify(String("WEB|") + (otaReady ? "ON|" : "ERR|") + WiFi.localIP().toString());
  } else {
    if (cameraWebStarted) {
      stopCameraServer();
      deinitCamera();
      cameraWebStarted = false;
      cameraWebStatusMsg = "CamWeb parado por BLE";
    }
    if (otaReady) {
      otaServer.close();
      otaReady = false;
    }
    webServerEnabled = false;
    webServerStatusMsg = "Web apagada por BLE";
    bleRemoteNotify("WEB|OFF");
  }
}

void bleRemoteSetFtp(bool on) {
  if (on) {
    if (!bleRemoteEnsureWifiConnected()) return;
    if (!ftpEnabled) ftpStartStop();
    bleRemoteNotify(String("FTP|") + (ftpEnabled ? "ON|" : "ERR|") + ftpStatus);
  } else {
    if (ftpEnabled) ftpStartStop();
    bleRemoteNotify("FTP|OFF");
  }
}

void bleRemoteSetCamera(bool on) {
  if (on) {
    if (!bleRemoteEnsureWifiConnected()) return;
    // La app necesita el portal web en puerto 80 para OTA y el stream/captura en puerto 81.
    webServerEnabled = true;
    webServerStatusMsg = "Web activa por Cam BLE";
    otaStartIfPossible();
    bool ok = initCameraWebMode();
    bleRemoteNotify(String("CAM|") + (ok && cameraWebStarted ? "ON|" : "ERR|") + WiFi.localIP().toString());
  } else {
    if (cameraWebStarted) stopCameraServer();
    deinitCamera();
    cameraWebStarted = false;
    cameraWebStatusMsg = "Camara parada por BLE";
    bleRemoteNotify("CAM|OFF");
  }
}

void bleRemoteSetClockFromEpoch(time_t epoch) {
  if (epoch < 1700000000UL) {
    bleRemoteNotify("ERR|Hora no valida");
    return;
  }
  setenv("TZ", SPAIN_TZ, 1);
  tzset();
  clockBaseEpoch = epoch;
  clockEpochValid = true;
  clockBaseMillis = millis();

  struct tm timeinfo;
  localtime_r(&epoch, &timeinfo);
  clockBaseSeconds = (uint32_t)timeinfo.tm_hour * 3600UL + (uint32_t)timeinfo.tm_min * 60UL + (uint32_t)timeinfo.tm_sec;
  char buf[32];
  snprintf(buf, sizeof(buf), "%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  clockDateText = String(buf);
  clockNtpSynced = false;
  bleRemoteNotify(String("TIME|OK|") + clockDateText);
  if (currentPage == PAGE_CLOCK) drawClockPage();
}

void bleRemoteSendStatus() {
  updateBatteryReading(false);
#if HAVE_AUDIO_LIB
  int bleAudioCur = (musicPlaying || musicPaused) ? (int)audio.getAudioCurrentTime() : 0;
  int bleAudioDur = (musicPlaying || musicPaused) ? (int)audio.getAudioFileDuration() : 0;
#else
  int bleAudioCur = 0;
  int bleAudioDur = 0;
#endif
  String bleTrack = "";
  if (musicCount > 0 && musicIndex >= 0 && musicIndex < musicCount) bleTrack = folderBaseName(musicFiles[musicIndex]);
  bleTrack.replace("|", " ");

  String msg = "STATUS";
  msg += "|wifi="; msg += (WiFi.status() == WL_CONNECTED ? "on" : "off");
  msg += "|ip="; msg += (WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : "0.0.0.0");
  msg += "|web="; msg += (webServerEnabled && otaReady ? "on" : "off");
  msg += "|ftp="; msg += (ftpEnabled ? "on" : "off");
  msg += "|cam="; msg += (cameraWebStarted ? "on" : "off");
  msg += "|music="; msg += (musicPlaying ? (musicPaused ? "pause" : "play") : "stop");
  msg += "|folder="; msg += folderBaseName(musicFolder);
  msg += "|track="; msg += bleTrack;
  msg += "|cur="; msg += String(bleAudioCur);
  msg += "|dur="; msg += String(bleAudioDur);
  msg += "|vol="; msg += String(audioVolumeUiLevel());
  msg += "|bat="; msg += (batteryValid ? String(batteryPercent) : String(-1));
  bleRemoteNotify(msg);
}

void bleRemoteSendFolders() {
  if (!sdTried) initSD();
  if (!sdOK) {
    bleRemoteNotify("ERR|microSD no disponible");
    return;
  }
  // La app lista las carpetas directas dentro de /music.
  musicBrowserPath = MUSIC_ROOT_FOLDER;
  musicFolderOffset = 0;
  musicFolderScanned = false;
  scanMusicFolders();
  bleRemoteNotify(String("FOLDERS_BEGIN|") + String(musicFolderCount));
  for (int i = 0; i < musicFolderCount; i++) {
    String name = folderBaseName(musicFolders[i]);
    String line = String("FOLDER|") + String(i) + "|" + name;
    bleRemoteNotify(line);
    yield();
  }
  bleRemoteNotify("FOLDERS_END");
}

void bleRemoteProcessCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "PING") { bleRemoteNotify("PONG"); return; }
  if (cmd == "GET_STATUS") { bleRemoteSendStatus(); return; }

  if (cmd == "WIFI_ON") {
    bleRemoteEnsureWifiConnected();
    bleRemoteSendStatus();
    return;
  }
  if (cmd == "WIFI_OFF" || cmd == "NET_OFF") {
    bleRemoteSetFtp(false);
    bleRemoteSetWeb(false);
    wifiManualOff();
    bleRemoteNotify("NET|OFF");
    bleRemoteSendStatus();
    return;
  }
  if (cmd == "WEB_ON") { bleRemoteSetWeb(true); bleRemoteSendStatus(); return; }
  if (cmd == "WEB_OFF") { bleRemoteSetWeb(false); bleRemoteSendStatus(); return; }
  if (cmd == "FTP_ON") { bleRemoteSetFtp(true); bleRemoteSendStatus(); return; }
  if (cmd == "FTP_OFF") { bleRemoteSetFtp(false); bleRemoteSendStatus(); return; }
  if (cmd == "CAM_ON" || cmd == "CAMERA_ON") { bleRemoteSetCamera(true); bleRemoteSendStatus(); return; }
  if (cmd == "CAM_OFF" || cmd == "CAMERA_OFF") { bleRemoteSetCamera(false); bleRemoteSendStatus(); return; }

  if (cmd == "MUSIC_PLAY_PAUSE") {
    if (!musicScanned) scanMusicFiles();
    audioPauseResume();
    bleRemoteNotify(String("MUSIC|") + audioStatusMsg);
    bleRemoteSendStatus();
    return;
  }
  if (cmd == "MUSIC_STOP") { audioStopPlayback(); if (currentPage == PAGE_AUDIO) drawAudioPage(); bleRemoteNotify("MUSIC|STOP"); bleRemoteSendStatus(); return; }
  if (cmd == "MUSIC_NEXT") {
    if (!musicScanned) scanMusicFiles();
    audioNextTrack(false);
    if (currentPage == PAGE_AUDIO) drawAudioPage();
    else if (currentPage == PAGE_MUSIC_LIST) drawMusicListPage();
    bleRemoteNotify(String("MUSIC|NEXT|") + (musicCount > 0 ? folderBaseName(musicFiles[musicIndex]) : String("sin canciones")));
    bleRemoteSendStatus();
    return;
  }
  if (cmd == "MUSIC_PREV") {
    if (!musicScanned) scanMusicFiles();
    audioPreviousTrack();
    if (currentPage == PAGE_AUDIO) drawAudioPage();
    else if (currentPage == PAGE_MUSIC_LIST) drawMusicListPage();
    bleRemoteNotify(String("MUSIC|PREV|") + (musicCount > 0 ? folderBaseName(musicFiles[musicIndex]) : String("sin canciones")));
    bleRemoteSendStatus();
    return;
  }
  if (cmd == "VOL_UP") { audioChangeVolume(1); bleRemoteNotify(String("VOLUME|") + String(audioVolumeUiLevel())); bleRemoteSendStatus(); return; }
  if (cmd == "VOL_DOWN") { audioChangeVolume(-1); bleRemoteNotify(String("VOLUME|") + String(audioVolumeUiLevel())); bleRemoteSendStatus(); return; }
  if (cmd.startsWith("VOLUME_SET:")) {
    int level = constrain(cmd.substring(String("VOLUME_SET:").length()).toInt(), 0, AUDIO_VOLUME_UI_STEPS);
    audioSetVolumeUiLevel(level);
    bleRemoteNotify(String("VOLUME|") + String(audioVolumeUiLevel()));
    bleRemoteSendStatus();
    return;
  }
  if (cmd.startsWith("SEEK_PERCENT:")) {
    int pct = constrain(cmd.substring(String("SEEK_PERCENT:").length()).toInt(), 0, 100);
    audioSetSeekPercent(pct);
    bleRemoteNotify(String("SEEK|") + String(pct));
    bleRemoteSendStatus();
    return;
  }
  if (cmd == "MUSIC_STATUS") { bleRemoteSendStatus(); return; }

  if (cmd == "FOLDERS") { bleRemoteSendFolders(); return; }
  if (cmd.startsWith("SELECT_FOLDER:")) {
    int idx = cmd.substring(String("SELECT_FOLDER:").length()).toInt();
    if (idx < 0 || idx >= musicFolderCount) {
      // Si la app selecciona tras una reconexion, recargamos primero.
      bleRemoteSendFolders();
      if (idx < 0 || idx >= musicFolderCount) {
        bleRemoteNotify("ERR|Carpeta no valida");
        return;
      }
    }
    musicFolder = musicFolders[idx];
    saveMusicFolderConfig();
    musicScanned = false;
    musicIndex = 0;
    musicListOffset = 0;
    scanMusicFiles();
    clearSdActivityLedNow();
    bleRemoteNotify(String("FOLDER_SELECTED|") + folderBaseName(musicFolder) + "|" + String(musicCount));
    if (currentPage == PAGE_AUDIO || currentPage == PAGE_MUSIC_FOLDER || currentPage == PAGE_MUSIC_LIST) drawAudioPage();
    return;
  }

  if (cmd.startsWith("TIME:")) {
    String n = cmd.substring(5);
    time_t epoch = (time_t)n.toInt();
    // toInt() es signed 32 bit; para fechas modernas aun sirve hasta 2038.
    bleRemoteSetClockFromEpoch(epoch);
    return;
  }

  bleRemoteNotify(String("ERR|Comando desconocido: ") + cmd);
}

// -------------------- Setup / loop --------------------
void setup() {
  Serial.begin(115200);
  uint32_t bootDelay = safeBootDelayForResetReason();
  delay(bootDelay);
  Serial.println();
  Serial.println("Sketch_21_Rehecho_FULL_ArduinoDroid_V60_Control_ESP32S3_App_V3 arrancando...");
  Serial.print("Reset reason: ");
  Serial.println((int)esp_reset_reason());
  randomSeed((uint32_t)micros());

  // V34: deja WiFi apagado durante la primera parte del arranque.
  // En esta placa, arrancar WiFi/FTP/OTA demasiado pronto despues de un reset corto
  // puede dejar pantalla negra o perifericos sin responder.
  WiFi.mode(WIFI_OFF);
  delay(120);

  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  pinMode(LEDS_PIN, OUTPUT);
  digitalWrite(LEDS_PIN, LOW);

  if (TFT_BACKLIGHT_PIN >= 0) {
    pinMode(TFT_BACKLIGHT_PIN, OUTPUT);
    digitalWrite(TFT_BACKLIGHT_PIN, !TFT_BACKLIGHT_ON_LEVEL);
    delay(SAFE_BOOT_TFT_POWER_DELAY_MS);
    digitalWrite(TFT_BACKLIGHT_PIN, TFT_BACKLIGHT_ON_LEVEL);
  }

  tft.init();
  delay(80);
  tft.setRotation(TFT_ROTATION);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  safeBootStage("Arranque seguro", TFT_CYAN);

  loadScreenTimeoutConfig();
  lastUserActivityMs = millis();

  safeBootStage("Iniciando I2C", TFT_WHITE);
  // V49: no tocar GPIO2 como salida; GPIO2 es SDA del bus tactil.
  Wire.begin(I2C_SDA, I2C_SCL);
  delay(120);

  safeBootStage("Iniciando tactil", TFT_WHITE);
  ft6336u.begin();
  delay(180);

  safeBootStage("Escaneando I2C", TFT_WHITE);
  scanI2C();
  touchDetected = false;
  for (uint8_t i = 0; i < i2cCount; i++) {
    if (i2cFound[i] == 0x38) touchDetected = true;
  }

  safeBootStage("Iniciando LED RGB", TFT_WHITE);
  ws2812Init();
  ws2812SetColor(ledR, ledG, ledB, ledBright);

  safeBootStage("Leyendo bateria", TFT_WHITE);
  batteryInit();
  updateBatteryReading(true);

  safeBootStage("Iniciando BLE", TFT_WHITE);
  bleRemoteBegin();

  if (bootBeepEnabled) {
    safeBootStage("Prueba buzzer", TFT_WHITE);
    buzzerBeep(1200, 70);
  }

  showSplash();
  delay(800);

  if (STARTUP_SYNC_TIME_ON_BOOT && KNOWN_WIFI_COUNT > 0) {
    startupSyncTimeAndDisconnect();
    delay(400);
  }

  if (SAFE_BOOT_DEFER_WIFI) {
    drawHome();
    // V57: actualizar y dibujar bateria inmediatamente en el primer menu,
    // sin esperar a los 15 segundos del intervalo normal.
    updateBatteryReading(true);
    drawBatteryHeaderWidget(true);
    if (WIFI_START_MANUAL_ONLY) {
      safeBootWifiPending = false;
      safeBootWifiDone = true;
      wifiStatusMsg = "WiFi apagado. Pulsa Conectar";
    } else {
      safeBootWifiPending = true;
      safeBootWifiDone = false;
      safeBootWifiAtMs = millis() + SAFE_BOOT_WIFI_DELAY_MS;
    }
  } else {
    autoConnectKnownWiFi(true);
    otaStartIfPossible();
    delay(500);
    drawHome();
    // V57: actualizar y dibujar bateria inmediatamente en el primer menu,
    // sin esperar a los 15 segundos del intervalo normal.
    updateBatteryReading(true);
    drawBatteryHeaderWidget(true);
    safeBootWifiDone = true;
  }
}

void loop() {
  unsigned long loopNow = millis();
  if (loopLastTickMs != 0) {
    unsigned long dt = loopNow - loopLastTickMs;
    if (dt > 60000UL) dt = 60000UL;
    loopLastDurationMs = (uint16_t)dt;
    if (loopLastDurationMs > loopWorstDurationMs) loopWorstDurationMs = loopLastDurationMs;
    if (loopStatsResetMs == 0 || loopNow - loopStatsResetMs > 10000UL) {
      loopStatsResetMs = loopNow;
      loopWorstDurationMs = loopLastDurationMs;
    }
  }
  loopLastTickMs = loopNow;

  // V58: atiende comandos BLE ligeros antes de tareas pesadas.
  bleRemoteHandle();

  // V42: el tactil tiene prioridad absoluta. Primero se lee y se atiende el toque;
  // los servicios pesados se procesan despues y solo si estan realmente activos.
  int x, y;
  bool touched = readTouchPoint(x, y);

  if (touched && screenSleeping) {
    screenSetOn(true);
    screenWakeWaitingRelease = true;
    lastUserActivityMs = millis();
    touchActive = true;
    swipeHandled = true;
  } else if (touched && screenWakeWaitingRelease) {
    lastUserActivityMs = millis();
    touchActive = true;
  } else if (touched) {
    lastUserActivityMs = millis();

    if (!touchActive) {
      touchActive = true;
      swipeHandled = false;
      touchTapConsumed = false;
      touchMovedTooMuch = false;
      touchStartMs = millis();
      swipeStartX = x;
      swipeStartY = y;
      swipeLastX = x;
      swipeLastY = y;
      lastX = x;
      lastY = y;
      touchCounter++;

      if (currentPage == PAGE_HOME) {
        // V57D: en el menu principal NO se ejecuta el boton en el primer contacto.
        // Se espera a soltar el dedo. Asi, si la primera lectura del tactil sale
        // desplazada o si el usuario queria deslizar entre menus, no abre otra cosa.
        lastTouchMs = millis();
      } else if (currentPage == PAGE_MUSIC_FOLDER || currentPage == PAGE_MUSIC_LIST) {
        // V44: en listas dejamos distinguir toque de deslizamiento vertical.
        // No ejecutamos la fila al primer contacto para que se pueda hacer scroll.
        lastTouchMs = millis();
      } else {
        // En paginas interiores se atiende el primer toque al instante.
        lastTouchMs = millis();
        handleTouch(x, y);
        touchTapConsumed = true;
      }
    } else {
      swipeLastX = x;
      swipeLastY = y;
      if (abs(x - swipeStartX) > TOUCH_TAP_MAX_MOVE || abs(y - swipeStartY) > TOUCH_TAP_MAX_MOVE) {
        touchMovedTooMuch = true;
      }

      // V55: salida de emergencia por gesto. Mantener Atras o empezar en el
      // borde izquierdo y deslizar hacia la derecha vuelve al menu principal.
      // Sirve cuando algun proceso hace que el toque normal de Atras responda tarde.
      if (currentPage != PAGE_HOME && !swipeHandled) {
        int dxExit = x - swipeStartX;
        int dyExit = y - swipeStartY;
        bool fromBackButton = (swipeStartX <= 95 && swipeStartY >= 260);
        bool fromLeftEdge = (swipeStartX <= 28);
        if ((fromBackButton || fromLeftEdge) && dxExit > 75 && abs(dyExit) < 85) {
          alarmMusicSelectMode = false;
          musicLockedByUnordered = false;
          musicNeedOrganizeConfirm = false;
          musicUserApprovedOrganization = false;
          if (!musicOrganizePending) musicOrgStatusMsg = "";
          drawHome();
          swipeHandled = true;
          touchMovedTooMuch = true;
          lastTouchMs = millis();
          return;
        }
      }

      if (currentPage == PAGE_HOME && !swipeHandled) {
        int dx = x - swipeStartX;
        int dy = y - swipeStartY;
        if (abs(dx) > 65 && abs(dy) < 80) {
          if (dx < 0 && homePageIndex == 0) {
            homePageIndex = 1;
            drawHome();
          } else if (dx > 0 && homePageIndex == 1) {
            homePageIndex = 0;
            drawHome();
          }
          swipeHandled = true;
          touchMovedTooMuch = true;
          lastTouchMs = millis();
        }
      }

      // V44: scroll vertical por deslizamiento en carpetas/lista de musica.
      if ((currentPage == PAGE_MUSIC_FOLDER || currentPage == PAGE_MUSIC_LIST) && !swipeHandled) {
        int dx = x - swipeStartX;
        int dy = y - swipeStartY;
        if (abs(dy) > 44 && abs(dx) < 95) {
          if (currentPage == PAGE_MUSIC_FOLDER) {
            scrollMusicFolderList(dy < 0 ? MUSIC_FOLDER_ROWS : -MUSIC_FOLDER_ROWS);
          } else {
            musicListOffset += (dy < 0 ? MUSIC_LIST_ROWS : -MUSIC_LIST_ROWS);
            if (musicListOffset < 0) musicListOffset = 0;
            if (musicListOffset >= musicCount) musicListOffset = max(0, musicCount - MUSIC_LIST_ROWS);
            drawMusicListPage();
          }
          swipeHandled = true;
          touchMovedTooMuch = true;
          lastTouchMs = millis();
        }
      }

      // Las paginas con barras necesitan poder responder mientras arrastras.
      // V57C: en Musica las barras son solo visuales, asi que NO repetimos handleTouch
      // mientras el dedo sigue apoyado. Esto evita que Repetir/Aleatorio cambien solos
      // muchas veces cuando el tactil da pequenas lecturas intermitentes.
      if ((currentPage == PAGE_WS2812 || (currentPage == PAGE_AUDIO && AUDIO_DIRECT_BAR_TOUCH_ENABLED)) && millis() - lastTouchMs > 90) {
        lastTouchMs = millis();
        lastX = x;
        lastY = y;
        handleTouch(x, y);
      }
    }
  } else {
    if (touchActive && !swipeHandled) {
      unsigned long touchDuration = millis() - touchStartMs;
      bool looksLikeTap = (!touchMovedTooMuch && touchDuration <= TOUCH_TAP_MAX_MS);

      if ((currentPage == PAGE_MUSIC_FOLDER || currentPage == PAGE_MUSIC_LIST)) {
        lastTouchMs = millis();
        handleTouch(swipeLastX, swipeLastY);
        touchTapConsumed = true;
      } else if (currentPage == PAGE_HOME && looksLikeTap && !touchTapConsumed) {
        // V57D: ejecucion estable al soltar en menu principal.
        // Esto corrige el caso de poder deslizar entre menus, pero que los botones
        // no reaccionen porque la primera lectura del toque no cayo justo en la zona.
        lastTouchMs = millis();
        handleTouch(swipeLastX, swipeLastY);
        touchTapConsumed = true;
      }
    }
    audioTouchConsumed = false;
    screenWakeWaitingRelease = false;
    touchActive = false;
    swipeHandled = false;
    touchTapConsumed = false;
    touchMovedTooMuch = false;
  }

  // V42: nada pesado debe ejecutarse si la pantalla esta dormida o si acabamos de tocar.
  // Un margen pequeno evita que el toque y el redibujado compitan.
  bool justTouched = (millis() - lastTouchMs) < 120;

  if (!screenSleeping && !justTouched && currentPage == PAGE_SYSTEM && millis() - lastRefreshMs > 2000) {
    lastRefreshMs = millis();
    updateSystemPageDynamic(false);
  }

  if (!screenSleeping && !justTouched && millis() - batteryHeaderLastDrawMs > BATTERY_READ_INTERVAL_MS) {
    drawBatteryHeaderWidget();
  }

  if (!screenSleeping && !justTouched && currentPage == PAGE_BATTERY && millis() - lastRefreshMs > 2000) {
    lastRefreshMs = millis();
    updateBatteryPageDynamic(false);
  }

  // Camara TFT: no refrescar demasiado rapido para no robar tiempo al tactil.
  if (!screenSleeping && !justTouched && currentPage == PAGE_CAMERA && cameraOK && millis() - lastCameraMs > 300) {
    lastCameraMs = millis();
    drawCameraFrame();
  }

  if (!screenSleeping && !justTouched && currentPage == PAGE_CLOCK && millis() - lastRefreshMs > 1000) {
    lastRefreshMs = millis();
    updateClockPageDynamic(false);
  }

  if (!screenSleeping && !justTouched && currentPage == PAGE_AUDIO) {
    unsigned long audioRefreshMs = (musicPlaying && !musicPaused) ? AUDIO_UI_REFRESH_PLAYING_MS : AUDIO_UI_REFRESH_IDLE_MS;
    if (millis() - lastAudioUiMs > audioRefreshMs && millis() > audioRedrawBlockUntil) {
      lastAudioUiMs = millis();
      updateAudioPageDynamic(false);
    }
  }

  // Tareas diferidas, solo si no hay FTP/camara web ni toque reciente.
  if (!justTouched && !ftpEnabled && !cameraWebStarted) {
    if (!musicIndexBuildActive) musicSaveIndexIfPending();

    // V53: crear indices de carpetas por pasos. Antes se hacian todos de golpe y
    // podia dejar el tactil sin responder con el LED morado parpadeando.
    if (musicIndexBuildActive && sdOK && !musicPlaying && millis() - musicIndexBuildLastMs > 120UL) {
      musicIndexBuildTick();
      if (currentPage == PAGE_AUDIO && millis() - lastAudioUiMs > 700UL) {
        lastAudioUiMs = millis();
        drawAudioPage();
      }
    }

    // V53: mover menos archivos por pasada para que el loop siga atendiendo el tactil.
    unsigned long musicOrgInterval = musicHighPerfActive ? 160UL : 700UL;
    if (currentPage == PAGE_AUDIO && musicUserApprovedOrganization && musicOrganizePending && sdOK && !musicPlaying && millis() - musicOrganizePendingMs > musicOrgInterval) {
      organizeMusicRootFiles();
      if (musicHighPerfActive && currentPage == PAGE_AUDIO && isMusicRootPath(musicFolder) && !musicOrganizePending && !musicIndexBuildActive) {
        audioStatusMsg = "Musica organizada. Elige Carpeta";
        drawAudioPage();
      } else if (currentPage == PAGE_AUDIO && (musicOrganizePending || musicIndexBuildActive) && millis() - lastAudioUiMs > 700) {
        lastAudioUiMs = millis();
        drawAudioPage();
      }
    }
  }

  // V57B: mientras suena musica, la microSD esta alimentando datos al decodificador.
  // Marcamos actividad de lectura de forma ligera para que el LED morado parpadee,
  // sin escanear archivos ni tocar la tarjeta mas de lo necesario.
  if (musicPlaying && !musicPaused && !audioChangingTrack) {
    unsigned long nowMusicLed = millis();
    if (nowMusicLed - lastMusicSdReadLedMs >= MUSIC_SD_READ_LED_PULSE_MS) {
      lastMusicSdReadLedMs = nowMusicLed;
      sdLedMarkRead();
    }
  }

  if (!justTouched) {
    updateActivityLedIndicator();
  }

  handleScreenSleep();

#if HAVE_AUDIO_LIB
  // V42/V53: no llamar al decodificador si no esta sonando nada.
  if (musicPlaying || musicPaused) {
    audio.loop();
    audioMonitorAutoNext();
  }
  musicHandleEofPending();
#endif

  // V42: servicios de red solo cuando el WiFi esta encendido y conectado.
  if (WiFi.status() == WL_CONNECTED) {
    otaHandle();
    if (ftpEnabled) ftpHandle();
  }

  alarmCheck();
  delay(1);
}

// -------------------- UI general --------------------
void showSplash() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString("Freenove ESP32-S3", SCREEN_W / 2, 38, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString("Sketch 21 rehecho", SCREEN_W / 2, 68, 2);
  tft.drawCentreString("FULL ArduinoDroid V58 BLE", SCREEN_W / 2, 92, 2);
  tft.drawFastHLine(20, 128, 200, TFT_DARKGREY);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawCentreString("TFT + tactil + modulos", SCREEN_W / 2, 150, 2);
  tft.setTextColor(touchDetected ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  tft.drawCentreString(touchDetected ? "FT6336U detectado" : "FT6336U no detectado", SCREEN_W / 2, 178, 2);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawCentreString("Sin LVGL para compilar mejor", SCREEN_W / 2, 232, 2);
}

void drawHeader(const char *title) {
  tft.fillScreen(TFT_BLACK);
  tft.fillRect(0, 0, SCREEN_W, 34, TFT_NAVY);
  // V56: bateria visible en todas las pantallas, colocada de izquierda a derecha.
  // V57: si es la primera cabecera, fuerza lectura para que no salga --% al arrancar.
  drawBatteryHeaderWidget(!batteryValid || batteryLastReadMs == 0);
  tft.setTextColor(TFT_WHITE, TFT_NAVY);
  tft.drawCentreString(title, SCREEN_W / 2, 9, 2);
}

void drawFooter(const char *txt) {
  tft.fillRect(0, SCREEN_H - 22, SCREEN_W, 22, TFT_DARKGREY);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.drawCentreString(txt, SCREEN_W / 2, SCREEN_H - 19, 1);
}

void drawButton(const Button &b, uint16_t color) {
  tft.fillRoundRect(b.x, b.y, b.w, b.h, 7, color);
  tft.drawRoundRect(b.x, b.y, b.w, b.h, 7, TFT_WHITE);
  // V42: el marco tactil se oculta por defecto. La zona real tiene margen invisible
  // para que el boton responda aunque el dedo no caiga perfecto en el centro.
  if (TOUCH_DRAW_HITBOX) drawButtonTouchFrame(b);
  tft.setTextColor(TFT_WHITE, color);
  // Centrado vertical aproximado para botones de distintas alturas.
  int16_t textY = b.y + max<int16_t>(4, (b.h - 16) / 2);
  tft.drawCentreString(b.label, b.x + b.w / 2, textY, 2);
}

void drawButtonTouchFrame(const Button &b) {
  // V41: el marco cian coincide exactamente con la zona tactil real.
  // Sin margenes para evitar que parezca desplazado respecto al boton dibujado.
  int16_t x1 = b.x - TOUCH_BUTTON_MARGIN_X;
  int16_t y1 = b.y - TOUCH_BUTTON_MARGIN_Y;
  int16_t x2 = b.x + b.w + TOUCH_BUTTON_MARGIN_X;
  int16_t y2 = b.y + b.h + TOUCH_BUTTON_MARGIN_Y;
  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 > SCREEN_W) x2 = SCREEN_W;
  if (y2 > SCREEN_H) y2 = SCREEN_H;
  int16_t w = x2 - x1;
  int16_t h = y2 - y1;
  if (w > 2 && h > 2) tft.drawRoundRect(x1, y1, w, h, 7, TFT_CYAN);
}

void drawBackButton() {
  Button back = { 8, 286, 68, 28, "Atras", ACT_BACK };
  drawButton(back, TFT_DARKGREY);
}

void drawHome() {
  if (musicHighPerfActive && (currentPage == PAGE_AUDIO || currentPage == PAGE_MUSIC_FOLDER || currentPage == PAGE_MUSIC_LIST)) {
    leaveMusicHighPerformanceMode();
  }
  currentPage = PAGE_HOME;
  drawHeader(homePageIndex == 0 ? "Menu principal 1/2" : "Menu principal 2/2");
  drawBatteryHeaderWidget();
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawCentreString(homePageIndex == 0 ? "Desliza <- para mas opciones" : "Desliza -> para volver", SCREEN_W / 2, 36, 1);

  if (homePageIndex == 0) {
    for (uint8_t i = 0; i < sizeof(homeButtons) / sizeof(homeButtons[0]); i++) {
      uint16_t c = TFT_BLUE;
      if (homeButtons[i].action == ACT_RESTART) c = TFT_MAROON;
      drawButton(homeButtons[i], c);
    }
  } else {
    for (uint8_t i = 0; i < sizeof(extraHomeButtons) / sizeof(extraHomeButtons[0]); i++) {
      uint16_t c = TFT_DARKGREEN;
      if (extraHomeButtons[i].action == ACT_RESTART) c = TFT_MAROON;
      if (extraHomeButtons[i].action == ACT_NONE) c = TFT_DARKGREY;
      drawButton(extraHomeButtons[i], c);
    }
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawCentreString("Reloj, bateria, pantalla", SCREEN_W / 2, 238, 1);
    tft.drawCentreString("pantalla y ajustes", SCREEN_W / 2, 252, 1);
  }
  drawFooter(homePageIndex == 0 ? "Toca modulo o desliza" : "Nuevas opciones");
}

void drawStatusLine(int y, const char *name, const char *value, uint16_t color) {
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString(name, 10, y, 2);
  tft.setTextColor(color, TFT_BLACK);
  tft.drawString(value, 116, y, 2);
}

void drawSystemPage() {
  currentPage = PAGE_SYSTEM;
  lastRefreshMs = millis();
  drawHeader("Sistema");
  int y = 44;
  drawStatusLine(y, "Chip", ESP.getChipModel(), TFT_GREEN); y += 22;
  drawStatusLine(y, "CPU", (String(ESP.getCpuFreqMHz()) + " MHz").c_str(), TFT_GREEN); y += 22;
  drawStatusLine(y, "Flash", bytesToText(ESP.getFlashChipSize()).c_str(), TFT_GREEN); y += 22;
  drawStatusLine(y, "PSRAM", psramFound() ? bytesToText(ESP.getPsramSize()).c_str() : "NO", psramFound() ? TFT_GREEN : TFT_RED); y += 22;
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Heap libre", 10, y, 2); y += 22;
  tft.drawString("Sketch", 10, y, 2); y += 22;
  tft.drawString("Loop max", 10, y, 2); y += 30;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("RAM libre aprox.", 10, y, 2); y += 22;
  drawBackButton();
  updateSystemPageDynamic(true);
}

void updateSystemPageDynamic(bool force) {
  (void)force;
  int yHeap = 44 + 22 * 4;
  tft.fillRect(116, yHeap, 114, 18, TFT_BLACK);
  drawStatusLine(yHeap, "Heap libre", bytesToText(ESP.getFreeHeap()).c_str(), TFT_CYAN);
  int ySketch = yHeap + 22;
  tft.fillRect(116, ySketch, 114, 18, TFT_BLACK);
  drawStatusLine(ySketch, "Sketch", bytesToText(ESP.getSketchSize()).c_str(), TFT_CYAN);
  int yLoop = ySketch + 22;
  tft.fillRect(116, yLoop, 114, 18, TFT_BLACK);
  drawStatusLine(yLoop, "Loop max", (String(loopWorstDurationMs) + " ms").c_str(), loopWorstDurationMs > 250 ? TFT_RED : (loopWorstDurationMs > 80 ? TFT_ORANGE : TFT_CYAN));
  int yBar = yLoop + 30 + 22;
  int heapPercent = 0;
  uint32_t freeHeap = ESP.getFreeHeap();
  uint32_t approxTotal = freeHeap + 140000;
  if (approxTotal > 0) heapPercent = (int)((freeHeap * 100UL) / approxTotal);
  drawBar(10, yBar, 220, 18, heapPercent, TFT_GREEN);
}


// -------------------- Bateria / VIN --------------------
void batteryInit() {
  pinMode(BATTERY_ADC_PIN, INPUT);
#if defined(ARDUINO_ARCH_ESP32)
  analogReadResolution(12);
  analogSetPinAttenuation(BATTERY_ADC_PIN, ADC_11db);
#endif
}

void updateBatteryReading(bool force) {
  unsigned long now = millis();
  if (!force && batteryLastReadMs != 0 && now - batteryLastReadMs < BATTERY_READ_INTERVAL_MS) return;
  batteryLastReadMs = now;

  uint32_t sum = 0;
  const uint8_t samples = 6;
  for (uint8_t i = 0; i < samples; i++) {
    sum += analogRead(BATTERY_ADC_PIN);
    delay(1);
  }
  batteryAdcRaw = (int)(sum / samples);
  batteryAdcVoltage = (float)batteryAdcRaw / 4095.0f * BATTERY_ADC_REF_V;
  batteryVoltage = batteryAdcVoltage * BATTERY_DIVIDER_FACTOR;

  if (batteryAdcRaw < 20 || batteryVoltage < 0.5f) {
    batteryValid = false;
    batteryPercent = 0;
    batteryStatusMsg = "Sin senal ADC";
    return;
  }

  batteryValid = true;
  float pct = (batteryVoltage - BATTERY_EMPTY_V) * 100.0f / (BATTERY_FULL_V - BATTERY_EMPTY_V);
  batteryPercent = constrain((int)(pct + 0.5f), 0, 100);
  batteryStatusMsg = String(batteryStatusText());
}

const char *batteryStatusText() {
  if (!batteryValid) return "No detectada";
  if (batteryVoltage >= BATTERY_FULL_V) return "Llena / alta";
  if (batteryPercent >= 60) return "OK";
  if (batteryPercent >= 30) return "Media";
  if (batteryPercent >= 15) return "Baja";
  return "Muy baja";
}

void drawBatteryIcon(int x, int y, int w, int h, int percent, bool valid) {
  if (w < 18 || h < 8) return;
  int nubW = max(2, w / 10);
  int bodyW = w - nubW - 2;
  uint16_t color = TFT_DARKGREY;
  if (valid) {
    if (percent <= 15) color = TFT_RED;
    else if (percent <= 35) color = TFT_ORANGE;
    else color = TFT_GREEN;
  }
  tft.drawRect(x, y, bodyW, h, TFT_WHITE);
  tft.drawRect(x + bodyW + 1, y + h / 3, nubW, h / 3 + 1, TFT_WHITE);
  tft.fillRect(x + 1, y + 1, bodyW - 2, h - 2, TFT_BLACK);
  if (valid) {
    int fillW = (bodyW - 2) * constrain(percent, 0, 100) / 100;
    if (fillW > 0) tft.fillRect(x + 1, y + 1, fillW, h - 2, color);
  }
}

void drawBatteryHeaderWidget() {
  drawBatteryHeaderWidget(false);
}

void drawBatteryHeaderWidget(bool force) {
  // V57: al arrancar se puede forzar la lectura para mostrar porcentaje real
  // inmediatamente. Despues vuelve al intervalo normal de 15 segundos.
  updateBatteryReading(force);
  batteryHeaderLastDrawMs = millis();
  tft.fillRect(1, 4, 62, 24, TFT_NAVY);
  drawBatteryIcon(4, 9, 27, 12, batteryPercent, batteryValid);
  tft.setTextColor(batteryValid ? TFT_WHITE : TFT_LIGHTGREY, TFT_NAVY);
  String pct = batteryValid ? (String(batteryPercent) + "%") : String("--%");
  tft.drawString(pct.c_str(), 35, 8, 1);
}

void drawBatteryPage() {
  currentPage = PAGE_BATTERY;
  lastRefreshMs = millis();
  drawHeader("Bateria");
  drawBackButton();
  updateBatteryPageDynamic(true);
}

void updateBatteryPageDynamic(bool force) {
  updateBatteryReading(force);
  tft.fillRect(0, 36, SCREEN_W, 238, TFT_BLACK);
  int y = 42;
  drawStatusLine(y, "Voltaje", batteryValid ? (String(batteryVoltage, 2) + " V").c_str() : "sin senal", batteryValid ? TFT_GREEN : TFT_ORANGE); y += 22;
  drawStatusLine(y, "Carga", batteryValid ? (String(batteryPercent) + " %").c_str() : "--", batteryValid ? (batteryPercent <= 15 ? TFT_RED : (batteryPercent <= 35 ? TFT_ORANGE : TFT_GREEN)) : TFT_ORANGE); y += 22;
  drawStatusLine(y, "Estado", batteryStatusMsg.c_str(), batteryValid ? (batteryPercent <= 15 ? TFT_RED : (batteryPercent <= 35 ? TFT_ORANGE : TFT_GREEN)) : TFT_ORANGE); y += 22;
  drawStatusLine(y, "ADC raw", String(batteryAdcRaw).c_str(), TFT_CYAN); y += 22;
  drawStatusLine(y, "ADC pin", (String(batteryAdcVoltage, 3) + " V").c_str(), TFT_CYAN); y += 22;
  drawStatusLine(y, "Entrada", "GPIO19 x4", TFT_CYAN); y += 28;

  drawBatteryIcon(24, y, 150, 34, batteryPercent, batteryValid);
  tft.setTextColor(batteryValid ? TFT_WHITE : TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString(batteryValid ? (String(batteryPercent) + "%").c_str() : "--%", 184, y + 8, 2);
  y += 48;

  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Calibrado para pila 9V", 10, y, 1); y += 16;
  tft.drawString("Rango aprox: 6.2V-9.4V", 10, y, 1); y += 16;
  tft.drawString("No conectar 9V a GPIO19.", 10, y, 1); y += 16;
  tft.drawString("Usa la entrada/bateria de la placa.", 10, y, 1);
}

void drawDiagPage() {
  currentPage = PAGE_DIAG;
  scanI2C();
  drawHeader("Diagnostico");
  int y = 42;
  drawStatusLine(y, "Tactil", touchDetected ? "OK" : "NO", touchDetected ? TFT_GREEN : TFT_ORANGE); y += 20;
  drawStatusLine(y, "microSD", sdTried ? (sdOK ? "OK" : "FALLO") : "sin probar", sdOK ? TFT_GREEN : TFT_ORANGE); y += 20;
  drawStatusLine(y, "Camara", cameraTried ? (cameraOK ? "OK" : "FALLO") : "sin probar", cameraOK ? TFT_GREEN : TFT_ORANGE); y += 20;
  drawStatusLine(y, "WiFi", WiFi.status() == WL_CONNECTED ? "conectado" : "manual/off", WiFi.status() == WL_CONNECTED ? TFT_GREEN : TFT_ORANGE); y += 20;
  drawStatusLine(y, "Loop max", (String(loopWorstDurationMs) + " ms").c_str(), loopWorstDurationMs > 250 ? TFT_RED : (loopWorstDurationMs > 80 ? TFT_ORANGE : TFT_GREEN)); y += 20;
  drawStatusLine(y, "Toque", (String(lastX) + "," + String(lastY)).c_str(), TFT_CYAN); y += 20;
  drawStatusLine(y, "I2C libre", "MAX30102 quitado", TFT_CYAN); y += 18;

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("I2C:", 10, y, 2); y += 22;
  if (i2cCount == 0) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("No encontrados", 10, y, 2);
  } else {
    int x = 10;
    for (uint8_t i = 0; i < i2cCount; i++) {
      char buf[8];
      snprintf(buf, sizeof(buf), "0x%02X", i2cFound[i]);
      tft.setTextColor(i2cFound[i] == 0x38 ? TFT_GREEN : TFT_CYAN, TFT_BLACK);
      tft.drawString(buf, x, y, 2);
      x += 58;
      if (x > 190) { x = 10; y += 20; }
    }
  }

  Button rescan = { 92, 286, 136, 28, "Reescanear", ACT_RESCAN_I2C };
  drawBackButton();
  drawButton(rescan, TFT_BLUE);
}

void drawCameraPage() {
#if HAVE_AUDIO_LIB
  if (musicPlaying || musicPaused) audio.stopSong();
#endif
  musicPlaying = false;
  musicPaused = false;
  if (cameraWebStarted) {
    stopCameraServer();
    cameraWebStarted = false;
    cameraWebStatusMsg = "Parado por camara TFT";
    esp_camera_deinit();
    cameraOK = false;
  }
  currentPage = PAGE_CAMERA;
  drawHeader("Camara");
  if (!cameraTried) {
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("Iniciando camara...", 10, 56, 2);
    initCamera();
  }
  if (!cameraOK) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Camara no iniciada", 10, 60, 2);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("Comprueba PSRAM/pines", 10, 86, 2);
  } else {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Camara OK", 10, 36, 2);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString(cameraColorModeLabel(), 98, 36, 1);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString(cameraOrientationLabel(), 4, 26, 1);
    drawCameraFrame();
  }
  Button back  = {   4, 286, 42, 28, "Atras", ACT_BACK };
  Button mode  = {  50, 286, 42, 28, "Color", ACT_CAM_COLOR };
  Button rot   = {  96, 286, 42, 28, "Rotar", ACT_CAM_ROTATE };
  Button cap   = { 142, 286, 42, 28, "Foto",  ACT_CAM_CAPTURE };
  Button save  = { 188, 286, 48, 28, "Galer", ACT_PHOTO_VIEWER };
  drawButton(back, TFT_DARKGREY);
  drawButton(mode, TFT_ORANGE);
  drawButton(rot, TFT_MAGENTA);
  drawButton(cap, TFT_BLUE);
  drawButton(save, TFT_DARKGREEN);
}


void drawPhotoViewerPage() {
  currentPage = PAGE_PHOTOS;
  drawHeader("Galeria fotos");
  if (!sdTried) initSD();

  if (!sdOK) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("microSD no disponible", 10, 60, 2);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("Las fotos estan en /picture", 10, 86, 2);
  } else {
    scanPhotoFiles();
    if (photoCount <= 0) {
      tft.setTextColor(TFT_ORANGE, TFT_BLACK);
      tft.drawString("No hay fotos en /picture", 10, 60, 2);
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      tft.drawString("Pulsa Foto en Camara", 10, 86, 2);
    } else {
      if (photoIndex < 0) photoIndex = 0;
      if (photoIndex >= photoCount) photoIndex = photoCount - 1;
      String title = String(photoIndex + 1) + "/" + String(photoCount) + " " + photoFiles[photoIndex];
      if (title.length() > 30) title = title.substring(title.length() - 30);
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      tft.drawString(title.c_str(), 4, 36, 1);
      if (!drawBmpPhoto(photoFiles[photoIndex].c_str(), 0, 40)) {
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawString("No se pudo abrir BMP", 10, 70, 2);
      }
    }
  }

  Button prev = { 82, 286, 44, 28, "<", ACT_PHOTO_PREV };
  Button next = { 132, 286, 44, 28, ">", ACT_PHOTO_NEXT };
  Button refresh = { 182, 286, 50, 28, "Act.", ACT_PHOTO_REFRESH };
  drawBackButton();
  drawButton(prev, TFT_BLUE);
  drawButton(next, TFT_BLUE);
  drawButton(refresh, TFT_DARKGREEN);
}

void drawSDPage() {
  currentPage = PAGE_SD;
  drawHeader("microSD");
  if (!sdTried) initSD();
  int y = 44;
  if (!sdOK) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("microSD no iniciada", 10, y, 2); y += 24;
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("Usa ranura de la placa", 10, y, 2); y += 22;
    tft.drawString("y formato FAT32/exFAT", 10, y, 2);
  } else {
    uint64_t cardSize = SD_MMC.cardSize();
    uint64_t used = SD_MMC.usedBytes();
    uint64_t total = SD_MMC.totalBytes();
    drawStatusLine(y, "Tarjeta", bytesToText(cardSize).c_str(), TFT_GREEN); y += 22;
    drawStatusLine(y, "Total FS", bytesToText(total).c_str(), TFT_CYAN); y += 22;
    drawStatusLine(y, "Usado", bytesToText(used).c_str(), TFT_CYAN); y += 28;
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Archivos raiz:", 10, y, 2); y += 22;
    listSDFiles("/", y);
  }
  Button refresh = { 82, 286, 70, 28, "Refres.", ACT_SD_REFRESH };
  Button test = { 158, 286, 74, 28, "Test", ACT_SD_TEST };
  drawBackButton();
  drawButton(refresh, TFT_BLUE);
  drawButton(test, TFT_DARKGREEN);
}

void drawWiFiPage() {
  currentPage = PAGE_WIFI;
  if (wifiKeyboard) {
    drawWifiKeyboard();
    return;
  }
  drawHeader("WiFi");
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString(wifiStatusMsg, 10, 38, 1);

  if (WiFi.status() == WL_CONNECTED) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Conectado:", 10, 52, 2);
    String ip = WiFi.localIP().toString();
    tft.drawString(ip, 108, 52, 2);
    tft.setTextColor((webServerEnabled && otaReady) ? TFT_CYAN : TFT_ORANGE, TFT_BLACK);
    tft.drawString((webServerEnabled && otaReady) ? "Web/OTA listo" : "Web apagada", 10, 72, 1);
    tft.drawString(OTA_HOSTNAME, 92, 72, 1);
  }

  if (!wifiScanned) {
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    if (WiFi.status() == WL_CONNECTED) {
      tft.drawString("Pulsa Apagar si no usas", 10, 92, 2);
      tft.drawString("red/FTP/OTA/camara web", 10, 116, 2);
    } else {
      tft.drawString("WiFi apagado por defecto", 10, 86, 2);
      tft.drawString("Conectar usa red guardada", 10, 110, 2);
      tft.drawString("Escanear para otra red", 10, 134, 2);
    }
  } else {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Toca una red:", 10, 74, 2);
    int maxShow = min(wifiCount, 5);
    for (int i = 0; i < maxShow; i++) {
      int y = 98 + i * 32;
      uint16_t bg = (i == wifiSelectedIndex) ? TFT_DARKGREEN : TFT_NAVY;
      tft.fillRoundRect(6, y, 228, 27, 5, bg);
      tft.drawRoundRect(6, y, 228, 27, 5, TFT_DARKGREY);
      String ssid = WiFi.SSID(i);
      if (ssid.length() > 17) ssid = ssid.substring(0, 17);
      String line = ssid + " " + String(WiFi.RSSI(i)) + "dBm";
      tft.setTextColor(TFT_WHITE, bg);
      tft.drawString(line, 12, y + 6, 1);
    }
    if (wifiCount == 0) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.drawString("Ninguna red", 10, 102, 2);
    }
  }

  Button scan = { 82, 286, 72, 28, "Escan.", ACT_WIFI_SCAN };
  Button conn = { 158, 286, 74, 28, WiFi.status() == WL_CONNECTED ? "Apagar" : "Conect.", ACT_WIFI_CONNECT };
  drawBackButton();
  drawButton(scan, TFT_BLUE);
  drawButton(conn, WiFi.status() == WL_CONNECTED || KNOWN_WIFI_COUNT > 0 || wifiSelectedIndex >= 0 ? TFT_DARKGREEN : TFT_DARKGREY);
}

void drawWS2812Page() {
  currentPage = PAGE_WS2812;
  drawHeader("LED RGB WS2812");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("GPIO48 / 1 LED", 10, 38, 1);

  tft.fillRoundRect(34, 54, 172, 28, 6, ws2812UiColor565(ledR, ledG, ledB));
  tft.drawRoundRect(34, 54, 172, 28, 6, TFT_WHITE);

  char buf[64];
  snprintf(buf, sizeof(buf), "%s R:%d G:%d B:%d Br:%d", ledPoweredOff ? "OFF" : "ON", ledR, ledG, ledB, ledBright);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString(buf, SCREEN_W / 2, 86, 1);

  drawSliderControl(10, 104, 220, "Brillo", ledBright, TFT_WHITE);
  drawSliderControl(10, 140, 220, "Rojo", ledR, ws2812UiColor565(255, 0, 0));
  drawSliderControl(10, 176, 220, "Verde", ledG, ws2812UiColor565(0, 255, 0));
  drawSliderControl(10, 212, 220, "Azul", ledB, ws2812UiColor565(0, 0, 255));

  Button effect = {  8, 248, 110, 28, "Efecto", ACT_LED_EFFECT };
  Button off    = {128, 248, 104, 28, ledPoweredOff ? "Encender" : "Apagar", ACT_LED_OFF };
  drawButton(effect, ledEffectMode == 0 ? TFT_DARKGREY : TFT_ORANGE);
  drawButton(off, ledPoweredOff ? TFT_DARKGREEN : TFT_MAROON);

  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  String effectText = String("Modo: ") + ledEffectName();
  if (effectText.length() > 26) effectText = effectText.substring(0, 26);
  tft.drawCentreString(effectText.c_str(), SCREEN_W / 2, 234, 1);

  drawBackButton();
}

void drawBuzzerPage() {
  currentPage = PAGE_BUZZER;
  drawHeader("Buzzer");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("GPIO45", 10, 48, 2);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Pitido inicio por defecto: OFF", 10, 76, 2);
  tft.setTextColor(bootBeepEnabled ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  tft.drawString(bootBeepEnabled ? "Inicio: activado" : "Inicio: desactivado", 10, 102, 2);
  Button beep = { 28, 128, 184, 34, "Beep", ACT_BUZZER_BEEP };
  Button melody = { 28, 170, 184, 34, "Melodia", ACT_BUZZER_MELODY };
  Button boot = { 28, 212, 184, 34, bootBeepEnabled ? "Inicio OFF" : "Inicio ON", ACT_BUZZER_BOOT_TOGGLE };
  drawButton(beep, TFT_BLUE);
  drawButton(melody, TFT_DARKGREEN);
  drawButton(boot, TFT_ORANGE);
  drawBackButton();
}

void drawCameraWebPage() {
  currentPage = PAGE_CAMERA_WEB;
  drawHeader("Servidor/FTP");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  int y = 44;

  if (WiFi.status() != WL_CONNECTED) {
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.drawString("WiFi apagado", 10, y, 2); y += 24;
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("Servidor Web y FTP estan OFF", 10, y, 1); y += 18;
    tft.drawString("Pulsa Conectar para usar", 10, y, 2); y += 22;
    tft.drawString("OTA Web, CamWeb o FTP SD.", 10, y, 1);

    Button conn = { 28, 214, 184, 34, "Conectar WiFi", ACT_WIFI_CONNECT };
    drawButton(conn, TFT_DARKGREEN);
  } else {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString((String("IP: ") + WiFi.localIP().toString()).c_str(), 10, y, 2); y += 22;

    tft.setTextColor(webServerEnabled && otaReady ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
    tft.drawString(webServerEnabled && otaReady ? "Web/OTA: ON" : "Web/OTA: OFF", 10, y, 1);
    tft.setTextColor(ftpEnabled ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
    tft.drawString(ftpEnabled ? "FTP: ON" : "FTP: OFF", 132, y, 1); y += 18;

    if (webServerEnabled && otaReady) {
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      tft.drawString((String("OTA: ") + WiFi.localIP().toString() + "/ota").c_str(), 10, y, 1); y += 16;
      tft.drawString((String("Cam: ") + WiFi.localIP().toString() + "/cam").c_str(), 10, y, 1); y += 16;
    } else {
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      tft.drawString("Web apagada hasta activarla", 10, y, 1); y += 16;
    }
    tft.setTextColor(cameraWebStarted ? TFT_GREEN : TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString(cameraWebStarted ? "Camara web: ON" : "Camara web: OFF", 10, y, 1); y += 16;
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString(cameraWebStatusMsg.c_str(), 10, y, 1);

    Button web = { 10, 150, 104, 34, webServerEnabled ? "Web OFF" : "Web ON", ACT_WEB_SERVER_TOGGLE };
    Button ftp = {126, 150, 104, 34, ftpEnabled ? "FTP OFF" : "FTP ON", ACT_FTP_TOGGLE };
    drawButton(web, webServerEnabled ? TFT_MAROON : TFT_DARKGREEN);
    drawButton(ftp, ftpEnabled ? TFT_MAROON : TFT_DARKGREEN);

    if (webServerEnabled && otaReady) {
      Button start = { 10, 196, 104, 32, cameraWebStarted ? "Cam rein." : "Cam ON", ACT_CAMERA_WEB_START };
      Button stop  = {126, 196, 104, 32, "Cam OFF", ACT_WEB_CAMERA_STOP };
      drawButton(start, TFT_BLUE);
      drawButton(stop, TFT_RED);
    }
  }
  drawBackButton();
}

void enterMusicPageFromHome() {
  // V55: cada entrada real desde el menu vuelve a comprobar si hay MP3 sueltos
  // en /music, pero NO mueve nada hasta que pulses Si.
  musicLockedByUnordered = false;
  musicNeedOrganizeConfirm = false;
  musicUserApprovedOrganization = false;
  musicOrganizePending = false;
  musicIndexBuildActive = false;
  musicIndexBuildPending = false;
  musicAutoOrganizeChecked = false;
  musicOrgStatusMsg = "";
  audioStatusMsg = "";
  drawAudioPage();
}

void drawAudioPage() {
  enterMusicHighPerformanceMode();
  if (cameraOK || currentPage == PAGE_CAMERA) {
    deinitCamera();
    cameraOK = false;
    cameraTried = false;
  }
  currentPage = PAGE_AUDIO;
  drawHeader("Musica SD");
  if (!sdTried) initSD();
  loadMusicFolderConfig();
  prepareMusicFoldersForMusicMode();
  if (musicNeedOrganizeConfirm) {
    drawMusicOrganizePrompt();
    drawBackButton();
    return;
  }
  if (musicLockedByUnordered) {
    tft.fillRect(0, 34, SCREEN_W, 248, TFT_BLACK);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.drawString("Musica sin ordenar", 10, 58, 2);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("No se reproducira hasta", 10, 92, 2);
    tft.drawString("aceptar la ordenacion.", 10, 116, 2);
    tft.drawString("Sal y vuelve a Musica.", 10, 150, 2);
    drawBackButton();
    return;
  }
  if (musicIndexBuildActive) {
    tft.fillRect(0, 34, SCREEN_W, 248, TFT_BLACK);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("Creando indices...", 10, 58, 2);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString((String("Carpeta ") + String(musicIndexBuildLastCount) + "/" + String(musicFolderCount)).c_str(), 10, 92, 2);
    tft.drawString("Esto evita listar de nuevo", 10, 126, 2);
    tft.drawString("las canciones al abrir.", 10, 150, 2);
    drawBackButton();
    return;
  }
  if (musicOrganizePending) {
    tft.fillRect(0, 34, SCREEN_W, 248, TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Ordenando musica...", 10, 58, 2);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString(musicOrgStatusMsg.length() ? musicOrgStatusMsg : audioStatusMsg, 10, 92, 2);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("Guardando en carpetas", 10, 126, 2);
    tft.drawString("de 50 canciones.", 10, 150, 2);
    tft.drawString("Espera a que termine.", 10, 184, 2);
    drawBackButton();
    return;
  }
  // V25: /music es solo carpeta contenedora. Primero reparte canciones sueltas
  // en /music/musica uno, /music/musica dos, etc.; despues debes elegir una
  // subcarpeta concreta para no listar cientos de canciones a la vez.
  if (isMusicRootPath(musicFolder)) {
    musicScanned = false;
    musicCount = 0;
    if (audioStatusMsg.length() == 0) audioStatusMsg = "Elige Carpeta";
  } else if (!musicScanned) {
    scanMusicFiles();
  }
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
#if HAVE_AUDIO_LIB
  tft.drawString("Audio.h activo", 10, 36, 1);
#else
  tft.drawString("Audio.h no instalado: solo beep", 10, 36, 1);
#endif
  String folderLine = "Carpeta: " + musicFolder;
  if (folderLine.length() > 30) folderLine = "..." + folderLine.substring(folderLine.length() - 27);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString(folderLine.c_str(), 10, 48, 1);

  if (musicLockedByUnordered || musicOrganizePending || musicIndexBuildActive) {
    return;
  }

  Button prev = {  8, 226, 48, 28, "<<", ACT_AUDIO_PREV };
  Button play = {  62,226, 52, 28, (musicPlaying && !musicPaused) ? "Pausa" : "Play", ACT_AUDIO_PLAY };
  Button stop = { 120,226, 52, 28, "Stop", ACT_AUDIO_STOP };
  Button next = { 178,226, 54, 28, ">>", ACT_AUDIO_NEXT };
  Button list = {   8,258, 68, 24, "Lista", ACT_AUDIO_LIST };
  Button repeat = {82,258, 72, 24, audioRepeatLabel(), ACT_AUDIO_REPEAT };
  Button shuffle = {160,258,72, 24, musicShuffle ? "Aleat.ON" : "Aleat.", ACT_AUDIO_SHUFFLE };
  Button folder = { 82, 286, 72, 28, "Carpeta", ACT_AUDIO_FOLDER };
  Button refresh = { 158, 286, 74, 28, "Refres.", ACT_AUDIO_REFRESH };

  drawButton(prev, TFT_BLUE);
  drawButton(play, TFT_DARKGREEN);
  drawButton(stop, TFT_MAROON);
  drawButton(next, TFT_BLUE);
  drawButton(list, TFT_NAVY);
  drawAudioRepeatButtonOnly();
  drawAudioShuffleButtonOnly();
  drawBackButton();
  drawButton(folder, TFT_DARKGREEN);
  drawButton(refresh, TFT_DARKGREY);
  updateAudioPageDynamic(true);
}

void drawMusicListPage() {
  currentPage = PAGE_MUSIC_LIST;
  drawHeader(alarmMusicSelectMode ? "Cancion alarma" : "Lista canciones");
  if (!sdTried) initSD();
  if (!musicScanned) scanMusicFiles();
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  String info = alarmMusicSelectMode ? String("Toca cancion alarma") : String(musicCount) + " canciones";
  if (musicCount > 0) info += "  " + String(musicListOffset + 1) + "-" + String(min(musicListOffset + MUSIC_LIST_ROWS, musicCount));
  tft.drawString(info.c_str(), 8, 36, 1);

  if (!sdOK) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("microSD no disponible", 10, 72, 2);
  } else if (musicCount <= 0) {
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.drawString("No hay canciones", 10, 72, 2);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("Pulsa Act. o cambia carpeta", 10, 102, 2);
  } else {
    if (musicListOffset < 0) musicListOffset = 0;
    if (musicListOffset >= musicCount) musicListOffset = max(0, musicCount - MUSIC_LIST_ROWS);
    for (int i = 0; i < MUSIC_LIST_ROWS; i++) {
      int idx = musicListOffset + i;
      if (idx >= musicCount) break;
      int y = 54 + i * 37;
      bool selected = alarmMusicSelectMode ? (musicFiles[idx] == alarmMusicPath) : (idx == musicIndex);
      uint16_t bg = selected ? TFT_DARKGREEN : TFT_NAVY;
      tft.fillRoundRect(6, y, 228, 32, 5, bg);
      tft.drawRoundRect(6, y, 228, 32, 5, selected ? TFT_GREEN : TFT_DARKGREY);
      String label = folderBaseName(musicFiles[idx]);
      if (label.length() > 24) label = label.substring(0, 24);
      tft.setTextColor(TFT_WHITE, bg);
      tft.drawString(label.c_str(), 12, y + 8, 2);
    }
  }

  Button up = { 82, 286, 44, 28, "Arr.", ACT_MUSIC_LIST_PREV };
  Button down = { 130, 286, 44, 28, "Abj.", ACT_MUSIC_LIST_NEXT };
  Button refresh = { 178, 286, 56, 28, "Act.", ACT_MUSIC_LIST_REFRESH };
  drawBackButton();
  drawButton(up, TFT_BLUE);
  drawButton(down, TFT_BLUE);
  drawButton(refresh, TFT_DARKGREY);
}

void drawMusicFolderPage() {
  currentPage = PAGE_MUSIC_FOLDER;
  drawHeader("Elegir carpeta musica");
  if (musicBrowserPath.length() == 0 || musicBrowserPath == "/") musicBrowserPath = MUSIC_ROOT_FOLDER;
  if (!sdTried) initSD();
  if (!sdOK) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("microSD no disponible", 10, 60, 2);
  } else {
    // V52: no ordenar aqui sin permiso. La pregunta aparece al abrir Musica.
    scanMusicFolders();
    String shown = musicBrowserPath;
    if (shown.length() > 28) shown = "..." + shown.substring(shown.length() - 25);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString(shown.c_str(), 8, 38, 1);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("Toca carpeta para entrar", 8, 54, 1);
    if (musicFolderCount <= 0) {
      tft.setTextColor(TFT_ORANGE, TFT_BLACK);
      tft.drawString("No hay subcarpetas", 10, 92, 2);
      tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
      tft.drawString("Pulsa Usar para esta", 10, 118, 2);
    } else {
      if (musicFolderOffset < 0) musicFolderOffset = 0;
      if (musicFolderOffset >= musicFolderCount) musicFolderOffset = max(0, musicFolderCount - MUSIC_FOLDER_ROWS);
      int maxShow = min(musicFolderCount - musicFolderOffset, MUSIC_FOLDER_ROWS);
      for (int i = 0; i < maxShow; i++) {
        int idx = musicFolderOffset + i;
        int y = 64 + i * 27;
        uint16_t bg = (musicFolders[idx] == musicFolder) ? TFT_DARKGREEN : TFT_NAVY;
        tft.fillRoundRect(6, y, 228, 25, 5, bg);
        tft.drawRoundRect(6, y, 228, 25, 5, TFT_DARKGREY);
        String label = folderBaseName(musicFolders[idx]);
        if (label.length() > 21) label = label.substring(0, 21);
        tft.setTextColor(TFT_WHITE, bg);
        tft.drawString(label.c_str(), 12, y + 5, 2);
      }
    }
  }
  Button parent = { 50, 286, 48, 28, "Padre", ACT_FOLDER_UP };
  Button prev = { 102, 286, 36, 28, "Arr", ACT_MUSIC_LIST_PREV };
  Button next = { 142, 286, 36, 28, "Abj", ACT_MUSIC_LIST_NEXT };
  Button use = { 182, 286, 52, 28, "Usar", ACT_FOLDER_USE };
  drawBackButton();
  drawButton(parent, TFT_BLUE);
  drawButton(prev, TFT_BLUE);
  drawButton(next, TFT_BLUE);
  drawButton(use, TFT_DARKGREEN);
}

void updateAudioPageDynamic(bool force) {
  // V23: si no es forzado, no limpiar toda la zona de musica.
  // Limpiar/redibujar pantalla mientras Audio.h reproduce MP3 puede provocar cortes.
  if (!force) {
    drawAudioProgressOnly();
    return;
  }

  tft.fillRect(0, 56, SCREEN_W, 162, TFT_BLACK);
  if (!sdOK) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("microSD no disponible", 10, 70, 2);
    return;
  }
  if (musicCount <= 0) {
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.drawString("No hay canciones", 10, 70, 2);
    tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    tft.drawString("Pulsa Carpeta o Refres.", 10, 96, 2);
    return;
  }
  String name = folderBaseName(musicFiles[musicIndex]);
  if (name.length() > 28) name = name.substring(0, 28);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString((String(musicIndex + 1) + "/" + String(musicCount)).c_str(), 10, 62, 2);
  tft.drawString(name.c_str(), 10, 86, 1);

  drawAudioStatusOnly();

  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Posicion", 10, 148, 1);
  Button back5 = {  10, 160, 38, 22, "-5s", ACT_NONE };
  Button fwd5  = { 192, 160, 38, 22, "+5s", ACT_NONE };
  drawButton(back5, TFT_BLUE);
  drawButton(fwd5, TFT_BLUE);
  drawAudioProgressOnly();

  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Volumen", 10, 184, 1);
  Button volDown = {  10, 196, 38, 22, "-1", ACT_NONE };
  Button volUp   = { 192, 196, 38, 22, "+1", ACT_NONE };
  drawButton(volDown, TFT_DARKGREEN);
  drawButton(volUp, TFT_DARKGREEN);
  drawAudioVolumeOnly();
}

void drawAudioStatusOnly() {
  tft.fillRect(10, 126, 220, 22, TFT_BLACK);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  String st = audioStatusMsg;
  if (st.length() > 24) st = st.substring(0, 24);
  tft.drawString(st.c_str(), 10, 126, 1);
  String modeLine = String(audioRepeatLabel());
  if (musicShuffle) modeLine += " + Aleatorio";
  if (modeLine.length() > 28) modeLine = modeLine.substring(0, 28);
  tft.drawString(modeLine.c_str(), 10, 138, 1);
}

void drawAudioRepeatButtonOnly() {
  Button repeat = {82,258, 72, 24, audioRepeatLabel(), ACT_AUDIO_REPEAT };
  drawButton(repeat, musicRepeatMode == 0 ? TFT_DARKGREY : TFT_DARKGREEN);
}

void drawAudioShuffleButtonOnly() {
  Button shuffle = {160,258,72, 24, musicShuffle ? "Aleat.ON" : "Aleat.", ACT_AUDIO_SHUFFLE };
  drawButton(shuffle, musicShuffle ? TFT_DARKGREEN : TFT_DARKGREY);
}

void drawAudioProgressOnly() {
#if HAVE_AUDIO_LIB
  int cur = (int)audio.getAudioCurrentTime();
  int dur = (int)audio.getAudioFileDuration();
  int pct = (dur > 0) ? constrain((cur * 100) / dur, 0, 100) : 0;
#else
  int cur = 0, dur = 0, pct = 0;
#endif
  char buf[32];
  snprintf(buf, sizeof(buf), "%02d:%02d / %02d:%02d", cur/60, cur%60, dur/60, dur%60);
  tft.fillRect(10, 108, 160, 18, TFT_BLACK);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString(buf, 10, 112, 2);
  drawBar(54, 164, 132, 14, pct, TFT_CYAN);
}

void drawAudioVolumeOnly() {
  // V58C/V2: esta funcion solo dibuja la barra central y el numero.
  // No debe llamarse fuera de la pagina de musica, porque si llega un comando BLE
  // desde otra pantalla dejaria restos de la barra encima del menu actual.
  if (currentPage != PAGE_AUDIO) return;
  int level = audioVolumeUiLevel();
  drawBar(54, 200, 132, 14, ::map(level, 0, AUDIO_VOLUME_UI_STEPS, 0, 100), TFT_GREEN);
  tft.fillRect(118, 184, 70, 10, TFT_BLACK);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  String txt = String(level) + "/" + String(AUDIO_VOLUME_UI_STEPS);
  tft.drawString(txt.c_str(), 142, 184, 1);
}

void drawAudioVolumeControlsOnly() {
  // V58C/V2: redibuja toda la zona del volumen, incluyendo botones.
  // Esto evita que la barra quede congelada a trozos o por encima/debajo de botones.
  if (currentPage != PAGE_AUDIO) return;
  tft.fillRect(0, 184, SCREEN_W, 36, TFT_BLACK);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Volumen", 10, 184, 1);
  Button volDown = {  10, 196, 38, 22, "-1", ACT_NONE };
  Button volUp   = { 192, 196, 38, 22, "+1", ACT_NONE };
  drawButton(volDown, TFT_DARKGREEN);
  drawButton(volUp, TFT_DARKGREEN);
  drawAudioVolumeOnly();
}

// -------------------- Touch --------------------
bool readTouchPoint(int &x, int &y) {
  // V42: lectura directa y sin esperas largas. El tactil manda.
  FT6336U_TouchPointType tp;
  bool ok = false;
  for (uint8_t i = 0; i < TOUCH_READ_RETRIES; i++) {
    tp = ft6336u.scan();
    if (tp.touch_count > 0) {
      ok = true;
      break;
    }
    delay(2);
  }
  if (!ok) return false;

  int tx = tp.tp[0].x;
  int ty = tp.tp[0].y;

  if (TOUCH_SWAP_XY) {
    int tmp = tx;
    tx = ty;
    ty = tmp;
  }
  if (TOUCH_INVERT_X) tx = SCREEN_W - 1 - tx;
  if (TOUCH_INVERT_Y) ty = SCREEN_H - 1 - ty;

  tx += TOUCH_CAL_OFFSET_X;
  ty += TOUCH_CAL_OFFSET_Y;

  if (tx < 0 || tx >= SCREEN_W || ty < 0 || ty >= SCREEN_H) return false;
  x = tx;
  y = ty;
  return true;
}

void handleTouch(int x, int y) {
  if (TOUCH_DEBUG_SERIAL) Serial.printf("Touch x=%d y=%d page=%d\n", x, y, currentPage);

  if (wifiKeyboard) {
    handleWifiKeyboardTouch(x, y);
    return;
  }

  if (currentPage == PAGE_HOME) {
    Button *buttons = homePageIndex == 0 ? homeButtons : extraHomeButtons;
    uint8_t count = homePageIndex == 0 ? (sizeof(homeButtons) / sizeof(homeButtons[0])) : (sizeof(extraHomeButtons) / sizeof(extraHomeButtons[0]));
    for (uint8_t i = 0; i < count; i++) {
      if (pointInButton(x, y, buttons[i])) {
        switch (buttons[i].action) {
          case ACT_SYSTEM: drawSystemPage(); break;
          case ACT_DIAG: drawDiagPage(); break;
          case ACT_CAMERA: drawCameraPage(); break;
          case ACT_SD: drawSDPage(); break;
          case ACT_WIFI: drawWiFiPage(); break;
          case ACT_WS2812: drawWS2812Page(); break;
          case ACT_BUZZER: drawBuzzerPage(); break;
          case ACT_CAMERA_WEB: drawCameraWebPage(); break;
          case ACT_AUDIO: enterMusicPageFromHome(); break;
          case ACT_CLOCK: drawClockPage(); break;
          case ACT_CALC: drawCalcPage(); break;
          case ACT_FTP: drawFTPPage(); break;
          case ACT_BATTERY: drawBatteryPage(); break;
          case ACT_SCREEN_TIMEOUT: drawScreenTimeoutPage(); break;
          case ACT_RESTART: safeRestartNow(); break;
          case ACT_NONE: homePageIndex = 0; drawHome(); break;
          default: break;
        }
        return;
      }
    }
    return;
  }

  Button back = { 8, 286, 68, 28, "Atras", ACT_BACK };
  if (currentPage != PAGE_CAMERA && pointInButton(x, y, back)) {
    if (currentPage == PAGE_MUSIC_LIST && alarmMusicSelectMode) { alarmMusicSelectMode = false; drawClockPage(); }
    else if (currentPage == PAGE_MUSIC_FOLDER || currentPage == PAGE_MUSIC_LIST) drawAudioPage();
    else if (currentPage == PAGE_AUDIO) { musicLockedByUnordered = false; musicNeedOrganizeConfirm = false; musicUserApprovedOrganization = false; musicOrganizePending = false; drawHome(); }
    else drawHome();
    return;
  }

  if (currentPage == PAGE_DIAG) {
    Button rescan = { 92, 286, 136, 28, "Reescanear", ACT_RESCAN_I2C };
    if (pointInButton(x, y, rescan)) drawDiagPage();
  }

  if (currentPage == PAGE_SD) {
    Button refresh = { 82, 286, 70, 28, "Refres.", ACT_SD_REFRESH };
    Button test = { 158, 286, 74, 28, "Test", ACT_SD_TEST };
    if (pointInButton(x, y, refresh)) { sdTried = false; drawSDPage(); }
    if (pointInButton(x, y, test)) { createSDTestFile(); drawSDPage(); }
  }


  if (currentPage == PAGE_PHOTOS) {
    handlePhotoViewerTouch(x, y);
    return;
  }

  if (currentPage == PAGE_MUSIC_FOLDER) {
    handleMusicFolderTouch(x, y);
    return;
  }

  if (currentPage == PAGE_MUSIC_LIST) {
    handleMusicListTouch(x, y);
    return;
  }

  if (currentPage == PAGE_SCREEN_TIMEOUT) {
    handleScreenTimeoutTouch(x, y);
    return;
  }

  if (currentPage == PAGE_WIFI) {
    if (wifiScanned) {
      int maxShow = min(wifiCount, 5);
      for (int i = 0; i < maxShow; i++) {
        Button row = { 6, (int16_t)(98 + i * 32), 228, 27, "", ACT_NONE };
        if (pointInButton(x, y, row)) {
          wifiSelectedIndex = i;
          drawWiFiPage();
          return;
        }
      }
    }
    Button scan = { 82, 286, 72, 28, "Escan.", ACT_WIFI_SCAN };
    Button conn = { 158, 286, 74, 28, "Conect.", ACT_WIFI_CONNECT };
    if (pointInButton(x, y, scan)) { scanWiFiAndDraw(); return; }
    if (pointInButton(x, y, conn)) {
      if (WiFi.status() == WL_CONNECTED) {
        wifiManualOff();
        drawWiFiPage();
        return;
      }
      if (wifiSelectedIndex >= 0) {
        int known = knownWiFiIndexForSSID(WiFi.SSID(wifiSelectedIndex));
        if (known >= 0) {
          wifiPassword = String(KNOWN_WIFI[known].password);
          connectToSelectedWiFi();
          drawWiFiPage();
        } else {
          wifiPassword = "";
          wifiKeyboard = true;
          wifiUpper = false;
          drawWifiKeyboard();
        }
      } else if (KNOWN_WIFI_COUNT > 0) {
        connectKnownWiFiByIndex(0, true, 15000);
        drawWiFiPage();
      }
      return;
    }
  }

  if (currentPage == PAGE_CAMERA) {
    Button back  = {   4, 286, 42, 28, "Atras", ACT_BACK };
    Button mode  = {  50, 286, 42, 28, "Color", ACT_CAM_COLOR };
    Button rot   = {  96, 286, 42, 28, "Rotar", ACT_CAM_ROTATE };
    Button cap   = { 142, 286, 42, 28, "Foto",  ACT_CAM_CAPTURE };
    Button save  = { 188, 286, 48, 28, "Galer", ACT_PHOTO_VIEWER };
    if (pointInButton(x, y, back)) { drawHome(); return; }
    if (pointInButton(x, y, mode)) { cameraNextColorMode(); drawCameraPage(); return; }
    if (pointInButton(x, y, rot)) { cameraNextOrientationMode(); drawCameraPage(); return; }
    if (pointInButton(x, y, cap)) { captureAndSavePhoto(); return; }
    if (pointInButton(x, y, save)) { drawPhotoViewerPage(); return; }
  }

  if (currentPage == PAGE_WS2812) {
    Button effect = {  8, 248, 110, 28, "Efecto", ACT_LED_EFFECT };
    Button off    = {128, 248, 104, 28, ledPoweredOff ? "Encender" : "Apagar", ACT_LED_OFF };
    bool changed = false;

    if (sliderTouchToValue(x, y, 10, 104, 220, ledBright)) changed = true;
    else if (sliderTouchToValue(x, y, 10, 140, 220, ledR)) changed = true;
    else if (sliderTouchToValue(x, y, 10, 176, 220, ledG)) changed = true;
    else if (sliderTouchToValue(x, y, 10, 212, 220, ledB)) changed = true;
    else if (pointInButton(x, y, effect)) {
      if (ledPoweredOff) restoreLedAfterOff();
      if (ledBright <= 0) ledBright = max(ledSavedBright, 80);
      if (ledR == 0 && ledG == 0 && ledB == 0) {
        ledR = max(ledSavedR, 80);
        ledG = ledSavedG;
        ledB = ledSavedB;
      }
      ledEffectMode = (ledEffectMode + 1) % 5;
      ledEffectStep = 0;
      ledBlinkState = false;
      changed = true;
    } else if (pointInButton(x, y, off)) {
      if (ledPoweredOff) {
        restoreLedAfterOff();
      } else {
        saveLedBeforeOff();
        ledPoweredOff = true;
        ledEffectMode = 0;
        ledEffectStep = 0;
        ledBlinkState = false;
        ws2812SetColor(0, 0, 0, 0);
      }
      changed = true;
    }

    if (changed) {
      if (ledPoweredOff) {
        ws2812SetColor(0, 0, 0, 0);
      } else if (ledEffectMode == 0) {
        ws2812SetColor(ledR, ledG, ledB, ledBright);
      } else {
        updateWs2812Effect(true);
      }
      drawWS2812Page();
    }
    return;
  }

  if (currentPage == PAGE_BUZZER) {
    Button beep = { 28, 128, 184, 34, "Beep", ACT_BUZZER_BEEP };
    Button melody = { 28, 170, 184, 34, "Melodia", ACT_BUZZER_MELODY };
    Button boot = { 28, 212, 184, 34, "Inicio", ACT_BUZZER_BOOT_TOGGLE };
    if (pointInButton(x, y, beep)) buzzerBeep(1500, 160);
    if (pointInButton(x, y, melody)) buzzerMelody();
    if (pointInButton(x, y, boot)) {
      bootBeepEnabled = !bootBeepEnabled;
      prefs.putBool("boot_beep", bootBeepEnabled);
      drawBuzzerPage();
    }
  }


  if (currentPage == PAGE_CAMERA_WEB) {
    if (WiFi.status() != WL_CONNECTED) {
      Button conn = { 28, 214, 184, 34, "Conectar WiFi", ACT_WIFI_CONNECT };
      if (pointInButton(x, y, conn)) {
        connectKnownWiFiByIndex(0, true, 15000);
        drawCameraWebPage();
        return;
      }
    } else {
      Button web = { 10, 150, 104, 34, webServerEnabled ? "Web OFF" : "Web ON", ACT_WEB_SERVER_TOGGLE };
      Button ftp = {126, 150, 104, 34, ftpEnabled ? "FTP OFF" : "FTP ON", ACT_FTP_TOGGLE };

      if (pointInButton(x, y, web)) {
        if (webServerEnabled) {
          if (cameraWebStarted) {
            stopCameraServer();
            deinitCamera();
            cameraWebStarted = false;
          }
          if (otaReady) {
            otaServer.close();
            otaReady = false;
          }
          webServerEnabled = false;
          webServerStatusMsg = "Web apagada";
          cameraWebStatusMsg = "Camara parada";
        } else {
          webServerEnabled = true;
          webServerStatusMsg = "Activando Web";
          otaStartIfPossible();
          webServerStatusMsg = otaReady ? "Web activa" : "No pudo iniciar Web";
        }
        drawCameraWebPage();
        return;
      }

      if (pointInButton(x, y, ftp)) {
        ftpStartStop();
        drawCameraWebPage();
        return;
      }

      if (webServerEnabled && otaReady) {
        Button start = { 10, 196, 104, 32, cameraWebStarted ? "Cam rein." : "Cam ON", ACT_CAMERA_WEB_START };
        Button stop  = {126, 196, 104, 32, "Cam OFF", ACT_WEB_CAMERA_STOP };
        if (pointInButton(x, y, start)) { initCameraWebMode(); drawCameraWebPage(); return; }
        if (pointInButton(x, y, stop)) { stopCameraServer(); deinitCamera(); cameraWebStarted = false; cameraWebStatusMsg = "Camara parada"; drawCameraWebPage(); return; }
      }
    }
  }
  if (currentPage == PAGE_AUDIO) {
    handleAudioTouch(x, y);
  }

  if (currentPage == PAGE_FTP) {
    handleFTPTouch(x, y);
  }

  if (currentPage == PAGE_CLOCK) {
    handleClockTouch(x, y);
  }

  if (currentPage == PAGE_CALC) {
    handleCalcTouch(x, y);
  }
}

bool pointInButton(int x, int y, const Button &b) {
  // V43: zona tactil separada. No hay margen invisible que pueda invadir
  // al boton vecino. El inset crea una pequena zona muerta alrededor del boton.
  int16_t x1 = b.x - TOUCH_BUTTON_MARGIN_X + TOUCH_HITBOX_INSET_X;
  int16_t y1 = b.y - TOUCH_BUTTON_MARGIN_Y + TOUCH_HITBOX_INSET_Y;
  int16_t x2 = b.x + b.w + TOUCH_BUTTON_MARGIN_X - TOUCH_HITBOX_INSET_X;
  int16_t y2 = b.y + b.h + TOUCH_BUTTON_MARGIN_Y - TOUCH_HITBOX_INSET_Y;
  if (x1 < 0) x1 = 0;
  if (y1 < 0) y1 = 0;
  if (x2 > SCREEN_W - 1) x2 = SCREEN_W - 1;
  if (y2 > SCREEN_H - 1) y2 = SCREEN_H - 1;
  return (x >= x1 && x <= x2 && y >= y1 && y <= y2);
}



// -------------------- OTA Web WiFi --------------------
String htmlEscape(const String &s) {
  String out;
  out.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '&') out += "&amp;";
    else if (c == '<') out += "&lt;";
    else if (c == '>') out += "&gt;";
    else if (c == '"') out += "&quot;";
    else out += c;
  }
  return out;
}

bool otaWebAuthorized() {
  if (strlen(OTA_PASSWORD) == 0) return true;
  if (!otaServer.hasArg("key")) return false;
  return otaServer.arg("key") == OTA_PASSWORD;
}

String otaWebPage(const String &msg = "") {
  // Compatibilidad: cualquier llamada antigua a otaWebPage devuelve ahora
  // el portal unificado con pestanas, dejando OTA Web como primera pestana.
  return webPortalPage("ota", msg);
}

String webPortalPage(const String &activeTab, const String &msg) {
  // V39: portal unico, pero la camara queda siempre en modo manual.
  // Al abrir la web no se carga ningun stream y no se inicia la camara automaticamente.
  String ip = WiFi.localIP().toString();
  String key = htmlEscape(String(OTA_PASSWORD));
  bool camActive = cameraWebStarted;
  String page;
  page.reserve(6200);
  page += F("<!doctype html><html><head><meta charset='utf-8'>");
  page += F("<meta name='viewport' content='width=device-width,initial-scale=1'>");
  page += F("<title>ESP32-S3 Freenove Web</title><style>");
  page += F("body{font-family:Arial,sans-serif;background:#101010;color:#eee;margin:0;padding:0}");
  page += F(".wrap{max-width:760px;margin:auto;padding:14px}.card{background:#202020;border-radius:16px;padding:14px;margin:10px 0;box-shadow:0 0 0 1px #333}");
  page += F(".tabs{display:flex;gap:6px;position:sticky;top:0;background:#101010;padding:8px 0;z-index:5}.tabbtn{flex:1;border:0;border-radius:12px;padding:12px 6px;font-size:15px;font-weight:bold;background:#333;color:#ddd}");
  page += F(".tabbtn.active{background:#0099cc;color:white}.tab{display:none}.tab.active{display:block}");
  page += F("input,button{font-size:18px;padding:12px;margin:8px 0;border-radius:10px;border:0}.wide{width:100%}.blue{background:#0099cc;color:white;font-weight:bold}.red{background:#b23b3b;color:white}.green{background:#278b45;color:white}.gray{background:#444;color:white}");
  page += F(".btnlink{display:block;text-decoration:none;text-align:center;font-size:18px;padding:12px;margin:8px 0;border-radius:10px;border:0}.btnlink.green{background:#278b45;color:white;font-weight:bold}.btnlink.red{background:#b23b3b;color:white;font-weight:bold}");
  page += F("a{color:#80d8ff}.warn{color:#ffd166}.ok{color:#80ff80}.bad{color:#ff8080}img{max-width:100%;height:auto;background:#000;border-radius:12px}.small{font-size:13px;color:#aaa}");
  page += F("</style></head><body><div class='wrap'><h2>ESP32-S3 Freenove</h2>");
  page += F("<p>IP: <b>"); page += ip; page += F("</b> &nbsp; WiFi: <b>"); page += (WiFi.status() == WL_CONNECTED ? F("OK") : F("NO")); page += F("</b></p>");
  if (msg.length()) { page += F("<div class='card ok'>"); page += htmlEscape(msg); page += F("</div>"); }

  page += F("<div class='tabs'>");
  page += F("<button class='tabbtn active' onclick=\"tab('ota',this)\">OTA Web</button>");
  page += F("<button class='tabbtn' onclick=\"tab('live',this)\">Camara</button>");
  page += F("</div>");

  page += F("<div id='ota' class='tab active card'><h3>OTA Web</h3>");
  page += F("<p class='warn'>Sube solo el archivo <b>.cpp.bin</b> compilado. No subas bootloader ni partitions.</p>");
  page += F("<form method='POST' action='/update?key="); page += key; page += F("' enctype='multipart/form-data'>");
  page += F("<input class='wide' type='file' name='firmware' required>");
  page += F("<button class='wide blue' type='submit'>Actualizar firmware</button></form>");
  page += F("<p>Clave OTA: <b>"); page += key; page += F("</b></p>");
  page += F("<p><a href='/reboot?key="); page += key; page += F("'>Reiniciar placa</a></p>");
  page += F("</div>");

  page += F("<div id='live' class='tab card'><h3>Camara</h3>");
  page += F("<p>Estado: <b class='"); page += (camActive ? F("ok") : F("bad")); page += F("'>"); page += htmlEscape(cameraWebStatusMsg); page += F("</b></p>");
  page += F("<p class='small'>La camara no se inicia sola. Modo estable: QVGA 320x240 y FPS limitado para evitar congelaciones.</p>");
  page += F("<a class='btnlink green' href='/camstart?key="); page += key; page += F("'>Iniciar/Reiniciar camara</a>");
  page += F("<a class='btnlink red' href='/camstop?key="); page += key; page += F("' onclick=\"document.getElementById('stream').src='';\">Parar camara</a>");
  page += F("<button class='wide blue' onclick=\"document.getElementById('stream').src='http://"); page += ip; page += F(":81/stream?'+Date.now();tab('live',document.querySelectorAll('.tabbtn')[1])\">Ver stream</button>");
  page += F("<button class='wide gray' onclick=\"document.getElementById('stream').src=''\">Detener vista</button>");
  page += F("<img id='stream' alt='Stream de camara'>");
  page += F("<p class='small'>La pagina va por el puerto 80. El video MJPEG sale por el puerto 81 solo cuando pulsas Ver stream. Si la red va justa, el FPS se mantiene bajo para priorizar estabilidad.</p></div>");

  page += F("<script>function tab(id,b){document.querySelectorAll('.tab').forEach(e=>e.classList.remove('active'));document.querySelectorAll('.tabbtn').forEach(e=>e.classList.remove('active'));document.getElementById(id).classList.add('active');if(b)b.classList.add('active');}");
  if (activeTab == "cam") page += F("window.addEventListener('load',()=>tab('live',document.querySelectorAll('.tabbtn')[1]));");
  page += F("</script></div></body></html>");
  return page;
}

void otaPrepareForUpdate() {
  if (otaInProgress) return;
  otaInProgress = true;
  otaLastPercent = 255;
  otaLastUiMs = 0;
#if HAVE_AUDIO_LIB
  audio.stopSong();
#endif
  musicPlaying = false;
  musicPaused = false;
  if (cameraOK) {
    esp_camera_deinit();
    cameraOK = false;
  }
  ftpStatus = "OTA Web activa";
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString("Actualizando OTA", SCREEN_W / 2, 64, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString("No apagues la placa", SCREEN_W / 2, 94, 2);
  Serial.println("OTA Web: inicio");
}


uint32_t safeBootDelayForResetReason() {
  if (!SAFE_BOOT_ENABLED) return 300;
  esp_reset_reason_t rr = esp_reset_reason();
  switch (rr) {
    case ESP_RST_POWERON:
      return 900;     // Arranque desde corriente: algo mas rapido.
    case ESP_RST_EXT:
      return 1800;    // Boton RESET externo: dar tiempo a perifericos.
    case ESP_RST_SW:
    case ESP_RST_PANIC:
    case ESP_RST_INT_WDT:
    case ESP_RST_TASK_WDT:
    case ESP_RST_WDT:
    case ESP_RST_DEEPSLEEP:
      return 2600;    // Reinicio software/OTA/watchdog: mas conservador.
    default:
      return 2200;
  }
}

void safeBootStage(const char *msg, uint16_t color) {
  if (!SAFE_BOOT_ENABLED) return;
  tft.fillRect(0, 44, SCREEN_W, 92, TFT_BLACK);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString("Arranque seguro V44", SCREEN_W / 2, 48, 2);
  tft.setTextColor(color, TFT_BLACK);
  tft.drawCentreString(msg, SCREEN_W / 2, 82, 2);
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawCentreString("No pulses botones", SCREEN_W / 2, 112, 1);
  delay(120);
}

void safeBootService() {
  if (WIFI_START_MANUAL_ONLY) return;
  if (!safeBootWifiPending || safeBootWifiDone) return;
  if ((long)(millis() - safeBootWifiAtMs) < 0) return;

  safeBootWifiPending = false;
  safeBootWifiDone = true;

  // Evita interrumpir musica/camara. Si el usuario entra rapido en una funcion pesada,
  // pospone la autoconexion WiFi hasta que vuelva al menu o a una pantalla ligera.
  if (musicPlaying || currentPage == PAGE_CAMERA || currentPage == PAGE_AUDIO) {
    safeBootWifiPending = true;
    safeBootWifiDone = false;
    safeBootWifiAtMs = millis() + 5000;
    return;
  }

  AppPage oldPage = currentPage;
  uint8_t oldHome = homePageIndex;
  autoConnectKnownWiFi(true);
  otaStartIfPossible();
  delay(250);

  // Devuelve al usuario al menu si la autoconexion ha redibujado la pantalla.
  currentPage = oldPage;
  homePageIndex = oldHome;
  if (oldPage == PAGE_HOME) drawHome();
}

void safeRestartNow() {
#if HAVE_AUDIO_LIB
  audio.stopSong();
#endif
  musicPlaying = false;
  musicPaused = false;
  otaRestartPending = false;
  otaInProgress = false;

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString("Reinicio seguro", SCREEN_W / 2, 88, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString("Cerrando servicios...", SCREEN_W / 2, 118, 2);

  if (ftpEnabled) {
    ftpStartStop();
    delay(150);
  }
  if (otaReady) {
    otaServer.close();
    otaReady = false;
    delay(150);
  }
  webServerEnabled = false;
  webServerStatusMsg = "Web apagada por reinicio";
  WiFi.disconnect(false);
  WiFi.mode(WIFI_OFF);
  ws2812SetColor(0, 0, 0, 0);
  delay(1000);
  Serial.flush();
  delay(200);
  esp_restart();

  // V53: si el reinicio normal se queda a medias en esta placa, forzamos panic.
  delay(500);
  esp_system_abort("reinicio seguro forzado");
  delay(300);
  abort();
}

void otaStartIfPossible() {
  if (!OTA_ENABLED) return;
  if (!webServerEnabled) return;   // V43: no arrancar servidor web hasta activarlo manualmente.
  if (otaReady) return;
  if (WiFi.status() != WL_CONNECTED) return;

  // V38: una sola pagina web en el puerto 80 con pestanas.
  // La primera pestana es OTA Web y las siguientes son para la camara.
  otaServer.on("/", HTTP_GET, []() {
    otaServer.sendHeader("Location", String("/ota?key=") + OTA_PASSWORD, true);
    otaServer.send(302, "text/plain", "Portal Web");
  });

  otaServer.on("/ota", HTTP_GET, []() {
    if (!otaWebAuthorized()) {
      otaServer.send(401, "text/plain", "Clave OTA incorrecta. Usa /ota?key=TU_CLAVE");
      return;
    }
    otaServer.send(200, "text/html", webPortalPage("ota", ""));
  });

  otaServer.on("/cam", HTTP_GET, []() {
    if (!otaWebAuthorized()) {
      otaServer.send(401, "text/plain", "Clave OTA incorrecta. Usa /cam?key=TU_CLAVE");
      return;
    }
    if (WiFi.status() != WL_CONNECTED) {
      otaServer.send(503, "text/plain", "WiFi no conectado");
      return;
    }
    // V39: abrir /cam NO inicia la camara. Solo muestra la pestana Camara.
    if (!cameraWebStarted && cameraWebStatusMsg.length() == 0) {
      cameraWebStatusMsg = "Camara parada. Pulsa Iniciar camara";
    }
    otaServer.send(200, "text/html", webPortalPage("cam", cameraWebStatusMsg));
  });

  otaServer.on("/camstart", HTTP_GET, []() {
    if (!otaWebAuthorized()) {
      otaServer.send(401, "text/plain", "Clave OTA incorrecta");
      return;
    }
    initCameraWebMode();
    otaServer.sendHeader("Location", String("/cam?key=") + OTA_PASSWORD, true);
    otaServer.send(302, "text/plain", "CamWeb iniciado");
  });

  otaServer.on("/camstop", HTTP_GET, []() {
    if (!otaWebAuthorized()) {
      otaServer.send(401, "text/plain", "Clave OTA incorrecta");
      return;
    }
    stopCameraServer();
    deinitCamera();
    cameraWebStarted = false;
    cameraWebStatusMsg = "CamWeb parado";
    otaServer.sendHeader("Location", String("/cam?key=") + OTA_PASSWORD, true);
    otaServer.send(302, "text/plain", "CamWeb parado");
  });

  otaServer.on("/update", HTTP_POST, []() {
    if (!otaWebAuthorized()) {
      otaServer.send(401, "text/plain", "Clave OTA incorrecta");
      return;
    }
    bool ok = !Update.hasError();
    otaServer.send(200, "text/html", ok ?
      F("<html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'></head><body><h2>OTA completada</h2><p>Reinicio profundo programado. La placa se apagara un instante y volvera a arrancar.</p><p>Espera 10-20 segundos y vuelve a abrir la IP.</p></body></html>") :
      F("<html><body><h2>Error OTA</h2><p>La actualizacion ha fallado.</p></body></html>"));
    if (ok) {
      // No reiniciamos dentro del callback HTTP: en algunos navegadores/Android
      // el servidor se queda esperando y no llega a ejecutar bien ESP.restart().
      // Dejamos responder la pagina y reiniciamos desde loop() unos segundos despues.
      otaRestartPending = true;
      otaRestartAtMs = millis() + 2500;
      otaStatusMsg = "OTA completada. Reinicio pendiente";
    } else {
      otaInProgress = false;
    }
  }, []() {
    if (!otaWebAuthorized()) return;
    HTTPUpload &upload = otaServer.upload();

    if (upload.status == UPLOAD_FILE_START) {
      otaPrepareForUpdate();
      Serial.print("OTA Web archivo: ");
      Serial.println(upload.filename);
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
      unsigned long now = millis();
      if (now - otaLastUiMs > 350) {
        otaLastUiMs = now;
        tft.fillRect(0, 150, SCREEN_W, 28, TFT_BLACK);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        char buf[40];
        snprintf(buf, sizeof(buf), "%u KB", (unsigned)(Update.progress() / 1024));
        tft.drawCentreString(buf, SCREEN_W / 2, 154, 2);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        otaStatusMsg = "OTA Web completada";
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.drawCentreString("OTA completada", SCREEN_W / 2, 100, 2);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawCentreString("Preparando reinicio", SCREEN_W / 2, 130, 2);
        Serial.printf("OTA Web completada: %u bytes\n", upload.totalSize);
      } else {
        otaStatusMsg = "Error OTA Web";
        otaInProgress = false;
        tft.fillScreen(TFT_BLACK);
        tft.setTextColor(TFT_RED, TFT_BLACK);
        tft.drawCentreString("Error OTA", SCREEN_W / 2, 90, 2);
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_ABORTED) {
      Update.abort();
      otaInProgress = false;
      otaStatusMsg = "OTA cancelada";
      Serial.println("OTA Web cancelada");
    }
  });

  otaServer.on("/reboot", HTTP_GET, []() {
    if (!otaWebAuthorized()) {
      otaServer.send(401, "text/plain", "Clave OTA incorrecta");
      return;
    }
    otaServer.send(200, "text/html", F("<html><body><h2>Reinicio solicitado</h2><p>La placa hara un reinicio profundo en unos segundos.</p></body></html>"));
    otaRestartPending = true;
    otaRestartAtMs = millis() + 1500;
    otaStatusMsg = "Reinicio Web pendiente";
  });

  otaServer.begin();
  otaReady = true;
  otaStatusMsg = String("OTA Web: http://") + WiFi.localIP().toString() + "/ota?key=" + OTA_PASSWORD;
  Serial.println(otaStatusMsg);
}


void otaForceRebootNow() {
  // V33: en esta placa Freenove ESP32-S3 se ha comprobado que, tras una OTA Web,
  // ESP.restart() y el reinicio por deep sleep pueden quedarse colgados mostrando
  // "reiniciando" hasta pulsar RESET durante 1,5-2 segundos.
  //
  // Por eso aqui usamos un reset forzado por panic/abort. Es intencionado:
  // provoca un reinicio completo del chip sin depender de que el servidor web termine
  // de cerrar la conexion ni de que el modo deep sleep despierte correctamente.
  otaRestartPending = false;
  otaInProgress = false;
  Serial.println("OTA Web: reset forzado por panic/abort");

  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawCentreString("OTA completada", SCREEN_W / 2, 82, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString("Reset forzado...", SCREEN_W / 2, 114, 2);
  tft.drawCentreString("Espera 10-20 s", SCREEN_W / 2, 144, 2);

  // Dar tiempo a que el navegador reciba la respuesta antes del reset.
  delay(1200);

  // Cerrar el servidor, pero NO apagar WiFi ni entrar en deep sleep:
  // en esta placa esos pasos pueden dejar el chip/perifericos en un estado del que
  // solo sale manteniendo pulsado RESET.
  otaServer.close();
  delay(150);

  Serial.flush();
  delay(100);

  // Primer intento: reset normal de ESP-IDF.
  // Si en esta placa se queda bloqueado, no deberia volver de esta llamada.
  esp_restart();

  // Si esp_restart() vuelve o no llega a reiniciar, forzamos panic.
  // El panic handler del ESP32 reinicia el chip automaticamente.
  delay(500);
  esp_system_abort("OTA Web reset forzado");

  // Ultimo recurso si el simbolo anterior no reiniciara por alguna razon.
  delay(500);
  abort();

  while (true) {
    delay(1000);
  }
}

void otaHandle() {
  if (!OTA_ENABLED) return;
  if (!webServerEnabled) return;   // V43: servidor web manual.
  if (WiFi.status() != WL_CONNECTED) return;
  if (!otaReady) otaStartIfPossible();
  if (otaReady) otaServer.handleClient();

  if (otaRestartPending && (long)(millis() - otaRestartAtMs) >= 0) {
    otaForceRebootNow();
  }
}


void startupSyncTimeAndDisconnect() {
  if (startupTimeSyncDone || KNOWN_WIFI_COUNT == 0) return;
  startupTimeSyncDone = true;
  AppPage oldPage = currentPage;
  uint8_t oldHome = homePageIndex;
  drawHeader("Hora Internet");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Conectando solo para", 10, 62, 2);
  tft.drawString("hora de Espana...", 10, 86, 2);
  bool ok = connectKnownWiFiByIndex(0, true, 12000);
  if (ok) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Hora sincronizada", 10, 238, 2);
  } else {
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.drawString("Sin hora por Internet", 10, 238, 2);
  }
  delay(600);
  wifiManualOff();
  currentPage = oldPage;
  homePageIndex = oldHome;
}

void updateClockDateTextFromEpoch() {
  if (!clockEpochValid) return;
  time_t nowEpoch = clockBaseEpoch + (time_t)((millis() - clockBaseMillis) / 1000UL);
  struct tm timeinfo;
  localtime_r(&nowEpoch, &timeinfo);
  char buf[40];
  snprintf(buf, sizeof(buf), "%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  clockDateText = String(buf);
}

void adjustClockBySeconds(int32_t delta) {
  if (clockEpochValid) {
    clockBaseEpoch += delta;
  } else {
    int32_t s = (int32_t)getClockSeconds() + delta;
    while (s < 0) s += 86400L;
    clockBaseSeconds = (uint32_t)(s % 86400L);
  }
  clockBaseMillis = millis();
  updateClockDateTextFromEpoch();
}

void scrollMusicFolderList(int deltaRows) {
  if (musicFolderCount <= MUSIC_FOLDER_ROWS) {
    musicFolderOffset = 0;
  } else {
    musicFolderOffset += deltaRows;
    if (musicFolderOffset < 0) musicFolderOffset = 0;
    int maxOffset = max(0, musicFolderCount - MUSIC_FOLDER_ROWS);
    if (musicFolderOffset > maxOffset) musicFolderOffset = maxOffset;
  }
  drawMusicFolderPage();
}

void alarmStopMusicIfActive() {
  if (!alarmMusicActive) return;
#if HAVE_AUDIO_LIB
  musicEofPending = false;
  audioChangingTrack = true;
  audio.stopSong();
  audioChangingTrack = false;
#endif
  alarmMusicActive = false;
  musicPlaying = false;
  musicPaused = false;
  alarmStatusMsg = "Alarma finalizada";
  audioStatusMsg = "Alarma finalizada";
  if (musicHighPerfActive && currentPage != PAGE_AUDIO && currentPage != PAGE_MUSIC_FOLDER && currentPage != PAGE_MUSIC_LIST) {
    leaveMusicHighPerformanceMode();
  }
  if (currentPage == PAGE_CLOCK) drawClockPage();
}

void alarmStartSelectedMusic() {
  enterMusicHighPerformanceMode();
  if (alarmMusicPath.length() > 0 && !sdTried) initSD();
#if HAVE_AUDIO_LIB
  if (alarmMusicPath.length() > 0 && sdOK && sdExistsCompat(alarmMusicPath.c_str())) {
    if (cameraOK) { deinitCamera(); cameraOK = false; cameraTried = false; }
    if (cameraWebStarted) { stopCameraServer(); deinitCamera(); cameraWebStarted = false; }
    audioChangingTrack = true;
    audio.stopSong();
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(musicVolume);
    sdLedMarkRead();
    if (audio.connecttoFS(SD_MMC, alarmMusicPath.c_str())) {
      musicPlaying = true;
      musicPaused = false;
      alarmMusicActive = true;
      lastAudioStartMs = millis();
      audioStatusMsg = "Alarma sonando";
      alarmStatusMsg = "Alarma sonando";
    } else {
      alarmMusicActive = false;
      audioChangingTrack = false;
      alarmStatusMsg = "No abre cancion";
      buzzerBeep(1800, 120); buzzerBeep(2200, 120); buzzerBeep(1800, 120);
      return;
    }
    audioChangingTrack = false;
    if (currentPage == PAGE_CLOCK) drawClockPage();
    return;
  }
#endif
  alarmStatusMsg = "Alarma beep";
  buzzerBeep(1800, 120);
  buzzerBeep(2200, 120);
  buzzerBeep(1800, 120);
}

// -------------------- WiFi teclado / conexion --------------------
int knownWiFiIndexForSSID(const String &ssid) {
  for (uint8_t i = 0; i < KNOWN_WIFI_COUNT; i++) {
    if (ssid == KNOWN_WIFI[i].ssid) return i;
  }
  return -1;
}


bool syncSpainTimeFromInternet(bool showOnTft) {
  if (WiFi.status() != WL_CONNECTED) return false;
  if (showOnTft) {
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.fillRect(0, 208, SCREEN_W, 50, TFT_BLACK);
    tft.drawString("Sincronizando hora...", 10, 214, 1);
  }

  // V47: zona horaria peninsular espanola con cambio automatico verano/invierno.
  // La V46 pedia la hora al servidor, pero a veces getLocalTime() tardaba mas
  // que el margen que le dabamos al arranque. Ahora se espera mas y se guarda
  // el epoch real del sistema con time(nullptr), no solo la hora visible.
  setenv("TZ", SPAIN_TZ, 1);
  tzset();
  configTzTime(SPAIN_TZ, "es.pool.ntp.org", "pool.ntp.org", "time.google.com");

  struct tm timeinfo;
  bool ok = false;
  uint32_t start = millis();
  uint8_t dots = 0;
  while (millis() - start < 15000UL) {
    if (getLocalTime(&timeinfo, 1000)) {
      time_t nowEpoch = time(nullptr);
      if (nowEpoch > 1700000000UL) {  // fecha razonable: despues de 2023
        clockBaseEpoch = nowEpoch;
        clockEpochValid = true;
        clockBaseMillis = millis();
        clockBaseSeconds = (uint32_t)timeinfo.tm_hour * 3600UL + (uint32_t)timeinfo.tm_min * 60UL + (uint32_t)timeinfo.tm_sec;
        char buf[32];
        snprintf(buf, sizeof(buf), "%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
        clockDateText = String(buf);
        clockNtpSynced = true;
        ok = true;
        break;
      }
    }
    if (showOnTft) {
      tft.fillRect(10, 226, 220, 14, TFT_BLACK);
      String msg = "Esperando NTP";
      for (uint8_t i = 0; i < dots; i++) msg += ".";
      tft.setTextColor(TFT_CYAN, TFT_BLACK);
      tft.drawString(msg.c_str(), 10, 226, 1);
      dots = (dots + 1) % 4;
    }
    delay(20);
  }

  if (!ok) {
    clockNtpSynced = false;
    if (!clockEpochValid) clockDateText = "NTP sin respuesta";
    if (showOnTft) {
      tft.setTextColor(TFT_ORANGE, TFT_BLACK);
      tft.fillRect(10, 226, 220, 14, TFT_BLACK);
      tft.drawString("Hora NTP no disponible", 10, 226, 1);
    }
    return false;
  }

  if (showOnTft) {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.fillRect(10, 226, 220, 14, TFT_BLACK);
    tft.drawString((String("Hora OK ") + clockDateText).c_str(), 10, 226, 1);
  }
  return true;
}

void wifiManualOff() {
  if (ftpEnabled) {
    ftpControlClient.stop();
    ftpEnabled = false;
    ftpLogged = false;
    ftpUserOk = false;
    ftpActiveReady = false;
    ftpStatus = "FTP parado por WiFi OFF";
  }
  if (cameraWebStarted) {
    stopCameraServer();
    deinitCamera();
    cameraWebStarted = false;
    cameraWebStatusMsg = "CamWeb parado por WiFi OFF";
  }
  if (otaReady) {
    otaServer.close();
    otaReady = false;
  }
  webServerEnabled = false;
  webServerStatusMsg = "Web apagada por WiFi OFF";
  WiFi.disconnect(false);
  WiFi.mode(WIFI_OFF);
  wifiScanned = false;
  wifiCount = 0;
  wifiSelectedIndex = -1;
  wifiStatusMsg = "WiFi apagado manualmente";
}

bool connectKnownWiFiByIndex(uint8_t idx, bool showOnTft, uint32_t timeoutMs) {
  if (idx >= KNOWN_WIFI_COUNT) return false;
  const char *ssid = KNOWN_WIFI[idx].ssid;
  const char *pass = KNOWN_WIFI[idx].password;

  if (showOnTft) {
    drawHeader("Auto WiFi");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Conectando a:", 10, 70, 2);
    String ssidShort = String(ssid);
    if (ssidShort.length() > 20) ssidShort = ssidShort.substring(0, 20);
    tft.drawString(ssidShort.c_str(), 10, 96, 2);
  }

  WiFi.mode(WIFI_STA);
  wifiUseRandomStaticIp = false;
  wifiRuntimeStaticIp = WIFI_LOCAL_IP;
  applyWiFiNetworkConfig();
  WiFi.begin(ssid, pass);
  uint32_t start = millis();
  int dots = 0;
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
    if (showOnTft) {
      tft.fillRect(10, 130, 220, 26, TFT_BLACK);
      String msg = "Esperando";
      for (int i = 0; i < dots; i++) msg += ".";
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.drawString(msg, 10, 130, 2);
      dots = (dots + 1) % 4;
    }
    delay(500);
  }

  // V56: si la IP fija preferida no consigue conectar, reintenta con IP aleatoria.
  if (WiFi.status() != WL_CONNECTED && WIFI_USE_STATIC_IP && WIFI_RANDOM_IP_IF_STATIC_FAILS && !wifiUseRandomStaticIp) {
    WiFi.disconnect(true);
    delay(350);
    chooseRandomWiFiStaticIp();
    if (showOnTft) {
      tft.fillRect(10, 130, 220, 58, TFT_BLACK);
      tft.setTextColor(TFT_ORANGE, TFT_BLACK);
      tft.drawString("IP .120 fallo", 10, 130, 2);
      tft.drawString((String("Probando ") + wifiRuntimeStaticIp.toString()).c_str(), 10, 154, 1);
    }
    WiFi.mode(WIFI_STA);
    applyWiFiNetworkConfig();
    WiFi.begin(ssid, pass);
    start = millis();
    dots = 0;
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
      if (showOnTft) {
        tft.fillRect(10, 180, 220, 22, TFT_BLACK);
        String msg = "Reintentando";
        for (int i = 0; i < dots; i++) msg += ".";
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.drawString(msg, 10, 180, 1);
        dots = (dots + 1) % 4;
      }
      delay(500);
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiStatusMsg = String("WiFi OK ") + WiFi.localIP().toString();
    otaStartIfPossible();
    syncSpainTimeFromInternet(showOnTft);
    if (showOnTft) {
      tft.setTextColor(TFT_GREEN, TFT_BLACK);
      tft.drawString("Conectado", 10, 166, 2);
      tft.drawString(WiFi.localIP().toString().c_str(), 10, 190, 2);
    }
    return true;
  }

  wifiStatusMsg = String("AutoWiFi fallo: ") + ssid;
  if (showOnTft) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("No conectado", 10, 166, 2);
  }
  return false;
}

void autoConnectKnownWiFi(bool showOnTft) {
  if (KNOWN_WIFI_COUNT == 0) return;
  if (WiFi.status() == WL_CONNECTED) return;

  if (showOnTft) {
    drawHeader("Auto WiFi");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Buscando redes...", 10, 72, 2);
  }

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false);
  applyWiFiNetworkConfig();
  delay(150);

  int n = WiFi.scanNetworks();
  wifiCount = n;
  wifiScanned = (n >= 0);

  int bestKnown = -1;
  int bestRssi = -1000;
  if (n > 0) {
    for (int net = 0; net < n; net++) {
      int known = knownWiFiIndexForSSID(WiFi.SSID(net));
      if (known >= 0 && WiFi.RSSI(net) > bestRssi) {
        bestKnown = known;
        bestRssi = WiFi.RSSI(net);
      }
    }
  }

  if (bestKnown >= 0) {
    connectKnownWiFiByIndex((uint8_t)bestKnown, showOnTft, 15000);
    return;
  }

  // Si el escaneo no ve la red, intenta igualmente la primera guardada.
  // Esto ayuda si el router tarda en aparecer o la red esta oculta.
  if (showOnTft) {
    tft.fillRect(0, 58, SCREEN_W, 170, TFT_BLACK);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.drawString("No vista en escaneo", 10, 72, 2);
    tft.drawString("Intentando guardada", 10, 96, 2);
  }
  connectKnownWiFiByIndex(0, showOnTft, 10000);
}

void drawWifiKeyboard() {
  currentPage = PAGE_WIFI;
  drawHeader("Clave WiFi");
  String ssid = wifiSelectedIndex >= 0 ? WiFi.SSID(wifiSelectedIndex) : "sin red";
  if (ssid.length() > 20) ssid = ssid.substring(0, 20);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString(ssid.c_str(), 6, 36, 1);
  // V5: mostrar la clave real, sin asteriscos.
  // Si es larga, se parte en dos lineas para que sea facil revisar errores.
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Clave:", 6, 52, 1);
  String shownPass = wifiPassword;
  if (shownPass.length() <= 31) {
    tft.drawString(shownPass.c_str(), 48, 52, 1);
  } else {
    tft.drawString(shownPass.substring(0, 31).c_str(), 48, 52, 1);
    tft.drawString(shownPass.substring(31).c_str(), 6, 64, 1);
  }

  const char *row1 = "1234567890";
  const char *row2l = "qwertyuiop";
  const char *row2u = "QWERTYUIOP";
  const char *row3l = "asdfghjkl";
  const char *row3u = "ASDFGHJKL";
  const char *row4l = "zxcvbnm.-_";
  const char *row4u = "ZXCVBNM.-_";
  const char *rows[] = { row1, wifiUpper ? row2u : row2l, wifiUpper ? row3u : row3l, wifiUpper ? row4u : row4l };
  int lens[] = { 10, 10, 9, 10 };
  int y0 = 78;
  for (int r = 0; r < 4; r++) {
    int keyW = 22;
    int gap = 2;
    int totalW = lens[r] * keyW + (lens[r] - 1) * gap;
    int x0 = (SCREEN_W - totalW) / 2;
    int y = y0 + r * 34;
    for (int i = 0; i < lens[r]; i++) {
      tft.fillRoundRect(x0 + i * (keyW + gap), y, keyW, 28, 4, TFT_NAVY);
      tft.drawRoundRect(x0 + i * (keyW + gap), y, keyW, 28, 4, TFT_DARKGREY);
      char s[2] = { rows[r][i], 0 };
      tft.setTextColor(TFT_WHITE, TFT_NAVY);
      tft.drawCentreString(s, x0 + i * (keyW + gap) + keyW / 2, y + 8, 1);
    }
  }

  Button shift = {  6, 222, 50, 28, wifiUpper ? "min" : "MAY", ACT_NONE };
  Button space = { 60, 222, 58, 28, "esp", ACT_NONE };
  Button del   = {122, 222, 50, 28, "bor", ACT_NONE };
  Button ok    = {176, 222, 58, 28, "OK", ACT_NONE };
  Button cancel= { 78, 286, 154, 28, "Cancelar", ACT_NONE };
  drawButton(shift, TFT_DARKGREEN);
  drawButton(space, TFT_BLUE);
  drawButton(del, TFT_MAROON);
  drawButton(ok, TFT_DARKGREEN);
  drawBackButton();
  drawButton(cancel, TFT_DARKGREY);
}

void handleWifiKeyboardTouch(int x, int y) {
  Button back = { 8, 286, 68, 28, "Atras", ACT_BACK };
  Button cancel= { 78, 286, 154, 28, "Cancelar", ACT_NONE };
  Button shift = {  6, 222, 50, 28, "MAY", ACT_NONE };
  Button space = { 60, 222, 58, 28, "esp", ACT_NONE };
  Button del   = {122, 222, 50, 28, "bor", ACT_NONE };
  Button ok    = {176, 222, 58, 28, "OK", ACT_NONE };
  if (pointInButton(x, y, back) || pointInButton(x, y, cancel)) {
    wifiKeyboard = false;
    drawWiFiPage();
    return;
  }
  if (pointInButton(x, y, shift)) { wifiUpper = !wifiUpper; drawWifiKeyboard(); return; }
  if (pointInButton(x, y, space)) { wifiPassword += ' '; drawWifiKeyboard(); return; }
  if (pointInButton(x, y, del)) {
    if (wifiPassword.length() > 0) wifiPassword.remove(wifiPassword.length() - 1);
    drawWifiKeyboard();
    return;
  }
  if (pointInButton(x, y, ok)) {
    wifiKeyboard = false;
    connectToSelectedWiFi();
    drawWiFiPage();
    return;
  }

  const char *row1 = "1234567890";
  const char *row2l = "qwertyuiop";
  const char *row2u = "QWERTYUIOP";
  const char *row3l = "asdfghjkl";
  const char *row3u = "ASDFGHJKL";
  const char *row4l = "zxcvbnm.-_";
  const char *row4u = "ZXCVBNM.-_";
  const char *rows[] = { row1, wifiUpper ? row2u : row2l, wifiUpper ? row3u : row3l, wifiUpper ? row4u : row4l };
  int lens[] = { 10, 10, 9, 10 };
  int y0 = 78;
  for (int r = 0; r < 4; r++) {
    int keyW = 22;
    int gap = 2;
    int totalW = lens[r] * keyW + (lens[r] - 1) * gap;
    int x0 = (SCREEN_W - totalW) / 2;
    int yy = y0 + r * 34;
    for (int i = 0; i < lens[r]; i++) {
      Button kb = { (int16_t)(x0 + i * (keyW + gap)), (int16_t)yy, (int16_t)keyW, 28, "", ACT_NONE };
      if (pointInButton(x, y, kb)) {
        if (wifiPassword.length() < 63) wifiPassword += rows[r][i];
        drawWifiKeyboard();
        return;
      }
    }
  }
}

void connectToSelectedWiFi() {
  if (wifiSelectedIndex < 0) return;
  String ssid = WiFi.SSID(wifiSelectedIndex);
  int known = knownWiFiIndexForSSID(ssid);
  if (known >= 0) wifiPassword = String(KNOWN_WIFI[known].password);
  drawHeader("WiFi");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Conectando a:", 10, 70, 2);
  String ssidShort = ssid.substring(0, 18);
  tft.drawString(ssidShort.c_str(), 10, 96, 2);
  WiFi.mode(WIFI_STA);
  wifiUseRandomStaticIp = false;
  wifiRuntimeStaticIp = WIFI_LOCAL_IP;
  applyWiFiNetworkConfig();
  WiFi.begin(ssid.c_str(), wifiPassword.c_str());
  uint32_t start = millis();
  int dots = 0;
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    tft.fillRect(10, 130, 220, 26, TFT_BLACK);
    String msg = "Esperando";
    for (int i = 0; i < dots; i++) msg += ".";
    tft.drawString(msg, 10, 130, 2);
    dots = (dots + 1) % 4;
    delay(500);
  }
  if (WiFi.status() != WL_CONNECTED && WIFI_USE_STATIC_IP && WIFI_RANDOM_IP_IF_STATIC_FAILS && !wifiUseRandomStaticIp) {
    WiFi.disconnect(true);
    delay(350);
    chooseRandomWiFiStaticIp();
    tft.fillRect(10, 130, 220, 58, TFT_BLACK);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.drawString("IP .120 fallo", 10, 130, 2);
    tft.drawString((String("Probando ") + wifiRuntimeStaticIp.toString()).c_str(), 10, 154, 1);
    WiFi.mode(WIFI_STA);
    applyWiFiNetworkConfig();
    WiFi.begin(ssid.c_str(), wifiPassword.c_str());
    start = millis();
    dots = 0;
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
      tft.fillRect(10, 180, 220, 22, TFT_BLACK);
      String msg = "Reintentando";
      for (int i = 0; i < dots; i++) msg += ".";
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.drawString(msg, 10, 180, 1);
      dots = (dots + 1) % 4;
      delay(500);
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifiStatusMsg = String("OK IP ") + WiFi.localIP().toString();
    otaStartIfPossible();
    syncSpainTimeFromInternet(true);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Conectado", 10, 166, 2);
  } else {
    wifiStatusMsg = "Fallo al conectar";
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("No conectado", 10, 166, 2);
  }
  delay(1000);
}

// -------------------- FTP microSD --------------------
void drawFTPPage() {
  currentPage = PAGE_FTP;
  drawHeader("Servidor FTP SD");
  int y = 44;
  if (!sdTried) initSD();
  String ftpIpLine = WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : String("NO");
  drawStatusLine(y, "WiFi", ftpIpLine.c_str(), WiFi.status() == WL_CONNECTED ? TFT_GREEN : TFT_RED); y += 24;
  drawStatusLine(y, "microSD", sdOK ? "OK" : "NO", sdOK ? TFT_GREEN : TFT_RED); y += 24;
  drawStatusLine(y, "FTP", ftpEnabled ? "ACTIVO" : "PARADO", ftpEnabled ? TFT_GREEN : TFT_ORANGE); y += 24;
  drawStatusLine(y, "Usuario", "esp32", TFT_CYAN); y += 24;
  drawStatusLine(y, "Clave", "esp32sd", TFT_CYAN); y += 24;
  drawStatusLine(y, "Puerto", "21 / PASV 50009", TFT_CYAN); y += 28;
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString(ftpStatus.c_str(), 10, y, 1);
  Button tog = { 82, 286, 72, 28, ftpEnabled ? "Parar" : "Iniciar", ACT_FTP_TOGGLE };
  Button ref = { 158, 286, 74, 28, "Refres.", ACT_FTP_REFRESH };
  drawBackButton();
  drawButton(tog, ftpEnabled ? TFT_MAROON : TFT_DARKGREEN);
  drawButton(ref, TFT_BLUE);
}

void handleFTPTouch(int x, int y) {
  Button tog = { 82, 286, 72, 28, "FTP", ACT_FTP_TOGGLE };
  Button ref = { 158, 286, 74, 28, "Refres.", ACT_FTP_REFRESH };
  if (pointInButton(x, y, tog)) { ftpStartStop(); drawFTPPage(); }
  if (pointInButton(x, y, ref)) { drawFTPPage(); }
}

// -------------------- Reloj con alarma --------------------
uint32_t getClockSeconds() {
  if (clockEpochValid) {
    time_t nowEpoch = clockBaseEpoch + (time_t)((millis() - clockBaseMillis) / 1000UL);
    struct tm timeinfo;
    localtime_r(&nowEpoch, &timeinfo);
    return (uint32_t)timeinfo.tm_hour * 3600UL + (uint32_t)timeinfo.tm_min * 60UL + (uint32_t)timeinfo.tm_sec;
  }
  return (clockBaseSeconds + (millis() - clockBaseMillis) / 1000UL) % 86400UL;
}

void drawAnalogClock(int cx, int cy, int r, int hh, int mm, int ss) {
  tft.drawCircle(cx, cy, r, TFT_WHITE);
  tft.drawCircle(cx, cy, r - 1, TFT_DARKGREY);
  for (int i = 0; i < 12; i++) {
    float a = (float)i * 30.0f * 0.0174532925f - 1.5707963f;
    int x1 = cx + (int)(cos(a) * (r - 5));
    int y1 = cy + (int)(sin(a) * (r - 5));
    int x2 = cx + (int)(cos(a) * (r - 12));
    int y2 = cy + (int)(sin(a) * (r - 12));
    tft.drawLine(x1, y1, x2, y2, TFT_LIGHTGREY);
  }
  float ah = ((hh % 12) + mm / 60.0f) * 30.0f * 0.0174532925f - 1.5707963f;
  float am = (mm + ss / 60.0f) * 6.0f * 0.0174532925f - 1.5707963f;
  float as = ss * 6.0f * 0.0174532925f - 1.5707963f;
  tft.drawLine(cx, cy, cx + (int)(cos(ah) * 34), cy + (int)(sin(ah) * 34), TFT_WHITE);
  tft.drawLine(cx, cy, cx + (int)(cos(am) * 52), cy + (int)(sin(am) * 52), TFT_CYAN);
  tft.drawLine(cx, cy, cx + (int)(cos(as) * 58), cy + (int)(sin(as) * 58), TFT_RED);
  tft.fillCircle(cx, cy, 3, TFT_YELLOW);
}

void drawScreenTimeoutPage() {
  currentPage = PAGE_SCREEN_TIMEOUT;
  drawHeader("Apagado pantalla");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Si no tocas la pantalla,", 10, 48, 2);
  tft.drawString("la TFT se apaga sola.", 10, 72, 2);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("El tactil queda activo.", 10, 102, 2);
  tft.drawString("Un toque la despierta", 10, 126, 2);
  tft.drawString("sin cambiar de pantalla.", 10, 150, 2);

  char buf[40];
  snprintf(buf, sizeof(buf), "Actual: %u minuto%s", screenTimeoutMinutes, screenTimeoutMinutes == 1 ? "" : "s");
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawCentreString(buf, SCREEN_W / 2, 178, 2);

  Button b1 = {  8, 210, 40, 30, "1m", ACT_SCREEN_1MIN };
  Button b2 = {  54, 210, 40, 30, "2m", ACT_SCREEN_2MIN };
  Button b3 = { 100, 210, 40, 30, "3m", ACT_SCREEN_3MIN };
  Button b4 = { 146, 210, 40, 30, "4m", ACT_SCREEN_4MIN };
  Button b5 = { 192, 210, 40, 30, "5m", ACT_SCREEN_5MIN };
  drawButton(b1, screenTimeoutMinutes == 1 ? TFT_DARKGREEN : TFT_BLUE);
  drawButton(b2, screenTimeoutMinutes == 2 ? TFT_DARKGREEN : TFT_BLUE);
  drawButton(b3, screenTimeoutMinutes == 3 ? TFT_DARKGREEN : TFT_BLUE);
  drawButton(b4, screenTimeoutMinutes == 4 ? TFT_DARKGREEN : TFT_BLUE);
  drawButton(b5, screenTimeoutMinutes == 5 ? TFT_DARKGREEN : TFT_BLUE);

  drawBackButton();
}

void drawClockPage() {
  currentPage = PAGE_CLOCK;
  lastRefreshMs = millis();
  drawHeader("Reloj / alarma");
  Button hplus  = {   8, 224, 46, 24, "H+", ACT_CLOCK_H_PLUS };
  Button mplus  = {  58, 224, 46, 24, "M+", ACT_CLOCK_M_PLUS };
  Button ahplus = { 112, 224, 46, 24, "AH+", ACT_CLOCK_AH_PLUS };
  Button amplus = { 164, 224, 46, 24, "AM+", ACT_CLOCK_AM_PLUS };
  Button song   = {   8, 252, 78, 26, "Cancion", ACT_CLOCK_ALARM_SONG };
  Button toggle = {  92, 252, 140, 26, alarmEnabled ? "Alarma OFF" : "Alarma ON", ACT_CLOCK_ALARM_TOGGLE };
  drawButton(hplus, TFT_BLUE);
  drawButton(mplus, TFT_BLUE);
  drawButton(ahplus, TFT_DARKGREEN);
  drawButton(amplus, TFT_DARKGREEN);
  drawButton(song, alarmMusicPath.length() ? TFT_DARKGREEN : TFT_DARKGREY);
  drawBackButton();
  drawButton(toggle, TFT_ORANGE);
  updateClockPageDynamic(true);
}

void updateClockPageDynamic(bool force) {
  (void)force;
  tft.fillRect(0, 36, SCREEN_W, 184, TFT_BLACK);
  updateClockDateTextFromEpoch();
  uint32_t sec = getClockSeconds();
  int hh = (sec / 3600UL) % 24;
  int mm = (sec / 60UL) % 60;
  int ss = sec % 60;
  char buf[32];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hh, mm, ss);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawCentreString(buf, SCREEN_W / 2, 38, 4);
  tft.setTextColor(clockNtpSynced ? TFT_CYAN : TFT_LIGHTGREY, TFT_BLACK);
  String dateLine = clockDateText;
  if (clockNtpSynced) dateLine += " NTP";
  tft.drawCentreString(dateLine.c_str(), SCREEN_W / 2, 72, 1);
  drawAnalogClock(120, 132, 58, hh, mm, ss);
  snprintf(buf, sizeof(buf), "Alarma %02u:%02u %s", alarmHour, alarmMinute, alarmEnabled ? "ON" : "OFF");
  tft.setTextColor(alarmEnabled ? TFT_ORANGE : TFT_LIGHTGREY, TFT_BLACK);
  tft.drawCentreString(buf, SCREEN_W / 2, 196, 2);
  String songLine = alarmMusicPath.length() ? folderBaseName(alarmMusicPath) : String("sin cancion: beep");
  if (songLine.length() > 26) songLine = songLine.substring(0, 26);
  tft.setTextColor(alarmMusicPath.length() ? TFT_CYAN : TFT_LIGHTGREY, TFT_BLACK);
  tft.drawCentreString(songLine.c_str(), SCREEN_W / 2, 214, 1);
}

void handleClockTouch(int x, int y) {
  Button hplus  = {   8, 224, 46, 24, "H+", ACT_CLOCK_H_PLUS };
  Button mplus  = {  58, 224, 46, 24, "M+", ACT_CLOCK_M_PLUS };
  Button ahplus = { 112, 224, 46, 24, "AH+", ACT_CLOCK_AH_PLUS };
  Button amplus = { 164, 224, 46, 24, "AM+", ACT_CLOCK_AM_PLUS };
  Button song   = {   8, 252, 78, 26, "Cancion", ACT_CLOCK_ALARM_SONG };
  Button toggle = {  92, 252, 140, 26, "Alarma", ACT_CLOCK_ALARM_TOGGLE };
  if (pointInButton(x, y, hplus)) adjustClockBySeconds(3600);
  if (pointInButton(x, y, mplus)) adjustClockBySeconds(60);
  if (pointInButton(x, y, ahplus)) alarmHour = (alarmHour + 1) % 24;
  if (pointInButton(x, y, amplus)) alarmMinute = (alarmMinute + 1) % 60;
  if (pointInButton(x, y, song)) { alarmMusicSelectMode = true; if (!sdTried) initSD(); loadMusicFolderConfig(); if (!musicScanned) scanMusicFiles(); musicListOffset = (musicIndex / MUSIC_LIST_ROWS) * MUSIC_LIST_ROWS; drawMusicListPage(); return; }
  if (pointInButton(x, y, toggle)) { alarmEnabled = !alarmEnabled; alarmWasActive = false; alarmStatusMsg = alarmEnabled ? "Alarma activada" : "Alarma desactivada"; }
  drawClockPage();
}

void alarmCheck() {
  if (!alarmEnabled) return;
  uint32_t sec = getClockSeconds();
  int hh = (sec / 3600UL) % 24;
  int mm = (sec / 60UL) % 60;
  int ss = sec % 60;
  bool activeMinute = (hh == alarmHour && mm == alarmMinute);
  if (activeMinute && !alarmWasActive && ss < 5) {
    alarmWasActive = true;
    alarmStartSelectedMusic();
  }
  if (!activeMinute) alarmWasActive = false;
}

// -------------------- Calculadora --------------------
void drawCalcPage() {
  currentPage = PAGE_CALC;
  drawHeader("Calculadora");
  tft.fillRoundRect(8, 42, 224, 42, 6, TFT_DARKGREY);
  tft.drawRoundRect(8, 42, 224, 42, 6, TFT_WHITE);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  String disp = calcDisplay;
  if (disp.length() > 18) disp = disp.substring(disp.length() - 18);
  tft.drawRightString(disp, 224, 55, 2);

  const char *keys[5][4] = {
    {"C", "<", "/", "x"},
    {"7", "8", "9", "-"},
    {"4", "5", "6", "+"},
    {"1", "2", "3", "="},
    {"0", ".", "+/-", "%"}
  };
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 4; c++) {
      int x = 8 + c * 58;
      int y = 96 + r * 36;
      uint16_t col = TFT_NAVY;
      if (c == 3 || r == 0) col = TFT_DARKGREEN;
      if (strcmp(keys[r][c], "C") == 0) col = TFT_MAROON;
      Button kb = { (int16_t)x, (int16_t)y, 52, 30, keys[r][c], ACT_NONE };
      drawButton(kb, col);
    }
  }
  drawBackButton();
}

void handleScreenTimeoutTouch(int x, int y) {
  Button b1 = {  8, 210, 40, 30, "1m", ACT_SCREEN_1MIN };
  Button b2 = {  54, 210, 40, 30, "2m", ACT_SCREEN_2MIN };
  Button b3 = { 100, 210, 40, 30, "3m", ACT_SCREEN_3MIN };
  Button b4 = { 146, 210, 40, 30, "4m", ACT_SCREEN_4MIN };
  Button b5 = { 192, 210, 40, 30, "5m", ACT_SCREEN_5MIN };
  uint8_t selected = 0;
  if (pointInButton(x, y, b1)) selected = 1;
  else if (pointInButton(x, y, b2)) selected = 2;
  else if (pointInButton(x, y, b3)) selected = 3;
  else if (pointInButton(x, y, b4)) selected = 4;
  else if (pointInButton(x, y, b5)) selected = 5;
  if (selected > 0) {
    screenTimeoutMinutes = selected;
    saveScreenTimeoutConfig();
    lastUserActivityMs = millis();
    drawScreenTimeoutPage();
  }
}

void handleCalcTouch(int x, int y) {
  const char *keys[5][4] = {
    {"C", "<", "/", "x"},
    {"7", "8", "9", "-"},
    {"4", "5", "6", "+"},
    {"1", "2", "3", "="},
    {"0", ".", "+/-", "%"}
  };
  for (int r = 0; r < 5; r++) {
    for (int c = 0; c < 4; c++) {
      Button kb = { (int16_t)(8 + c * 58), (int16_t)(96 + r * 36), 52, 30, keys[r][c], ACT_NONE };
      if (pointInButton(x, y, kb)) {
        calcPress(keys[r][c]);
        drawCalcPage();
        return;
      }
    }
  }
}

void calcPress(const char *key) {
  if (strcmp(key, "C") == 0) {
    calcDisplay = "0";
    calcStored = 0;
    calcOperator = 0;
    calcNewEntry = true;
    return;
  }
  if (strcmp(key, "<") == 0) {
    if (!calcNewEntry && calcDisplay.length() > 1) calcDisplay.remove(calcDisplay.length() - 1);
    else calcDisplay = "0";
    return;
  }
  if (strcmp(key, "+/-") == 0) {
    if (calcDisplay.startsWith("-")) calcDisplay.remove(0, 1);
    else if (calcDisplay != "0") calcDisplay = "-" + calcDisplay;
    return;
  }
  if (strcmp(key, "%") == 0) {
    double value = calcDisplay.toDouble();
    if (calcOperator == '+' || calcOperator == '-') value = calcStored * value / 100.0;
    else value = value / 100.0;
    calcDisplay = formatDouble(value);
    calcNewEntry = false;
    return;
  }
  if (strlen(key) == 1 && isdigit(key[0])) {
    if (calcNewEntry || calcDisplay == "0") {
      calcDisplay = key;
      calcNewEntry = false;
    } else if (calcDisplay.length() < 18) {
      calcDisplay += key;
    }
    return;
  }
  if (strcmp(key, ".") == 0) {
    if (calcNewEntry) { calcDisplay = "0"; calcNewEntry = false; }
    if (calcDisplay.indexOf('.') < 0) calcDisplay += ".";
    return;
  }
  if (strlen(key) == 1 && strchr("+-/x*", key[0])) {
    calcStored = calcDisplay.toDouble();
    calcOperator = key[0];
    calcNewEntry = true;
    return;
  }
  if (strcmp(key, "=") == 0) {
    double b = calcDisplay.toDouble();
    double result = b;
    if (calcOperator == '+') result = calcStored + b;
    else if (calcOperator == '-') result = calcStored - b;
    else if (calcOperator == '*' || calcOperator == 'x') result = calcStored * b;
    else if (calcOperator == '/') {
      if (b == 0) { calcDisplay = "Error div 0"; calcNewEntry = true; return; }
      result = calcStored / b;
    }
    calcDisplay = formatDouble(result);
    calcOperator = 0;
    calcNewEntry = true;
  }
}

String formatDouble(double value) {
  char buf[32];
  dtostrf(value, 0, 6, buf);
  String s = String(buf);
  s.trim();
  while (s.indexOf('.') >= 0 && s.endsWith("0")) s.remove(s.length() - 1);
  if (s.endsWith(".")) s.remove(s.length() - 1);
  if (s.length() == 0) s = "0";
  return s;
}

uint16_t swapRedBlue565(uint16_t c) {
  return (uint16_t)((c & 0x07E0) | ((c & 0x001F) << 11) | ((c & 0xF800) >> 11));
}

// -------------------- I2C --------------------
void scanI2C() {
  i2cCount = 0;
  Wire.begin(I2C_SDA, I2C_SCL);
  for (uint8_t address = 1; address < 127 && i2cCount < 20; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    if (error == 0) i2cFound[i2cCount++] = address;
  }
  touchDetected = false;
  for (uint8_t i = 0; i < i2cCount; i++) {
    if (i2cFound[i] == 0x38) touchDetected = true;
  }
  Serial.print("I2C encontrados: ");
  Serial.println(i2cCount);
}

// -------------------- microSD --------------------
// V4: ArduinoDroid estaba dando error con fopen() y antes tambien con SD_MMC.open()/mkdir().
// Para asegurar compilacion en Android, esta version monta la microSD y muestra tamano/uso,
// pero NO escribe ni lista archivos. La escritura se anadira despues con una version especifica
// cuando confirmemos que el resto carga en la placa.
bool initSD() {
  sdTried = true;
  SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
  sdOK = SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5);
  if (sdOK) {
    Serial.println("SD_MMC OK");
  } else {
    Serial.println("SD_MMC fallo");
  }
  return sdOK;
}

void createSDTestFile() {
  if (!sdTried) initSD();
  // Escritura desactivada en V3 para evitar errores de compilacion en ArduinoDroid.
}

void listSDFiles(const char *path, int yStart) {
  (void)path;
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString("Listado SD desactivado", 10, yStart, 2);
  tft.drawString("V4 evita fopen/open", 10, yStart + 20, 2);
}

// -------------------- Camara --------------------
bool initCamera() {
  cameraTried = true;
  esp_camera_deinit();
  camera_config_t config;
  memset(&config, 0, sizeof(config));
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
#else
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
#endif
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.frame_size = FRAMESIZE_240X240;
  config.pixel_format = PIXFORMAT_RGB565;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10;
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    cameraOK = false;
    return false;
  }
  sensor_t *s = esp_camera_sensor_get();
  if (s) {
    // Igual que Sketch_15_Lvgl_Camera: sin efectos extra y con espejo horizontal.
    s->set_vflip(s, cameraFlipVertical);
    s->set_hmirror(s, cameraMirrorHorizontal);
    s->set_brightness(s, 0);
    s->set_contrast(s, 1);
    s->set_saturation(s, 1);
    s->set_special_effect(s, 0);   // 0 = sin negativo / sin filtro de color
    s->set_whitebal(s, 1);
    s->set_awb_gain(s, 1);
    s->set_gain_ctrl(s, 1);
    s->set_exposure_ctrl(s, 1);
  }
  cameraOK = true;
  Serial.println("Camara OK metodo Sketch_15 + salida TFT corregida V11");
  return true;
}

void deinitCamera() {
  if (cameraOK) esp_camera_deinit();
  cameraOK = false;
}

// Compatibilidad SD_MMC ArduinoDroid / ESP32 core 3.x.
// Algunas versiones de SDMMCFS aceptan char* en vez de const char*.
// Estos wrappers evitan los errores "no matching member function".
char *sdPathCompat(const char *path) {
  return (char *)(path ? path : "/");
}

File sdOpenCompat(const char *path) {
  sdLedMarkRead();
  return SD_MMC.open(sdPathCompat(path));
}

File sdOpenCompat(const char *path, const char *mode) {
  if (sdModeIsWrite(mode)) sdLedMarkWrite();
  else sdLedMarkRead();
  return SD_MMC.open(sdPathCompat(path), mode);
}

bool sdExistsCompat(const char *path) {
  File f = sdOpenCompat(path);
  if (f) {
    f.close();
    return true;
  }
  return false;
}

bool sdMkdirCompat(const char *path) {
  sdLedMarkWrite();
  return SD_MMC.mkdir(sdPathCompat(path));
}

bool sdRemoveCompat(const char *path) {
  sdLedMarkWrite();
  return SD_MMC.remove(sdPathCompat(path));
}

bool sdRmdirCompat(const char *path) {
  sdLedMarkWrite();
  return SD_MMC.rmdir(sdPathCompat(path));
}

bool sdRenameCompat(const char *from, const char *to) {
  sdLedMarkWrite();
  return SD_MMC.rename(sdPathCompat(from), sdPathCompat(to));
}

bool ensureCameraTftBuffer() {
  if (cameraTftBuf != nullptr) return true;
  cameraTftBuf = (uint16_t *)ps_malloc(CAMERA_RGB565_PIXELS * sizeof(uint16_t));
  if (cameraTftBuf == nullptr) {
    cameraTftBuf = (uint16_t *)malloc(CAMERA_RGB565_PIXELS * sizeof(uint16_t));
  }
  if (cameraTftBuf == nullptr) {
    Serial.println("No hay memoria para buffer de camara TFT");
    return false;
  }
  memset(cameraTftBuf, 0, CAMERA_RGB565_PIXELS * sizeof(uint16_t));
  return true;
}

uint16_t cameraPixelFromBytes(uint8_t hi, uint8_t lo) {
  // Logica base del Sketch_15_Lvgl_Camera:
  // El sensor entrega cada pixel RGB565 como dos bytes. El sketch que funciona
  // intercambia esos bytes antes de mostrarlo. En una variable uint16_t del ESP32
  // esto equivale a guardar el valor RGB565 normal y enviarlo despues con
  // tft.pushColors(..., true), igual que hace el flush de LVGL.
  uint16_t sketch15_rgb = ((uint16_t)hi << 8) | lo;

  // Modos de rescate por si algun modulo/camara entrega rojo y azul cruzados o
  // una imagen invertida. El modo 0 es el que replica el sketch comprimido.
  switch (cameraColorMode) {
    case 0: return sketch15_rgb;                         // Color real Sketch_15
    case 1: return swapRedBlue565(sketch15_rgb);         // Rojo/azul cruzados
    case 2: return (uint16_t)(~sketch15_rgb);            // Negativo, por diagnostico
    case 3: return (uint16_t)(~swapRedBlue565(sketch15_rgb));
    default: return sketch15_rgb;
  }
}

const char *cameraColorModeLabel() {
  switch (cameraColorMode) {
    case 0: return "Color: Sketch15";
    case 1: return "Color: R/B";
    case 2: return "Color: INV";
    case 3: return "Color: INV R/B";
    default: return "Color: Sketch15";
  }
}

void cameraNextColorMode() {
  cameraColorMode = (cameraColorMode + 1) % 4;
}

const char *cameraOrientationLabel() {
  switch (cameraOrientationMode) {
    case 0: return "Ori: Normal";
    case 1: return "Ori: Arriba/abajo";
    case 2: return "Ori: Izq/der";
    case 3: return "Ori: 180";
    default: return "Ori: Normal";
  }
}

void cameraNextOrientationMode() {
  cameraOrientationMode = (cameraOrientationMode + 1) % 4;
}

bool prepareCameraFrameForTFT(camera_fb_t *fb) {
  if (!fb || fb->format != PIXFORMAT_RGB565 || fb->width != 240 || fb->height != 240) return false;
  if (!ensureCameraTftBuffer()) return false;

  const int w = 240;
  const int h = 240;
  size_t pixels = (size_t)fb->width * (size_t)fb->height;
  if (pixels > CAMERA_RGB565_PIXELS) pixels = CAMERA_RGB565_PIXELS;
  size_t availablePixels = fb->len / 2;
  if (pixels > availablePixels) pixels = availablePixels;

  for (size_t p = 0, i = 0; p < pixels; p++, i += 2) {
    uint16_t color = cameraPixelFromBytes(fb->buf[i], fb->buf[i + 1]);
    int x = p % w;
    int y = p / w;
    size_t dst = p;
    switch (cameraOrientationMode) {
      case 1: dst = (size_t)(h - 1 - y) * w + x; break;                 // invertir arriba/abajo
      case 2: dst = (size_t)y * w + (w - 1 - x); break;                 // invertir izquierda/derecha
      case 3: dst = (size_t)(h - 1 - y) * w + (w - 1 - x); break;       // rotacion 180
      default: dst = p; break;
    }
    if (dst < CAMERA_RGB565_PIXELS) cameraTftBuf[dst] = color;
  }
  return true;
}

void drawCameraFrame() {
  if (!cameraOK) return;
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) return;

  if (prepareCameraFrameForTFT(fb)) {
    // Misma salida efectiva que el Sketch_15_Lvgl_Camera:
    // buffer RGB565 corregido + envio TFT_eSPI con swap activo.
    tft.startWrite();
    tft.setAddrWindow(0, 40, 240, 240);
    tft.pushColors(cameraTftBuf, CAMERA_RGB565_PIXELS, true);
    tft.endWrite();
  }

  esp_camera_fb_return(fb);
}

void ensurePictureFolder() {
  if (!sdTried) initSD();
  if (!sdOK) return;
  if (!sdExistsCompat("/picture")) sdMkdirCompat("/picture");
}

String nextPhotoPath() {
  ensurePictureFolder();
  for (int i = 1; i < 10000; i++) {
    char path[40];
    snprintf(path, sizeof(path), "/picture/%04d.bmp", i);
    if (!sdExistsCompat(path)) return String(path);
  }
  return "/picture/captura.bmp";
}

bool saveCameraBMP(const char *path) {
  if (!cameraOK || !sdOK || !path) return false;
  ensurePictureFolder();
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) return false;

  bool ok = false;
  if (prepareCameraFrameForTFT(fb)) {
    File file = sdOpenCompat(path, FILE_WRITE);
    if (file) {
      uint8_t bmp_info[] = {
        0x42,0x4d,0x48,0xc2,0x01,0x00,0x00,0x00,0x00,0x00,0x46,0x00,0x00,0x00,0x38,0x00,
        0x00,0x00,0xf0,0x00,0x00,0x00,0xf0,0x00,0x00,0x00,0x01,0x00,0x10,0x00,0x03,0x00,
        0x00,0x00,0x02,0xc2,0x01,0x00,0x12,0x0b,0x00,0x00,0x12,0x0b,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x00,0x00,0xe0,0x07,0x00,0x00,0x1f,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00
      };
      uint8_t bmp_efo[] = {0x00,0x00};
      file.write(bmp_info, 70);
      for (int yy = 239; yy >= 0; yy--) {
        file.write((uint8_t *)&cameraTftBuf[yy * 240], 240 * 2);
      }
      file.write(bmp_efo, 2);
      file.close();
      ok = true;
    }
  }

  esp_camera_fb_return(fb);
  return ok;
}


bool captureAndSavePhoto() {
  if (!sdTried) initSD();
  tft.fillRect(0, 34, SCREEN_W, 22, TFT_BLACK);
  if (!sdOK || !cameraOK) {
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("SD o camara no disponible", 4, 36, 1);
    return false;
  }

  String path = nextPhotoPath();
  if (saveCameraBMP(path.c_str())) {
    lastPhotoPath = path;
    photoStatusMsg = "Guardada: " + path;
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(path.c_str(), 4, 36, 1);
    return true;
  }

  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("No se pudo guardar", 4, 36, 1);
  return false;
}

bool isBmpFileName(const String &name) {
  String lower = name;
  lower.toLowerCase();
  return lower.endsWith(".bmp");
}

void scanPhotoFiles() {
  photoCount = 0;
  ensurePictureFolder();
  if (!sdOK) return;

  File dir = sdOpenCompat("/picture");
  if (!dir || !dir.isDirectory()) {
    if (dir) dir.close();
    return;
  }

  File file = dir.openNextFile();
  while (file && photoCount < MAX_PHOTO_FILES) {
    if (!file.isDirectory()) {
      String name = String(file.name());
      if (isBmpFileName(name)) {
        if (!name.startsWith("/")) name = String("/picture/") + name;
        photoFiles[photoCount++] = name;
      }
    }
    file.close();
    file = dir.openNextFile();
  }
  dir.close();

  // Orden simple para que /picture/0001.bmp, /picture/0002.bmp... salgan en orden.
  for (int i = 0; i < photoCount - 1; i++) {
    for (int j = i + 1; j < photoCount; j++) {
      if (photoFiles[j] < photoFiles[i]) {
        String tmp = photoFiles[i];
        photoFiles[i] = photoFiles[j];
        photoFiles[j] = tmp;
      }
    }
  }

  if (lastPhotoPath.length() > 0) {
    for (int i = 0; i < photoCount; i++) {
      if (photoFiles[i] == lastPhotoPath) {
        photoIndex = i;
        break;
      }
    }
  }
  if (photoIndex < 0) photoIndex = 0;
  if (photoIndex >= photoCount) photoIndex = photoCount - 1;
}

bool drawBmpPhoto(const char *path, int x, int y) {
  if (!path || !sdOK) return false;
  File file = sdOpenCompat(path);
  if (!file) return false;

  uint8_t header[70];
  int got = file.read(header, sizeof(header));
  if (got < 54 || header[0] != 'B' || header[1] != 'M') {
    file.close();
    return false;
  }

  uint32_t dataOffset = (uint32_t)header[10] | ((uint32_t)header[11] << 8) | ((uint32_t)header[12] << 16) | ((uint32_t)header[13] << 24);
  int32_t width = (int32_t)((uint32_t)header[18] | ((uint32_t)header[19] << 8) | ((uint32_t)header[20] << 16) | ((uint32_t)header[21] << 24));
  int32_t height = (int32_t)((uint32_t)header[22] | ((uint32_t)header[23] << 8) | ((uint32_t)header[24] << 16) | ((uint32_t)header[25] << 24));
  uint16_t bpp = (uint16_t)header[28] | ((uint16_t)header[29] << 8);

  if (width <= 0 || height == 0 || bpp != 16) {
    file.close();
    return false;
  }

  bool bottomUp = height > 0;
  int32_t absH = bottomUp ? height : -height;
  int32_t drawW = min((int32_t)SCREEN_W, width);
  int32_t drawH = min((int32_t)240, absH);
  uint32_t rowSize = ((uint32_t)width * 2U + 3U) & ~3U;

  static uint16_t rowBuf[SCREEN_W];
  tft.startWrite();
  for (int32_t row = 0; row < drawH; row++) {
    int32_t srcRow = bottomUp ? (absH - 1 - row) : row;
    uint32_t pos = dataOffset + (uint32_t)srcRow * rowSize;
    if (!file.seek(pos)) {
      tft.endWrite();
      file.close();
      return false;
    }
    int need = drawW * 2;
    int readBytes = file.read((uint8_t *)rowBuf, need);
    if (readBytes < need) {
      tft.endWrite();
      file.close();
      return false;
    }
    tft.setAddrWindow(x, y + row, drawW, 1);
    tft.pushColors(rowBuf, drawW, true);
  }
  tft.endWrite();
  file.close();
  return true;
}

void handlePhotoViewerTouch(int x, int y) {
  Button prev = { 82, 286, 44, 28, "<", ACT_PHOTO_PREV };
  Button next = { 132, 286, 44, 28, ">", ACT_PHOTO_NEXT };
  Button refresh = { 182, 286, 50, 28, "Act.", ACT_PHOTO_REFRESH };

  if (pointInButton(x, y, prev)) {
    lastPhotoPath = "";
    if (photoCount > 0) {
      photoIndex--;
      if (photoIndex < 0) photoIndex = photoCount - 1;
    }
    drawPhotoViewerPage();
    return;
  }
  if (pointInButton(x, y, next)) {
    lastPhotoPath = "";
    if (photoCount > 0) {
      photoIndex++;
      if (photoIndex >= photoCount) photoIndex = 0;
    }
    drawPhotoViewerPage();
    return;
  }
  if (pointInButton(x, y, refresh)) {
    scanPhotoFiles();
    drawPhotoViewerPage();
    return;
  }
}

// -------------------- WiFi --------------------
void scanWiFiAndDraw() {
  drawHeader("WiFi");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Escaneando...", 10, 64, 2);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(false);
  applyWiFiNetworkConfig();
  delay(150);
  wifiCount = WiFi.scanNetworks();
  wifiSelectedIndex = -1;
  wifiScanned = true;
  wifiStatusMsg = String(wifiCount) + " redes encontradas";
  drawWiFiPage();
}

// -------------------- WS2812 simple --------------------
// Version segura para ArduinoDroid: no usa ensamblador, no usa noInterrupts(),
// y no usa la libreria Freenove_WS2812 que estaba dando errores de enlazado.
// En ESP32 core 3.x, neopixelWrite suele estar disponible desde Arduino.h.
// Si tu build no tiene neopixelWrite, cambia USE_SAFE_NEOPIXEL a 0 y compilará
// como prueba basica encendiendo/apagando GPIO48 sin color real.
#define USE_SAFE_NEOPIXEL 1

void ws2812Init() {
  pinMode(LEDS_PIN, OUTPUT);
  digitalWrite(LEDS_PIN, LOW);
  ws2812SetColor(0, 0, 0, 0);
}

void ws2812SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
  uint16_t br = brightness;
  uint8_t rr = (uint16_t)r * br / 255;
  uint8_t gg = (uint16_t)g * br / 255;
  uint8_t bb = (uint16_t)b * br / 255;
#if USE_SAFE_NEOPIXEL
  neopixelWrite(LEDS_PIN, rr, gg, bb);
  if (rr == 0 && gg == 0 && bb == 0) {
    delay(1);
    neopixelWrite(LEDS_PIN, 0, 0, 0);  // segunda orden para garantizar apagado real
    digitalWrite(LEDS_PIN, LOW);
  }
#else
  digitalWrite(LEDS_PIN, (rr || gg || bb) ? HIGH : LOW);
#endif
  delay(1);
}

uint16_t ws2812UiColor565(uint8_t r, uint8_t g, uint8_t b) {
  // En esta pantalla concreta los colores UI del WS2812 salian como
  // inversion + rojo/azul cruzados. Esta compensacion solo afecta a las
  // barras/preview de esta pagina; no cambia el color real enviado al LED.
  uint16_t desired = tft.color565(r, g, b);
  return swapRedBlue565((uint16_t)(~desired));
}

bool ledHasVisibleValue() {
  return ledBright > 0 && (ledR > 0 || ledG > 0 || ledB > 0);
}

void saveLedBeforeOff() {
  if (ledHasVisibleValue()) {
    ledSavedR = ledR;
    ledSavedG = ledG;
    ledSavedB = ledB;
    ledSavedBright = ledBright;
  }
  if (ledSavedBright <= 0) ledSavedBright = 80;
  if (ledSavedR == 0 && ledSavedG == 0 && ledSavedB == 0) ledSavedB = 80;
}

void restoreLedAfterOff() {
  ledPoweredOff = false;
  ledEffectMode = 0;
  ledEffectStep = 0;
  ledBlinkState = false;
  ledR = constrain(ledSavedR, 0, 255);
  ledG = constrain(ledSavedG, 0, 255);
  ledB = constrain(ledSavedB, 0, 255);
  ledBright = constrain(ledSavedBright, 1, 255);
  if (ledR == 0 && ledG == 0 && ledB == 0) ledB = 80;
}

void drawSliderControl(int x, int y, int w, const char *label, int value, uint16_t color) {
  value = constrain(value, 0, 255);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  char txt[34];
  snprintf(txt, sizeof(txt), "%s %d", label, value);
  tft.drawString(txt, x, y - 16, 1);
  tft.drawRoundRect(x, y, w, 18, 5, TFT_DARKGREY);
  int filled = ::map(value, 0, 255, 0, w - 4);
  if (filled > 0) tft.fillRoundRect(x + 2, y + 2, filled, 14, 4, color);
  if (filled < w - 4) tft.fillRoundRect(x + 2 + filled, y + 2, (w - 4) - filled, 14, 4, TFT_BLACK);
  int knobX = x + 2 + filled;
  tft.drawFastVLine(knobX, y - 1, 20, TFT_WHITE);
}

bool sliderTouchToValue(int x, int y, int sx, int sy, int sw, int &value) {
  if (x < sx || x > sx + sw || y < sy - 10 || y > sy + 28) return false;
  int raw = constrain(::map(x, sx, sx + sw, 0, 255), 0, 255);

  // Zona muerta en los extremos: con el dedo es dificil tocar el pixel exacto
  // del inicio/final de la barra. Asi se puede llegar a 0 real y a 255 real.
  if (x <= sx + 10 || raw <= 4) raw = 0;
  if (x >= sx + sw - 10 || raw >= 251) raw = 255;

  value = raw;
  ledPoweredOff = false;
  ledEffectMode = 0;  // tocar barras vuelve al modo manual para ajustar color real
  return true;
}

const char *ledEffectName() {
  switch (ledEffectMode) {
    case 1: return "Arcoiris suave";
    case 2: return "Respirar color";
    case 3: return "Parpadeo";
    case 4: return "Navidad";
    default: return "Manual";
  }
}

void wheelColor(uint8_t pos, uint8_t &r, uint8_t &g, uint8_t &b) {
  pos = 255 - pos;
  if (pos < 85) {
    r = 255 - pos * 3;
    g = 0;
    b = pos * 3;
  } else if (pos < 170) {
    pos -= 85;
    r = 0;
    g = pos * 3;
    b = 255 - pos * 3;
  } else {
    pos -= 170;
    r = pos * 3;
    g = 255 - pos * 3;
    b = 0;
  }
}

// -------------------- Indicador WiFi --------------------
void wifiLedMarkBytes(uint32_t bytes) {
  if (bytes == 0) return;
  unsigned long now = millis();
  wifiLedActiveUntilMs = now + WIFI_LED_HOLD_MS;
  if (wifiLedWindowStartMs == 0) wifiLedWindowStartMs = now;
  wifiLedWindowBytes += bytes;
  if (now - wifiLedWindowStartMs >= WIFI_LED_WINDOW_MS) {
    uint32_t instant = (uint32_t)(((uint64_t)wifiLedWindowBytes * 1000ULL) / max(1UL, now - wifiLedWindowStartMs));
    wifiLedBytesPerSec = instant;
    // V45: suavizado exponencial. Antes el LED dependia de rafagas TCP y parecia irregular.
    if (wifiLedBytesPerSecSmooth == 0) wifiLedBytesPerSecSmooth = instant;
    else wifiLedBytesPerSecSmooth = (wifiLedBytesPerSecSmooth * 3 + instant) / 4;
    wifiLedWindowBytes = 0;
    wifiLedWindowStartMs = now;
  }
}

void wifiLedMarkActivity() {
  // Marca actividad minima sin falsear demasiado la velocidad real.
  wifiLedMarkBytes(128);
}

bool wifiLedIndicatorActive() {
  return (int32_t)(wifiLedActiveUntilMs - millis()) > 0;
}

uint16_t wifiLedBlinkIntervalMs() {
  uint32_t bps = wifiLedBytesPerSecSmooth ? wifiLedBytesPerSecSmooth : wifiLedBytesPerSec;
  if (bps < 2048) return 420;
  if (bps < 10UL * 1024UL) return 320;
  if (bps < 40UL * 1024UL) return 220;
  if (bps < 120UL * 1024UL) return 140;
  return 80;
}

// V48: se conserva por compatibilidad interna, pero durante FTP/camara
// se usa updateActivityLedIndicator() para que WiFi y microSD no se tapen.
void updateWifiLedIndicator() {
  unsigned long now = millis();
  if (!wifiLedIndicatorActive()) {
    if (wifiLedShown) {
      wifiLedShown = 0;
      wifiLedBlinkOn = false;
      wifiLedBytesPerSec = 0;
      wifiLedBytesPerSecSmooth = 0;
      ws2812RestoreNormalState();
    }
    return;
  }
  wifiLedShown = 1;
  uint16_t intervalMs = wifiLedBlinkIntervalMs();
  if (wifiLedLastBlinkMs != 0 && now - wifiLedLastBlinkMs < intervalMs) return;
  wifiLedLastBlinkMs = now;
  wifiLedBlinkOn = !wifiLedBlinkOn;
  if (wifiLedBlinkOn) {
    // Verde: actividad WiFi. Mas bytes/segundo = parpadeo mas rapido.
    ws2812SetColor(0, 255, 0, 170);
  } else {
    ws2812SetColor(0, 0, 0, 0);
  }
}

bool sdModeIsWrite(const char *mode) {
  if (!mode) return false;
  return strchr(mode, 'w') != nullptr || strchr(mode, 'a') != nullptr || strchr(mode, '+') != nullptr;
}

void sdLedMarkActivity(uint8_t mode) {
  if (mode == 0) return;
  unsigned long now = millis();

  // V27: lectura y escritura se guardan por separado. Asi podemos detectar
  // cuando ocurren a la vez y alternar 3 parpadeos morados y 3 amarillos.
  if (mode == 2) {
    unsigned long until = now + SD_LED_WRITE_HOLD_MS;
    if ((int32_t)(until - sdLedWriteUntilMs) > 0) sdLedWriteUntilMs = until;
  } else {
    unsigned long until = now + SD_LED_READ_HOLD_MS;
    if ((int32_t)(until - sdLedReadUntilMs) > 0) sdLedReadUntilMs = until;
  }
}

void sdLedMarkRead() {
  sdLedMarkActivity(1);
}

void sdLedMarkWrite() {
  sdLedMarkActivity(2);
}

uint8_t sdLedDesiredMode() {
  // V27: el LED de actividad SD funciona en cualquier menu, incluido WS2812.
  unsigned long now = millis();

  bool readActive = ((int32_t)(sdLedReadUntilMs - now) > 0);
  bool writeActive = ((int32_t)(sdLedWriteUntilMs - now) > 0);

  // V57B: la reproduccion de musica marca pulsos de lectura desde loop(),
  // de forma ligera, para que el morado parpadee sin hacer lecturas extra.

  if (readActive && writeActive) return 3;
  if (writeActive) return 2;
  if (readActive) return 1;
  return 0;
}

bool sdLedIndicatorActive() {
  return sdLedDesiredMode() != 0;
}

void clearSdActivityLedNow() {
  sdLedReadUntilMs = 0;
  sdLedWriteUntilMs = 0;
  sdLedShownMode = 0;
  sdLedBlinkOn = false;
  sdLedBlinkPulseCount = 0;
  sdLedBlinkPhaseMode = 1;
  activityLedResetPattern(0);
  ws2812RestoreNormalState();
}

void ws2812RestoreNormalState() {
  if (ledPoweredOff) {
    ws2812SetColor(0, 0, 0, 0);
  } else if (ledEffectMode == 0) {
    ws2812SetColor(ledR, ledG, ledB, ledBright);
  } else {
    updateWs2812Effect(true);
  }
}

void sdLedShowIndicatorColor(uint8_t mode) {
  if (mode == 2) {
    // Escritura: amarillo.
    ws2812SetColor(255, 190, 0, 170);
  } else {
    // Lectura: morado.
    ws2812SetColor(170, 0, 255, 150);
  }
}

void updateSdLedIndicator() {
  uint8_t desired = sdLedDesiredMode();
  unsigned long now = millis();

  if (desired == 0) {
    if (sdLedShownMode != 0) {
      sdLedShownMode = 0;
      sdLedBlinkOn = false;
      sdLedBlinkPulseCount = 0;
      sdLedBlinkPhaseMode = 1;
      ws2812RestoreNormalState();
    }
    return;
  }

  // Al cambiar de estado reiniciamos el patron de parpadeo.
  if (desired != sdLedShownMode) {
    sdLedShownMode = desired;
    sdLedBlinkOn = false;
    sdLedBlinkPulseCount = 0;
    sdLedBlinkPhaseMode = (desired == 2) ? 2 : 1;
    sdLedBlinkLastMs = 0;
  }

  if (sdLedBlinkLastMs != 0 && now - sdLedBlinkLastMs < SD_LED_BLINK_MS) return;
  sdLedBlinkLastMs = now;
  sdLedBlinkOn = !sdLedBlinkOn;

  if (desired == 3) {
    // Lectura + escritura: 3 parpadeos morados y luego 3 amarillos, en bucle.
    if (sdLedBlinkOn) {
      sdLedShowIndicatorColor(sdLedBlinkPhaseMode);
    } else {
      ws2812SetColor(0, 0, 0, 0);
      sdLedBlinkPulseCount++;
      if (sdLedBlinkPulseCount >= 3) {
        sdLedBlinkPulseCount = 0;
        sdLedBlinkPhaseMode = (sdLedBlinkPhaseMode == 1) ? 2 : 1;
      }
    }
    return;
  }

  // Solo lectura o solo escritura: parpadeo continuo del color correspondiente.
  if (sdLedBlinkOn) sdLedShowIndicatorColor(desired);
  else ws2812SetColor(0, 0, 0, 0);
}


uint8_t sdLedPreferredVisibleMode(uint8_t desired) {
  if (desired == 3) {
    // Si hay lectura y escritura a la vez, alternamos entre morado y amarillo
    // cada vez que termina el bloque SD de 3 parpadeos.
    return activityLedSdAltMode;
  }
  return desired;
}

void activityLedResetPattern(uint8_t mode) {
  activityLedShownMode = mode;
  activityLedGroupMode = 1;
  activityLedPulseCount = 0;
  activityLedBlinkOn = false;
  activityLedLastBlinkMs = 0;
  wifiLedBlinkOn = false;
  sdLedBlinkOn = false;
}

void activityLedShowMode(uint8_t mode) {
  if (mode == 1) {
    // WiFi: verde.
    ws2812SetColor(0, 255, 0, 170);
  } else if (mode == 3) {
    // Escritura microSD: amarillo.
    ws2812SetColor(255, 190, 0, 170);
  } else if (mode == 2) {
    // Lectura microSD: morado.
    ws2812SetColor(170, 0, 255, 150);
  } else {
    ws2812SetColor(0, 0, 0, 0);
  }
}

uint16_t activityLedIntervalMs(bool wifiActive) {
  if (wifiActive) return wifiLedBlinkIntervalMs();
  return SD_LED_BLINK_MS;
}

void updateActivityLedIndicator() {
  bool wifiActive = wifiLedIndicatorActive();
  uint8_t sdDesired = sdLedDesiredMode();
  bool sdActive = sdDesired != 0;
  uint8_t combinedMode = 0;
  if (wifiActive && sdActive) combinedMode = 3;
  else if (wifiActive) combinedMode = 1;
  else if (sdActive) combinedMode = 2;

  unsigned long now = millis();

  if (combinedMode == 0) {
    if (activityLedShownMode != 0 || wifiLedShown || sdLedShownMode) {
      activityLedResetPattern(0);
      wifiLedShown = 0;
      sdLedShownMode = 0;
      sdLedBlinkPulseCount = 0;
      sdLedBlinkPhaseMode = 1;
      wifiLedBytesPerSec = 0;
      wifiLedBytesPerSecSmooth = 0;
      ws2812RestoreNormalState();
    } else {
      updateWs2812Effect(false);
    }
    return;
  }

  if (combinedMode != activityLedShownMode) {
    activityLedResetPattern(combinedMode);
    sdLedShownMode = sdDesired;
    wifiLedShown = wifiActive ? 1 : 0;
  }

  uint16_t intervalMs = activityLedIntervalMs(wifiActive);
  if (activityLedLastBlinkMs != 0 && now - activityLedLastBlinkMs < intervalMs) return;
  activityLedLastBlinkMs = now;
  activityLedBlinkOn = !activityLedBlinkOn;

  if (combinedMode == 1) {
    // Solo WiFi: verde a velocidad proporcional a la transferencia.
    if (activityLedBlinkOn) activityLedShowMode(1);
    else activityLedShowMode(0);
    return;
  }

  if (combinedMode == 2) {
    // Solo microSD: lectura morado, escritura amarillo. Si hay ambas,
    // 3 morados y 3 amarillos, como antes.
    uint8_t visible = sdLedPreferredVisibleMode(sdDesired);
    if (activityLedBlinkOn) {
      activityLedShowMode(visible == 2 ? 3 : 2);
    } else {
      activityLedShowMode(0);
      activityLedPulseCount++;
      if (activityLedPulseCount >= 3) {
        activityLedPulseCount = 0;
        if (sdDesired == 3) activityLedSdAltMode = (activityLedSdAltMode == 1) ? 2 : 1;
      }
    }
    return;
  }

  // WiFi + microSD: no se mezclan colores. Se muestran 3 parpadeos verdes
  // y luego 3 parpadeos del estado de la microSD: morado si lee, amarillo si escribe.
  if (activityLedBlinkOn) {
    if (activityLedGroupMode == 1) {
      activityLedShowMode(1);  // verde WiFi
    } else {
      uint8_t visible = sdLedPreferredVisibleMode(sdDesired);
      activityLedShowMode(visible == 2 ? 3 : 2);  // 3=amarillo, 2=morado
    }
  } else {
    activityLedShowMode(0);
    activityLedPulseCount++;
    if (activityLedPulseCount >= 3) {
      activityLedPulseCount = 0;
      if (activityLedGroupMode == 1) {
        activityLedGroupMode = 2;
      } else {
        if (sdDesired == 3) activityLedSdAltMode = (activityLedSdAltMode == 1) ? 2 : 1;
        activityLedGroupMode = 1;
      }
    }
  }
}

void updateWs2812Effect(bool force) {
  if (ledEffectMode == 0) return;
  unsigned long now = millis();
  uint16_t intervalMs = (ledEffectMode == 3) ? 250 : 70;
  if (!force && now - lastLedEffectMs < intervalMs) return;
  lastLedEffectMs = now;

  uint8_t r = ledR;
  uint8_t g = ledG;
  uint8_t b = ledB;
  uint8_t br = ledBright;

  if (ledEffectMode == 1) {              // arcoiris suave
    wheelColor(ledEffectStep++, r, g, b);
  } else if (ledEffectMode == 2) {       // respirar con el color elegido
    uint8_t phase = ledEffectStep++;
    uint8_t maxBr = max((int)ledBright, 4);
    br = phase < 128 ? ::map(phase, 0, 127, 4, maxBr) : ::map(phase, 128, 255, maxBr, 4);
  } else if (ledEffectMode == 3) {       // parpadeo
    ledBlinkState = !ledBlinkState;
    if (!ledBlinkState) br = 0;
  } else if (ledEffectMode == 4) {       // navidad: rojo, verde, blanco, apagado breve
    uint8_t phase = (ledEffectStep++ / 10) % 4;
    if (phase == 0) { r = 255; g = 0;   b = 0;   }
    if (phase == 1) { r = 0;   g = 255; b = 0;   }
    if (phase == 2) { r = 255; g = 255; b = 180; }
    if (phase == 3) { r = 0;   g = 0;   b = 0;   }
  }

  ws2812SetColor(r, g, b, br);
}

void loadScreenTimeoutConfig() {
  prefs.begin(PREF_NAMESPACE_UI, false);
  screenTimeoutMinutes = prefs.getUChar("timeout", 1);
  if (screenTimeoutMinutes < 1 || screenTimeoutMinutes > 5) screenTimeoutMinutes = 1;
  // V56: recordar si el buzzer debe pitar al inicio.
  bootBeepEnabled = prefs.getBool("boot_beep", false);
}

void saveScreenTimeoutConfig() {
  prefs.putUChar("timeout", screenTimeoutMinutes);
}

void screenSetOn(bool on) {
  if (on) {
    if (TFT_BACKLIGHT_PIN >= 0) digitalWrite(TFT_BACKLIGHT_PIN, TFT_BACKLIGHT_ON_LEVEL);
    tft.writecommand(0x29);  // DISPON
    screenSleeping = false;
  } else {
    tft.writecommand(0x28); // DISPOFF
    if (TFT_BACKLIGHT_PIN >= 0) digitalWrite(TFT_BACKLIGHT_PIN, !TFT_BACKLIGHT_ON_LEVEL);
    screenSleeping = true;
  }
}

void handleScreenSleep() {
  if (screenSleeping) return;
  if (screenTimeoutMinutes < 1 || screenTimeoutMinutes > 5) screenTimeoutMinutes = 1;
  uint32_t timeoutMs = (uint32_t)screenTimeoutMinutes * 60000UL;
  if (millis() - lastUserActivityMs >= timeoutMs) {
    screenSetOn(false);
  }
}

// -------------------- Buzzer / audio simple --------------------
void buzzerBeep(int freq, int durationMs) {
  tone(PIN_BUZZER, freq, durationMs);
  delay(durationMs + 20);
  noTone(PIN_BUZZER);
}

void buzzerMelody() {
  const int notes[] = { 784, 988, 1175, 988, 784, 659, 784 };
  const int times[] = { 120, 120, 180, 120, 120, 120, 240 };
  for (uint8_t i = 0; i < sizeof(notes) / sizeof(notes[0]); i++) {
    tone(PIN_BUZZER, notes[i], times[i]);
    delay(times[i] + 35);
  }
  noTone(PIN_BUZZER);
}

// -------------------- Reproductor MP3 SD --------------------
void loadMusicFolderConfig() {
  if (!sdTried) initSD();
  if (!sdOK) return;
  if (!sdExistsCompat("/music")) sdMkdirCompat("/music");
  File f = sdOpenCompat(MUSIC_FOLDER_CONFIG, FILE_READ);
  if (f) {
    String saved = f.readStringUntil('\n');
    saved.trim();
    f.close();
    if (saved.length() > 0) {
      if (!saved.startsWith("/")) saved = "/" + saved;
      musicFolder = saved;
    }
  }
  if (musicFolder.length() == 0) musicFolder = MUSIC_ROOT_FOLDER;
}

void saveMusicFolderConfig() {
  if (!sdTried) initSD();
  if (!sdOK) return;
  if (musicFolder.length() == 0) musicFolder = MUSIC_ROOT_FOLDER;
  if (!musicFolder.startsWith("/")) musicFolder = "/" + musicFolder;
  if (sdExistsCompat(MUSIC_FOLDER_CONFIG)) sdRemoveCompat(MUSIC_FOLDER_CONFIG);
  File f = sdOpenCompat(MUSIC_FOLDER_CONFIG, FILE_WRITE);
  if (f) {
    f.println(musicFolder);
    f.close();
  }
}

String folderBaseName(const String &path) {
  if (path == "/") return "/";
  int idx = path.lastIndexOf('/');
  if (idx < 0) return path;
  return path.substring(idx + 1);
}

String parentFolderPath(const String &path) {
  if (path.length() <= 1 || path == "/") return "/";
  int idx = path.lastIndexOf('/');
  if (idx <= 0) return "/";
  return path.substring(0, idx);
}

void sortStringArray(String *arr, int count) {
  for (int i = 0; i < count - 1; i++) {
    for (int j = i + 1; j < count; j++) {
      String a = arr[i];
      String b = arr[j];
      a.toLowerCase();
      b.toLowerCase();
      if (b < a) {
        String tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
      }
    }
  }
}

String normalizePathSimple(String path) {
  path.trim();
  if (path.length() == 0) return "/";
  if (!path.startsWith("/")) path = "/" + path;
  while (path.length() > 1 && path.endsWith("/")) path.remove(path.length() - 1);
  return path;
}

bool isMusicRootPath(const String &path) {
  String p = normalizePathSimple(path);
  return p == MUSIC_ROOT_FOLDER;
}

bool isDirectChildOfMusicRoot(const String &path) {
  String p = normalizePathSimple(path);
  return parentFolderPath(p) == MUSIC_ROOT_FOLDER;
}

String joinPathSimple(const String &folder, const String &name) {
  if (name.startsWith("/")) return name;
  if (folder == "/") return "/" + name;
  return folder + "/" + name;
}

String musicBatchFolderPath(uint16_t index) {
  static const char *names[] = {
    "uno", "dos", "tres", "cuatro", "cinco", "seis", "siete", "ocho", "nueve", "diez",
    "once", "doce", "trece", "catorce", "quince", "dieciseis", "diecisiete", "dieciocho", "diecinueve", "veinte"
  };
  String suffix;
  if (index >= 1 && index <= (sizeof(names) / sizeof(names[0]))) suffix = names[index - 1];
  else suffix = String(index);
  return String(MUSIC_ROOT_FOLDER) + "/musica " + suffix;
}

uint16_t countMusicFilesDirect(const String &folder) {
  uint16_t count = 0;
  File root = sdOpenCompat(folder.c_str(), FILE_READ);
  if (!root || !root.isDirectory()) { if (root) root.close(); return 0; }
  File file = root.openNextFile();
  while (file) {
    String n = String(file.name());
    if (!file.isDirectory() && isMusicFileName(n)) count++;
    file.close();
    file = root.openNextFile();
    yield();
  }
  root.close();
  return count;
}

String findFirstMusicFileDirect(const String &folder) {
  File root = sdOpenCompat(folder.c_str(), FILE_READ);
  if (!root || !root.isDirectory()) { if (root) root.close(); return ""; }
  File file = root.openNextFile();
  while (file) {
    String n = String(file.name());
    bool ok = !file.isDirectory() && isMusicFileName(n);
    if (ok && !n.startsWith("/")) n = joinPathSimple(folder, n);
    file.close();
    if (ok) { root.close(); return n; }
    file = root.openNextFile();
    yield();
  }
  root.close();
  return "";
}

String uniqueMusicDestPath(const String &folder, const String &baseName) {
  String clean = baseName;
  clean.replace("/", "_");
  String dest = joinPathSimple(folder, clean);
  if (!sdExistsCompat(dest.c_str())) return dest;
  int dot = clean.lastIndexOf('.');
  String stem = dot > 0 ? clean.substring(0, dot) : clean;
  String ext = dot > 0 ? clean.substring(dot) : "";
  for (int i = 2; i < 1000; i++) {
    dest = joinPathSimple(folder, stem + "_" + String(i) + ext);
    if (!sdExistsCompat(dest.c_str())) return dest;
  }
  return joinPathSimple(folder, String(millis()) + "_" + clean);
}

void organizeMusicRootFiles() {
  if (!sdTried) initSD();
  if (!sdOK) return;
  if (!sdExistsCompat(MUSIC_ROOT_FOLDER)) sdMkdirCompat(MUSIC_ROOT_FOLDER);

  // Si ya se comprobo y no hay trabajo pendiente, no volver a recorrer la SD.
  if (musicAutoOrganizeChecked && !musicOrganizePending) return;

  // V26: antes se movian todas las canciones de golpe. Con 400 canciones eso podia
  // dejar la pantalla aparentemente bloqueada. Ahora se mueven pocas por pasada.
  const int MUSIC_ORGANIZE_FILES_PER_TICK = (musicHighPerfActive ? 2 : 1);
  int moved = 0;
  bool hitLimit = false;

  for (int guard = 0; guard < MUSIC_ORGANIZE_FILES_PER_TICK; guard++) {
    String src = findFirstMusicFileDirect(MUSIC_ROOT_FOLDER);
    if (src.length() == 0) break;

    uint16_t batch = 1;
    String targetFolder;
    while (batch < 100) {
      targetFolder = musicBatchFolderPath(batch);
      if (!sdExistsCompat(targetFolder.c_str())) sdMkdirCompat(targetFolder.c_str());
      if (countMusicFilesDirect(targetFolder) < MUSIC_AUTO_FOLDER_SIZE) break;
      batch++;
      yield();
    }

    String dest = uniqueMusicDestPath(targetFolder, folderBaseName(src));
    sdLedMarkWrite();
    if (sdRenameCompat(src.c_str(), dest.c_str())) {
      moved++;
      musicOrganizedMovedTotal++;
    } else {
      // Evita bucle infinito si una tarjeta no permite renombrar algun archivo.
      break;
    }
    yield();
  }

  hitLimit = (moved >= MUSIC_ORGANIZE_FILES_PER_TICK);
  musicAutoOrganizeChecked = !hitLimit;
  musicOrganizePending = hitLimit;
  musicOrganizePendingMs = millis();
  musicFolderScanned = false;

  if (moved > 0) {
    musicScanned = false;
    musicCount = 0;
    musicIndex = 0;
    musicListOffset = 0;
    if (sdExistsCompat(MUSIC_INDEX_FILE)) sdRemoveCompat(MUSIC_INDEX_FILE);
    musicOrgStatusMsg = String("Ordenando: ") + String(musicOrganizedMovedTotal) + " movidas";
    audioStatusMsg = hitLimit ? musicOrgStatusMsg : String("Ordenadas ") + String(musicOrganizedMovedTotal) + " canciones";
  }

  if (!hitLimit) {
    // V54: al terminar de ordenar no creamos indices de TODAS las carpetas de golpe.
    // Eso era lo que podia dejar la pantalla parpadeando y el LED morado activo.
    // Cada carpeta crea/carga su propio _music_index.txt cuando la seleccionas.
    musicOrganizePending = false;
    musicUserApprovedOrganization = false;
    musicIndexBuildPending = false;
    musicIndexBuildActive = false;
    musicAutoOrganizeChecked = true;
    musicFolderScanned = false;
    scanMusicFolders();
    audioStatusMsg = "Ordenada. Elige Carpeta";
    musicOrgStatusMsg = "Ordenacion terminada";
    clearSdActivityLedNow();
  }
}

void scanMusicFolders() {
  musicFolderScanned = true;
  musicFolderCount = 0;
  if (!sdTried) initSD();
  if (!sdOK) return;
  if (musicBrowserPath.length() == 0 || musicBrowserPath == "/") musicBrowserPath = MUSIC_ROOT_FOLDER;
  if (!musicBrowserPath.startsWith("/")) musicBrowserPath = "/" + musicBrowserPath;
  File root = sdOpenCompat(musicBrowserPath.c_str(), FILE_READ);
  if (!root || !root.isDirectory()) {
    if (root) root.close();
    musicBrowserPath = "/";
    root = sdOpenCompat("/", FILE_READ);
  }
  if (!root || !root.isDirectory()) {
    if (root) root.close();
    return;
  }
  File file = root.openNextFile();
  while (file && musicFolderCount < MAX_MUSIC_FOLDERS) {
    if (file.isDirectory()) {
      String n = String(file.name());
      if (n.length() > 0 && !n.endsWith(".")) {
        if (!n.startsWith("/")) {
          if (musicBrowserPath == "/") n = "/" + n;
          else n = musicBrowserPath + "/" + n;
        }
        musicFolders[musicFolderCount++] = n;
      }
    }
    file.close();
    file = root.openNextFile();
    yield();
  }
  root.close();
  sortStringArray(musicFolders, musicFolderCount);
}

bool isMusicFileName(const String &name) {
  // V52: para aligerar el ESP32-S3, el reproductor trabaja solo con MP3.
  // WAV/M4A/AAC se ignoran para evitar cortes y sobrecarga.
  String lower = name;
  lower.toLowerCase();
  return lower.endsWith(".mp3");
}

void addMusicFilePath(const String &path) {
  if (musicCount >= MAX_MUSIC_FILES) return;
  musicFiles[musicCount++] = path;
}

void scanMusicFilesRecursive(const String &folder, uint8_t depth) {
  if (depth > 6 || musicCount >= MAX_MUSIC_FILES) return;
  File root = sdOpenCompat(folder.c_str(), FILE_READ);
  if (!root || !root.isDirectory()) {
    if (root) root.close();
    return;
  }
  File file = root.openNextFile();
  while (file && musicCount < MAX_MUSIC_FILES) {
    String n = String(file.name());
    if (!n.startsWith("/")) {
      if (folder == "/") n = "/" + n;
      else n = folder + "/" + n;
    }
    if (file.isDirectory()) {
      String base = folderBaseName(n);
      if (!base.startsWith(".") && base != "System Volume Information") {
        scanMusicFilesRecursive(n, depth + 1);
      }
    } else if (isMusicFileName(n)) {
      addMusicFilePath(n);
    }
    file.close();
    file = root.openNextFile();
    yield();
  }
  root.close();
}

void scanMusicFilesDirect(const String &folder) {
  File root = sdOpenCompat(folder.c_str(), FILE_READ);
  if (!root || !root.isDirectory()) {
    if (root) root.close();
    return;
  }
  File file = root.openNextFile();
  while (file && musicCount < MAX_MUSIC_FILES) {
    String n = String(file.name());
    if (!n.startsWith("/")) n = joinPathSimple(folder, n);
    if (!file.isDirectory() && isMusicFileName(n)) addMusicFilePath(n);
    file.close();
    file = root.openNextFile();
    yield();
  }
  root.close();
}

void saveMusicIndexFile() {
  if (!sdOK) return;
  String idxPath = musicIndexPathForFolder(musicFolder);
  if (sdExistsCompat(idxPath.c_str())) sdRemoveCompat(idxPath.c_str());
  File f = sdOpenCompat(idxPath.c_str(), FILE_WRITE);
  if (!f) return;
  f.println("# Indice automatico de canciones del ESP32-S3");
  f.print("# Carpeta: "); f.println(musicFolder);
  f.print("# Total: "); f.println(musicCount);
  for (int i = 0; i < musicCount; i++) f.println(musicFiles[i]);
  f.close();
  sdLedMarkWrite();

  // Compatibilidad con versiones anteriores: tambien guarda el ultimo indice usado.
  if (String(MUSIC_INDEX_FILE) != idxPath) {
    if (sdExistsCompat(MUSIC_INDEX_FILE)) sdRemoveCompat(MUSIC_INDEX_FILE);
    File g = sdOpenCompat(MUSIC_INDEX_FILE, FILE_WRITE);
    if (g) {
      g.println("# Ultimo indice de carpeta usado por el ESP32-S3");
      g.print("# Carpeta: "); g.println(musicFolder);
      g.print("# Total: "); g.println(musicCount);
      for (int i = 0; i < musicCount; i++) g.println(musicFiles[i]);
      g.close();
    }
  }
}

bool loadMusicIndexFile() {
  if (!sdOK) return false;
  String idxPath = musicIndexPathForFolder(musicFolder);
  File f = sdOpenCompat(idxPath.c_str(), FILE_READ);
  if (!f && String(MUSIC_INDEX_FILE) != idxPath) f = sdOpenCompat(MUSIC_INDEX_FILE, FILE_READ);
  if (!f) return false;

  String cachedFolder = "";
  musicCount = 0;

  while (f.available() && musicCount < MAX_MUSIC_FILES) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) { yield(); continue; }
    if (line.startsWith("# Carpeta:")) {
      cachedFolder = line.substring(10);
      cachedFolder.trim();
      yield();
      continue;
    }
    if (line[0] == '#') { yield(); continue; }
    if (!line.startsWith("/")) line = "/" + line;
    if (isMusicFileName(line)) musicFiles[musicCount++] = line;
    yield();
  }
  f.close();

  if (cachedFolder.length() > 0 && cachedFolder != musicFolder) {
    musicCount = 0;
    return false;
  }
  if (musicCount <= 0) return false;

  if (musicIndex >= musicCount) musicIndex = 0;
  if (musicListOffset >= musicCount) musicListOffset = max(0, musicCount - MUSIC_LIST_ROWS);
  musicScanned = true;
  musicIndexSavePending = false;
  audioStatusMsg = String("Indice cargado: ") + String(musicCount);
  return true;
}

uint32_t fingerprintMusicRecursive(const String &folder, uint8_t depth) {
  if (depth > 6) return 0;
  uint32_t fp = 2166136261UL;
  File root = sdOpenCompat(folder.c_str(), FILE_READ);
  if (!root || !root.isDirectory()) {
    if (root) root.close();
    return fp;
  }
  File file = root.openNextFile();
  while (file) {
    String n = String(file.name());
    if (!n.startsWith("/")) {
      if (folder == "/") n = "/" + n;
      else n = folder + "/" + n;
    }
    if (file.isDirectory()) {
      String base = folderBaseName(n);
      if (!base.startsWith(".") && base != "System Volume Information") fp ^= fingerprintMusicRecursive(n, depth + 1);
    } else if (isMusicFileName(n)) {
      for (uint16_t i = 0; i < n.length(); i++) { fp ^= (uint8_t)n[i]; fp *= 16777619UL; }
      fp ^= (uint32_t)file.size(); fp *= 16777619UL;
    }
    file.close();
    file = root.openNextFile();
    yield();
  }
  root.close();
  return fp;
}

uint32_t computeMusicFolderFingerprint() {
  if (!sdOK) return 0;
  return fingerprintMusicRecursive(musicFolder, 0);
}

void musicAutoRescanIfChanged() {
  // V22: antes esta funcion recorria toda la carpeta de musica cada 15 segundos.
  // Con 400+ canciones eso bloquea la pantalla tactil. Ahora solo guarda el
  // indice si hay cambios notificados por FTP o si el usuario pulsa Refres.
  musicSaveIndexIfPending();
}

void musicSaveIndexIfPending() {
  if (!musicIndexSavePending) return;
  if (!sdOK || !musicScanned) return;
  // V23: no escribir /music_index.txt mientras suena musica; la microSD compartida
  // con Audio.h puede provocar cortes. Se guardara al parar/pausar.
  if (musicPlaying && !musicPaused) return;
  if (millis() - musicIndexSavePendingMs < 2500) return;
  sortStringArray(musicFiles, musicCount);
  if (musicIndex >= musicCount) musicIndex = max(0, musicCount - 1);
  if (musicListOffset >= musicCount) musicListOffset = max(0, musicCount - MUSIC_LIST_ROWS);
  saveMusicIndexFile();
  musicIndexSavePending = false;
  audioStatusMsg = String("Lista: ") + String(musicCount) + " canciones";
}

int findMusicPathIndex(const String &path) {
  for (int i = 0; i < musicCount; i++) {
    if (musicFiles[i] == path) return i;
  }
  return -1;
}

void removeMusicPathAt(int idx) {
  if (idx < 0 || idx >= musicCount) return;
  for (int i = idx; i < musicCount - 1; i++) musicFiles[i] = musicFiles[i + 1];
  musicFiles[musicCount - 1] = "";
  musicCount--;
  if (musicIndex >= musicCount) musicIndex = max(0, musicCount - 1);
  if (musicListOffset >= musicCount) musicListOffset = max(0, musicCount - MUSIC_LIST_ROWS);
}

void invalidateMusicListIfPathLooksMusic(const String &path) {
  String p = normalizePathSimple(path);
  String lower = p;
  lower.toLowerCase();
  bool isMusic = lower.endsWith(".mp3") || lower.endsWith(".wav") || lower.endsWith(".m4a") || lower.endsWith(".aac");
  String lowerMusicFolder = musicFolder;
  lowerMusicFolder.toLowerCase();
  bool underMusicRoot = lower == "/music" || lower.startsWith("/music/");
  bool underSelectedFolder = (lowerMusicFolder.length() > 0 && lower.startsWith(lowerMusicFolder));
  if (!underMusicRoot && !underSelectedFolder && !isMusic) return;

  // V25: si se sube una cancion suelta directamente a /music, no se anade a una
  // lista gigante; se marca para repartirla automaticamente en subcarpetas de 50.
  if (isMusic && isDirectChildOfMusicRoot(p)) {
    // V53: no organizar automaticamente una nueva cancion suelta. Al abrir
    // Musica se vuelve a preguntar Si/No como pidio el usuario.
    musicOrganizePending = false;
    musicAutoOrganizeChecked = false;
    musicOrganizePendingMs = millis();
    musicFolderScanned = false;
    musicScanned = false;
    musicIndexSavePending = false;
    musicIndexBuildActive = false;
    if (sdOK && sdExistsCompat(MUSIC_INDEX_FILE)) sdRemoveCompat(MUSIC_INDEX_FILE);
    musicNeedOrganizeConfirm = true;
    musicUserApprovedOrganization = false;
    musicLockedByUnordered = false;
    audioStatusMsg = "Cancion nueva: confirmar";
    return;
  }

  // Si el archivo esta dentro de la carpeta actualmente elegida, actualizamos la
  // lista pequena en memoria sin reescanear todo /music.
  if (isMusic && musicScanned && !isMusicRootPath(musicFolder) && underSelectedFolder) {
    int pos = findMusicPathIndex(p);
    bool existsNow = sdExistsCompat(p.c_str());
    if (existsNow) {
      if (pos < 0 && musicCount < MAX_MUSIC_FILES) musicFiles[musicCount++] = p;
    } else {
      if (pos >= 0) removeMusicPathAt(pos);
    }
    musicIndexSavePending = true;
    musicIndexSavePendingMs = millis();
    audioStatusMsg = String("Lista carpeta: ") + String(musicCount);
    return;
  }

  // Cambios de carpetas o cambios cuando aun no hay lista cargada: no bloquear
  // ahora. Se reescanea al entrar en Musica o al pulsar Refres./Act.
  musicScanned = false;
  musicFolderScanned = false;
  musicIndexSavePending = false;
  musicFolderFingerprint = 0;
  if (sdOK && sdExistsCompat(MUSIC_INDEX_FILE)) sdRemoveCompat(MUSIC_INDEX_FILE);
}
void scanMusicFiles(bool forceFullScan) {
  musicScanned = true;
  musicCount = 0;
  if (!sdTried) initSD();
  if (!sdOK) return;
  loadMusicFolderConfig();

  if (isMusicRootPath(musicFolder)) {
    musicScanned = false;
    musicCount = 0;
    audioStatusMsg = musicRootHasLooseFiles() ? "Hay canciones sin ordenar" : "Elige una subcarpeta";
    return;
  }

  // V22: si existe /music_index.txt, cargarlo primero. Es mucho mas rapido
  // que recorrer 400+ archivos cada vez que se abre Musica.
  if (!forceFullScan && loadMusicIndexFile()) {
    // El indice ya esta en RAM; no dejar el indicador SD activo indefinidamente.
    clearSdActivityLedNow();
    return;
  }

  File root = sdOpenCompat(musicFolder.c_str(), FILE_READ);
  if (!root || !root.isDirectory()) {
    if (root) root.close();
    if (isMusicRootPath(musicFolder)) sdMkdirCompat(MUSIC_ROOT_FOLDER);
    audioStatusMsg = "Carpeta no encontrada";
    return;
  }
  root.close();
  // V25: al elegir una subcarpeta se listan solo sus canciones directas.
  // Asi cada carpeta de 50 canciones responde mucho mas rapido.
  scanMusicFilesDirect(musicFolder);
  sortStringArray(musicFiles, musicCount);
  if (musicIndex >= musicCount) musicIndex = 0;
  if (musicListOffset >= musicCount) musicListOffset = max(0, musicCount - MUSIC_LIST_ROWS);
  // V22: no calcular huella recorriendo toda la carpeta por segunda vez.
  // El propio escaneo ya es suficiente y guardamos el indice.
  musicFolderFingerprint = 0;
  saveMusicIndexFile();
  musicIndexSavePending = false;
  if (musicCount > 0) audioStatusMsg = String("Canciones: ") + String(musicCount) + " listas";
  else audioStatusMsg = "No hay canciones";
  clearSdActivityLedNow();
}

void audioStartCurrent() {
  enterMusicHighPerformanceMode();
  if (musicCount <= 0) return;
#if HAVE_AUDIO_LIB
  if (!sdOK) return;
  if (musicIndex < 0) musicIndex = 0;
  if (musicIndex >= musicCount) musicIndex = 0;
  musicEofPending = false;
  audioChangingTrack = true;
  audio.stopSong();
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(musicVolume);
  sdLedMarkRead();
  if (audio.connecttoFS(SD_MMC, musicFiles[musicIndex].c_str())) {
    lastMusicSdReadLedMs = 0;
    musicPlaying = true;
    musicPaused = false;
    lastAudioStartMs = millis();
    audioStatusMsg = "Reproduciendo";
  } else {
    musicPlaying = false;
    musicPaused = false;
    audioStatusMsg = "No se pudo abrir";
  }
  audioChangingTrack = false;
#else
  musicPlaying = false;
  musicPaused = false;
  audioStatusMsg = "Audio.h no disponible";
#endif
}

void audioStopPlayback() {
  musicEofPending = false;
#if HAVE_AUDIO_LIB
  audioChangingTrack = true;
  audio.stopSong();
  audioChangingTrack = false;
#endif
  musicPlaying = false;
  musicPaused = false;
  lastMusicSdReadLedMs = 0;
  audioStatusMsg = "Parado";
}

void audioPauseResume() {
#if HAVE_AUDIO_LIB
  if (musicCount <= 0) return;
  if (!musicPlaying) {
    audioStartCurrent();
  } else {
    audio.pauseResume();
    musicPaused = !musicPaused;
    audioStatusMsg = musicPaused ? "Pausado" : "Reproduciendo";
  }
#else
  buzzerBeep(1000, 80);
#endif
}

const char *audioRepeatLabel() {
  switch (musicRepeatMode) {
    case 1: return "Rep.Todo";
    case 2: return "Rep.1";
    default: return "Normal";
  }
}

void audioToggleRepeatMode() {
  musicRepeatMode = (musicRepeatMode + 1) % 3;
  audioStatusMsg = String("Modo: ") + audioRepeatLabel();
}

void audioToggleShuffle() {
  musicShuffle = !musicShuffle;
  audioStatusMsg = musicShuffle ? "Aleatorio ON" : "Aleatorio OFF";
}

void audioPreviousTrack() {
  if (musicCount <= 0) return;
  if (musicShuffle && musicCount > 1) {
    int next = musicIndex;
    while (next == musicIndex) next = random(0, musicCount);
    musicIndex = next;
  } else {
    musicIndex = (musicIndex + musicCount - 1) % musicCount;
  }
  audioStartCurrent();
}

void audioNextTrack(bool fromEof) {
  if (musicCount <= 0) return;
  if (musicRepeatMode == 2 && fromEof) {
    // Repetir una: reinicia la misma cancion indefinidamente.
  } else if (musicShuffle && musicCount > 1) {
    int next = musicIndex;
    while (next == musicIndex) next = random(0, musicCount);
    musicIndex = next;
  } else if (musicIndex + 1 < musicCount) {
    musicIndex++;
  } else if (musicRepeatMode == 1) {
    musicIndex = 0;
  } else {
#if HAVE_AUDIO_LIB
    audioChangingTrack = true;
    audio.stopSong();
    audioChangingTrack = false;
#endif
    musicPlaying = false;
    musicPaused = false;
    audioStatusMsg = "Fin de lista";
    clearSdActivityLedNow();
    return;
  }
  audioStartCurrent();
}

void audioMonitorAutoNext() {
#if HAVE_AUDIO_LIB
  if (!musicPlaying || musicPaused || audioChangingTrack || musicCount <= 0) return;
  if (millis() - lastAudioStartMs < 2500) return;
  int dur = (int)audio.getAudioFileDuration();
  int cur = (int)audio.getAudioCurrentTime();
  if (dur > 0 && cur >= dur - 1) {
    // V53: no cambiar de cancion dentro de la comprobacion/loop de audio.
    // Solo marcamos pendiente y el cambio se hace fuera, una vez por EOF.
    musicEofPending = true;
    musicEofPendingMs = millis();
    audioStatusMsg = "Siguiente...";
  }
#endif
}

void musicHandleEofPending() {
#if HAVE_AUDIO_LIB
  if (!musicEofPending) return;
  if (audioChangingTrack) return;
  if (millis() - musicEofPendingMs < 60UL) return;
  musicEofPending = false;
  if (!musicPlaying || musicPaused || musicCount <= 0) return;
  if (alarmMusicActive) {
    alarmStopMusicIfActive();
    return;
  }
  audioNextTrack(true);
  if (currentPage == PAGE_AUDIO) updateAudioPageDynamic(true);
  else if (currentPage == PAGE_MUSIC_LIST) drawMusicListPage();
#endif
}

void audioSetSeekPercent(int percent) {
#if HAVE_AUDIO_LIB
  percent = constrain(percent, 0, 100);
  int dur = (int)audio.getAudioFileDuration();
  if (dur > 0) audio.setAudioPlayPosition((uint16_t)((dur * percent) / 100));
#endif
}

void audioSeekRelativeSeconds(int deltaSeconds) {
#if HAVE_AUDIO_LIB
  unsigned long nowSeek = millis();
  if (nowSeek - lastAudioSeekButtonMs < AUDIO_SEEK_BUTTON_GUARD_MS) return;
  lastAudioSeekButtonMs = nowSeek;
  int dur = (int)audio.getAudioFileDuration();
  int cur = (int)audio.getAudioCurrentTime();
  if (dur <= 0) return;
  int target = constrain(cur + deltaSeconds, 0, dur);
  audio.setAudioPlayPosition((uint16_t)target);
  audioStatusMsg = deltaSeconds < 0 ? "Retrocede 5 s" : "Avanza 5 s";
#endif
  if (currentPage == PAGE_AUDIO) {
    drawAudioProgressOnly();
    drawAudioStatusOnly();
    audioRedrawBlockUntil = millis() + 700;
  }
}

int audioVolumeUiLevel() {
  // Convierte el volumen interno 0..21 de Audio.h a 10 secciones visibles: 0..10.
  return constrain((musicVolume * AUDIO_VOLUME_UI_STEPS + (AUDIO_HW_VOLUME_MAX / 2)) / AUDIO_HW_VOLUME_MAX, 0, AUDIO_VOLUME_UI_STEPS);
}

void audioSetVolumeUiLevel(int level) {
  level = constrain(level, 0, AUDIO_VOLUME_UI_STEPS);
  musicVolume = constrain((level * AUDIO_HW_VOLUME_MAX + (AUDIO_VOLUME_UI_STEPS / 2)) / AUDIO_VOLUME_UI_STEPS, 0, AUDIO_HW_VOLUME_MAX);
#if HAVE_AUDIO_LIB
  audio.setVolume(musicVolume);
#endif
  audioStatusMsg = "Volumen " + String(level) + "/" + String(AUDIO_VOLUME_UI_STEPS);
}

void audioSetVolumeFromX(int x) {
  int pct = constrain(::map(x, 54, 186, 0, 100), 0, 100);
  int level = constrain(::map(pct, 0, 100, 0, AUDIO_VOLUME_UI_STEPS), 0, AUDIO_VOLUME_UI_STEPS);
  audioSetVolumeUiLevel(level);
  if (currentPage == PAGE_AUDIO) {
    drawAudioVolumeControlsOnly();
    drawAudioStatusOnly();
    audioRedrawBlockUntil = millis() + 700;
  }
}

void audioChangeVolume(int deltaSections) {
  unsigned long nowVol = millis();
  if (nowVol - lastAudioVolumeButtonMs < AUDIO_VOLUME_BUTTON_GUARD_MS) return;
  lastAudioVolumeButtonMs = nowVol;
  audioSetVolumeUiLevel(audioVolumeUiLevel() + deltaSections);
  if (currentPage == PAGE_AUDIO) {
    drawAudioVolumeControlsOnly();
    drawAudioStatusOnly();
    audioRedrawBlockUntil = millis() + 700;
  }
}

void handleAudioTouch(int x, int y) {
  // V23: en Musica, un toque mantenido solo debe contar una vez.
  // Antes, cada 180 ms repetia el mismo boton: por eso el volumen subia/bajaba de golpe.
  if (audioTouchConsumed) return;

  if (musicNeedOrganizeConfirm) {
    Button yes = { 24, 198, 88, 38, "Si", ACT_MUSIC_ORG_YES };
    Button no  = {128, 198, 88, 38, "No", ACT_MUSIC_ORG_NO };
    if (pointInButton(x, y, yes)) { audioTouchConsumed = true; musicStartConfirmedOrganization(); drawAudioPage(); return; }
    if (pointInButton(x, y, no))  { audioTouchConsumed = true; musicCancelOrganizationForNow(); drawAudioPage(); return; }
    return;
  }
  if (musicLockedByUnordered || musicOrganizePending) {
    return;
  }

  Button prev = {  8, 226, 48, 28, "<<", ACT_AUDIO_PREV };
  Button play = {  62,226, 52, 28, "Play", ACT_AUDIO_PLAY };
  Button stop = { 120,226, 52, 28, "Stop", ACT_AUDIO_STOP };
  Button next = { 178,226, 54, 28, ">>", ACT_AUDIO_NEXT };
  Button list = {   8,258, 68, 24, "Lista", ACT_AUDIO_LIST };
  Button repeat = {82,258, 72, 24, "Rep.", ACT_AUDIO_REPEAT };
  Button shuffle = {160,258,72, 24, "Aleat.", ACT_AUDIO_SHUFFLE };
  Button folder = { 82, 286, 72, 28, "Carpeta", ACT_AUDIO_FOLDER };
  Button refresh = { 158, 286, 74, 28, "Refres.", ACT_AUDIO_REFRESH };
  Button back5 = {  10, 160, 38, 22, "-5s", ACT_NONE };
  Button fwd5  = { 192, 160, 38, 22, "+5s", ACT_NONE };
  Button volDown = {  10, 196, 38, 22, "-1", ACT_NONE };
  Button volUp   = { 192, 196, 38, 22, "+1", ACT_NONE };

  if (pointInButton(x, y, back5)) { audioTouchConsumed = true; audioSeekRelativeSeconds(-5); return; }
  if (pointInButton(x, y, fwd5)) { audioTouchConsumed = true; audioSeekRelativeSeconds(5); return; }
  if (pointInButton(x, y, volDown)) { audioTouchConsumed = true; audioChangeVolume(-1); return; }
  if (pointInButton(x, y, volUp)) { audioTouchConsumed = true; audioChangeVolume(1); return; }

  if (pointInButton(x, y, prev)) { audioTouchConsumed = true; audioPreviousTrack(); drawAudioPage(); return; }
  if (pointInButton(x, y, play)) { audioTouchConsumed = true; audioPauseResume(); drawAudioPage(); return; }
  if (pointInButton(x, y, stop)) { audioTouchConsumed = true; audioStopPlayback(); drawAudioPage(); return; }
  if (pointInButton(x, y, next)) { audioTouchConsumed = true; audioNextTrack(false); drawAudioPage(); return; }
  if (pointInButton(x, y, list)) { audioTouchConsumed = true; musicListOffset = (musicIndex / MUSIC_LIST_ROWS) * MUSIC_LIST_ROWS; drawMusicListPage(); return; }
  if (pointInButton(x, y, repeat)) {
    unsigned long nowMode = millis();
    if (nowMode - lastAudioRepeatButtonMs < AUDIO_MODE_BUTTON_GUARD_MS) { audioTouchConsumed = true; return; }
    lastAudioRepeatButtonMs = nowMode;
    audioTouchConsumed = true;
    audioToggleRepeatMode();
    // V57C: no redibujar toda la pantalla. Solo el boton y el estado.
    drawAudioRepeatButtonOnly();
    drawAudioStatusOnly();
    audioRedrawBlockUntil = millis() + 900;
    return;
  }
  if (pointInButton(x, y, shuffle)) {
    unsigned long nowMode = millis();
    if (nowMode - lastAudioShuffleButtonMs < AUDIO_MODE_BUTTON_GUARD_MS) { audioTouchConsumed = true; return; }
    lastAudioShuffleButtonMs = nowMode;
    audioTouchConsumed = true;
    audioToggleShuffle();
    // V57C: no redibujar toda la pantalla. Solo el boton y el estado.
    drawAudioShuffleButtonOnly();
    drawAudioStatusOnly();
    audioRedrawBlockUntil = millis() + 900;
    return;
  }
  if (pointInButton(x, y, folder)) { audioTouchConsumed = true; musicBrowserPath = MUSIC_ROOT_FOLDER; musicFolderScanned = false; drawMusicFolderPage(); return; }
  if (pointInButton(x, y, refresh)) {
    audioTouchConsumed = true;
    musicScanned = false;
    musicFolderScanned = false;
    if (musicRootHasLooseFiles()) {
      musicNeedOrganizeConfirm = true;
      musicUserApprovedOrganization = false;
      musicOrganizePending = false;
      drawAudioPage();
    } else if (!isMusicRootPath(musicFolder)) {
      // V54: refrescar solo la carpeta seleccionada. No construir todos los indices.
      scanMusicFiles(true);
      clearSdActivityLedNow();
      drawAudioPage();
    } else {
      musicAutoOrganizeChecked = true;
      musicFolderScanned = false;
      scanMusicFolders();
      audioStatusMsg = "Elige Carpeta";
      clearSdActivityLedNow();
      drawAudioPage();
    }
    return;
  }
  // V57B: barras solo visuales por defecto.
  // Si se reactiva AUDIO_DIRECT_BAR_TOUCH_ENABLED, volveran a aceptar toque directo.
  if (AUDIO_DIRECT_BAR_TOUCH_ENABLED) {
    if (x >= 54 && x <= 186 && y >= 160 && y <= 184) { audioTouchConsumed = true; audioSetSeekPercent(::map(x, 54, 186, 0, 100)); drawAudioProgressOnly(); return; }
    if (x >= 54 && x <= 186 && y >= 196 && y <= 220) { audioTouchConsumed = true; audioSetVolumeFromX(x); return; }
  }
}
void handleMusicListTouch(int x, int y) {
  if (sdOK && musicCount > 0) {
    for (int i = 0; i < MUSIC_LIST_ROWS; i++) {
      int idx = musicListOffset + i;
      if (idx >= musicCount) break;
      Button row = { 6, (int16_t)(54 + i * 37), 228, 32, "", ACT_NONE };
      if (pointInButton(x, y, row)) {
        musicIndex = idx;
        if (alarmMusicSelectMode) {
          alarmMusicPath = musicFiles[idx];
          alarmMusicSelectMode = false;
          alarmStatusMsg = String("Alarma: ") + folderBaseName(alarmMusicPath);
          drawClockPage();
        } else {
          audioStartCurrent();
          drawAudioPage();
        }
        return;
      }
    }
  }
  Button up = { 82, 286, 44, 28, "Arr.", ACT_MUSIC_LIST_PREV };
  Button down = { 130, 286, 44, 28, "Abj.", ACT_MUSIC_LIST_NEXT };
  Button refresh = { 178, 286, 56, 28, "Act.", ACT_MUSIC_LIST_REFRESH };
  if (pointInButton(x, y, up)) {
    musicListOffset -= MUSIC_LIST_ROWS;
    if (musicListOffset < 0) musicListOffset = 0;
    drawMusicListPage();
    return;
  }
  if (pointInButton(x, y, down)) {
    musicListOffset += MUSIC_LIST_ROWS;
    if (musicListOffset >= musicCount) musicListOffset = max(0, musicCount - MUSIC_LIST_ROWS);
    drawMusicListPage();
    return;
  }
  if (pointInButton(x, y, refresh)) {
    musicScanned = false;
    if (isMusicRootPath(musicFolder)) organizeMusicRootFiles();
    else scanMusicFiles(true);
    drawMusicListPage();
    return;
  }
}

void handleMusicFolderTouch(int x, int y) {
  if (sdOK && musicFolderCount > 0) {
    if (musicFolderOffset < 0) musicFolderOffset = 0;
    if (musicFolderOffset >= musicFolderCount) musicFolderOffset = max(0, musicFolderCount - MUSIC_FOLDER_ROWS);
    int maxShow = min(musicFolderCount - musicFolderOffset, MUSIC_FOLDER_ROWS);
    for (int i = 0; i < maxShow; i++) {
      int idx = musicFolderOffset + i;
      Button row = { 6, (int16_t)(64 + i * 27), 228, 25, "", ACT_NONE };
      if (pointInButton(x, y, row)) {
        musicBrowserPath = musicFolders[idx];
        musicFolderOffset = 0;
        musicFolderScanned = false;
        drawMusicFolderPage();
        return;
      }
    }
  }
  Button up = { 50, 286, 48, 28, "Padre", ACT_FOLDER_UP };
  Button prev = { 102, 286, 36, 28, "Arr", ACT_MUSIC_LIST_PREV };
  Button next = { 142, 286, 36, 28, "Abj", ACT_MUSIC_LIST_NEXT };
  Button use = { 182, 286, 52, 28, "Usar", ACT_FOLDER_USE };
  if (pointInButton(x, y, up)) {
    musicBrowserPath = parentFolderPath(musicBrowserPath);
    musicFolderScanned = false;
    musicFolderOffset = 0;
    drawMusicFolderPage();
    return;
  }
  if (pointInButton(x, y, prev)) { scrollMusicFolderList(-MUSIC_FOLDER_ROWS); return; }
  if (pointInButton(x, y, next)) { scrollMusicFolderList(MUSIC_FOLDER_ROWS); return; }
  if (pointInButton(x, y, use)) {
    musicFolder = musicBrowserPath.length() ? musicBrowserPath : MUSIC_ROOT_FOLDER;
    if (isMusicRootPath(musicFolder)) {
      audioStatusMsg = "Selecciona una subcarpeta";
      drawAudioPage();
      return;
    }
    saveMusicFolderConfig();
    musicScanned = false;
    musicIndex = 0;
    musicListOffset = 0;
    scanMusicFiles();
    if (musicCount > 0) audioStatusMsg = String("Lista preparada: ") + String(musicCount) + " canciones. Pulsa Play";
    clearSdActivityLedNow();
    drawAudioPage();
    return;
  }
}

#if HAVE_AUDIO_LIB
void audio_eof_mp3(const char *info) {
  Serial.print("eof_mp3     ");
  Serial.println(info);
  if (!audioChangingTrack && musicPlaying && !musicPaused) {
    // V53: el callback solo marca el fin. Cambiar pista aqui puede provocar
    // reentrada en Audio.h, parpadeos y tactil sin responder.
    musicEofPending = true;
    musicEofPendingMs = millis();
    audioStatusMsg = "Siguiente...";
  }
}

void audio_info(const char *info) {
  Serial.print("audio_info  ");
  Serial.println(info);
}
#endif

// -------------------- FTP basico para microSD --------------------
void ftpWriteRaw(WiFiClient &client, const char *text) {
  if (!text) return;
  size_t __n = strlen(text); client.write((const uint8_t*)text, __n); wifiLedMarkBytes(__n);
}

void ftpWriteRaw(WiFiClient &client, const String &text) {
  size_t __n = text.length(); client.write((const uint8_t*)text.c_str(), __n); wifiLedMarkBytes(__n);
}

void ftpReply(int code, const char *message) {
  if (!ftpControlClient || !ftpControlClient.connected()) return;
  char head[12];
  snprintf(head, sizeof(head), "%d ", code);
  ftpWriteRaw(ftpControlClient, head);
  ftpWriteRaw(ftpControlClient, message);
  ftpWriteRaw(ftpControlClient, "\r\n");
  ftpControlClient.flush();
}

void ftpReplyStr(int code, const String &message) {
  if (!ftpControlClient || !ftpControlClient.connected()) return;
  char head[12];
  snprintf(head, sizeof(head), "%d ", code);
  ftpWriteRaw(ftpControlClient, head);
  ftpWriteRaw(ftpControlClient, message);
  ftpWriteRaw(ftpControlClient, "\r\n");
  ftpControlClient.flush();
}

String ftpNormalizePath(String arg) {
  arg.trim();
  arg.replace("\\", "/");
  if (arg.length() == 0) arg = ftpCurrentDir;
  if (!arg.startsWith("/")) arg = (ftpCurrentDir == "/") ? String("/") + arg : ftpCurrentDir + "/" + arg;
  while (arg.indexOf("//") >= 0) arg.replace("//", "/");
  if (arg.length() > 1 && arg.endsWith("/")) arg.remove(arg.length() - 1);
  return arg;
}

String ftpBaseName(String path) {
  path.replace("\\", "/");
  int p = path.lastIndexOf('/');
  return p >= 0 ? path.substring(p + 1) : path;
}

String ftpParentDir(String path) {
  path.replace("\\", "/");
  int p = path.lastIndexOf('/');
  if (p <= 0) return "/";
  return path.substring(0, p);
}

bool ftpPathIsDir(const String &path) {
  File f = sdOpenCompat(path.c_str(), FILE_READ);
  bool ok = f && f.isDirectory();
  if (f) f.close();
  return ok;
}

void ftpEnterPassive() {
  ftpActiveReady = false;
  ftpDataServer.begin();
  IPAddress ip = WiFi.localIP();
  uint8_t p1 = FTP_PASSIVE_PORT / 256;
  uint8_t p2 = FTP_PASSIVE_PORT % 256;
  char msg[112];
  snprintf(msg, sizeof(msg), "Entering Passive Mode (%u,%u,%u,%u,%u,%u).", (unsigned)ip[0], (unsigned)ip[1], (unsigned)ip[2], (unsigned)ip[3], (unsigned)p1, (unsigned)p2);
  ftpReply(227, msg);
}

void ftpEnterExtendedPassive() {
  ftpActiveReady = false;
  ftpDataServer.begin();
  ftpReplyStr(229, String("Entering Extended Passive Mode (|||") + String(FTP_PASSIVE_PORT) + "|)");
}

bool ftpParsePort(String arg) {
  int v[6] = {0,0,0,0,0,0};
  int n = sscanf(arg.c_str(), "%d,%d,%d,%d,%d,%d", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]);
  if (n != 6) return false;
  for (int i = 0; i < 6; i++) if (v[i] < 0 || v[i] > 255) return false;
  ftpActiveIp = IPAddress((uint8_t)v[0], (uint8_t)v[1], (uint8_t)v[2], (uint8_t)v[3]);
  ftpActivePort = (uint16_t)(v[4] * 256 + v[5]);
  ftpActiveReady = true;
  return ftpActivePort > 0;
}

bool ftpParseEprt(String arg) {
  arg.trim();
  if (arg.length() < 7) return false;
  char sep = arg[0];
  int p1 = arg.indexOf(sep, 1);
  int p2 = arg.indexOf(sep, p1 + 1);
  int p3 = arg.indexOf(sep, p2 + 1);
  if (p1 < 0 || p2 < 0 || p3 < 0) return false;
  String af = arg.substring(1, p1);
  String ip = arg.substring(p1 + 1, p2);
  String port = arg.substring(p2 + 1, p3);
  if (af != "1") return false; // Solo IPv4.
  IPAddress parsed;
  if (!parsed.fromString(ip)) return false;
  int pp = port.toInt();
  if (pp <= 0 || pp > 65535) return false;
  ftpActiveIp = parsed;
  ftpActivePort = (uint16_t)pp;
  ftpActiveReady = true;
  return true;
}

WiFiClient ftpWaitPassiveData(uint32_t timeoutMs = 15000) {
  uint32_t start = millis();
  while (millis() - start < timeoutMs) {
    WiFiClient c = ftpDataServer.available();
    if (c && c.connected()) {
      c.setNoDelay(true);
      return c;
    }
    delay(5);
    yield();
  }
  WiFiClient empty;
  return empty;
}

WiFiClient ftpOpenData(uint32_t timeoutMs = 15000) {
  if (ftpActiveReady && ftpActivePort > 0) {
    WiFiClient active;
    active.setNoDelay(true);
    if (active.connect(ftpActiveIp, ftpActivePort)) {
      ftpActiveReady = false;
      return active;
    }
    ftpActiveReady = false;
  }
  return ftpWaitPassiveData(timeoutMs);
}

void ftpList(String arg, bool namesOnly) {
  if (!sdOK) { ftpReply(550, "SD no montada"); return; }
  String path = ftpNormalizePath(arg);
  File root = sdOpenCompat(path.c_str(), FILE_READ);
  if (!root) { ftpReply(550, "No se puede abrir"); return; }
  ftpReply(150, "Abriendo datos");
  WiFiClient data = ftpOpenData();
  if (!data || !data.connected()) { root.close(); ftpReply(425, "Sin conexion de datos"); return; }
  if (root.isDirectory()) {
    File f = root.openNextFile();
    while (f) {
      String n = ftpBaseName(String(f.name()));
      if (namesOnly) {
        ftpWriteRaw(data, n + "\r\n");
      } else {
        char line[220];
        const char *perm = f.isDirectory() ? "drwxr-xr-x" : "-rw-r--r--";
        snprintf(line, sizeof(line), "%s 1 owner group %10lu Jan 01 00:00 %s\r\n", perm, (unsigned long)f.size(), n.c_str());
        ftpWriteRaw(data, line);
      }
      f.close();
      f = root.openNextFile();
      yield();
    }
  } else {
    String n = ftpBaseName(path);
    if (namesOnly) ftpWriteRaw(data, n + "\r\n");
    else {
      char line[220];
      snprintf(line, sizeof(line), "-rw-r--r-- 1 owner group %10lu Jan 01 00:00 %s\r\n", (unsigned long)root.size(), n.c_str());
      ftpWriteRaw(data, line);
    }
  }
  root.close();
  data.stop();
  ftpReply(226, "Transferencia completa");
}

void ftpRetrieve(String arg) {
  if (!sdOK) { ftpReply(550, "SD no montada"); return; }
  String path = ftpNormalizePath(arg);
  File file = sdOpenCompat(path.c_str(), FILE_READ);
  if (!file || file.isDirectory()) { if (file) file.close(); ftpReply(550, "No se puede leer"); return; }
  ftpReply(150, "Enviando");
  WiFiClient data = ftpOpenData();
  if (!data || !data.connected()) { file.close(); ftpReply(425, "Sin conexion de datos"); return; }
  uint8_t buf[1024];
  while (file.available()) {
    int n = file.read(buf, sizeof(buf));
    if (n > 0) {
      sdLedMarkRead();
      data.write(buf, n);
      wifiLedMarkBytes((uint32_t)n);
      updateActivityLedIndicator();
    }
    yield();
  }
  file.close();
  data.stop();
  ftpReply(226, "Transferencia completa");
}

void ftpStore(String arg) {
  if (!sdOK) { ftpReply(550, "SD no montada"); return; }
  String path = ftpNormalizePath(arg);
  if (path == "/" || path.length() < 2) { ftpReply(550, "Nombre no valido"); return; }

  String parent = ftpParentDir(path);
  if (!ftpPathIsDir(parent)) { ftpReply(550, "Carpeta destino no existe"); return; }

  // FileZilla espera una respuesta 150 antes de empezar o continuar la transferencia.
  ftpReply(150, "Listo para recibir datos");
  WiFiClient data = ftpOpenData(20000);
  if (!data || !data.connected()) { ftpReply(425, "No se abrio conexion de datos"); return; }

  // Para que subir de nuevo un archivo lo sustituya y no lo anada al final.
  if (sdExistsCompat(path.c_str())) sdRemoveCompat(path.c_str());
  File file = sdOpenCompat(path.c_str(), FILE_WRITE);
  if (!file) {
    data.stop();
    ftpReply(550, "No se puede crear archivo");
    return;
  }

  uint8_t buf[1460];
  uint32_t lastData = millis();
  uint32_t totalBytes = 0;
  bool writeError = false;

  while (data.connected() || data.available()) {
    int avail = data.available();
    if (avail > 0) {
      size_t want = (avail > (int)sizeof(buf)) ? sizeof(buf) : (size_t)avail;
      int n = data.read(buf, want);
      if (n > 0) {
        wifiLedMarkBytes((uint32_t)n);
        sdLedMarkWrite();
        updateActivityLedIndicator();
        size_t w = file.write(buf, n);
        if (w != (size_t)n) { writeError = true; break; }
        totalBytes += w;
        lastData = millis();
      }
    } else {
      // Si el cliente ha terminado, normalmente cierra el canal de datos.
      // Damos margen amplio para microSD lenta o WiFi irregular, pero evitamos bloqueo infinito.
      if (millis() - lastData > 15000) break;
      delay(1);
      yield();
    }
  }

  file.flush();
  file.close();
  data.stop();

  if (writeError) {
    sdRemoveCompat(path.c_str());
    ftpReply(451, "Error escribiendo en microSD");
  } else {
    ftpStatus = String("Ultimo archivo: ") + ftpBaseName(path) + " (" + String((unsigned long)totalBytes) + " bytes)";
    invalidateMusicListIfPathLooksMusic(path);
    ftpReply(226, "Archivo recibido OK");
  }
}

void ftpProcessCommand(String line) {
  line.trim();
  if (line.length() == 0) return;
  int sp = line.indexOf(' ');
  String cmd = sp >= 0 ? line.substring(0, sp) : line;
  String arg = sp >= 0 ? line.substring(sp + 1) : "";
  cmd.toUpperCase(); arg.trim();

  if (cmd == "USER") { ftpUserOk = (arg == "esp32"); ftpReply(ftpUserOk ? 331 : 530, ftpUserOk ? "OK clave" : "Usuario mal"); }
  else if (cmd == "PASS") { ftpLogged = ftpUserOk && (arg == "esp32sd"); ftpReply(ftpLogged ? 230 : 530, ftpLogged ? "Sesion OK" : "Clave mal"); }
  else if (cmd == "QUIT") { ftpReply(221, "Adios"); ftpControlClient.stop(); }
  else if (cmd == "SYST") ftpReply(215, "UNIX Type: L8");
  else if (cmd == "FEAT") ftpWriteRaw(ftpControlClient, "211-Features\r\n UTF8\r\n PASV\r\n EPSV\r\n SIZE\r\n REST STREAM\r\n211 End\r\n");
  else if (cmd == "OPTS" || cmd == "TYPE" || cmd == "NOOP" || cmd == "MODE" || cmd == "STRU" || cmd == "ALLO") ftpReply(200, "OK");
  else if (!ftpLogged) ftpReply(530, "Login primero");
  else if (cmd == "PWD" || cmd == "XPWD") ftpReplyStr(257, String("\"") + ftpCurrentDir + "\"");
  else if (cmd == "CWD") {
    String p = ftpNormalizePath(arg);
    if (ftpPathIsDir(p)) { ftpCurrentDir = p; ftpReply(250, "CWD OK"); }
    else ftpReply(550, "Carpeta no existe");
  }
  else if (cmd == "CDUP") {
    if (ftpCurrentDir != "/") ftpCurrentDir = ftpParentDir(ftpCurrentDir);
    ftpReply(250, "OK");
  }
  else if (cmd == "PASV") ftpEnterPassive();
  else if (cmd == "EPSV") ftpEnterExtendedPassive();
  else if (cmd == "PORT") { if (ftpParsePort(arg)) ftpReply(200, "PORT OK"); else ftpReply(501, "PORT invalido"); }
  else if (cmd == "EPRT") { if (ftpParseEprt(arg)) ftpReply(200, "EPRT OK"); else ftpReply(501, "EPRT invalido"); }
  else if (cmd == "LIST" || cmd == "MLSD") ftpList(arg, false);
  else if (cmd == "NLST") ftpList(arg, true);
  else if (cmd == "RETR") ftpRetrieve(arg);
  else if (cmd == "STOR" || cmd == "APPE") ftpStore(arg);
  else if (cmd == "REST") ftpReply(350, "REST aceptado pero se reinicia desde cero");
  else if (cmd == "DELE") { String p = ftpNormalizePath(arg); if (sdRemoveCompat(p.c_str())) { invalidateMusicListIfPathLooksMusic(p); ftpReply(250, "Eliminado"); } else ftpReply(550, "Fallo"); }
  else if (cmd == "MKD" || cmd == "XMKD") { if (sdMkdirCompat(ftpNormalizePath(arg).c_str())) ftpReplyStr(257, String("\"") + ftpNormalizePath(arg) + "\" creado"); else ftpReply(550, "Fallo"); }
  else if (cmd == "RMD" || cmd == "XRMD") { String p = ftpNormalizePath(arg); if (sdRmdirCompat(p.c_str())) { invalidateMusicListIfPathLooksMusic(p); ftpReply(250, "Eliminado"); } else ftpReply(550, "Fallo"); }
  else if (cmd == "RNFR") { ftpRenameFrom = ftpNormalizePath(arg); ftpReply(350, "RNTO"); }
  else if (cmd == "RNTO") { String p = ftpNormalizePath(arg); if (sdRenameCompat(ftpRenameFrom.c_str(), p.c_str())) { invalidateMusicListIfPathLooksMusic(ftpRenameFrom); invalidateMusicListIfPathLooksMusic(p); ftpReply(250, "Renombrado"); } else ftpReply(550, "Fallo"); }
  else if (cmd == "SIZE") { File f = sdOpenCompat(ftpNormalizePath(arg).c_str(), FILE_READ); if (f && !f.isDirectory()) { ftpReplyStr(213, String((unsigned long)f.size())); f.close(); } else { if (f) f.close(); ftpReply(550,"Fallo"); } }
  else if (cmd == "MDTM") ftpReply(550, "Fecha no disponible");
  else if (cmd == "MFMT") ftpReply(213, "Fecha ignorada");
  else ftpReply(502, "No implementado");
}


void enterMusicHighPerformanceMode() {
  if (musicHighPerfActive) return;
  musicHighPerfActive = true;
  musicHighPerfStartedMs = millis();
  musicHighPerfStoppedWiFi = false;
  musicHighPerfStoppedWeb = false;
  musicHighPerfStoppedFTP = false;
  musicHighPerfStoppedCamera = false;

  // Prioridad al audio: camara TFT y camara web no deben competir con Audio.h.
  if (cameraWebStarted) {
    stopCameraServer();
    deinitCamera();
    cameraWebStarted = false;
    cameraWebStatusMsg = "Camara parada por Musica";
    musicHighPerfStoppedCamera = true;
  }
  if (cameraOK) {
    deinitCamera();
    cameraOK = false;
    cameraTried = false;
    musicHighPerfStoppedCamera = true;
  }

  // Apagar servidor web/OTA y FTP mientras se escucha musica.
  // No se reactivan solos al salir: quedan disponibles para activarlos manualmente.
  if (webServerEnabled || otaReady) {
    otaServer.close();
    otaReady = false;
    webServerEnabled = false;
    webServerStatusMsg = "Web apagada por Musica";
    musicHighPerfStoppedWeb = true;
  }
  if (ftpEnabled) {
    ftpStartStop();
    ftpStatus = "FTP parado por Musica";
    musicHighPerfStoppedFTP = true;
  }

  if (WiFi.status() == WL_CONNECTED || WiFi.getMode() != WIFI_OFF) {
    WiFi.disconnect(false);
    delay(40);
    WiFi.mode(WIFI_OFF);
    wifiStatusMsg = "WiFi apagado por Musica";
    musicHighPerfStoppedWiFi = true;
  }

#if defined(setCpuFrequencyMhz)
  setCpuFrequencyMhz(240);
#endif
  audioStatusMsg = "Modo Musica alto rendimiento";
}

void leaveMusicHighPerformanceMode() {
  if (!musicHighPerfActive) return;
  musicHighPerfActive = false;
  // Dejamos WiFi/Web/FTP apagados para evitar que roben recursos al salir.
  // Se pueden activar de nuevo manualmente desde WiFi, FTP o Servidor.
  if (musicHighPerfStoppedWiFi) wifiStatusMsg = "WiFi apagado. Pulsa Conectar";
  if (musicHighPerfStoppedWeb) webServerStatusMsg = "Web apagada. Activa desde Servidor";
  if (musicHighPerfStoppedFTP) ftpStatus = "FTP parado";
  musicHighPerfStoppedWiFi = false;
  musicHighPerfStoppedWeb = false;
  musicHighPerfStoppedFTP = false;
  musicHighPerfStoppedCamera = false;
}

bool musicRootHasLooseFiles() {
  if (!sdTried) initSD();
  if (!sdOK) return false;
  if (!sdExistsCompat(MUSIC_ROOT_FOLDER)) sdMkdirCompat(MUSIC_ROOT_FOLDER);
  String first = findFirstMusicFileDirect(MUSIC_ROOT_FOLDER);
  return first.length() > 0;
}

String musicIndexPathForFolder(const String &folder) {
  String f = normalizePathSimple(folder);
  if (isMusicRootPath(f)) return MUSIC_INDEX_FILE;
  return joinPathSimple(f, "_music_index.txt");
}

void saveMusicIndexFileForFolder(const String &folder) {
  if (!sdOK) return;
  String fpath = musicIndexPathForFolder(folder);
  if (sdExistsCompat(fpath.c_str())) sdRemoveCompat(fpath.c_str());

  String oldFolder = musicFolder;
  int oldCount = musicCount;
  int oldIndex = musicIndex;
  int oldOffset = musicListOffset;
  bool oldScanned = musicScanned;

  String tempFiles[MUSIC_AUTO_FOLDER_SIZE + 5];
  int tempCount = 0;
  File root = sdOpenCompat(folder.c_str(), FILE_READ);
  if (root && root.isDirectory()) {
    File file = root.openNextFile();
    while (file && tempCount < MUSIC_AUTO_FOLDER_SIZE + 5) {
      String n = String(file.name());
      if (!n.startsWith("/")) n = joinPathSimple(folder, n);
      if (!file.isDirectory() && isMusicFileName(n)) tempFiles[tempCount++] = n;
      file.close();
      file = root.openNextFile();
      yield();
    }
  }
  if (root) root.close();
  sortStringArray(tempFiles, tempCount);

  File out = sdOpenCompat(fpath.c_str(), FILE_WRITE);
  if (out) {
    out.println("# Indice automatico por carpeta del ESP32-S3");
    out.print("# Carpeta: "); out.println(folder);
    out.print("# Total: "); out.println(tempCount);
    for (int i = 0; i < tempCount; i++) out.println(tempFiles[i]);
    out.close();
    sdLedMarkWrite();
  }

  musicFolder = oldFolder;
  musicCount = oldCount;
  musicIndex = oldIndex;
  musicListOffset = oldOffset;
  musicScanned = oldScanned;
}

void buildAllMusicFolderIndexes() {
  // V53: compatibilidad con llamadas antiguas. Ya no se bloquea creando todos
  // los indices de golpe; se preparan y el loop los va creando por pasos.
  musicStartIndexBuildDeferred();
}

void musicStartIndexBuildDeferred() {
  if (!sdTried) initSD();
  if (!sdOK) return;
  musicIndexBuildPending = false;
  musicIndexBuildActive = true;
  musicIndexBuildPos = 0;
  musicIndexBuildLastCount = 0;
  musicIndexBuildLastMs = 0;
  musicFolderScanned = false;
  scanMusicFolders();
  audioStatusMsg = String("Creando indices: 0/") + String(musicFolderCount);
  musicOrgStatusMsg = audioStatusMsg;
  if (musicFolderCount <= 0) {
    musicIndexBuildActive = false;
    audioStatusMsg = "No hay carpetas de musica";
    musicOrgStatusMsg = audioStatusMsg;
    clearSdActivityLedNow();
  }
}

void musicIndexBuildTick() {
  if (!musicIndexBuildActive) return;
  if (!sdOK) {
    musicIndexBuildActive = false;
    return;
  }
  if (musicIndexBuildPos < musicFolderCount) {
    saveMusicIndexFileForFolder(musicFolders[musicIndexBuildPos]);
    musicIndexBuildPos++;
    musicIndexBuildLastCount++;
    musicIndexBuildLastMs = millis();
    audioStatusMsg = String("Creando indices: ") + String(musicIndexBuildLastCount) + "/" + String(musicFolderCount);
    musicOrgStatusMsg = audioStatusMsg;
    return;
  }

  musicIndexBuildActive = false;
  musicFolderScanned = false;
  scanMusicFolders();
  audioStatusMsg = String("Indices listos: ") + String(musicIndexBuildLastCount) + " carpetas";
  musicOrgStatusMsg = "Ordenacion terminada";
  clearSdActivityLedNow();
}

void musicStartConfirmedOrganization() {
  musicNeedOrganizeConfirm = false;
  musicLockedByUnordered = false;
  musicUserApprovedOrganization = true;
  musicOrganizePending = true;
  musicAutoOrganizeChecked = false;
  // V54: no crear indices globales despues de ordenar; cada carpeta genera
  // su _music_index.txt al seleccionarla, evitando bloqueos y parpadeos.
  musicIndexBuildPending = false;
  musicIndexBuildActive = false;
  musicOrganizedMovedTotal = 0;
  musicOrganizePendingMs = 0;
  musicOrgStatusMsg = "Ordenando musica...";
  audioStatusMsg = musicOrgStatusMsg;
}

void musicCancelOrganizationForNow() {
  musicNeedOrganizeConfirm = false;
  musicLockedByUnordered = true;
  musicUserApprovedOrganization = false;
  musicOrganizePending = false;
  musicAutoOrganizeChecked = false;
  musicScanned = false;
  musicCount = 0;
  audioStatusMsg = "Musica bloqueada: ordena primero";
}

void drawMusicOrganizePrompt() {
  tft.fillRect(0, 34, SCREEN_W, 248, TFT_BLACK);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString("Hay canciones sueltas", 10, 44, 2);
  tft.drawString("en /music sin ordenar.", 10, 68, 2);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("Para reproducirlas,", 10, 102, 2);
  tft.drawString("se guardaran en carpetas", 10, 126, 2);
  tft.drawString("de 50 canciones.", 10, 150, 2);

  Button yes = { 24, 198, 88, 38, "Si", ACT_MUSIC_ORG_YES };
  Button no  = {128, 198, 88, 38, "No", ACT_MUSIC_ORG_NO };
  drawButton(yes, TFT_DARKGREEN);
  drawButton(no, TFT_MAROON);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawCentreString("Si eliges No, no reproduce", SCREEN_W / 2, 250, 1);
}

void prepareMusicFoldersForMusicMode() {
  if (!sdTried) initSD();
  if (!sdOK) return;
  if (!sdExistsCompat(MUSIC_ROOT_FOLDER)) sdMkdirCompat(MUSIC_ROOT_FOLDER);

  // V54: esta funcion se llama cada vez que se redibuja la pantalla Musica.
  // No debe recorrer /music una y otra vez, porque eso provoca parpadeos,
  // LED morado constante y retrasos tactiles. Solo comprueba canciones sueltas
  // la primera vez, o cuando FTP/Refres. invalida musicAutoOrganizeChecked.
  // V55: si quedo un estado antiguo de ordenacion pendiente pero el usuario no
  // ha pulsado Si en esta entrada, lo limpiamos. Esto evita que empiece a mover
  // canciones al abrir Musica o tras subir archivos por FTP.
  if (musicOrganizePending && !musicUserApprovedOrganization) {
    musicOrganizePending = false;
    musicNeedOrganizeConfirm = false;
    musicOrgStatusMsg = "";
  }
  if (musicOrganizePending || musicLockedByUnordered || musicIndexBuildActive) return;

  if (musicAutoOrganizeChecked) {
    musicNeedOrganizeConfirm = false;
    musicIndexBuildPending = false;
    if (!musicFolderScanned) { scanMusicFolders(); clearSdActivityLedNow(); }
    if (isMusicRootPath(musicFolder) && audioStatusMsg.length() == 0) audioStatusMsg = "Elige Carpeta";
    return;
  }

  musicNeedOrganizeConfirm = false;
  if (musicRootHasLooseFiles()) {
    musicNeedOrganizeConfirm = true;
    musicUserApprovedOrganization = false;
    musicOrganizePending = false;
    musicIndexBuildPending = false;
    musicIndexBuildActive = false;
    musicScanned = false;
    musicCount = 0;
    audioStatusMsg = "Hay canciones sin ordenar";
    clearSdActivityLedNow();
    return;
  }

  // No hay canciones sueltas: queda preparado para elegir carpeta y reproducir.
  musicOrganizePending = false;
  musicUserApprovedOrganization = false;
  musicLockedByUnordered = false;
  musicAutoOrganizeChecked = true;
  musicIndexBuildPending = false;
  musicIndexBuildActive = false;
  if (!musicFolderScanned) scanMusicFolders();
  if (isMusicRootPath(musicFolder)) audioStatusMsg = "Elige Carpeta";
  clearSdActivityLedNow();
}

void ftpStartStop() {
  if (ftpEnabled) {
    ftpControlClient.stop();
    ftpEnabled = false;
    ftpLogged = false;
    ftpUserOk = false;
    ftpActiveReady = false;
    ftpStatus = "FTP parado";
    return;
  }
  if (!sdTried) initSD();
  if (!sdOK) { ftpStatus = "No hay microSD"; return; }
  if (WiFi.status() != WL_CONNECTED) { ftpStatus = "Conecta WiFi primero"; return; }
  ftpServer.begin();
  ftpDataServer.begin();
  ftpEnabled = true;
  ftpLogged = false;
  ftpUserOk = false;
  ftpActiveReady = false;
  ftpCurrentDir = "/";
  ftpStatus = String("ftp://") + WiFi.localIP().toString() + ":21";
}

void ftpHandle() {
  if (!ftpEnabled) return;

  // FileZilla suele mantener una conexion de control abierta para navegar y,
  // al subir un archivo desde la cola, abre otra conexion de control nueva.
  // El ESP32 no puede mantener bien varias sesiones FTP completas a la vez.
  // En V19 la segunda conexion quedaba aceptada por TCP pero sin mensaje 220,
  // por eso FileZilla mostraba: "esperando el mensaje de bienvenida" y fallaba.
  // Solucion V20: si llega una conexion nueva, se cierra la anterior y se
  // atiende inmediatamente la nueva enviando el saludo 220.
  WiFiClient newClient = ftpServer.available();
  if (newClient && newClient.connected()) {
    if (ftpControlClient && ftpControlClient.connected()) {
      ftpWriteRaw(ftpControlClient, "421 Nueva conexion FTP, cerrando sesion anterior.\r\n");
      ftpControlClient.flush();
      delay(20);
      ftpControlClient.stop();
      delay(20);
    }

    ftpControlClient = newClient;
    ftpControlClient.setNoDelay(true);
    ftpLogged = false;
    ftpUserOk = false;
    ftpActiveReady = false;
    ftpControlLine = "";
    ftpStatus = "FTP cliente conectado";
    ftpReply(220, "ESP32-S3 FTP listo");
  }

  while (ftpControlClient && ftpControlClient.connected() && ftpControlClient.available()) {
    int ch = ftpControlClient.read();
    if (ch < 0) break;
    char c = (char)ch;
    if (c == '\r') continue;
    if (c == '\n') { String line = ftpControlLine; ftpControlLine = ""; ftpProcessCommand(line); }
    else if (ftpControlLine.length() < 320) ftpControlLine += c;
  }
}

// -------------------- Camara Web --------------------
bool initCameraWebMode() {
  cameraWebTried = true;
  if (WiFi.status() != WL_CONNECTED) {
    cameraWebStarted = false;
    cameraWebStatusMsg = "Sin WiFi";
    return false;
  }

  // Si ya habia un servidor, lo paramos antes de reconfigurar la camara.
  if (cameraWebStarted) {
    stopCameraServer();
    cameraWebStarted = false;
    delay(100);
  }

  deinitCamera();
  esp_camera_deinit();
  delay(120);

  camera_config_t config;
  memset(&config, 0, sizeof(config));
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
#else
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
#endif
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  // V45: CamWeb en modo estable. VGA generaba tirones y congelaciones
  // en ESP32-S3 al servir MJPEG por WiFi. QVGA reduce CPU, RAM y trafico.
  config.xclk_freq_hz = 10000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 16;
  config.fb_count = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    cameraOK = false;
    cameraWebStarted = false;
    cameraWebStatusMsg = String("Error camara 0x") + String((uint32_t)err, HEX);
    return false;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s) {
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -1);
    // V45: parametros suaves para priorizar estabilidad del stream.
    s->set_framesize(s, FRAMESIZE_QVGA);
    s->set_quality(s, 16);
  }

  startCameraServer();
  cameraOK = true;
  cameraWebStarted = true;
  cameraWebStatusMsg = "Activo: /cam y puerto 81";
  return true;
}


// -------------------- Servidor CamWeb integrado V36 --------------------
// Esta version NO depende de app_httpd.cpp, camera_index.h ni otros archivos extra.
// Se integra aqui para que ArduinoDroid compile el sketch aunque solo abra el .ino.
static httpd_handle_t camWebControlHttpd = NULL;
static httpd_handle_t camWebStreamHttpd = NULL;
static const char *CAMWEB_BOUNDARY = "123456789000000000000987654321";
static const char *CAMWEB_STREAM_TYPE = "multipart/x-mixed-replace;boundary=123456789000000000000987654321";
static const char *CAMWEB_STREAM_BOUNDARY = "\r\n--123456789000000000000987654321\r\n";
static const uint16_t CAMWEB_STREAM_FRAME_INTERVAL_MS = 115;  // V45: ~8 fps max para estabilidad.

String camWebBuildPage() {
  // Compatibilidad con las rutas antiguas de CamWeb.
  return webPortalPage("cam", cameraWebStatusMsg);
}

static esp_err_t camWebIndexHandler(httpd_req_t *req) {
  wifiLedMarkActivity();
  String html = camWebBuildPage();
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  return httpd_resp_send(req, html.c_str(), html.length());
}

static esp_err_t camWebCaptureHandler(httpd_req_t *req) {
  wifiLedMarkActivity();
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
  esp_err_t res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  wifiLedMarkBytes((uint32_t)fb->len);
  esp_camera_fb_return(fb);
  return res;
}

static esp_err_t camWebStreamHandler(httpd_req_t *req) {
  wifiLedMarkActivity();
  esp_err_t res = httpd_resp_set_type(req, CAMWEB_STREAM_TYPE);
  if (res != ESP_OK) return res;
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  char partBuf[96];
  unsigned long lastFrameMs = 0;
  while (true) {
    // V45: limitar FPS evita que el servidor MJPEG acapare CPU/WiFi y deje
    // congelada la pagina o el tactil. Mientras esperamos, mantenemos vivo el LED.
    unsigned long now = millis();
    if (lastFrameMs != 0 && now - lastFrameMs < CAMWEB_STREAM_FRAME_INTERVAL_MS) {
      updateActivityLedIndicator();
      delay(2);
      yield();
      continue;
    }
    lastFrameMs = millis();

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      res = ESP_FAIL;
      break;
    }

    res = httpd_resp_send_chunk(req, CAMWEB_STREAM_BOUNDARY, strlen(CAMWEB_STREAM_BOUNDARY));
    if (res == ESP_OK) {
      size_t hlen = snprintf(partBuf, sizeof(partBuf), "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", (unsigned int)fb->len);
      res = httpd_resp_send_chunk(req, partBuf, hlen);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
      wifiLedMarkBytes((uint32_t)fb->len);
      updateActivityLedIndicator();
    }
    esp_camera_fb_return(fb);

    if (res != ESP_OK) break;
    delay(1);
    yield();
  }
  return res;
}

void startCameraServer() {
  if (camWebControlHttpd || camWebStreamHttpd) {
    stopCameraServer();
    delay(80);
  }

  // V37: un solo servidor de camara en puerto 81.
  // La pagina principal se sirve desde el WebServer ya existente en puerto 80 (/cam).
  httpd_config_t streamConfig = HTTPD_DEFAULT_CONFIG();
  streamConfig.server_port = 81;
  streamConfig.ctrl_port = 32769;
  streamConfig.max_uri_handlers = 4;
  streamConfig.stack_size = 8192;
  streamConfig.recv_wait_timeout = 5;
  streamConfig.send_wait_timeout = 5;

  httpd_uri_t indexUri = {};
  indexUri.uri = "/";
  indexUri.method = HTTP_GET;
  indexUri.handler = camWebIndexHandler;
  indexUri.user_ctx = NULL;

  httpd_uri_t captureUri = {};
  captureUri.uri = "/capture";
  captureUri.method = HTTP_GET;
  captureUri.handler = camWebCaptureHandler;
  captureUri.user_ctx = NULL;

  httpd_uri_t streamUri = {};
  streamUri.uri = "/stream";
  streamUri.method = HTTP_GET;
  streamUri.handler = camWebStreamHandler;
  streamUri.user_ctx = NULL;

  if (httpd_start(&camWebStreamHttpd, &streamConfig) == ESP_OK) {
    httpd_register_uri_handler(camWebStreamHttpd, &indexUri);
    httpd_register_uri_handler(camWebStreamHttpd, &captureUri);
    httpd_register_uri_handler(camWebStreamHttpd, &streamUri);
  } else {
    camWebStreamHttpd = NULL;
    cameraWebStatusMsg = "Error iniciando puerto 81";
  }
}

void stopCameraServer() {
  if (camWebStreamHttpd) {
    httpd_stop(camWebStreamHttpd);
    camWebStreamHttpd = NULL;
  }
  if (camWebControlHttpd) {
    httpd_stop(camWebControlHttpd);
    camWebControlHttpd = NULL;
  }
}

// -------------------- Utilidades --------------------// -------------------- Utilidades --------------------
void drawBar(int x, int y, int w, int h, int percent, uint16_t color) {
  percent = constrain(percent, 0, 100);
  tft.drawRect(x, y, w, h, TFT_WHITE);
  int fillW = (w - 2) * percent / 100;
  tft.fillRect(x + 1, y + 1, fillW, h - 2, color);
  tft.fillRect(x + 1 + fillW, y + 1, (w - 2) - fillW, h - 2, TFT_BLACK);
}

String bytesToText(uint64_t bytes) {
  if (bytes >= 1024ULL * 1024ULL * 1024ULL) return String(bytes / (1024.0 * 1024.0 * 1024.0), 2) + " GB";
  if (bytes >= 1024ULL * 1024ULL) return String(bytes / (1024.0 * 1024.0), 1) + " MB";
  if (bytes >= 1024ULL) return String(bytes / 1024.0, 1) + " KB";
  return String((uint32_t)bytes) + " B";
}
