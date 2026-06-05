# Control ESP32-S3 V3

Proyecto preparado para compilar la APK en GitHub Actions y usarla con el sketch incluido para ESP32-S3 Freenove.

## Qué cambia en V3

- La app se llama **Control ESP32-S3**.
- Se mantiene la estética clara de la captura: fondo blanco, botones grises y botones de estado en rojo/verde.
- Botones de estado sincronizados por BLE:
  - **WiFi ON/OFF**
  - **Web ON/OFF**
  - **FTP ON/OFF**
  - **Cámara ON/OFF**
- Escaneo BLE y reconexión automática al último ESP32-S3 usado.
- Botón **Enviar hora**: manda la hora del móvil al ESP32 por BLE.
- Cámara desde la app:
  - `CAM_ON` activa WiFi, Web/OTA y el servidor de cámara del sketch.
  - La vista de la app carga el stream MJPEG desde `http://IP:81/stream`.
  - **Guardar foto en Descargas** descarga `http://IP:81/capture` y guarda un JPG en la carpeta Descargas del móvil.
- OTA desde la app:
  - Navegador interno para buscar archivos `.bin`.
  - Sube el `.bin` a `http://IP/update?key=esp32ota` usando el endpoint del sketch.
- Música:
  - Play/Pausa, Stop, anterior, siguiente, volumen.
  - Barra de progreso y barra de volumen sincronizadas con el estado BLE del sketch.
  - Comandos nuevos: `VOLUME_SET:n` y `SEEK_PERCENT:n`.

## Cómo compilar la APK en GitHub

1. Sube todo este ZIP descomprimido a un repositorio de GitHub.
2. Entra en la pestaña **Actions**.
3. Ejecuta **Build Android APK**.
4. Al terminar, descarga el artefacto **Control-ESP32-S3-V3-debug-apk**.
5. Dentro estará `app-debug.apk`.

## Firmware ESP32-S3

El sketch está en:

`firmware/Sketch_21_Rehecho_FULL_ArduinoDroid_V58C_BLE_Remoto_V2_App.ino`

Aunque conserva el nombre de archivo anterior para no romper tu flujo de trabajo, internamente se ha añadido compatibilidad V3/V60 para la app.

Ajustes recomendados de la placa:

- Board: **ESP32S3 Dev Module**
- Flash: **8 MB**
- PSRAM: **OPI PSRAM**
- Partition: una partición con OTA, por ejemplo **8M with spiffs (3MB APP/1.5MB SPIFFS)**
- CPU: **240 MHz**
- USB CDC: habilitado si lo usas en ArduinoDroid/Arduino IDE

## Importante para Android 11 o superior

El navegador interno de `.bin` intenta abrir Descargas directamente. En Android 11+ el sistema puede bloquear el acceso a archivos generales. Si no se ven los `.bin`, pulsa **Permiso archivos** en el navegador interno y concede acceso a todos los archivos para esta app.

## Protocolo BLE usado

Servicio BLE:

`b6a00001-7a9b-4d22-93c4-9bb2f2c9a001`

Característica comandos:

`b6a00002-7a9b-4d22-93c4-9bb2f2c9a002`

Característica estado/notificaciones:

`b6a00003-7a9b-4d22-93c4-9bb2f2c9a003`

Comandos principales:

`GET_STATUS`, `WIFI_ON`, `WIFI_OFF`, `WEB_ON`, `WEB_OFF`, `FTP_ON`, `FTP_OFF`, `CAM_ON`, `CAM_OFF`, `NET_OFF`, `TIME:epoch`, `MUSIC_PLAY_PAUSE`, `MUSIC_STOP`, `MUSIC_NEXT`, `MUSIC_PREV`, `VOL_UP`, `VOL_DOWN`, `VOLUME_SET:n`, `SEEK_PERCENT:n`, `FOLDERS`, `SELECT_FOLDER:n`.
