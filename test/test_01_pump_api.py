import requests
import pytest
import time

# =========================
# KONFIGURATION
# =========================
ESP_IP = "192.168.178.66"
BASE_URL = f"http://{ESP_IP}/api/pump"

# =========================
# FIXTURES
# =========================
@pytest.fixture(scope="session")
def session():
    s = requests.Session()
    s.trust_env = False
    return s


@pytest.fixture(autouse=True)
def ensure_stopped(session):
    """
    Ensure pumps are stopped before and after each test
    """
    session.post(f"http://{ESP_IP}/api/stop")
    yield
    session.post(f"http://{ESP_IP}/api/stop")


# =========================
# TESTS
# =========================

def test_pump_0_initially_stopped(session):
    r = session.get(f"{BASE_URL}/status", params={"id": 0})

    assert r.status_code == 200
    assert r.json().get("running") is False


def test_status_all_pumps(session):
    r = session.get(f"{BASE_URL}/status")
    data = r.json()

    assert r.status_code == 200
    assert "pumps" in data
    assert isinstance(data["pumps"], list)


def test_start_pump_0(session):
    r = session.post(f"{BASE_URL}/start", params={
        "id": 0,
        "duration": 3000
    })

    assert r.status_code == 200

    time.sleep(0.3)

    r = session.get(f"{BASE_URL}/status", params={"id": 0})
    assert r.json().get("running") is True


def test_start_all_pumps(session):
    r = session.post(f"{BASE_URL}/start", params={
        "duration": 2000
    })

    assert r.status_code == 200

    time.sleep(0.3)

    r = session.get(f"{BASE_URL}/status")
    data = r.json()

    assert all(p["running"] is True for p in data.get("pumps", []))

def test_calibration(session):
    r = session.post(f"{BASE_URL}/calibrate",
                     params={"id": 0, "ml": 200, "seconds": 5})
    assert r.status_code == 200

def test_get_calibration(session):
    r = session.get(f"{BASE_URL}/calibration")
    assert r.status_code == 200
    data = r.json()
    assert "pumps" in data
    assert isinstance(data["pumps"], list)


def test_cleanup(session):
    # Stop all pumps
    session.post(f"http://{ESP_IP}/api/stop")
