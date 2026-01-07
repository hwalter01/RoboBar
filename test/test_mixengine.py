import requests
import pytest
import time

ESP_IP = "192.168.178.66"
BASE = f"http://{ESP_IP}/api"

@pytest.fixture(scope="session")
def session():
    s = requests.Session()
    s.trust_env = False
    return s


def test_mixengine_starts_pump(session):
    # Ingredient
    session.post(f"{BASE}/ingredients", params={"name": "test_ingredient_mix"})
    session.post(f"{BASE}/ingredients/assign",
                 params={"name": "test_ingredient_mix", "pumpId": 0})

    # Rezept
    recipe = {
        "name": "mix_test",
        "ingredient0_name": "test_ingredient_mix",
        "ingredient0_percent": "100"
    }
    session.post(f"{BASE}/recipes", data=recipe)

    # Rezept starten
    r = session.post(
        f"{BASE}/recipe/start",
        params={"name": "mix_test", "totalMl": 3000}
    )

    assert r.status_code == 200

    time.sleep(0.3)

    # Pumpenstatus prüfen
    r = session.get(f"{BASE}/pump/status", params={"id": 0})
    data = r.json()

    assert data["running"] is True

def test_cleanup(session):
    # Stop all pumps
    session.post(f"{BASE}/stop")

    # Delete recipe
    session.delete(f"{BASE}/recipes", params={"name": "mix_test"})

    # Delete ingredient
    session.delete(f"{BASE}/ingredients", params={"name": "test_ingredient_mix"})