#include <sstream>
#include <string>
#include <chrono>
using namespace std;
using namespace chrono;

string current_time_str() {
    using namespace chrono;

    auto now = system_clock::now();
    time_t t = system_clock::to_time_t(now);
    
    tm tm = *localtime(&t);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000;

    stringstream ss;
    ss << put_time(localtime(&t), "%d-%m-%Y ")
       << setfill('0') << setw(2) << tm.tm_hour << ":"
       << setfill('0') << setw(2) << tm.tm_min << ":"
       << setfill('0') << setw(2) << tm.tm_sec << "."
       << setfill('0') << setw(3) << ms;

    return ss.str();
}