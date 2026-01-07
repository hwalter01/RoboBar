import requests
import time

ESP_IP = "192.168.178.66"
BASE = f"http://{ESP_IP}/api"

session = requests.Session()
session.trust_env = False

INGREDIENT1 = "rum"
PUMP_ID1 = 0

INGREDIENT2 = "coke"
PUMP_ID2 = 1

PUMP_ID3 = 2
PUMP_ID4 = 3

RECIPE = "quick_test"
TOTAL_ML = 250

# -------------------------
# ADD INGREDIENTS
# -------------------------
print("➕ Adding ingredients...")
session.post(f"{BASE}/ingredients", params={"name": INGREDIENT1})
session.post(f"{BASE}/ingredients", params={"name": INGREDIENT2})

# -------------------------
# ASSIGN PUMPS
# -------------------------
print("🔌 Assigning pumps...")
session.post(f"{BASE}/ingredients/assign",
             params={"name": INGREDIENT1, "pumpId": PUMP_ID1})
session.post(f"{BASE}/ingredients/assign",
             params={"name": INGREDIENT2, "pumpId": PUMP_ID2})

# -------------------------
# CALIBRATION
# -------------------------
print("⚙️ Calibrating pumps...")

# Beispiel: Pumpe 0 → 200 ml in 5 Sekunden
r = session.post(
    f"{BASE}/pump/calibrate",
    params={"id": PUMP_ID1, "ml": 200, "seconds": 5}
)
print("Pump 0 calibration:", r.status_code, r.text)

# Beispiel: Pumpe 1 → 150 ml in 5 Sekunden
r = session.post(
    f"{BASE}/pump/calibrate",
    params={"id": PUMP_ID2, "ml": 150, "seconds": 5}
)
print("Pump 1 calibration:", r.status_code, r.text) 

# Beispiel: Pumpe 0 → 200 ml in 5 Sekunden
r = session.post(
    f"{BASE}/pump/calibrate",
    params={"id": PUMP_ID3, "ml": 200, "seconds": 5}
)
print("Pump 2 calibration:", r.status_code, r.text)

# Beispiel: Pumpe 1 → 150 ml in 5 Sekunden
r = session.post(
    f"{BASE}/pump/calibrate",
    params={"id": PUMP_ID4, "ml": 150, "seconds": 5}
)
print("Pump 3 calibration:", r.status_code, r.text) 

# -------------------------
# ADD RECIPE
# -------------------------
print("➕ Adding recipe...")
recipe_data = {
    "name": RECIPE,
    "ingredient0_name": INGREDIENT1,
    "ingredient0_percent": "75",
    "ingredient1_name": INGREDIENT2,
    "ingredient1_percent": "25",
}
session.post(f"{BASE}/recipes", data=recipe_data)

# -------------------------
# RUN RECIPE
# -------------------------
print("▶️ Starting recipe...")
r = session.post(
    f"{BASE}/recipe/start",
    params={"name": RECIPE, "totalMl": TOTAL_ML}
)

if r.status_code != 200:
    print("❌ Failed:", r.text)
    exit(1)

print("🍹 Recipe running!")

# -------------------------
# CHECK PUMPS
# -------------------------
time.sleep(0.3)

r = session.get(f"{BASE}/pump/status", params={"id": PUMP_ID1})
print("Pump 0 status:", r.json())

r = session.get(f"{BASE}/pump/status", params={"id": PUMP_ID2})
print("Pump 1 status:", r.json())
