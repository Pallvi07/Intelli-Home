#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ESP_Mail_Client.h>


#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

#define AUTHOR_EMAIL "tarunpal4794.be21@chitkara.edu.in"
#define AUTHOR_PASSWORD "mwnzruoyftpascdj"

#define RECIPIENT_EMAIL "tarunpal4794.be21@chitkara.edu.in"

SMTPSession smtp;
void smtpCallback(SMTP_Status status);



LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DHT_SENSOR 2
int motor1Pin1 = 27;
int motor1Pin2 = 26;
int enable1Pin = 14;

int motor2Pin1 = 18;
int motor2Pin2 = 19;
int enable2Pin = 5;

#define GAS 33
#define BUZZER 15

const int freq = 30000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int resolution = 8;



DHT dht(DHT_SENSOR, DHT22);

#define SSID "ROBOT"
#define PASS "adminrobot1"
#define ID 12

#define SERVER "http://computersshiksha.info/db.php/"
HTTPClient http;
WiFiClient wifiClient;

int Val;

void setup() {

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  ledcSetup(pwmChannel1, freq, resolution);
  ledcSetup(pwmChannel2, freq, resolution);
  ledcAttachPin(enable1Pin, pwmChannel1);
  ledcAttachPin(enable2Pin, pwmChannel2);

  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);

  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);

  Serial.begin(9600);

  digitalWrite(BUZZER, LOW);

  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("ARDUINO BASED");
  lcd.setCursor(0, 1);
  lcd.print("INTELLI HOME");
  delay(1000);
  // initWiFi();
  delay(1500);
  lcd.clear();
}

float temp_val, FAN;
int smoke,cfan,clamp;
void loop() {
  if (Val > 255) Val = 0;
  temp_val = dht.readTemperature();
  if (temp_val > 100) temp_val = 99;
  smoke = analogRead(GAS);
  smoke = map(smoke, 3000, 4095, 0, 99);
  FAN = map(temp_val, 32, 60, 0, 255);
  if (FAN > 255) FAN = 255;
  if (smoke > 100) smoke = 99;
  if (smoke < 0) smoke = 0;

  if (smoke > 50) {
    lcd.setCursor(0, 1);
    lcd.print("SMOKE DETECTED");
    beep(20, 100);
    send_to_server(cfan,0,clamp);
    SendEmail();

  } else {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    send_to_server(cfan,1,clamp);

  }

  Serial.print(temp_val);
  Serial.print(" ");
  Serial.print(FAN);
  Serial.print(" ");
  Serial.println(smoke);

  // ledcWrite(pwmChannel1, FAN);
  // ledcWrite(pwmChannel2, 0);
  get_from_server();

  lcd.setCursor(0, 0);
  lcd.print("S(%):");
  display3val((int)smoke);
  lcd.print(" T(C):");
  display3val((int)temp_val);
  delay(500);
  Val = Val + 10;
}


void beep(int times, int Dly) {
  while (times--) {
    digitalWrite(BUZZER, HIGH);
    delay(Dly);
    digitalWrite(BUZZER, LOW);
    delay(Dly);
  }
}


void send_to_server(int val1,int val2,int val3) {
  if (WiFi.status() == WL_CONNECTED) {
    String url = String(SERVER) + "input_values?fan_control=" + String(val1) + "&gas_control=" + String(val2) +"&light_control=" + String(val3) + "&submit=submit";
    http.begin(wifiClient, url);
    int code = http.GET();
    if (code == 200) {
      String payload = http.getString();
      // Serial.println(payload);
      http.end();
    } else {
      // Serial.println(code);
    }
    http.end();
  } else {
    initWiFi();
  }
}


void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  unsigned long s_time = millis();
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    if (millis() - s_time > 5000)
      break;
    delay(500);
  }
  Serial.print("WiFi Connected ");
  Serial.println(WiFi.localIP());
}

void get_from_server() {
  if (WiFi.status() == WL_CONNECTED) {
    String url = "http://computersshiksha.info/Get_values.php";
    Serial.println(url);
    http.begin(wifiClient, url);
    int code = http.GET();
    if (code == 200) {
      String payload = http.getString();
      JSONVar json_object = JSON.parse(payload);
      if (JSON.typeof(json_object) == "undefined") {
        Serial.println("Parsing input failed!");
      }
      // Serial.println(json_object);

      char marray[5];

      strcpy(marray, json_object["light_control"]);
      clamp = stoi(marray);
      if(clamp==1){
        clamp=128;
      }
      ledcWrite(pwmChannel2, clamp);
      Serial.print("Light Control ");
      Serial.println(clamp);
      strcpy(marray, json_object["fan_control"]);
      cfan = stoi(marray);
      Serial.print("FAN Control ");
      Serial.println(cfan);
      if (cfan == 1) {

        temp_val = dht.readTemperature();
        if (temp_val > 100) temp_val = 99;
        FAN = map(temp_val, 32, 60, 0, 255);
        ledcWrite(pwmChannel1, FAN);

      } else {

        ledcWrite(pwmChannel1, 0);
      }


      http.end();
    } else {
      // Serial.println(code);
    }
    http.end();
  } else {
    initWiFi();
  }
}


int stoi(char *string) {
  int num = 0;
  for (int i = 0; string[i] != '\0'; i++) {
    num = num * 10 + (string[i] - 48);
  }
  return num;
}

void display3val(int d) {
  unsigned int temparray[2], temp;
  for (temp = 2; temp > 0; temp--) {
    temparray[temp - 1] = d % 10;
    d = d / 10;
  }

  for (temp = 0; temp < 2; temp++) {
    lcd.write(temparray[temp] + 48);
  }
}


/* Callback function to get the Email sending status */
void smtpCallback(SMTP_Status status) {
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()) {
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++) {
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}


void SendEmail()
{
  smtp.debug(1);
  smtp.callback(smtpCallback);
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  SMTP_Message message;
  message.sender.name = "Intelli Home";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "SMOKE Detected";
  message.addRecipient("My Home", RECIPIENT_EMAIL);

  //  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Hello World!</h1><p>- Sent from ESP board</p></div>";
  //  message.html.content = htmlMsg.c_str();
  //  message.html.content = htmlMsg.c_str();
  //  message.text.charSet = "us-ascii";
  //  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  String textMsg = "Attention!!!!!!! gas detected ";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  //  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_low;
  //  message.response.notify = esp_mail_smtp_notify_success | esp_mail_smtp_notify_failure | esp_mail_smtp_notify_delay;*/

  /* Set the custom message header */
  //message.addHeader("Message-ID: <abcde.fghij@gmail.com>");

  /* Connect to server with the session config */
  if (!smtp.connect(&session))
    return;

  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}
