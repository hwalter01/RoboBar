import requests
import pytest

ESP_IP = "192.168.178.66"
BASE = f"http://{ESP_IP}/api/ingredients"

@pytest.fixture(scope="session")
def session():
    s = requests.Session()
    s.trust_env = False
    return s


def ingredient_exists(data, name):
    return any(i["name"] == name for i in data)


def test_add_ingredient(session):
    r = session.post(BASE, params={"name": "test_ingredient_api"})
    assert r.status_code in (200, 201)


def test_list_ingredients(session):
    r = session.get(BASE)
    assert r.status_code == 200
    assert ingredient_exists(r.json(), "test_ingredient_api")


def test_assign_pump(session):
    r = session.post(f"{BASE}/assign",
                     params={"name": "test_ingredient_api", "pumpId": 0})
    assert r.status_code == 200


def test_delete_ingredient(session):
    r = session.delete(BASE, params={"name": "test_ingredient_api"})
    assert r.status_code == 200

def test_cleanup(session):
    # Delete ingredient (ignore result)
    session.delete(BASE, params={"name": "test_ingredient_api"})