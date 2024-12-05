#include <vector>
#include <string>
#include <algorithm>
using namespace std;

vector<string> split (string const& str, char separator) {
    vector<string> views;
    auto subBeg = str.begin();


    while (true) {
        auto subEnd = find (subBeg, str.end(), separator);
        views.emplace_back (subBeg, subEnd);

        if (subEnd == str.end())
            break;

        subBeg = subEnd + 1;
    }

    return views;
}