import requests
import pytest

ESP_IP = "192.168.178.66"
BASE = f"http://{ESP_IP}/api"

@pytest.fixture(scope="session")
def session():
    s = requests.Session()
    s.trust_env = False
    return s


def test_recipe_fails_without_ingredient_mapping(session):
    # Ingredient anlegen
    session.post(f"{BASE}/ingredients", params={"name": "test_ingredient_avail"})

    # Rezept anlegen
    recipe = {
        "name": "test_recipe",
        "ingredient0_name": "test_ingredient_avail",
        "ingredient0_percent": "100"
    }
    session.post(f"{BASE}/recipes", data=recipe)

    # Rezept starten → MUSS FEHLSCHLAGEN
    r = session.post(
        f"{BASE}/recipe/start",
        params={"name": "test_recipe", "totalMl": 2000}
    )

    assert r.status_code == 409
    assert "not available" in r.text.lower()

def test_cleanup(session):
    # Delete recipe
    session.delete(f"{BASE}/recipes", params={"name": "test_recipe"})

    # Delete ingredient
    session.delete(f"{BASE}/ingredients", params={"name": "test_ingredient_avail"})
