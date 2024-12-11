#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "network_credentials.h"

//Wifi - Connection variables
const char* serverUrl = "http://<Flask_Server_IP>:5000/handle_morse";
const int max_attempts = 10;
//Pins
const int pinLED = 32;
const int pinMorse = 35;
const int pinAccept = 34;
const int pinErase = 33;
//Morse variables
int morseButtonState = 0;
unsigned long startTime = 0;
unsigned long endTime=0;
bool morseButtonPress = false;
String morseBuffer = "";
const int dotThreshold = 500;
const int letterThreshold = 1000;
//Accept/Erase buttons
unsigned long lastAcceptPress = 0;
unsigned long lastErasePress = 0;
//Debounce solution constante
const unsigned long debounceDelay = 500;
//HTTP client
HTTPClient http;

//Morse alphabet dictionary
struct MorseCode{
    char letter;
    const char* code;
};
MorseCode morseAlphabet[] = {
  {'A', ".-"}, {'B', "-..."}, {'C', "-.-."}, {'D', "-.."}, {'E', "."},
  {'F', "..-."}, {'G', "--."}, {'H', "...."}, {'I', ".."}, {'J', ".---"},
  {'K', "-.-"}, {'L', ".-.."}, {'M', "--"}, {'N', "-."}, {'O', "---"},
  {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."}, {'S', "..."}, {'T', "-"},
  {'U', "..-"}, {'V', "...-"}, {'W', ".--"}, {'X', "-..-"}, {'Y', "-.--"},
  {'Z', "--.."}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"}, 
  {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."}, 
  {'8', "---.."}, {'9', "----."}, {'0', "-----"},{'?',"..--.."},
  {'!',"-.-.--"},{'.',".-.-.-"},{',',"--..--"},{';',"-.-.-."},{':',"---..."},
  {'+',".-.-."},{'-',"-....-"},{'/',"-..-."},{'=',"-...-"},{' ', "......."},
};
int numLetters = sizeof(morseAlphabet) / sizeof(morseAlphabet[0]);

//Function to retrieve letter
char getLetterFromMorse(const String& morseCode){
    for (int i = 0; i < numLetters; i ++){
        if (morseCode.equals(morseAlphabet[i].code))
        {
            return morseAlphabet[i].letter;
        }
    }
    return '?';
}

void setup(){
    pinMode(pinLED, OUTPUT);
    pinMode(pinMorse, INPUT);
    pinMode(pinAccept, INPUT);
    pinMode(pinErase, INPUT);

    Serial.begin(115200); //For monitoring purposes
    int attempt = 0;
    WiFi.begin(NETWORK_SSID, NETWORK_PASS);
    while (WiFi.status() != WL_CONNECTED && attempt < max_attempts){
        delay(1000);
        Serial.print("Attempt ");
        Serial.print(attempt + 1);
        attempt ++;
    }
    if (WiFi.status() == WL_CONNECTED){
        Serial.println("Connected to WiFi");
        digitalWrite(pinLED, HIGH);
        delay(100);
        digitalWrite(pinLED,LOW);
        delay(100);
        digitalWrite(pinLED, HIGH);
        delay(100);
        digitalWrite(pinLED,LOW);
    } else {
        Serial.println("Failed to connect to WiFi");
    }
}
void loop(){
    //Morse inputs
    HandleMorseInput();
    //Accept Logic
    if (digitalRead(pinAccept) == HIGH && (millis() - lastAcceptPress > debounceDelay)){
        lastAcceptPress = millis();
        SendActionToServer("Accept");
    }
    if (digitalRead(pinErase) == HIGH && (millis() - lastErasePress > debounceDelay)){
        lastAcceptPress = millis();
        SendActionToServer("Erase");
    }
}
void HandleMorseInput(){
    morseButtonState = digitalRead(pinMorse);
    //Morse button logic
    if (morseButtonState == HIGH && !morseButtonPress){
        startTime = millis();
        morseButtonPress = true;
        digitalWrite(pinLED,HIGH);
    }
    if (morseButtonState == LOW && morseButtonPress){
        unsigned long duration = millis() - startTime;
        morseButtonPress = false;
        digitalWrite(pinLED,LOW);
        endTime = millis();
        //Dot-Dash logic
        if (duration < dotThreshold && duration > 5){
            morseBuffer += ".";
        }else if (duration >= dotThreshold){
            morseBuffer += "-";
        }
    }
    //Letter logic
    if (!morseButtonPress && (millis() - endTime > letterThreshold) && morseBuffer.length() > 0){
        char letter = getLetterFromMorse(morseBuffer);
        Serial.println(letter); //Monitoring purposes
        SendLetterToServer(letter);
        //Cleaning buffer
        morseBuffer = "";
    }
}
void SendLetterToServer(char letter){
    if (WiFi.status() == WL_CONNECTED){
        String endPoint = String(serverUrl) + "/letter";
        http.begin(endPoint);
        http.addHeader("Content-Type", "application/json");
        String payload = "{\"letter\": \"" + String(letter) + "\"}";
        int httpResponseCode = http.POST(payload);
        if(httpResponseCode > 0){
            Serial.println("Sent succesfully");
            Serial.print("Server Response:");
            Serial.println(http.getString());
        }else{
            Serial.print("Failed to send");
            Serial.println(httpResponseCode);
        }
        http.end();
    }else{
        Serial.println("Wifi not connected");
    }
}
void SendActionToServer(const String& action){
    if (WiFi.status() == WL_CONNECTED){
        String endPoint = String(serverUrl) + "/action";
        http.begin(endPoint);
        http.addHeader("Content-Type", "application/json");
        String payload = "{\"action\": \"" + action + "\"}";
        int httpResponseCode = http.POST(payload);
        if(httpResponseCode > 0){
            Serial.println("Sent succesfully");
            Serial.print("Server Response:");
            Serial.println(http.getString());
        }else{
            Serial.print("Failed to send");
            Serial.println(httpResponseCode);
        }
        http.end();
    }else{
        Serial.println("Wifi not connected");
    }
}