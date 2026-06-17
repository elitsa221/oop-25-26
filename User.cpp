#include "User.h"
#include <algorithm>
#include <cctype>
#include <vector>

User::User(const std::string& u, const std::string& p, const Date& reg)
    : username(u), password(p), registrationDate(reg) {}

bool User::addFollower(const std::string& u) {
    if (isFollowedBy(u)) return false;
    followers.push_back(u);
    return true;
}

bool User::removeFollower(const std::string& u) {
    auto it = std::find(followers.begin(), followers.end(), u);
    if (it == followers.end()) return false;
    followers.erase(it);
    return true;
}

bool User::isFollowedBy(const std::string& u) const {
    return std::find(followers.begin(), followers.end(), u) != followers.end();
}

bool User::isFollowing(const std::string& u) const {
    return std::find(following.begin(), following.end(), u) != following.end();
}

bool User::validateUsername(const std::string& un) {
    return un.size() >= 6 && un.size() <= 24;
}

bool User::validatePassword(const std::string& pw) {
    if (pw.size() < 12 || pw.size() > 36) return false;
    bool hasLower = false, hasUpper = false, hasNonLetter = false;
    for (char c : pw) {
        if (std::islower((unsigned char)c)) hasLower = true;
        else if (std::isupper((unsigned char)c)) hasUpper = true;
        else hasNonLetter = true;
    }
    return hasLower && hasUpper && hasNonLetter;
}

std::string User::toLower(const std::string& s) {
    std::string r = s;
    for (char& c : r) c = (char)std::tolower((unsigned char)c);
    return r;
}

int User::levenshtein(const std::string& a, const std::string& b) {
    int n = (int)a.size(), m = (int)b.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    for (int i = 0; i <= n; ++i) dp[i][0] = i;
    for (int j = 0; j <= m; ++j) dp[0][j] = j;
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j) {
            if (a[i - 1] == b[j - 1]) dp[i][j] = dp[i - 1][j - 1];
            else dp[i][j] = 1 + std::min({ dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1] });
        }
    return dp[n][m];
}