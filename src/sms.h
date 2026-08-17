#pragma once

// Backend-agnostic GSM interface. Exactly one backend implementation
// (sms_backend_fona.cpp or sms_backend_sim908_at.cpp) is compiled in,
// selected at build time via the SMS_BACKEND_FONA / SMS_BACKEND_SIM908_AT
// macro set per-environment in platformio.ini.

// Initializes the GSM module. Call once from setup().
// Returns false if the module can't be found/initialized.
bool smsBackendBegin();

// Sends `message` to `number`. Returns true on success.
bool sendSms(const char *number, const char *message);

// Attempts to read a GPS fix. Returns true and fills lat/lon (and, if
// supported, speed/heading/altitude) when a fix is available.
// Backends without GPS support (currently the SIM908 AT-command backend)
// always return false.
bool getGpsFix(float *lat, float *lon, float *speed_kph = nullptr, float *heading = nullptr, float *altitude = nullptr);
