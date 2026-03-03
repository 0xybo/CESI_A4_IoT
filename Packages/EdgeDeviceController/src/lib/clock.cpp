#include <lib/clock.h>

// ╭────────────────────────────────────────────────────────╮
// │                        DATETIME                        │
// ╰────────────────────────────────────────────────────────╯

Datetime::Datetime(
    int year,
    int month,
    int day,
    int hours,
    int minutes,
    int seconds
) :
    year(year),
    month(month),
    day(day),
    hours(hours),
    minutes(minutes),
    seconds(seconds) {}

// ╭────────────────────────────────────────────────────────╮
// │                         CLOCK                          │
// ╰────────────────────────────────────────────────────────╯

DS1307 Clock::clock;

void Clock::setup() {
    Clock::clock.begin();
}

void Clock::loop() {

}

void Clock::setDatetime(
    int year,
    int month,
    int day,
    int hours,
    int minutes,
    int seconds
) {
    Clock::clock.fillByYMD(year, month, day);
    Clock::clock.fillByHMS(hours, minutes, seconds);
    Clock::clock.setTime();
}

void Clock::setDatetime(Datetime& datetime) {
    Clock::setDatetime(
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hours,
        datetime.minutes,
        datetime.seconds
    );
}

Datetime Clock::getDatetime() {
    Clock::clock.getTime();
    return Datetime(
        Clock::clock.year,
        Clock::clock.month,
        Clock::clock.dayOfMonth,
        Clock::clock.hour,
        Clock::clock.minute,
        Clock::clock.second
    );
}

unsigned long Clock::getTimestamp() {
    Clock::clock.getTime();
    return Clock::clock.second +
        Clock::clock.minute * 60 +
        Clock::clock.hour * 3600 +
        (Clock::clock.dayOfMonth - 1) * 86400 +
        (Clock::clock.month - 1) * 2592000 +
        (Clock::clock.year - 1970) * 31536000;
}

String Clock::getFormattedDatetime() {
    Datetime datetime = Clock::getDatetime();
    return String(datetime.year) + "-" +
        (datetime.month < 10 ? "0" : "") + String(datetime.month) + "-" +
        (datetime.day < 10 ? "0" : "") + String(datetime.day) + " " +
        (datetime.hours < 10 ? "0" : "") + String(datetime.hours) + ":" +
        (datetime.minutes < 10 ? "0" : "") + String(datetime.minutes) + ":" +
        (datetime.seconds < 10 ? "0" : "") + String(datetime.seconds);
}