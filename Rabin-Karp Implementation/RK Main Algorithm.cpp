#include <iostream>
#include <exception>
#include <limits>
#include <string>
#include <vector>

using namespace std;

vector<size_t> rabinKarp(const string& text, const string& pattern,
						 unsigned long long modulus = 1000000007) {
	const unsigned long long base = 256;
	const size_t textLength = text.length();
	const size_t patternLength = pattern.length();
	vector<size_t> matches;

	if (modulus < 2 ||
		modulus > numeric_limits<unsigned long long>::max() / base) {
		return matches;
	}
	if (patternLength == 0) {
		for (size_t position = 0; position <= textLength; ++position) {
			matches.push_back(position);
		}
		return matches;
	}
	if (patternLength > textLength) return matches;

	unsigned long long highestPower = 1;
	for (size_t i = 1; i < patternLength; ++i) {
		highestPower = (highestPower * base) % modulus;
	}

	unsigned long long patternHash = 0;
	unsigned long long windowHash = 0;
	for (size_t i = 0; i < patternLength; ++i) {
		patternHash = (patternHash * base + static_cast<unsigned char>(pattern[i])) % modulus;
		windowHash = (windowHash * base + static_cast<unsigned char>(text[i])) % modulus;
	}

	for (size_t position = 0; position <= textLength - patternLength; ++position) {
		if (patternHash == windowHash &&
			text.compare(position, patternLength, pattern) == 0) {
			matches.push_back(position);
		}

		if (position < textLength - patternLength) {
			const unsigned long long outgoing =
				(static_cast<unsigned char>(text[position]) * highestPower) % modulus;
			windowHash = (windowHash + modulus - outgoing) % modulus;
			windowHash = (windowHash * base +
						  static_cast<unsigned char>(text[position + patternLength])) % modulus;
		}
	}

	return matches;
}

#ifndef STRING_MATCHING_NO_MAIN
int main(int argc, char* argv[]) {
	string text;
	string pattern;
	unsigned long long modulus = 1000000007;

	if (argc > 1) {
		try {
			size_t parsedLength = 0;
			modulus = stoull(argv[1], &parsedLength);
			if (parsedLength != string(argv[1]).length() || modulus < 2 ||
				modulus > numeric_limits<unsigned long long>::max() / 256) {
				cerr << "Modulus must be between 2 and ULLONG_MAX / 256.\n";
				return 1;
			}
		} catch (const exception&) {
			cerr << "Invalid modulus.\n";
			return 1;
		}
	}

	cout << "Enter text: ";
	if (!getline(cin, text)) {
		cerr << "Could not read text.\n";
		return 1;
	}

	cout << "Enter pattern: ";
	if (!getline(cin, pattern)) {
		cerr << "Could not read pattern.\n";
		return 1;
	}

	const vector<size_t> matches = rabinKarp(text, pattern, modulus);
	if (matches.empty()) {
		cout << "No matches found.\n";
		return 0;
	}

	cout << "Match positions (0-based):";
	for (size_t position : matches) {
		cout << ' ' << position;
	}
	cout << '\n';
	return 0;
}
#endif
