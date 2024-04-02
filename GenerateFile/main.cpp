
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <random>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

struct Pair {
	string word;
	string previous;
	string next;
	int position = 0;
};

struct WordSet {
	string word;
	vector<string> next;
	vector<string> previous;
	bool is_unique = false;
};

vector<string> split(const string& text)
{
	vector<string> output;
	string temp = "";

	for (char c : text) {
		if (c == ' ') {
			output.push_back(temp);
			temp = "";
		}
		else {
			temp += c;
		}
	}

	if (!temp.empty()) {
		output.push_back(temp);
	}

	return output;

}

vector<Pair> find_seed(const vector<string>& words) {
	map<string, int> word_count;
	map<string, int> word_positions;

	// Count each word and remember the first position
	for (int i = 0; i < words.size(); i++) {
		word_count[words[i]]++;
		// Only record the position the first time the word is encountered
		if (word_count[words[i]] == 1) {
			word_positions[words[i]] = i;
		}
	}

	vector<Pair> seed;

	/* Now add to the seed only those words with a count of 1 */
	for (const auto& wc : word_count) {
		if (wc.second == 1) {
			seed.push_back({ wc.first, "", "", word_positions[wc.first]});
		}
	}

	return seed;
}

int get_word_index(const string& word, vector<Pair>& words)
{
	for (int i = 0; i < words.size(); i++) {
		if (words[i].word == word) {
			return i;
		}
	}

	return -1;

}

vector<int> get_instances_of_word(string& word, vector<string>& words)
{
	vector<int> output;

	for (int i = 0; i < words.size(); i++) {
		if (words[i] == word) {
			output.push_back(i);
		}
	}

	return output;

}

std::map<std::string, WordSet> rule_set(vector<string>& words, vector<Pair>& seed) {
	std::map<std::string, WordSet> rules;

	for (const auto& word : words) {
		auto& ws = rules[word]; // This ensures a WordSet is created for each word, even if it's not in the seed.
	}

	// Process seed words
	for (const auto& s : seed) {
		rules[s.word].is_unique = true;
	}

	// Construct rules based on words vector
	for (size_t i = 0; i < words.size(); ++i) {
		string& word = words[i];
		if (i > 0) {
			// Add the previous word if not the first word
			rules[word].previous.push_back(words[i - 1]);
		}
		if (i < words.size() - 1) {
			// Add the next word if not the last word
			rules[word].next.push_back(words[i + 1]);
		}
	}

	// Remove duplicates from previous and next vectors
	for (auto& rule : rules) {
		auto& ws = rule.second;
		std::sort(ws.previous.begin(), ws.previous.end());
		ws.previous.erase(std::unique(ws.previous.begin(), ws.previous.end()), ws.previous.end());

		std::sort(ws.next.begin(), ws.next.end());
		ws.next.erase(std::unique(ws.next.begin(), ws.next.end()), ws.next.end());
	}

	return rules;
}

void initialize_array(std::vector<std::string>& arr, int size) {
	arr.resize(size);
	for (int i = 0; i < size; ++i) {
		arr[i] = "_____";
	}
}

bool vec_contains_str(vector<string>& words, string& word)
{
	for (string s : words) {
		if (s == word) { return true; }
	}
	return false;
}

int get_combinations(string& left, string& right, std::map<std::string, WordSet>& rule_set)
{
	vector<string> combos;
	if (left != "_____") {
		combos = rule_set[left].next;
	}
	
	if (right != "_____") {
		if (combos.empty()) {
			combos = rule_set[right].previous;
		}
		else {
			for (string s : rule_set[right].previous) {
				if (!vec_contains_str(combos, s)) { combos.push_back(s); }
			}
		}
	}

	return (combos.empty() ? INT_MAX : combos.size());
}

/* 
index: index is the position in the vector<string> we are trying to solve
words: the array we are trying to solve
rule_set: the rules of given words
*/
int number_of_combinations(int index, vector<string>& words, std::map<std::string, WordSet>& rule_set) {
	int num_combos = 0;
	string prev_word = "_____";
	string next_word = "_____";
	/* Check if we can get the previous word */
	if (index - 1 >= 0) {
		/* Get all words this word could be */
		prev_word = words[index - 1];
	}

	if (index + 1 < words.size()) {
		next_word = words[index + 1];
	}

	return get_combinations(prev_word, next_word, rule_set);

}


vector<int> all_entropy(vector<string>& words, std::map<std::string, WordSet>& rule_set)
{
	vector<int> output;

	for (int i = 0; i < words.size(); i++) {
		output.push_back(number_of_combinations(i, words, rule_set));
	}

	return output;
}

int find_least_combos(vector<string>& words, std::map<std::string, WordSet>& rule_set)
{
	int lowest_combo = INT_MAX;
	int index = -1;

	for (int i = 0; i < words.size(); i++) {
		if (words[i] != "_____") {
			continue;
		}

		int value = number_of_combinations(i, words, rule_set);
		if (value < lowest_combo) {
			lowest_combo = value;
			index = i;
		}
	}

	return index;

}

string vector_to_string(vector<string>& words)
{
	string output;

	for (string s : words) {
		output += s + " ";
	}

	return output;

}

