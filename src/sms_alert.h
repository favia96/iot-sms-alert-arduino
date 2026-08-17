#pragma once

// "Are you OK?" safety-check flow: sends a check-in SMS to a user and, if
// unanswered, broadcasts their last known GPS position to their emergency
// contacts. Hardware-agnostic - talks to the GSM module only through
// sms.h's sendSms().

// Sends "Hey <name>, are you OK?" to the given user.
void sendSmsUser(int userIndex);

// Notifies the given user's emergency contacts with their last known
// position (lat/lon as formatted strings, e.g. from dtostrf()).
void sendSmsEmergency(int userIndex, const char *lat, const char *lon);
