#define STRING_MATCHING_NO_MAIN
#include "Knuth-Morris-Pratt Implementation/KMP Main Algorithm.cpp"
#include "Rabin-Karp Implementation/RK Main Algorithm.cpp"

#include <chrono>
#include <exception>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <utility>
#include <vector>

struct TestCase {
	unsigned long long modulus;
	std::string pattern;
	std::string text;
};

bool parseUnsigned(const std::string& value, unsigned long long& result) {
	try {
		size_t parsedLength = 0;
		result = std::stoull(value, &parsedLength);
		return parsedLength == value.length();
	} catch (const std::exception&) {
		return false;
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2 || argc > 3) {
		std::cerr << "Usage: benchmark <test-file> [repetitions]\n";
		return 1;
	}

	unsigned long long repetitionsValue = 10;
	if ((argc > 2 && !parseUnsigned(argv[2], repetitionsValue)) ||
		repetitionsValue == 0 ||
		repetitionsValue > std::numeric_limits<size_t>::max()) {
		std::cerr << "Repetitions must be a positive integer.\n";
		return 1;
	}
	const size_t repetitions = static_cast<size_t>(repetitionsValue);

	std::ifstream input(argv[1]);
	if (!input) {
		std::cerr << "Could not open test file: " << argv[1] << '\n';
		return 1;
	}

	unsigned long long testCountValue = 0;
	if (!(input >> testCountValue) || testCountValue == 0 ||
		testCountValue > std::numeric_limits<size_t>::max()) {
		std::cerr << "The first line must contain a positive test count.\n";
		return 1;
	}
	input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	std::vector<TestCase> testCases;
	testCases.reserve(static_cast<size_t>(testCountValue));
	for (size_t i = 0; i < static_cast<size_t>(testCountValue); ++i) {
		TestCase testCase;
		std::string modulusLine;
		if (!std::getline(input, modulusLine) || !parseUnsigned(modulusLine, testCase.modulus) ||
			testCase.modulus < 2 ||
			testCase.modulus > std::numeric_limits<unsigned long long>::max() / 256) {
			std::cerr << "Test " << i + 1
					  << " must start with a modulus between 2 and ULLONG_MAX / 256.\n";
			return 1;
		}
		if (!std::getline(input, testCase.pattern) || !std::getline(input, testCase.text)) {
			std::cerr << "Each test needs a modulus line, pattern line, and text line.\n";
			return 1;
		}
		testCases.push_back(std::move(testCase));
	}

	double totalKmpMilliseconds = 0.0;
	double totalRabinKarpMilliseconds = 0.0;
	unsigned long long matchChecksum = 0;
	std::cout << std::fixed << std::setprecision(6);
	std::cout << "Tests: " << testCases.size() << ", repetitions per test: "
			  << repetitions << ", Rabin-Karp modulus: per test case\n";

	for (size_t testIndex = 0; testIndex < testCases.size(); ++testIndex) {
		const TestCase& testCase = testCases[testIndex];
		const std::vector<int> kmpMatches = kmpSearch(testCase.text, testCase.pattern);
		const std::vector<size_t> rabinKarpMatches =
			rabinKarp(testCase.text, testCase.pattern, testCase.modulus);
		bool resultsMatch = kmpMatches.size() == rabinKarpMatches.size();
		for (size_t i = 0; resultsMatch && i < kmpMatches.size(); ++i) {
			resultsMatch = static_cast<size_t>(kmpMatches[i]) == rabinKarpMatches[i];
		}
		if (!resultsMatch) {
			std::cerr << "Algorithm results differ on test " << testIndex + 1 << ".\n";
			return 1;
		}

		double kmpMilliseconds = 0.0;
		double rabinKarpMilliseconds = 0.0;
		for (size_t repetition = 0; repetition < repetitions; ++repetition) {
			if (repetition % 2 == 0) {
				const auto start = std::chrono::steady_clock::now();
				const auto matches = kmpSearch(testCase.text, testCase.pattern);
				const auto end = std::chrono::steady_clock::now();
				kmpMilliseconds += std::chrono::duration<double, std::milli>(end - start).count();
				matchChecksum += matches.size();

				const auto rabinKarpStart = std::chrono::steady_clock::now();
				const auto rabinKarpResult =
					rabinKarp(testCase.text, testCase.pattern, testCase.modulus);
				const auto rabinKarpEnd = std::chrono::steady_clock::now();
				rabinKarpMilliseconds += std::chrono::duration<double, std::milli>(
					rabinKarpEnd - rabinKarpStart).count();
				matchChecksum += rabinKarpResult.size();
			} else {
				const auto rabinKarpStart = std::chrono::steady_clock::now();
				const auto rabinKarpResult =
					rabinKarp(testCase.text, testCase.pattern, testCase.modulus);
				const auto rabinKarpEnd = std::chrono::steady_clock::now();
				rabinKarpMilliseconds += std::chrono::duration<double, std::milli>(
					rabinKarpEnd - rabinKarpStart).count();
				matchChecksum += rabinKarpResult.size();

				const auto start = std::chrono::steady_clock::now();
				const auto matches = kmpSearch(testCase.text, testCase.pattern);
				const auto end = std::chrono::steady_clock::now();
				kmpMilliseconds += std::chrono::duration<double, std::milli>(end - start).count();
				matchChecksum += matches.size();
			}
		}

		kmpMilliseconds /= repetitions;
		rabinKarpMilliseconds /= repetitions;
		totalKmpMilliseconds += kmpMilliseconds;
		totalRabinKarpMilliseconds += rabinKarpMilliseconds;
		std::cout << "Test " << testIndex + 1 << " average: KMP " << kmpMilliseconds
				  << " ms, Rabin-Karp " << rabinKarpMilliseconds << " ms\n";
	}

	std::cout << "Overall average per test: KMP "
			  << totalKmpMilliseconds / testCases.size() << " ms, Rabin-Karp "
			  << totalRabinKarpMilliseconds / testCases.size() << " ms\n";
	std::cout << "Match checksum: " << matchChecksum << '\n';
	return 0;
}