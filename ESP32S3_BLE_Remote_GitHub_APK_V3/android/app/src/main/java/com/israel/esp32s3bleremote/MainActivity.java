package com.israel.esp32s3bleremote;

import android.Manifest;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.provider.MediaStore;
import android.provider.Settings;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.UUID;

public class MainActivity extends Activity {
    private static final UUID SERVICE_UUID = UUID.fromString("b6a00001-7a9b-4d22-93c4-9bb2f2c9a001");
    private static final UUID COMMAND_UUID = UUID.fromString("b6a00002-7a9b-4d22-93c4-9bb2f2c9a002");
    private static final UUID STATUS_UUID  = UUID.fromString("b6a00003-7a9b-4d22-93c4-9bb2f2c9a003");
    private static final UUID CCCD_UUID    = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

    private static final String DEFAULT_BLE_NAME = "ESP32S3-Freenove-BLE";
    private static final String DEFAULT_OTA_KEY = "esp32ota";

    private static final int COLOR_BG = Color.rgb(250, 250, 250);
    private static final int COLOR_NEUTRAL = Color.rgb(211, 214, 214);
    private static final int COLOR_GREEN = Color.rgb(42, 157, 74);
    private static final int COLOR_RED = Color.rgb(197, 53, 53);
    private static final int COLOR_TEXT = Color.rgb(40, 40, 40);
    private static final int COLOR_TITLE = Color.rgb(130, 130, 130);

    private final Handler handler = new Handler(Looper.getMainLooper());
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner scanner;
    private BluetoothGatt bluetoothGatt;
    private BluetoothGattCharacteristic commandCharacteristic;
    private BluetoothGattCharacteristic statusCharacteristic;

    private SharedPreferences prefs;
    private final Map<String, BluetoothDevice> foundDevices = new LinkedHashMap<>();
    private BluetoothDevice lastDevice;
    private String lastDeviceAddress = "";
    private boolean scanning = false;
    private boolean autoConnectArmed = true;

    private LinearLayout root;
    private LinearLayout deviceContainer;
    private LinearLayout folderContainer;
    private TextView statusView;
    private TextView smallLogView;
    private TextView selectedBinView;
    private TextView musicStatusView;
    private EditText nameFilterEdit;
    private EditText otaKeyEdit;
    private Button scanButton;
    private Button reconnectButton;
    private Button wifiButton;
    private Button webButton;
    private Button ftpButton;
    private Button cameraStartButton;
    private Button cameraStopButton;
    private Button uploadButton;
    private WebView cameraWebView;
    private SeekBar songSeek;
    private SeekBar volumeSeek;
    private ProgressBar otaProgress;

    private boolean wifiOn = false;
    private boolean webOn = false;
    private boolean ftpOn = false;
    private boolean cameraOn = false;
    private String currentIp = "";
    private String musicState = "stop";
    private String currentTrack = "";
    private int songCur = 0;
    private int songDur = 0;
    private int volumeLevel = 0;
    private boolean updatingSeekBars = false;
    private File selectedBinFile = null;

