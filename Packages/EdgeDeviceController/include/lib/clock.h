#pragma once
#include <Arduino.h>
#include <DS1307.h>
#include <constants.h>

// ╭────────────────────────────────────────────────────────╮
// │                        DATETIME                        │
// ╰────────────────────────────────────────────────────────╯

class Datetime {
public:
    int year;
    int month;
    int day;
    int hours;
    int minutes;
    int seconds;

    Datetime(
        int year,
        int month,
        int day,
        int hours,
        int minutes,
        int seconds
    );
};

// ╭────────────────────────────────────────────────────────╮
// │                         CLOCK                          │
// ╰────────────────────────────────────────────────────────╯

class Clock {
private:
    static DS1307 clock;

public:
    static void setup();
    static void loop();
    /**
     * Set the date and time of the RTC module.
     * @param year The year (e.g., 2024)
     * @param month The month (1-12)
     * @param day The day of the month (1-31)
     * @param hours The hours (0-23)
     * @param minutes The minutes (0-59)
     * @param seconds The seconds (0-59)
     *
     * Example usage:
     * ```cpp
     * clock.setDatetime(2024, 6, 30, 14, 30, 0);
     * // Set to June 30, 2024, 14:30:00
     * ```
     */
    static void setDatetime(
        int year,
        int month,
        int day,
        int hours,
        int minutes,
        int seconds
    );
    /**
     * Set the date and time of the RTC module using a Datetime object.
     * @param datetime A Datetime object containing the date and time information.
     *
     * Example usage:
     * ```cpp
     * Datetime datetime(2024, 6, 30, 14, 30, 0);
     * clock.setDatetime(datetime);
     * // Set to June 30, 2024, 14:30:00
     * ```
     */
    static void setDatetime(Datetime& datetime);
    static Datetime getDatetime();
    static unsigned long getTimestamp();
    static String getFormattedDatetime();
};