void fill_least_entropy_position(vector<string>& words, std::map<std::string, WordSet>& rule_set, std::mt19937& rng) {
	int pos = find_least_combos(words, rule_set);
	cout << "Looking at index: " << pos << endl;
	if (pos == -1 || words[pos] != "_____") {
		return; // No suitable position found or all positions are already filled.
	}

	const auto& prev_word = pos > 0 ? words[pos - 1] : "";
	const auto& next_word = pos < words.size() - 1 ? words[pos + 1] : "";

	std::vector<string> possible_words;
	if (!prev_word.empty() && rule_set.find(prev_word) != rule_set.end()) {
		possible_words = rule_set.at(prev_word).next;
	}
	if (!next_word.empty() && rule_set.find(next_word) != rule_set.end()) {
		// Intersect with next options if necessary...
		possible_words = rule_set.at(next_word).previous;
	}

	// Randomly choose one of the possible words (ensure `possible_words` is not empty)
	if (!possible_words.empty()) {
		std::uniform_int_distribution<int> dist(0, possible_words.size() - 1);
		words[pos] = possible_words[dist(rng)]; // Set the word at the position with most vertain word
	}

	// cout << "Possible: " << vector_to_string(possible_words) << endl;

}

vector<string> generate_text(std::map<std::string, WordSet>& rule_set, vector<Pair>& seed, int size)
{
	vector<string> temp_words;
	initialize_array(temp_words, size);
	std::mt19937 rng(std::random_device{}());

	for (int i = 0; i < seed.size(); i++) {
		string word = seed[i].word;
		temp_words[seed[i].position] = word;

		if (rule_set[word].previous.size() > 0) {
			temp_words[seed[i].position - 1] = rule_set[word].previous[0];
		}

		if (rule_set[word].next.size() > 0) {
			temp_words[seed[i].position + 1] = rule_set[word].next[0];
		}

	}

	/* Find least entropy position */
	while (find_least_combos(temp_words, rule_set) != -1)
	{
		fill_least_entropy_position(temp_words, rule_set, rng);
	}

	return temp_words;
}

void print_seed_context(const vector<Pair>& seed, const std::map<std::string, WordSet>& rule_set) {
	for (const auto& s : seed) {
		auto it = rule_set.find(s.word);
		if (it != rule_set.end()) {
			const WordSet& ws = it->second;

			cout << "Seed word: " << s.word << endl;
			cout << "Previous words: ";
			for (const auto& prev : ws.previous) {
				cout << prev << " ";
			}
			cout << endl << "Next words: ";
			for (const auto& next : ws.next) {
				cout << next << " ";
			}
			cout << endl << "----------------------" << endl;
		}
	}
}

void print_rules_set(const std::map<std::string, WordSet>& rule_set) {
	for (const auto& pair : rule_set) {
		cout << "Word: " << pair.first << "\n";

		cout << "  Previous: ";
		for (const auto& prev : pair.second.previous) {
			cout << prev << ", ";
		}
		cout << "\n";

		cout << "  Next: ";
		for (const auto& next : pair.second.next) {
			cout << next << ", ";
		}
		cout << "\n";

		cout << "  Is Unique: " << (pair.second.is_unique ? "Yes" : "No") << "\n";
		cout << "--------------------------------\n";
	}
}

void serialize(const string& filename, const vector<Pair>& seed, const map<string, WordSet>& rules, int totalWords) {
	ofstream seed_file(filename + ".seed");
	// Write the total number of words at the beginning
	seed_file << totalWords << "\n";
	for (const auto& s : seed) {
		seed_file << s.word << " " << s.position << "\n";
	}
	seed_file.close();

	ofstream rules_file(filename + ".rules");
	for (const auto& r : rules) {
		rules_file << r.first << "|"; // Word
		for (const auto& prev : r.second.previous) rules_file << prev << ",";
		rules_file << "|";
		for (const auto& next : r.second.next) rules_file << next << ",";
		rules_file << "\n";
	}
	rules_file.close();
}

// Simplified for brevity
void deserializeRules(const string& filename, map<string, WordSet>& rules) {
	ifstream rules_file(filename + ".rules");
	string line, word, token;
	while (getline(rules_file, line)) {
		stringstream ss(line);
		getline(ss, word, '|'); // Extract word before '|'
		WordSet ws;

		// Get previous words
		getline(ss, token, '|');
		stringstream prevStream(token);
		while (getline(prevStream, token, ',')) {
			if (!token.empty()) ws.previous.push_back(token);
		}

		// Get next words
		while (getline(ss, token, ',')) {
			if (!token.empty()) ws.next.push_back(token);
		}

		rules[word] = ws;
	}
}

void deserializeSeed(const string& filename, vector<Pair>& seed, int& size) {
	ifstream seed_file(filename + ".seed");
	seed_file >> size; // First line is the size
	string word;
	int position;
	while (seed_file >> word >> position) {
		seed.push_back({ word, "", "", position });
	}
}


void deserialize(const string& filename, vector<Pair>& seed, map<string, WordSet>& rules, int& totalWords) {
	deserializeSeed(filename, seed, totalWords);
	deserializeRules(filename, rules);
}

int main() {
	
	string text = "hello goodbye hello";
	vector<string> words = split(text);
	vector<Pair> seed = find_seed(words);
	auto rules = rule_set(words, seed);

	/*vector<Pair> seed;
	map<string, WordSet> rules;
	int size = 0;*/
	serialize("test", seed, rules, words.size());
	/*string fname = "test";
	deserialize(fname, seed, rules, size);*/

	// Print previous and next words for each seed
	// print_seed_context(seed, rules);

	// Generate text with a specified size
	vector<string> generatedText = generate_text(rules, seed, words.size());
	cout << "Generated Text: " << vector_to_string(generatedText) << endl;

	/*cout << "DISPLAYING SEEDED WORDS" << endl;
	for (Pair p : seed) {
		cout << p.word << " ";
	}
	cout << endl;*/

	return 0;
}

