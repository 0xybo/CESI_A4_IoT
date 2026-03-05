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
    month(month),
    day(day),
    hours(hours),
    minutes(minutes),
    seconds(seconds) {
    if (year < 2000) this->year = year + 2000;
    else this->year = year;
}

static const uint8_t daysInMonth[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

Datetime::Datetime(unsigned long long timestamp) {
    unsigned long long _seconds = timestamp;

    // Extract time of day
    this->seconds = _seconds % 60;
    _seconds /= 60;
    this->minutes = _seconds % 60;
    _seconds /= 60;
    this->hours = _seconds % 24;
    unsigned long _days = _seconds / 24;

    // Calculate year
    int _year = 1970;
    while (true) {
        unsigned long daysInYear = isLeapYear(_year) ? 366 : 365;
        if (_days < daysInYear) break;
        _days -= daysInYear;
        _year++;
    }
    this->year = _year;

    // Calculate month
    int _month = 0;
    while (true) {
        uint8_t dim = daysInMonth[_month];
        if (_month == 1 && isLeapYear(_year)) dim = 29;  // February leap year
        if (_days < dim) break;
        _days -= dim;
        _month++;
    }
    _month += 1;  // months start from 1
    this->month = _month;

    // Remaining days
    this->day = _days + 1;  // days start from 1
}

unsigned long long Datetime::toTimestamp() {
    // Calculate days from epoch (1970)
    unsigned long totalDays = 0;

    // Add days for complete years since 1970
    for (int y = 1970; y < year; y++) {
        totalDays += isLeapYear(y) ? 366 : 365;
    }

    // Add days for complete months in current year
    for (int m = 1; m < month; m++) {
        uint8_t dim = daysInMonth[m - 1];
        if (m == 2 && isLeapYear(year)) dim = 29;
        totalDays += dim;
    }

    // Add remaining days (day is 1-indexed)
    totalDays += day - 1;

    // Convert to seconds and add time components
    unsigned long long timestamp = totalDays * 86400;  // 86400 = 24 * 60 * 60
    timestamp += hours * 3600;                     // 3600 = 60 * 60
    timestamp += minutes * 60;
    timestamp += seconds;

    return timestamp;
}

String Datetime::getFormattedDatetime() {
    return String(this->year) + "-" +
        (this->month < 10 ? "0" : "") + String(this->month) + "-" +
        (this->day < 10 ? "0" : "") + String(this->day) + " " +
        (this->hours < 10 ? "0" : "") + String(this->hours) + ":" +
        (this->minutes < 10 ? "0" : "") + String(this->minutes) + ":" +
        (this->seconds < 10 ? "0" : "") + String(this->seconds);
}

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
    Serial.print(year);
    Serial.print(" => ");
    Clock::clock.fillByYMD(year, month, day);
    Serial.print(Clock::clock.year + 2000);
    Serial.print(" => ");
    Clock::clock.fillByHMS(hours, minutes, seconds);
    Clock::clock.fillDayOfWeek(MON);
    Clock::clock.setTime();

    Clock::clock.getTime();
    Serial.println(Clock::clock.year);
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
        Clock::clock.year + 2000,
        Clock::clock.month,
        Clock::clock.dayOfMonth,
        Clock::clock.hour,
        Clock::clock.minute,
        Clock::clock.second
    );
}

unsigned long long Clock::getTimestamp() {
    Clock::clock.getTime();
    return getDatetime().toTimestamp();
}

String Clock::getFormattedDatetime() {
    return getDatetime().getFormattedDatetime();
}