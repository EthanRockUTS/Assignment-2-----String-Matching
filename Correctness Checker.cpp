#define STRING_MATCHING_NO_MAIN
#include "Knuth-Morris-Pratt Implementation/KMP Main Algorithm.cpp"
#include "Rabin-Karp Implementation/RK Main Algorithm.cpp"

#include <climits>
#include <exception>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

bool parseUnsigned(const std::string& value, unsigned long long& result) {
	if (value.empty() || value[0] == '-') {
		return false;
	}
	try {
		size_t parsedLength = 0;
		result = std::stoull(value, &parsedLength);
		return parsedLength == value.length();
	} catch (const std::exception&) {
		return false;
	}
}

std::string randomString(size_t length, std::mt19937_64& generator) {
	static const std::string alphabet = "abcd";
	std::uniform_int_distribution<size_t> characterIndex(0, alphabet.length() - 1);
	std::string result;
	result.reserve(length);
	for (size_t i = 0; i < length; ++i) {
		result.push_back(alphabet[characterIndex(generator)]);
	}
	return result;
}

std::vector<size_t> findMatches(const std::string& text, const std::string& pattern) {
	std::vector<size_t> matches;
	for (size_t position = text.find(pattern); position != std::string::npos;
		 position = text.find(pattern, position + 1)) {
		matches.push_back(position);
	}
	return matches;
}

template <typename Position>
void printPositions(const std::vector<Position>& positions) {
	std::cerr << '[';
	for (size_t i = 0; i < positions.size(); ++i) {
		if (i != 0) {
			std::cerr << ", ";
		}
		std::cerr << positions[i];
	}
	std::cerr << ']';
}

bool checkCase(const std::string& text, const std::string& pattern,
			   unsigned long long modulus, const std::string& label) {
	const std::vector<size_t> expected = findMatches(text, pattern);
	const std::vector<int> kmpMatches = kmpSearch(text, pattern);
	const std::vector<size_t> rabinKarpMatches = rabinKarp(text, pattern, modulus);

	bool kmpCorrect = kmpMatches.size() == expected.size();
	for (size_t i = 0; kmpCorrect && i < expected.size(); ++i) {
		kmpCorrect = kmpMatches[i] >= 0 &&
			static_cast<size_t>(kmpMatches[i]) == expected[i];
	}
	if (!kmpCorrect || rabinKarpMatches != expected) {
		std::cerr << "Mismatch in " << label << ".\n"
				  << "text=" << std::quoted(text) << ", pattern=" << std::quoted(pattern)
				  << ", modulus=" << modulus << '\n'
				  << "Expected: ";
		printPositions(expected);
		std::cerr << "\nKMP: ";
		printPositions(kmpMatches);
		std::cerr << "\nRabin-Karp: ";
		printPositions(rabinKarpMatches);
		std::cerr << '\n';
		return false;
	}
	return true;
}

int main(int argc, char* argv[]) {
	if (argc > 6) {
		std::cerr << "Usage: correctness-checker [cases] [max-text-length] "
				  << "[max-pattern-length] [seed] [modulus]\n";
		return 1;
	}

	unsigned long long casesValue = 10000;
	unsigned long long maxTextLengthValue = 256;
	unsigned long long maxPatternLengthValue = 64;
	unsigned long long seed = std::random_device{}();
	unsigned long long modulus = 2;
	if ((argc > 1 && !parseUnsigned(argv[1], casesValue)) || casesValue == 0 ||
		(argc > 2 && !parseUnsigned(argv[2], maxTextLengthValue)) ||
		(argc > 3 && !parseUnsigned(argv[3], maxPatternLengthValue)) ||
		(argc > 4 && !parseUnsigned(argv[4], seed)) ||
		(argc > 5 && !parseUnsigned(argv[5], modulus))) {
		std::cerr << "Arguments must be non-negative integers; cases must be positive.\n";
		return 1;
	}
	if (maxTextLengthValue > INT_MAX || maxPatternLengthValue > INT_MAX) {
		std::cerr << "Maximum lengths must not exceed INT_MAX for the KMP implementation.\n";
		return 1;
	}
	if (modulus < 2 || modulus > std::numeric_limits<unsigned long long>::max() / 256) {
		std::cerr << "Modulus must be between 2 and ULLONG_MAX / 256.\n";
		return 1;
	}

	const size_t cases = static_cast<size_t>(casesValue);
	const size_t maxTextLength = static_cast<size_t>(maxTextLengthValue);
	const size_t maxPatternLength = static_cast<size_t>(maxPatternLengthValue);
	std::mt19937_64 generator(seed);
	std::uniform_int_distribution<size_t> textLengthDistribution(0, maxTextLength);
	std::uniform_int_distribution<size_t> patternLengthDistribution(0, maxPatternLength);
	std::bernoulli_distribution useTextSubstring(0.5);

	const std::vector<std::pair<std::string, std::string>> edgeCases = {
		{"", ""},
		{"abc", ""},
		{"", "a"},
		{"aaaaa", "aaa"},
		{"ababa", "aba"},
		{"abc", "abcd"},
		{"abcdef", "xyz"}
	};
	for (size_t i = 0; i < edgeCases.size(); ++i) {
		if (!checkCase(edgeCases[i].first, edgeCases[i].second, modulus,
					   "edge case " + std::to_string(i + 1))) {
			return 1;
		}
	}

	for (size_t test = 0; test < cases; ++test) {
		const std::string text = randomString(textLengthDistribution(generator), generator);
		std::string pattern;
		if (!text.empty() && maxPatternLength > 0 && useTextSubstring(generator)) {
			std::uniform_int_distribution<size_t> startDistribution(0, text.length() - 1);
			const size_t start = startDistribution(generator);
			const size_t available = text.length() - start;
			const size_t maxSubstringLength = std::min(maxPatternLength, available);
			std::uniform_int_distribution<size_t> lengthDistribution(1, maxSubstringLength);
			pattern = text.substr(start, lengthDistribution(generator));
		} else {
			pattern = randomString(patternLengthDistribution(generator), generator);
		}
		if (!checkCase(text, pattern, modulus, "random case " + std::to_string(test + 1))) {
			std::cerr << "Reproduce with seed " << seed << ".\n";
			return 1;
		}
	}

	std::cout << "Passed " << edgeCases.size() << " edge cases and " << cases
			  << " random cases. Seed: " << seed << ", Rabin-Karp modulus: "
			  << modulus << ".\n";
	return 0;
}