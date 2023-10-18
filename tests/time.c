#include <stdio.h>

void convertCMOSTime(unsigned int cmos_time[2]) {
    // Extract high and low words
    unsigned int high_word = cmos_time[0];
    unsigned int low_word = cmos_time[1];

    // Calculate total ticks (1 tick = 18 seconds)
    unsigned int total_ticks = (high_word << 16) | low_word;
    unsigned int total_seconds = total_ticks / 18;

    // Calculate hours, minutes, and seconds
    unsigned int hours = total_seconds / 3600;
    unsigned int remaining_seconds = total_seconds % 3600;
    unsigned int minutes = remaining_seconds / 60;
    unsigned int seconds = remaining_seconds % 60;

    // Output results
    printf("Total Time: %u seconds\n", total_seconds);
    printf("Hours: %u, Minutes: %u, Seconds: %u\n", hours, minutes, seconds);
}

void convertCMOSTime2(unsigned int cmos_time[2]) {
    unsigned int ticks =
        (cmos_time[0] << 16) | cmos_time[1];  // Combine high and low words
    unsigned int seconds = ticks / 18;        // 1 tick = 18 seconds

    // Calculate hours, minutes, and remaining seconds
    unsigned int hours = seconds / 3600;
    unsigned int remainingSeconds = seconds % 3600;
    unsigned int minutes = remainingSeconds / 60;
    seconds = remainingSeconds % 60;

    // Output results
    printf("Total Ticks: %u\n", ticks);
    printf("Total Time: %u seconds\n", seconds);
    printf("Hours: %u, Minutes: %u, Seconds: %u\n", hours, minutes, seconds);
}

int main() {
    unsigned int cmos_time[2] = {
        14, 21778};  // Example values for high and low words
    convertCMOSTime(cmos_time);
    return 0;
}
