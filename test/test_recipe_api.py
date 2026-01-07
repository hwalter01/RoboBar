import requests
import time
import pytest

# =========================
# KONFIGURATION
# =========================
ESP_IP = "192.168.178.66"
BASE_URL = f"http://{ESP_IP}/api"

# =========================
# FIXTURES
# =========================
@pytest.fixture(scope="session")
def session():
    s = requests.Session()
    s.trust_env = False
    return s

@pytest.fixture
def recipe_data():
    return {
        "name": "cuba_libre",
        "ingredient0_name": "rum",
        "ingredient0_percent": "40",
        "ingredient1_name": "cola",
        "ingredient1_percent": "50",
        "ingredient2_name": "lime",
        "ingredient2_percent": "10"
    }

# =========================
# HELPER
# =========================
def recipe_exists(recipes, name):
    return any(r["name"] == name for r in recipes)

# =========================
# TESTS
# =========================

def test_add_recipe(session, recipe_data):
    r = session.post(f"{BASE_URL}/recipes", data=recipe_data)

    assert r.status_code == 201
    assert r.json().get("status") == "Recipe added"



def test_list_recipes_contains_recipe(session):
    r = session.get(f"{BASE_URL}/recipes")
    data = r.json()

    assert r.status_code == 200
    assert recipe_exists(data, "cuba_libre")



def test_add_duplicate_recipe_fails(session, recipe_data):
    r = session.post(f"{BASE_URL}/recipes", data=recipe_data)

    assert r.status_code == 409



def test_delete_recipe(session):
    r = session.delete(
        f"{BASE_URL}/recipes",
        params={"name": "cuba_libre"}
    )

    assert r.status_code == 200
    assert r.json().get("status") == "Recipe deleted"


def test_recipe_is_removed(session):
    r = session.get(f"{BASE_URL}/recipes")
    data = r.json()

    assert r.status_code == 200
    assert not recipe_exists(data, "cuba_libre")

def test_cleanup(session):
    # Delete recipe (ignore result)
    session.delete(f"{BASE_URL}/recipes", params={"name": "cuba_libre"})