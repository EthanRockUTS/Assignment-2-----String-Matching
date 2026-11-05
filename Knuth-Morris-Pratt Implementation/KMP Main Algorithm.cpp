#include <iostream>
#include <string>
#include <vector>

// Builds the LPS (longest proper prefix which is also a suffix) table.
// lps[i] = length of the longest proper prefix of pattern[0..i]
//          that is also a suffix of pattern[0..i].
std::vector<int> buildLPS(const std::string& pattern) 
{
    int m = pattern.size();
    std::vector<int> lps(m, 0);

    int len = 0;  // length of the previous longest prefix-suffix
    int i = 1;

    while (i < m) 
    {
        if (pattern[i] == pattern[len]) 
        {
            lps[i++] = ++len;
        } else if (len != 0) 
        {
            // Fall back to the next-best candidate; don't advance i
            len = lps[len - 1];
        } else 
        {
            lps[i++] = 0;
        }
    }
    return lps;
}

// Returns the starting indices of all occurrences of pattern in text.
std::vector<int> kmpSearch(const std::string& text, const std::string& pattern) 
{
    std::vector<int> matches;
    int n = text.size();
    int m = pattern.size();

    if (m == 0) 
    {
        for (int position = 0; position <= n; ++position) 
        {
            matches.push_back(position);
        }
        return matches;
    }
    if (m > n) return matches;

    std::vector<int> lps = buildLPS(pattern);

    int i = 0;  // index in text
    int j = 0;  // index in pattern

    while (i < n) 
    {
        if (text[i] == pattern[j]) 
        {
            i++;
            j++;
            if (j == m) {
                matches.push_back(i - j);  // full match found
                j = lps[j - 1];            // continue to find overlapping matches
            }
        } else if (j != 0) 
        {
            j = lps[j - 1];  // shift pattern using the LPS table; i stays put
        } else 
        {
            i++;
        }
    }
    return matches;
}

#ifndef STRING_MATCHING_NO_MAIN
int main() 
{
    std::string text;
    std::string pattern;

    std::cout << "Enter text: ";
    if (!std::getline(std::cin, text)) {
        std::cerr << "Could not read text.\n";
        return 1;
    }

    std::cout << "Enter pattern: ";
    if (!std::getline(std::cin, pattern)) {
        std::cerr << "Could not read pattern.\n";
        return 1;
    }

    const std::vector<int> matches = kmpSearch(text, pattern);
    if (matches.empty()) {
        std::cout << "No matches found.\n";
        return 0;
    }

    std::cout << "Match positions (0-based):";
    for (int position : matches) {
        std::cout << ' ' << position;
    }
    std::cout << '\n';

    return 0;
}
#endif