    private final Runnable statusPoll = new Runnable() {
        @Override
        public void run() {
            if (bluetoothGatt != null && commandCharacteristic != null) {
                sendCommandSilent("GET_STATUS");
            }
            handler.postDelayed(this, 2500);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        prefs = getSharedPreferences("control_esp32s3_v3", MODE_PRIVATE);
        lastDeviceAddress = prefs.getString("last_ble_address", "");
        currentIp = prefs.getString("last_ip", "");
        BluetoothManager manager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = manager != null ? manager.getAdapter() : null;
        scanner = bluetoothAdapter != null ? bluetoothAdapter.getBluetoothLeScanner() : null;
        buildUi();
        requestNeededPermissions();
        handler.postDelayed(() -> startBleScan(true), 700);
        handler.postDelayed(statusPoll, 2500);
    }

    private void buildUi() {
        ScrollView scroll = new ScrollView(this);
        scroll.setFillViewport(false);
        scroll.setBackgroundColor(COLOR_BG);
        root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setPadding(dp(12), dp(6), dp(12), dp(18));
        root.setBackgroundColor(COLOR_BG);
        scroll.addView(root, new ScrollView.LayoutParams(
                ScrollView.LayoutParams.MATCH_PARENT,
                ScrollView.LayoutParams.WRAP_CONTENT));

        TextView title = title("BLE");
        root.addView(title, fullWidth());

        LinearLayout scanRow = row();
        scanButton = neutralButton("Escanear BLE");
        scanButton.setOnClickListener(v -> startBleScan(false));
        scanRow.addView(scanButton, weight());
        reconnectButton = neutralButton("Reconectar");
        reconnectButton.setOnClickListener(v -> reconnectLast());
        scanRow.addView(reconnectButton, weight());
        root.addView(scanRow, fullWidth());

        statusView = smallText("Estado BLE: sin conectar" + (currentIp.length() > 0 ? " | IP guardada: " + currentIp : ""));
        root.addView(statusView, fullWidth());

        nameFilterEdit = new EditText(this);
        nameFilterEdit.setSingleLine(true);
        nameFilterEdit.setText(DEFAULT_BLE_NAME);
        nameFilterEdit.setHint("Nombre BLE de la placa");
        nameFilterEdit.setTextColor(COLOR_TEXT);
        nameFilterEdit.setTextSize(14);
        nameFilterEdit.setVisibility(View.GONE);
        root.addView(nameFilterEdit, fullWidth());

        deviceContainer = new LinearLayout(this);
        deviceContainer.setOrientation(LinearLayout.VERTICAL);
        root.addView(deviceContainer, fullWidth());

        root.addView(section("Control BLE"), fullWidth());
        LinearLayout control1 = row();
        Button statusButton = neutralButton("Ver IP/estado");
        statusButton.setOnClickListener(v -> sendCommand("GET_STATUS"));
        control1.addView(statusButton, weight());
        wifiButton = stateButton("WiFi OFF", false);
        wifiButton.setOnClickListener(v -> sendCommand(wifiOn ? "WIFI_OFF" : "WIFI_ON"));
        control1.addView(wifiButton, weight());
        root.addView(control1, fullWidth());

        LinearLayout control2 = row();
        webButton = stateButton("Web OFF", false);
        webButton.setOnClickListener(v -> sendCommand(webOn ? "WEB_OFF" : "WEB_ON"));
        control2.addView(webButton, weight());
        ftpButton = stateButton("FTP OFF", false);
        ftpButton.setOnClickListener(v -> sendCommand(ftpOn ? "FTP_OFF" : "FTP_ON"));
        control2.addView(ftpButton, weight());
        root.addView(control2, fullWidth());

        LinearLayout control3 = row();
        Button netOffButton = neutralButton("Apagar red");
        netOffButton.setOnClickListener(v -> {
            stopCameraView();
            sendCommand("NET_OFF");
        });
        control3.addView(netOffButton, weight());
        Button timeButton = neutralButton("Enviar hora");
        timeButton.setOnClickListener(v -> sendCurrentTime());
        control3.addView(timeButton, weight());
        root.addView(control3, fullWidth());

        root.addView(section("Cámara"), fullWidth());
        FrameLayout camFrame = new FrameLayout(this);
        camFrame.setBackgroundColor(Color.BLACK);
        cameraWebView = new WebView(this);
        cameraWebView.setBackgroundColor(Color.BLACK);
        WebSettings ws = cameraWebView.getSettings();
        ws.setJavaScriptEnabled(false);
        ws.setLoadWithOverviewMode(true);
        ws.setUseWideViewPort(true);
        ws.setBuiltInZoomControls(true);
        ws.setDisplayZoomControls(true);
        if (Build.VERSION.SDK_INT >= 21) {
            ws.setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);
        }
        camFrame.addView(cameraWebView, new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT, dp(260)));
        root.addView(camFrame, fullWidth());
        stopCameraView();

        LinearLayout camButtons = row();
        cameraStartButton = stateButton("Activar cámara / ver stream", false);
        cameraStartButton.setOnClickListener(v -> startCameraFromApp());
        camButtons.addView(cameraStartButton, weight());
        cameraStopButton = stateButton("Parar cámara", false);
        cameraStopButton.setOnClickListener(v -> {
            stopCameraView();
            sendCommand("CAM_OFF");
        });
        camButtons.addView(cameraStopButton, weight());
        root.addView(camButtons, fullWidth());

        Button savePhoto = neutralButton("Guardar foto en Descargas");
        savePhoto.setOnClickListener(v -> saveCurrentPhotoToDownloads());
        root.addView(savePhoto, fullWidthWithMargins());

        root.addView(section("OTA .bin"), fullWidth());
        otaKeyEdit = new EditText(this);
        otaKeyEdit.setSingleLine(true);
        otaKeyEdit.setText(prefs.getString("ota_key", DEFAULT_OTA_KEY));
        otaKeyEdit.setHint("Clave OTA");
        otaKeyEdit.setTextSize(14);
        otaKeyEdit.setTextColor(COLOR_TEXT);
        root.addView(otaKeyEdit, fullWidth());

        LinearLayout otaRow = row();
        Button browseButton = neutralButton("Buscar .bin con navegador interno");
        browseButton.setOnClickListener(v -> showBinBrowser(defaultBrowseFolder()));
        otaRow.addView(browseButton, weight());
        uploadButton = neutralButton("Subir .bin por OTA");
        uploadButton.setOnClickListener(v -> uploadSelectedBin());
        otaRow.addView(uploadButton, weight());
        root.addView(otaRow, fullWidth());

        selectedBinView = smallText("Ningún .bin seleccionado");
        root.addView(selectedBinView, fullWidth());
        otaProgress = new ProgressBar(this, null, android.R.attr.progressBarStyleHorizontal);
        otaProgress.setMax(100);
        otaProgress.setProgress(0);
        otaProgress.setVisibility(View.GONE);
        root.addView(otaProgress, fullWidthWithMargins());

        root.addView(section("Música"), fullWidth());
        LinearLayout musicRow1 = row();
        musicRow1.addView(commandNeutralButton("Anterior", "MUSIC_PREV"), weight());
        musicRow1.addView(commandNeutralButton("Play/Pausa", "MUSIC_PLAY_PAUSE"), weight());
        musicRow1.addView(commandNeutralButton("Siguiente", "MUSIC_NEXT"), weight());
        root.addView(musicRow1, fullWidth());

