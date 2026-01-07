#include <Arduino.h>
#include <controller/pump/PumpManager.h>
#include <server/ServerHandler.h>
#include <LittleFS.h>

const char* ssid     = "FRITZ!Box 7590 XF";
const char* password = "08826474974857768306";

const uint8_t relay_1_PIN = 33;
const uint8_t relay_2_PIN = 32;
const uint8_t relay_3_PIN = 26;
const uint8_t relay_4_PIN = 27;

//PumpController pump(relay_1_PIN, false);

PumpManager pumps;
RecipeController recipes;
IngredientController ingredients;
MixEngine mixEngine(pumps, ingredients, recipes);

ServerHandler server(ssid, password, pumps, recipes, ingredients, mixEngine);

void setup() {
  Serial.begin(115200);
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount FAILED");
    return;
  }

  Serial.println("LittleFS mounted");
  //add pumps
  pumps.addPump(relay_1_PIN, false);
  pumps.addPump(relay_2_PIN, false);
  pumps.addPump(relay_3_PIN, false);
  pumps.addPump(relay_4_PIN, false);

  pumps.load();
  ingredients.load();
  recipes.load();
  
  server.begin();
}

void loop() {
  pumps.update();
  server.handle();
}
