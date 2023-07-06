#include <WiFi.h>

#include <Firebase_ESP_Client.h>  // ESTO ES PARA EL FIREBASE

// ESTO ES PARA EL SENSOR EL FAMOSO BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// INCLUIR ESTO PARA FIREBASE - MAS INFO POR CHATGPT
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define PIN_SDA 21
#define PIN_SCL 22

// ESTO ES PARA EL INTERNET XD
#define WIFI_SSID "Red Local"
#define WIFI_PASSWORD "@TheCosmos"

// INSTETAR AQUI LA API KEY DEL PROYECTO EN FIREBASE
#define API_KEY "AIzaSyCnbMapw0VWfi1L5MCsDrav5ImZmeAP8rE"

// AUTENTICACION DE USUARIO REGISTRADOS EN EL PROYECTO DE FIREBASE
#define USER_EMAIL "sergiovelez749@gmail.com"
#define USER_PASSWORD ""

// URL DE LA BASE DE DATOS DE REALTIMEDATABSE DE FIREBASE
#define DATABASE_URL "https://esp-firebase-demo-cc274-default-rtdb.firebaseio.com/"

FirebaseData fbdo;      // Define Firebase objects
FirebaseAuth auth;      // Define un FirebaseAuthobjeto necesario para la autenticación
FirebaseConfig config;  // Define un FirebaseConfigobjeto requerido para los datos de configuración.

String uid;  // VARIABLE PARA GUARDAR EL UID DEL USUARIO

// VARIABLES PARA GUARDAR LAS RUTAS DE LA BASE DE DATOS
String databasePath;
String tempPath;
String humPath;
String presPath;

Adafruit_BME280 bme;  // CREAMOS UN OBJETO DEL BME280

float temperature;
float humidity;
float pressure;

// VARIABLES QUE ESTABLECEN EL TIEMPO DE RETRASO EN CADA ENVIO (3 MINUTOS)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 120000;

// INICIALIZACION DEL BME280
void initBME() {
  Wire.begin(PIN_SDA, PIN_SCL);
  if (!bme.begin(0x76)) {  // Inicializa el sensor BME280 y verificar si está correctamente conectado
    Serial.println("No se pudo encontrar un sensor BME280 válido, ¡verifique la conexión!!");
    delay(5000);
  }
}

// CONNECTA EL ESP A INTERNET
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println();
  Serial.println("----------------------------------------------------------------------");
  Serial.print("Conectando a Wi-Fi..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println("----------------------------------------------------------------------");
}

// AQUI ENVIAREMOS DATOS FLOTANTES A LA DB
void sendFloat(String path, float value) {
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)) {
    Serial.println("-----------------------------------------------------------------------------------------------------------------");
    Serial.print("Valor de escritura: ");
    Serial.print(value);
    Serial.print(" en la siguiente ruta:");
    Serial.println(path);
    Serial.println("APROBADO");
    Serial.println("RUTA: " + fbdo.dataPath());
    Serial.println("TIPO: " + fbdo.dataType());
    Serial.println("-----------------------------------------------------------------------------------------------------------------");
    Serial.println();
  } else {
    Serial.println();
    Serial.println("-------------------");
    Serial.println("FALLIDO");
    Serial.println("RAZÓN: " + fbdo.errorReason());
    Serial.println("-------------------");
    Serial.println();
    delay(2000);
  }
}

void setup() {
  Serial.begin(115200);

  initBME();
  initWiFi();

  // Asignar la clave api (obligatorio)
  config.api_key = API_KEY;

  // Asignar la URL de RTDB (obligatorio)
  config.database_url = DATABASE_URL;

  // Asignar las credenciales de inicio de sesión del usuario
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Asignar la función de devolución de llamada para la tarea de generación de tokens de ejecución prolongada */
  config.token_status_callback = tokenStatusCallback;  //ver complementos/TokenHelper.h
  // Asignar el reintento máximo de generación de tokens
  config.max_token_generation_retry = 5;

  // Inicia la biblioteca Firebase (autenticar)
  Firebase.begin(&config, &auth);

  // Obtener el UID del usuario puede tardar unos segundos
  Serial.println("Obtener UID de usuario");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }

  uid = auth.token.uid.c_str();  // Imprimir UID de usuario
  Serial.print("ID de usuario: ");
  Serial.println(uid);
  Serial.println("----------------------------------------------------------------------");
  Serial.println();

  databasePath = "/UsersData/" + uid;  // Actualizar la ruta de la base de datos

  // Actualizar la ruta de la base de datos para las lecturas del sensor
  tempPath = databasePath + "/TEMPERATURA";  // --> UsersData/<user_uid>/Temperatura
  humPath = databasePath + "/HUMEDAD";       // --> UsersData/<user_uid>/Humedad
  presPath = databasePath + "/PRESION";      // --> UsersData/<user_uid>/Presión atmosférica
}

void loop() {
  // VERIFIQUA SI ES HORA DE ENVIAR NUEVAS LECTURAS
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // OBTIENE LAS NUEVAS LECTURAS DEL SENSOR
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure() / 100.0F;

    // ENVIA LAS LECTURAS A LA DATABASE
    sendFloat(tempPath, temperature);
    sendFloat(humPath, humidity);
    sendFloat(presPath, pressure);

    Serial.println("-------------------------");
    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" °C");
    Serial.print("Humedad: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Presión: ");
    Serial.print(pressure);
    Serial.println(" hPa");
    Serial.println("-------------------------");
    delay(120000);  // Espera 2 segundos entre cada lectura
  }
  Serial.println();
}