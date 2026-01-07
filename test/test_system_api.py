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


# =========================
# TESTS
# =========================

def test_machine_status(session):
    r = session.get(f"{BASE_URL}/status")

    assert r.status_code == 200
    assert r.json().get("running") is False

    time.sleep(0.5)

    r = session.post(f"{BASE_URL}/pump/start", params={
        "id": 0,
        "duration": 3000
    })

    assert r.status_code == 200

    time.sleep(0.3)

    r = session.get(f"{BASE_URL}/status")
    assert r.json().get("running") is True

def test_system_stop(session):
    # Start a pump to ensure the system is running
    r = session.post(f"{BASE_URL}/pump/start", params={
        "id": 0,
        "duration": 5000
    })
    assert r.status_code == 200

    time.sleep(0.3)

    # Now stop the system
    r = session.post(f"{BASE_URL}/stop")
    assert r.status_code == 200

    time.sleep(0.3)

    # Check that the system is no longer running
    r = session.get(f"{BASE_URL}/status")
    assert r.json().get("running") is False



def test_cleanup(session):
    # Stop all pumps
    session.post(f"{BASE_URL}/stop")