        LinearLayout musicRow2 = row();
        musicRow2.addView(commandNeutralButton("Vol -", "VOL_DOWN"), weight());
        musicRow2.addView(commandNeutralButton("Stop", "MUSIC_STOP"), weight());
        musicRow2.addView(commandNeutralButton("Vol +", "VOL_UP"), weight());
        root.addView(musicRow2, fullWidth());

        musicStatusView = smallText("Música: stop | 00:00 / 00:00");
        root.addView(musicStatusView, fullWidth());

        TextView songLabel = tinyLabel("Progreso de canción");
        root.addView(songLabel, fullWidth());
        songSeek = new SeekBar(this);
        songSeek.setMax(100);
        songSeek.setProgress(0);
        songSeek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) { }
            @Override public void onStartTrackingTouch(SeekBar seekBar) { }
            @Override public void onStopTrackingTouch(SeekBar seekBar) {
                if (!updatingSeekBars) sendCommand("SEEK_PERCENT:" + seekBar.getProgress());
            }
        });
        root.addView(songSeek, fullWidth());

        TextView volumeLabel = tinyLabel("Volumen de audio");
        root.addView(volumeLabel, fullWidth());
        volumeSeek = new SeekBar(this);
        volumeSeek.setMax(10);
        volumeSeek.setProgress(0);
        volumeSeek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) { }
            @Override public void onStartTrackingTouch(SeekBar seekBar) { }
            @Override public void onStopTrackingTouch(SeekBar seekBar) {
                if (!updatingSeekBars) sendCommand("VOLUME_SET:" + seekBar.getProgress());
            }
        });
        root.addView(volumeSeek, fullWidth());

        Button foldersButton = neutralButton("Listar carpetas de /music");
        foldersButton.setOnClickListener(v -> sendCommand("FOLDERS"));
        root.addView(foldersButton, fullWidthWithMargins());

        folderContainer = new LinearLayout(this);
        folderContainer.setOrientation(LinearLayout.VERTICAL);
        root.addView(folderContainer, fullWidth());

        smallLogView = smallText("Listo.");
        root.addView(smallLogView, fullWidth());

        updateStateButtons();
        updateMusicUi();
        setContentView(scroll);
    }

    private TextView title(String text) {
        TextView t = new TextView(this);
        t.setText(text);
        t.setTextSize(24);
        t.setTextColor(COLOR_TITLE);
        t.setPadding(0, 0, 0, dp(8));
        return t;
    }

    private TextView section(String text) {
        TextView t = new TextView(this);
        t.setText(text);
        t.setTextSize(24);
        t.setTextColor(COLOR_TITLE);
        t.setPadding(0, dp(22), 0, dp(8));
        return t;
    }

    private TextView smallText(String text) {
        TextView t = new TextView(this);
        t.setText(text);
        t.setTextSize(13);
        t.setTextColor(Color.rgb(70, 70, 70));
        t.setPadding(0, dp(3), 0, dp(3));
        return t;
    }

    private TextView tinyLabel(String text) {
        TextView t = smallText(text);
        t.setTextSize(14);
        t.setTextColor(Color.rgb(120, 120, 120));
        return t;
    }

    private LinearLayout row() {
        LinearLayout r = new LinearLayout(this);
        r.setOrientation(LinearLayout.HORIZONTAL);
        r.setGravity(Gravity.CENTER);
        r.setPadding(0, dp(2), 0, dp(2));
        return r;
    }

    private Button neutralButton(String text) {
        Button b = new Button(this);
        b.setAllCaps(false);
        b.setText(text);
        b.setTextSize(15);
        b.setTextColor(COLOR_TEXT);
        b.setGravity(Gravity.CENTER);
        b.setBackgroundColor(COLOR_NEUTRAL);
        b.setMinHeight(dp(44));
        return b;
    }

    private Button stateButton(String text, boolean on) {
        Button b = neutralButton(text);
        b.setTextColor(Color.WHITE);
        b.setBackgroundColor(on ? COLOR_GREEN : COLOR_RED);
        return b;
    }

    private Button commandNeutralButton(String text, String command) {
        Button b = neutralButton(text);
        b.setOnClickListener(v -> sendCommand(command));
        return b;
    }

    private LinearLayout.LayoutParams fullWidth() {
        return new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
    }

    private LinearLayout.LayoutParams fullWidthWithMargins() {
        LinearLayout.LayoutParams p = fullWidth();
        p.setMargins(dp(4), dp(5), dp(4), dp(5));
        return p;
    }

    private LinearLayout.LayoutParams weight() {
        LinearLayout.LayoutParams p = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.WRAP_CONTENT, 1f);
        p.setMargins(dp(4), dp(4), dp(4), dp(4));
        return p;
    }

    private int dp(int value) {
        return (int) (value * getResources().getDisplayMetrics().density + 0.5f);
    }

    private boolean hasBlePermissions() {
        if (Build.VERSION.SDK_INT >= 31) {
            return checkSelfPermission(Manifest.permission.BLUETOOTH_SCAN) == PackageManager.PERMISSION_GRANTED
                    && checkSelfPermission(Manifest.permission.BLUETOOTH_CONNECT) == PackageManager.PERMISSION_GRANTED;
        }
        return checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) == PackageManager.PERMISSION_GRANTED;
    }

    private void requestNeededPermissions() {
        List<String> perms = new ArrayList<>();
        if (Build.VERSION.SDK_INT >= 31) {
            if (checkSelfPermission(Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) perms.add(Manifest.permission.BLUETOOTH_SCAN);
            if (checkSelfPermission(Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) perms.add(Manifest.permission.BLUETOOTH_CONNECT);
        } else {
            if (checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) perms.add(Manifest.permission.ACCESS_FINE_LOCATION);
        }
        if (Build.VERSION.SDK_INT <= 28 && checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
            perms.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        }
        if (!perms.isEmpty()) requestPermissions(perms.toArray(new String[0]), 100);
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 100) handler.postDelayed(() -> startBleScan(true), 300);
    }

    @SuppressLint("MissingPermission")
    private void startBleScan(boolean autoConnect) {
        if (!hasBlePermissions()) {
            requestNeededPermissions();
            log("Permisos BLE pendientes");
            return;
        }
        if (bluetoothAdapter == null || !bluetoothAdapter.isEnabled()) {
            toast("Activa Bluetooth en Android");
            return;
        }
        scanner = bluetoothAdapter.getBluetoothLeScanner();
        if (scanner == null) {
            toast("No hay escáner BLE disponible");
            return;
        }
        if (scanning) return;
        autoConnectArmed = autoConnect;
        foundDevices.clear();
        deviceContainer.removeAllViews();
        setStatus("Escaneando BLE...");
        scanning = true;
        scanner.startScan(scanCallback);
        handler.postDelayed(() -> {
            if (scanning && scanner != null) {
                scanner.stopScan(scanCallback);
                scanning = false;
                setStatus(foundDevices.isEmpty() ? "Escaneo terminado: no encontrado" : "Escaneo terminado");
            }
        }, 10000);
    }

    private final ScanCallback scanCallback = new ScanCallback() {
        @SuppressLint("MissingPermission")
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            BluetoothDevice device = result.getDevice();
            if (device == null) return;
            String address = device.getAddress();
            if (address == null || foundDevices.containsKey(address)) return;
            String name = device.getName();
            if (name == null) name = "Sin nombre";
            String filter = nameFilterEdit.getText().toString().trim();
            boolean matchesName = filter.length() == 0 || name.toLowerCase(Locale.ROOT).contains(filter.toLowerCase(Locale.ROOT));
            boolean matchesLast = lastDeviceAddress.length() > 0 && lastDeviceAddress.equalsIgnoreCase(address);
            if (!matchesName && !matchesLast) return;
            foundDevices.put(address, device);
            addDeviceButton(device, name, result.getRssi());
            if (autoConnectArmed && (matchesLast || matchesName)) {
                autoConnectArmed = false;
                handler.postDelayed(() -> connectToDevice(device), 250);
            }
        }
    };

    @SuppressLint("MissingPermission")
    private void addDeviceButton(BluetoothDevice device, String name, int rssi) {
        runOnUiThread(() -> {
            Button b = neutralButton(name + "  " + device.getAddress() + "  " + rssi + " dBm");
            b.setTextSize(12);
            b.setOnClickListener(v -> connectToDevice(device));
            deviceContainer.addView(b, fullWidthWithMargins());
        });
    }

    @SuppressLint("MissingPermission")
    private void reconnectLast() {
        if (lastDevice != null) {
            connectToDevice(lastDevice);
            return;
        }
        if (lastDeviceAddress.length() > 0 && bluetoothAdapter != null) {
            try {
                connectToDevice(bluetoothAdapter.getRemoteDevice(lastDeviceAddress));
                return;
            } catch (IllegalArgumentException ignored) { }
        }
        startBleScan(true);
    }

    @SuppressLint("MissingPermission")
    private void connectToDevice(BluetoothDevice device) {
        if (!hasBlePermissions()) {
            requestNeededPermissions();
            return;
        }
        if (device == null) return;
        if (scanning && scanner != null) {
            scanner.stopScan(scanCallback);
            scanning = false;
        }
        if (bluetoothGatt != null) {
            bluetoothGatt.close();
            bluetoothGatt = null;
        }
        lastDevice = device;
        lastDeviceAddress = device.getAddress();
        prefs.edit().putString("last_ble_address", lastDeviceAddress).apply();
        setStatus("Conectando a " + lastDeviceAddress);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            bluetoothGatt = device.connectGatt(this, false, gattCallback, BluetoothDevice.TRANSPORT_LE);
        } else {
            bluetoothGatt = device.connectGatt(this, false, gattCallback);
        }
    }

    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @SuppressLint("MissingPermission")
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int statusCode, int newState) {
            if (newState == BluetoothGatt.STATE_CONNECTED) {
                setStatus("BLE conectado. Descubriendo servicio...");
                gatt.discoverServices();
            } else if (newState == BluetoothGatt.STATE_DISCONNECTED) {
                commandCharacteristic = null;
                statusCharacteristic = null;
                setStatus("BLE desconectado");
            }
        }

        @SuppressLint("MissingPermission")
        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int statusCode) {
            BluetoothGattService service = gatt.getService(SERVICE_UUID);
            if (service == null) {
                setStatus("Servicio BLE no encontrado. Revisa que el firmware V3 esté cargado.");
                return;
            }
            commandCharacteristic = service.getCharacteristic(COMMAND_UUID);
            statusCharacteristic = service.getCharacteristic(STATUS_UUID);
            if (commandCharacteristic == null || statusCharacteristic == null) {
                setStatus("Características BLE incompletas");
                return;
            }
            gatt.setCharacteristicNotification(statusCharacteristic, true);
            BluetoothGattDescriptor descriptor = statusCharacteristic.getDescriptor(CCCD_UUID);
            if (descriptor != null) {
                descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                gatt.writeDescriptor(descriptor);
            }
            setStatus("BLE listo");
            handler.postDelayed(() -> sendCommand("GET_STATUS"), 400);
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            handleNotification(characteristic.getValue());
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, byte[] value) {
            handleNotification(value);
        }
    };

    private void handleNotification(byte[] value) {
        if (value == null) return;
        String msg = new String(value, StandardCharsets.UTF_8).trim();
        if (msg.length() == 0) return;
        runOnUiThread(() -> parseEspMessage(msg));
    }

    private void parseEspMessage(String msg) {
        log("ESP32: " + msg);
        if (msg.startsWith("STATUS")) {
            parseStatus(msg);
            setStatus(formatStatusMessage(msg));
        } else if (msg.startsWith("WIFI|")) {
            parseWifiMessage(msg);
        } else if (msg.startsWith("WEB|")) {
            webOn = msg.contains("|ON");
            maybeExtractIpFromBars(msg);
        } else if (msg.startsWith("FTP|")) {
            ftpOn = msg.contains("|ON");
        } else if (msg.startsWith("CAM|")) {
            cameraOn = msg.contains("|ON");
            maybeExtractIpFromBars(msg);
            if (cameraOn) loadCameraStream(); else stopCameraView();
        } else if (msg.startsWith("NET|OFF")) {
            wifiOn = false; webOn = false; ftpOn = false; cameraOn = false; stopCameraView();
        } else if (msg.startsWith("MUSIC|")) {
            musicState = msg;
        } else if (msg.startsWith("VOLUME|")) {
            volumeLevel = parseTrailingInt(msg, volumeLevel);
        } else if (msg.startsWith("FOLDERS_BEGIN")) {
            folderContainer.removeAllViews();
        } else if (msg.startsWith("FOLDER|")) {
            addFolderButtonFromMessage(msg);
        } else if (msg.startsWith("FOLDER_SELECTED")) {
            setStatus(msg.replace('|', ' '));
            sendCommandSilent("GET_STATUS");
        } else if (msg.startsWith("ERR|")) {
            toast(msg.substring(4));
        }
        updateStateButtons();
        updateMusicUi();
    }

    private void parseStatus(String msg) {
        String[] parts = msg.split("\\|");
        for (String part : parts) {
            if (part.startsWith("wifi=")) wifiOn = isOnValue(part.substring(5));
            else if (part.startsWith("ip=")) setCurrentIp(part.substring(3));
            else if (part.startsWith("web=")) webOn = isOnValue(part.substring(4));
            else if (part.startsWith("ftp=")) ftpOn = isOnValue(part.substring(4));
            else if (part.startsWith("cam=")) cameraOn = isOnValue(part.substring(4));
            else if (part.startsWith("music=")) musicState = part.substring(6);
            else if (part.startsWith("track=")) currentTrack = part.substring(6);
            else if (part.startsWith("cur=")) songCur = safeInt(part.substring(4), songCur);
            else if (part.startsWith("dur=")) songDur = safeInt(part.substring(4), songDur);
            else if (part.startsWith("vol=")) volumeLevel = safeInt(part.substring(4), volumeLevel);
        }
    }

    private boolean isOnValue(String v) {
        return "on".equalsIgnoreCase(v) || "1".equals(v) || "true".equalsIgnoreCase(v) || "play".equalsIgnoreCase(v);
    }

    private void parseWifiMessage(String msg) {
        wifiOn = msg.contains("|ON");
        maybeExtractIpFromBars(msg);
    }

    private void maybeExtractIpFromBars(String msg) {
        String[] parts = msg.split("\\|");
        for (String p : parts) {
            if (p.matches("\\d+\\.\\d+\\.\\d+\\.\\d+")) setCurrentIp(p);
            if (p.startsWith("ip=")) setCurrentIp(p.substring(3));
        }
    }

    private int parseTrailingInt(String msg, int def) {
        int bar = msg.lastIndexOf('|');
        if (bar >= 0 && bar + 1 < msg.length()) return safeInt(msg.substring(bar + 1), def);
        return def;
    }

    private int safeInt(String s, int def) {
        try { return Integer.parseInt(s.trim()); } catch (Exception e) { return def; }
    }

    private void setCurrentIp(String ip) {
        if (ip == null) return;
        ip = ip.trim();
        if (ip.length() == 0 || "0.0.0.0".equals(ip)) return;
        currentIp = ip;
        prefs.edit().putString("last_ip", currentIp).apply();
    }

    private String formatStatusMessage(String msg) {
        String ip = currentIp.length() == 0 ? "0.0.0.0" : currentIp;
        return "WiFi " + (wifiOn ? "ON" : "OFF") + " | IP " + ip + " | Web " + (webOn ? "ON" : "OFF")
                + " | FTP " + (ftpOn ? "ON" : "OFF") + " | Cámara " + (cameraOn ? "ON" : "OFF");
    }

    private void addFolderButtonFromMessage(String msg) {
        String[] parts = msg.split("\\|", 3);
        if (parts.length < 3) return;
        Button b = neutralButton(parts[1] + " - " + parts[2]);
        b.setOnClickListener(v -> sendCommand("SELECT_FOLDER:" + parts[1]));
        folderContainer.addView(b, fullWidthWithMargins());
    }

    private void updateStateButtons() {
        updateStateButton(wifiButton, wifiOn ? "WiFi ON" : "WiFi OFF", wifiOn);
        updateStateButton(webButton, webOn ? "Web ON" : "Web OFF", webOn);
        updateStateButton(ftpButton, ftpOn ? "FTP ON" : "FTP OFF", ftpOn);
        updateStateButton(cameraStartButton, cameraOn ? "Cámara ON / ver stream" : "Activar cámara / ver stream", cameraOn);
        updateStateButton(cameraStopButton, "Parar cámara", false);
    }

    private void updateStateButton(Button b, String text, boolean on) {
        if (b == null) return;
        b.setText(text);
        b.setTextColor(Color.WHITE);
        b.setBackgroundColor(on ? COLOR_GREEN : COLOR_RED);
    }

    private void updateMusicUi() {
        if (musicStatusView == null) return;
        String time = secToTime(songCur) + " / " + secToTime(songDur);
        String track = currentTrack == null || currentTrack.length() == 0 ? "" : " | " + currentTrack;
        musicStatusView.setText("Música: " + musicState + " | " + time + track);
        updatingSeekBars = true;
        if (songSeek != null) {
            int pct = songDur > 0 ? Math.max(0, Math.min(100, (songCur * 100) / songDur)) : 0;
            songSeek.setProgress(pct);
        }
        if (volumeSeek != null) volumeSeek.setProgress(Math.max(0, Math.min(10, volumeLevel)));
        updatingSeekBars = false;
    }

    private String secToTime(int seconds) {
        if (seconds < 0) seconds = 0;
        return String.format(Locale.getDefault(), "%02d:%02d", seconds / 60, seconds % 60);
    }

    @SuppressLint("MissingPermission")
    private void sendCommand(String command) {
        if (sendCommandInternal(command, true)) log("APP → " + command);
    }

    @SuppressLint("MissingPermission")
    private void sendCommandSilent(String command) {
        sendCommandInternal(command, false);
    }

    @SuppressLint("MissingPermission")
    private boolean sendCommandInternal(String command, boolean warn) {
        if (bluetoothGatt == null || commandCharacteristic == null) {
            if (warn) toast("Primero conecta por BLE");
            return false;
        }
        byte[] data = command.getBytes(StandardCharsets.UTF_8);
        boolean ok;
        if (Build.VERSION.SDK_INT >= 33) {
            int result = bluetoothGatt.writeCharacteristic(commandCharacteristic, data, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
            ok = result == BluetoothGatt.GATT_SUCCESS;
        } else {
            commandCharacteristic.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT);
            commandCharacteristic.setValue(data);
            ok = bluetoothGatt.writeCharacteristic(commandCharacteristic);
        }
        if (!ok && warn) toast("No se pudo enviar: " + command);
        return ok;
    }

    private void sendCurrentTime() {
        long epoch = System.currentTimeMillis() / 1000L;
        SimpleDateFormat fmt = new SimpleDateFormat("dd/MM/yyyy HH:mm:ss", Locale.getDefault());
        log("Enviando hora del móvil: " + fmt.format(System.currentTimeMillis()));
        sendCommand("TIME:" + epoch);
    }

    private void startCameraFromApp() {
        if (cameraOn && currentIp.length() > 0) {
            loadCameraStream();
            return;
        }
        stopCameraView();
        sendCommand("CAM_ON");
        setStatus("Activando cámara web en la placa...");
        handler.postDelayed(() -> sendCommandSilent("GET_STATUS"), 1600);
        handler.postDelayed(() -> { if (cameraOn) loadCameraStream(); }, 2600);
    }

    private void loadCameraStream() {
        if (currentIp.length() == 0) {
            stopCameraViewWithText("Sin IP. Pulsa Ver IP/estado o activa WiFi.");
            return;
        }
        String stream = "http://" + currentIp + ":81/stream?t=" + System.currentTimeMillis();
        String html = "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>" +
                "</head><body style='margin:0;background:#000;color:#fff;display:flex;align-items:center;justify-content:center;height:100vh;'>" +
                "<img src='" + stream + "' style='width:100%;height:auto;max-height:100%;object-fit:contain;background:#000;'>" +
                "</body></html>";
        cameraWebView.loadDataWithBaseURL("http://" + currentIp + "/", html, "text/html", "UTF-8", null);
    }

    private void stopCameraView() {
        stopCameraViewWithText("Cámara parada");
    }

    private void stopCameraViewWithText(String text) {
        String html = "<!doctype html><html><body style='margin:0;background:#000;color:#fff;font-family:sans-serif;'>" +
                "<div style='padding:26px;font-size:22px;position:absolute;top:38%;'>" + text + "</div>" +
                "</body></html>";
        cameraWebView.loadDataWithBaseURL(null, html, "text/html", "UTF-8", null);
    }

    private void saveCurrentPhotoToDownloads() {
        if (currentIp.length() == 0) {
            toast("No tengo IP de la placa. Pulsa Ver IP/estado.");
            return;
        }
        if (!cameraOn) sendCommand("CAM_ON");
        setStatus("Guardando foto en Descargas...");
        new Thread(() -> {
            try {
                Thread.sleep(cameraOn ? 300 : 1800);
                byte[] jpg = httpGetBytes("http://" + currentIp + ":81/capture");
                String name = "ESP32S3_" + new SimpleDateFormat("yyyyMMdd_HHmmss", Locale.US).format(System.currentTimeMillis()) + ".jpg";
                saveJpegToDownloads(name, jpg);
                runOnUiThread(() -> {
                    setStatus("Foto guardada en Descargas: " + name);
                    toast("Foto guardada en Descargas");
                });
            } catch (Exception e) {
                runOnUiThread(() -> {
                    setStatus("Error guardando foto: " + e.getMessage());
                    toast("No se pudo guardar la foto");
                });
            }
        }).start();
    }

    private byte[] httpGetBytes(String urlText) throws IOException {
        HttpURLConnection conn = (HttpURLConnection) new URL(urlText).openConnection();
        conn.setConnectTimeout(8000);
        conn.setReadTimeout(10000);
        conn.setRequestMethod("GET");
        int code = conn.getResponseCode();
        InputStream in = code >= 200 && code < 300 ? conn.getInputStream() : conn.getErrorStream();
        if (in == null) throw new IOException("HTTP " + code);
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        byte[] buffer = new byte[8192];
        int n;
        while ((n = in.read(buffer)) >= 0) out.write(buffer, 0, n);
        in.close();
        conn.disconnect();
        if (code < 200 || code >= 300) throw new IOException("HTTP " + code);
        return out.toByteArray();
    }

    private void saveJpegToDownloads(String name, byte[] data) throws IOException {
        if (Build.VERSION.SDK_INT >= 29) {
            ContentValues values = new ContentValues();
            values.put(MediaStore.MediaColumns.DISPLAY_NAME, name);
            values.put(MediaStore.MediaColumns.MIME_TYPE, "image/jpeg");
            values.put(MediaStore.MediaColumns.RELATIVE_PATH, Environment.DIRECTORY_DOWNLOADS);
            Uri uri = getContentResolver().insert(MediaStore.Downloads.EXTERNAL_CONTENT_URI, values);
            if (uri == null) throw new IOException("MediaStore no creó el archivo");
            try (OutputStream out = getContentResolver().openOutputStream(uri)) {
                if (out == null) throw new IOException("No se pudo abrir Descargas");
                out.write(data);
            }
        } else {
            File dir = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
            if (!dir.exists() && !dir.mkdirs()) throw new IOException("No se pudo crear Descargas");
            File f = new File(dir, name);
            try (OutputStream out = new FileOutputStream(f)) { out.write(data); }
        }
    }

    private File defaultBrowseFolder() {
        File downloads = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS);
        if (downloads != null && downloads.exists() && downloads.canRead()) return downloads;
        File external = Environment.getExternalStorageDirectory();
        if (external != null && external.exists() && external.canRead()) return external;
        File app = getExternalFilesDir(null);
        return app != null ? app : getFilesDir();
    }

    private void showBinBrowser(File dir) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R && !Environment.isExternalStorageManager()) {
            log("Para que el navegador interno vea Descargas en Android 11+, concede acceso a todos los archivos si Android lo pide.");
        }
        if (dir == null) dir = defaultBrowseFolder();
        final File current = dir;
        File[] listed = current.listFiles();
        if (listed == null) {
            askAllFilesAccess();
            toast("No puedo leer esta carpeta");
            return;
        }
        List<File> dirs = new ArrayList<>();
        List<File> bins = new ArrayList<>();
        for (File f : listed) {
            if (f.isDirectory() && !f.isHidden()) dirs.add(f);
            else if (f.isFile() && f.getName().toLowerCase(Locale.ROOT).endsWith(".bin")) bins.add(f);
        }
        Comparator<File> byName = (a, b) -> a.getName().compareToIgnoreCase(b.getName());
        Collections.sort(dirs, byName);
        Collections.sort(bins, byName);
        List<String> labels = new ArrayList<>();
        List<File> targets = new ArrayList<>();
        File parent = current.getParentFile();
        if (parent != null && parent.canRead()) {
            labels.add("..  Subir a carpeta anterior");
            targets.add(parent);
        }
        for (File d : dirs) {
            labels.add("📁 " + d.getName());
            targets.add(d);
        }
        for (File f : bins) {
            labels.add("📦 " + f.getName() + "  (" + bytesToText(f.length()) + ")");
            targets.add(f);
        }
        if (labels.isEmpty()) {
            labels.add("No hay carpetas ni archivos .bin aquí");
            targets.add(current);
        }
        new AlertDialog.Builder(this)
                .setTitle(current.getAbsolutePath())
                .setItems(labels.toArray(new String[0]), (dialog, which) -> {
                    File chosen = targets.get(which);
                    if (chosen.isDirectory()) showBinBrowser(chosen);
                    else if (chosen.isFile()) selectBinFile(chosen);
                })
                .setNegativeButton("Cancelar", null)
                .setNeutralButton("Permiso archivos", (d, w) -> askAllFilesAccess())
                .show();
    }

    private void askAllFilesAccess() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            try {
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                intent.setData(Uri.parse("package:" + getPackageName()));
                startActivity(intent);
            } catch (Exception e) {
                startActivity(new Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION));
            }
        }
    }

    private void selectBinFile(File f) {
        selectedBinFile = f;
        selectedBinView.setText(f.getAbsolutePath());
        prefs.edit().putString("last_bin_dir", f.getParent()).apply();
    }

    private void uploadSelectedBin() {
        if (selectedBinFile == null || !selectedBinFile.isFile()) {
            toast("Selecciona primero un archivo .bin");
            return;
        }
        String key = otaKeyEdit.getText().toString().trim();
        if (key.length() == 0) key = DEFAULT_OTA_KEY;
        prefs.edit().putString("ota_key", key).apply();
        uploadButton.setEnabled(false);
        otaProgress.setProgress(0);
        otaProgress.setVisibility(View.VISIBLE);
        setStatus("Preparando Web/OTA...");
        final String otaKey = key;
        new Thread(() -> {
            try {
                runOnUiThread(() -> sendCommandSilent("WEB_ON"));
                waitForWebReady();
                if (currentIp.length() == 0) throw new IOException("No tengo IP de la placa");
                String result = postBinOta("http://" + currentIp + "/update?key=" + otaKey, selectedBinFile);
                runOnUiThread(() -> {
                    setStatus("OTA enviada. Espera el reinicio de la placa.");
                    log("Respuesta OTA: " + result);
                    toast("OTA enviada");
                });
            } catch (Exception e) {
                runOnUiThread(() -> {
                    setStatus("Error OTA: " + e.getMessage());
                    toast("Error OTA: " + e.getMessage());
                });
            } finally {
                runOnUiThread(() -> {
                    uploadButton.setEnabled(true);
                    otaProgress.setVisibility(View.GONE);
                });
            }
        }).start();
    }

    private void waitForWebReady() throws InterruptedException, IOException {
        for (int i = 0; i < 12; i++) {
            if (currentIp.length() > 0 && webOn) return;
            runOnUiThread(() -> sendCommandSilent("GET_STATUS"));
            Thread.sleep(1000);
        }
        if (currentIp.length() > 0) return;
        throw new IOException("Activa WiFi/Web y pulsa Ver IP/estado");
    }

    private String postBinOta(String urlText, File file) throws IOException {
        String boundary = "----ControlESP32S3" + System.currentTimeMillis();
        HttpURLConnection conn = (HttpURLConnection) new URL(urlText).openConnection();
        conn.setConnectTimeout(10000);
        conn.setReadTimeout(60000);
        conn.setDoOutput(true);
        conn.setRequestMethod("POST");
        conn.setRequestProperty("Content-Type", "multipart/form-data; boundary=" + boundary);
        conn.setChunkedStreamingMode(64 * 1024);
        long total = Math.max(1L, file.length());
        try (OutputStream raw = new BufferedOutputStream(conn.getOutputStream());
             FileInputStream fis = new FileInputStream(file);
             BufferedInputStream in = new BufferedInputStream(fis)) {
            writeAscii(raw, "--" + boundary + "\r\n");
            writeAscii(raw, "Content-Disposition: form-data; name=\"firmware\"; filename=\"" + file.getName() + "\"\r\n");
            writeAscii(raw, "Content-Type: application/octet-stream\r\n\r\n");
            byte[] buffer = new byte[64 * 1024];
            long sent = 0;
            int n;
            while ((n = in.read(buffer)) >= 0) {
                raw.write(buffer, 0, n);
                sent += n;
                int pct = (int) Math.min(100, (sent * 100) / total);
                runOnUiThread(() -> otaProgress.setProgress(pct));
            }
            writeAscii(raw, "\r\n--" + boundary + "--\r\n");
        }
        int code = conn.getResponseCode();
        InputStream response = code >= 200 && code < 300 ? conn.getInputStream() : conn.getErrorStream();
        String body = response == null ? "" : readText(response);
        conn.disconnect();
        if (code < 200 || code >= 300) throw new IOException("HTTP " + code + " " + body);
        return body.length() > 180 ? body.substring(0, 180) : body;
    }

    private void writeAscii(OutputStream out, String s) throws IOException {
        out.write(s.getBytes(StandardCharsets.UTF_8));
    }

    private String readText(InputStream in) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        byte[] buffer = new byte[4096];
        int n;
        while ((n = in.read(buffer)) >= 0) out.write(buffer, 0, n);
        in.close();
        return new String(out.toByteArray(), StandardCharsets.UTF_8);
    }

    private String bytesToText(long bytes) {
        if (bytes < 1024) return bytes + " B";
        double kb = bytes / 1024.0;
        if (kb < 1024) return String.format(Locale.US, "%.1f KB", kb);
        double mb = kb / 1024.0;
        if (mb < 1024) return String.format(Locale.US, "%.1f MB", mb);
        return String.format(Locale.US, "%.1f GB", mb / 1024.0);
    }

    private void setStatus(String text) {
        runOnUiThread(() -> statusView.setText("Estado BLE: " + text));
    }

    private void log(String text) {
        runOnUiThread(() -> {
            if (smallLogView != null) smallLogView.setText(text);
        });
    }

    private void toast(String text) {
        runOnUiThread(() -> Toast.makeText(this, text, Toast.LENGTH_SHORT).show());
    }

    @SuppressLint("MissingPermission")
    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        if (scanner != null && scanning) scanner.stopScan(scanCallback);
        if (bluetoothGatt != null) {
            bluetoothGatt.close();
            bluetoothGatt = null;
        }
        if (cameraWebView != null) cameraWebView.destroy();
    }
}
