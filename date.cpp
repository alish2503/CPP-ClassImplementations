#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <ctime>
#include <chrono>

class Date {
public:
    Date() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* local_time = std::localtime(&now_time);

        year = local_time->tm_year + 1900;
        month = local_time->tm_mon + 1;
        day = local_time->tm_mday;
    }
    Date(int year, int month, int day) : year(year), month(month), day(day) {
        if (!isValidDate(year, month, day)) {
            throw std::invalid_argument("Invalid date");
        }
    }

    void setYear(int year) {
        if (!isValidDate(year, month, day)) {
            throw std::invalid_argument("Invalid date");
        }
        year = year;
    }

    void setMonth(int month) {
        if (!isValidDate(year, month, day)) {
            throw std::invalid_argument("Invalid date");
        }
        month = month;
    }

    void setDay(int day) {
        if (!isValidDate(year, month, day)) {
            throw std::invalid_argument("Invalid date");
        }
        day = day;
    }

    static bool isValidDate(int year, int month, int day) {
        if (month < 1 || month > 12 || day < 1 || day > 31) {
            return false;
        }
        return day <= getDaysInMonth(year, month);
    }

    static bool isLeapYear(int year) {
        return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    }

    friend std::ostream& operator<<(std::ostream& os, const Date& date) {
        os << std::setw(4) << std::setfill('0') << date.year << "-"
           << std::setw(2) << std::setfill('0') << date.month << "-"
           << std::setw(2) << std::setfill('0') << date.day;
        return os;
    }

    Date& operator++() { 
        incrementDay();
        return *this;
    }

    Date operator++(int) { 
        Date temp = *this;
        incrementDay();
        return temp;
    }

    Date& operator--() { 
        decrementDay();
        return *this;
    }

    Date operator--(int) { 
        Date temp = *this;
        decrementDay();
        return temp;
    }

    Date& addDays(int days) {
        for (int i = 0; i < std::abs(days); ++i) {
            days > 0 ? incrementDay() : decrementDay();
        }
        return *this;
    }

    Date& subtractDays(int days) {
        return addDays(-days);
    }

    Date& addMonths(int months) {
        int newMonth = month + months;
        int yearsToAdd = (newMonth - 1) / 12;
        newMonth = (newMonth - 1) % 12 + 1;
        year += yearsToAdd;
        month = newMonth;
        if (day > getDaysInMonth(year, month)) {
            day = getDaysInMonth(year, month);
        }
        return *this;
    }

    Date& subtractMonths(int months) {
        int newMonth = month - months;
        int yearsToSubtract = 0;
        if (newMonth < 1) {
            yearsToSubtract = (1 - newMonth + 11) / 12;
            newMonth = 12 + newMonth % 12;
        }
        year -= yearsToSubtract;
        month = newMonth;
        if (day > getDaysInMonth(year, month)) {
            day = getDaysInMonth(year, month);
        }
        return *this;
    }

    Date& addYears(int years) {
        year += years;
        if (day > getDaysInMonth(year, month)) {
            day = getDaysInMonth(year, month);
        }
        return *this;
    }

    Date& subtractYears(int years) {
        return addYears(-years);
    }

private:
    int year;
    int month;
    int day;

    void incrementDay() {
        ++day;
        if (day > getDaysInMonth(year, month)) {
            day = 1;
            ++month;
            if (month > 12) {
                month = 1;
                ++year;
            }
        }
    }

    void decrementDay() {
        --day;
        if (day < 1) {
            --month;
            if (month < 1) {
                month = 12;
                --year;
            }
            day = getDaysInMonth(year, month);
        }
    }

    static int getDaysInMonth(int year, int month) {
        static const int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        if (month == 2 && isLeapYear(year)) {
            return 29;
        }
        return daysInMonth[month - 1];
    }
};


int main() {
    try {
        Date date(2024, 2, 28);
        std::cout << "Initial Date: " << date << std::endl;
        
        date.addDays(5);
        std::cout << "After adding 5 days: " << date << std::endl;
    
        date.subtractDays(10);
        std::cout << "After subtracting 10 days: " << date << std::endl;

        date.addMonths(3);
        std::cout << "After adding 3 months: " << date << std::endl;

        date.subtractMonths(6);
        std::cout << "After subtracting 6 months: " << date << std::endl;

        date.addYears(2);
        std::cout << "After adding 2 years: " << date << std::endl;

        date.subtractYears(1);
        std::cout << "After subtracting 1 year: " << date << std::endl;

        ++date;
        std::cout << "After prefix increment: " << date << std::endl;

        date++;
        std::cout << "After postfix increment: " << date << std::endl;

        --date;
        std::cout << "After prefix decrement: " << date << std::endl;

        date--;
        std::cout << "After postfix decrement: " << date << std::endl;
        date = Date(2024, 12, 31);
        std::cout << "Initial Date: " << date << std::endl;
        date.addDays(1);
        std::cout << "After adding 1 day to the end of the year: " << date << std::endl;

        date.subtractDays(2);
        std::cout << "After subtracting 2 days from the new year: " << date << std::endl;
    
        date = Date(2024, 1, 1);
        std::cout << "Initial Date: " << date << std::endl;
        date.subtractDays(1);
        std::cout << "After subtracting 1 day at the start of the year: " << date << std::endl;

        date.addMonths(12);
        std::cout << "After adding 12 months: " << date << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
