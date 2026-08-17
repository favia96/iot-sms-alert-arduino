# IoT Send SMS Alert System: "Are you OK?" SMS Safety Alert

An Arduino sketch implementing a button-triggered safety check: press a
button to send a check-in SMS ("Hey \<name\>, are you OK?") to a user; if
they don't respond, press again to broadcast their last known GPS position
to a list of emergency contacts, with a Google Maps link.

The GSM/SMS logic is abstracted behind a single interface (`src/sms.h`) so
the same application code (`src/main.cpp`, `src/sms_alert.cpp`) runs on
either of two GSM hardware backends. This unifies what used to be two
separate prototype repos (`send_sms`, built on the Adafruit FONA shield, and
`sendSMS`, built on the DFRobot SIM908 shield using raw AT commands).

---

## Hardware backends

| Backend | Hardware | PlatformIO env | Notes |
|---|---|---|---|
| `SMS_BACKEND_FONA` | Adafruit FONA (via `SoftwareSerial`, `Adafruit_FONA` lib) | `fona` | Full support: SMS send + GPS fix (`fona.getGPS`) |
| `SMS_BACKEND_SIM908_AT` | DFRobot GPS/GPRS/GSM Module V3.0 (SIM908), raw AT commands over hardware `Serial` | `sim908_at` | SMS send only. GPS is not implemented for this backend yet — see `iot-gps-arduino`'s `gps_sensor.h` for a reference AT-command GPS implementation that could be ported in. |

Exactly one backend is compiled in per build, selected via a build flag set
in `platformio.ini`:

```
pio run -e fona          # Adafruit FONA
pio run -e sim908_at      # DFRobot SIM908, AT commands
```

## Project layout

```
src/
  main.cpp                  application flow: button state machine, GPS polling
  sms.h                     backend-agnostic interface (sendSms, getGpsFix, smsBackendBegin)
  sms_backend_fona.cpp       FONA implementation (compiled when SMS_BACKEND_FONA is set)
  sms_backend_sim908_at.cpp  SIM908 AT-command implementation (compiled when SMS_BACKEND_SIM908_AT is set)
  sms_alert.h / .cpp         domain logic: contacts, "are you OK?" message, emergency broadcast
```

## Configuration

Update the placeholder names/numbers in `src/sms_alert.cpp` before use:

```cpp
static char *names[] = {"User1", "User2", "User3"};
static char *phone_number = "+390000000000";
static char *numbers[] = {phone_number, "+392222222222", "+4911111111111"};
static char *emergency_numbers[3][3] = { ... };
```

## Wiring

- **FONA**: `FONA_RX`=D2, `FONA_TX`=D10, `FONA_RST`=D4 (see `sms_backend_fona.cpp`).
- **SIM908**: driver pins D3/D4/D5, module talks over hardware `Serial` — see
  the setup steps in `sms_backend_sim908_at.cpp` (S1/S2 switch positions).

Both environments share: buzzer on D7, button on D8.

## Author

**Federico Favia** — April 2019
