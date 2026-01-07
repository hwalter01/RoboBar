import requests
import pytest
import time

ESP_IP = "192.168.178.66"
BASE = f"http://{ESP_IP}/api"

TEST_INGREDIENT = "rum"
TEST_RECIPE = "flow_test"


# ======================================================
# SESSION FIXTURE
# ======================================================
@pytest.fixture(scope="function")
def session():
    s = requests.Session()
    s.trust_env = False

    # 🔥 WICHTIG: definierter Startzustand
    s.post(f"{BASE}/system/reset")
    time.sleep(0.3)

    yield s

    # =========================
    # TEARDOWN
    # =========================
    s.post(f"{BASE}/stop")
    s.delete(f"{BASE}/recipes", params={"name": TEST_RECIPE})
    s.delete(f"{BASE}/ingredients", params={"name": TEST_INGREDIENT})


# ======================================================
# TEST 1: REZEPT MUSS OHNE KALIBRIERUNG FEHLSCHLAGEN
# ======================================================
def test_recipe_fails_without_calibration(session):
    # Ingredient
    r = session.post(f"{BASE}/ingredients", params={"name": TEST_INGREDIENT})
    assert r.status_code in (200, 201, 409)

    # Recipe
    recipe = {
        "name": TEST_RECIPE,
        "ingredient0_name": TEST_INGREDIENT,
        "ingredient0_percent": "100"
    }
    r = session.post(f"{BASE}/recipes", data=recipe)
    assert r.status_code in (200, 201, 409)

    # Ingredient → Pump (aber KEINE Kalibrierung!)
    r = session.post(
        f"{BASE}/ingredients/assign",
        params={"name": TEST_INGREDIENT, "pumpId": 0}
    )
    assert r.status_code == 200

    # Start → MUSS FEHLSCHLAGEN
    r = session.post(
        f"{BASE}/recipe/start",
        params={"name": TEST_RECIPE, "totalMl": 200}
    )
    assert r.status_code == 409


# ======================================================
# TEST 2: REZEPT LÄUFT MIT KALIBRIERUNG
# ======================================================
def test_recipe_runs_with_calibration(session):
    # Ingredient
    session.post(f"{BASE}/ingredients", params={"name": TEST_INGREDIENT})

    # Assign pump
    session.post(
        f"{BASE}/ingredients/assign",
        params={"name": TEST_INGREDIENT, "pumpId": 0}
    )

    # 🔧 Kalibrierung
    r = session.post(
        f"{BASE}/pump/calibrate",
        params={"id": 0, "ml": 200, "seconds": 5}
    )
    assert r.status_code == 200

    # Recipe
    recipe = {
        "name": TEST_RECIPE,
        "ingredient0_name": TEST_INGREDIENT,
        "ingredient0_percent": "100"
    }
    session.post(f"{BASE}/recipes", data=recipe)

    # Start → MUSS KLAPPEN
    r = session.post(
        f"{BASE}/recipe/start",
        params={"name": TEST_RECIPE, "totalMl": 200}
    )
    assert r.status_code == 200

    time.sleep(0.3)

    # Pump running?
    r = session.get(f"{BASE}/pump/status", params={"id": 0})
    assert r.status_code == 200
    assert r.json()["running"] is True
