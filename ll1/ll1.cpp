#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;

class LL1Parser {
private:
    unordered_map<string, vector<vector<string>>> grammar;
    unordered_map<string, unordered_map<string, vector<string>>> parsingTable;
    string startSymbol;
    
    // Helper functions for table construction
    unordered_map<string, unordered_set<string>> firstSets;
    unordered_map<string, unordered_set<string>> followSets;

    void computeFirstSets() {
        bool changed;
        do {
            changed = false;
            for (const auto& prod : grammar) {
                const string& nonTerminal = prod.first;
                for (const auto& sequence : prod.second) {
                    unordered_set<string> first = computeFirst(sequence);
                    size_t oldSize = firstSets[nonTerminal].size();
                    firstSets[nonTerminal].insert(first.begin(), first.end());
                    if (firstSets[nonTerminal].size() != oldSize) {
                        changed = true;
                    }
                }
            }
        } while (changed);
    }

    unordered_set<string> computeFirst(const vector<string>& sequence) {
        unordered_set<string> result;
        if (sequence.empty() || sequence[0] == "ε") {
            result.insert("ε");
            return result;
        }

        for (const string& symbol : sequence) {
            // If terminal
            if (grammar.find(symbol) == grammar.end()) {
                result.insert(symbol);
                break;
            }
            // If non-terminal
            else {
                unordered_set<string> firstOfSymbol = firstSets[symbol];
                result.insert(firstOfSymbol.begin(), firstOfSymbol.end());
                if (firstOfSymbol.find("ε") == firstOfSymbol.end()) {
                    result.erase("ε");
                    break;
                }
            }
        }
        return result;
    }

    void computeFollowSets() {
        followSets[startSymbol].insert("$");
        
        bool changed;
        do {
            changed = false;
            for (const auto& prod : grammar) {
                const string& A = prod.first;
                for (const auto& sequence : prod.second) {
                    for (size_t i = 0; i < sequence.size(); i++) {
                        const string& B = sequence[i];
                        if (grammar.find(B) != grammar.end()) { // If non-terminal
                            size_t oldSize = followSets[B].size();
                            
                            // Rule 1: First of beta
                            if (i + 1 < sequence.size()) {
                                vector<string> beta(sequence.begin() + i + 1, sequence.end());
                                unordered_set<string> firstBeta = computeFirst(beta);
                                followSets[B].insert(firstBeta.begin(), firstBeta.end());
                                followSets[B].erase("ε");
                            }
                            
                            // Rule 2: Follow of A if beta =>* ε
                            if (i + 1 >= sequence.size() || 
                                (i + 1 < sequence.size() && computeFirst({sequence[i+1]}).count("ε"))) {
                                followSets[B].insert(followSets[A].begin(), followSets[A].end());
                            }
                            
                            if (followSets[B].size() != oldSize) {
                                changed = true;
                            }
                        }
                    }
                }
            }
        } while (changed);
    }

    void buildParsingTable() {
        for (const auto& prod : grammar) {
            const string& nonTerminal = prod.first;
            for (const auto& sequence : prod.second) {
                unordered_set<string> first = computeFirst(sequence);
                
                for (const string& terminal : first) {
                    if (terminal != "ε") {
                        parsingTable[nonTerminal][terminal] = sequence;
                    }
                }
                
                if (first.count("ε")) {
                    for (const string& terminal : followSets[nonTerminal]) {
                        parsingTable[nonTerminal][terminal] = sequence;
                    }
                }
            }
        }
    }

public:
    LL1Parser(unordered_map<string, vector<vector<string>>> grammar, string startSymbol)
        : grammar(grammar), startSymbol(startSymbol) {
        // Initialize first sets
        for (const auto& prod : grammar) {
            firstSets[prod.first] = {};
        }
        
        // Compute first and follow sets
        computeFirstSets();
        computeFollowSets();
        
        // Build parsing table
        buildParsingTable();
        
        // Print for debugging
        cout << "FIRST sets:\n";
        for (const auto& entry : firstSets) {
            cout << entry.first << ": ";
            for (const string& s : entry.second) cout << s << " ";
            cout << endl;
        }
        
        cout << "\nFOLLOW sets:\n";
        for (const auto& entry : followSets) {
            cout << entry.first << ": ";
            for (const string& s : entry.second) cout << s << " ";
            cout << endl;
        }
        
        cout << "\nParsing Table:\n";
        for (const auto& row : parsingTable) {
            cout << row.first << ":\n";
            for (const auto& col : row.second) {
                cout << "  " << col.first << " -> ";
                for (const string& s : col.second) cout << s << " ";
                cout << endl;
            }
        }
    }

    vector<string> tokenize(const string& input) {
        vector<string> tokens;
        istringstream iss(input);
        string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        tokens.push_back("$");
        return tokens;
    }

    bool parse(const string& input) {
        vector<string> inputTokens = tokenize(input);
        stack<string> parseStack;
        parseStack.push("$");
        parseStack.push(startSymbol);

        size_t inputPtr = 0;
        bool accepted = false;

        cout << "\nParsing steps:\n";
        cout << left << setw(25) << "Stack" << setw(25) << "Input" << "Action" << endl;
        cout << string(70, '-') << endl;

        while (!parseStack.empty()) {
            string stackStr = stackToString(parseStack);
            string inputStr = "";
            for (size_t i = inputPtr; i < inputTokens.size(); i++) {
                inputStr += inputTokens[i] + " ";
            }

            cout << left << setw(25) << stackStr << setw(25) << inputStr;

            string top = parseStack.top();
            string currentInput = inputTokens[inputPtr];

            if (top == currentInput) {
                if (top == "$") {
                    cout << "Accept" << endl;
                    accepted = true;
                    break;
                }
                cout << "Match " << top << endl;
                parseStack.pop();
                inputPtr++;
            } 
            else if (grammar.find(top) != grammar.end()) {
                if (parsingTable[top].find(currentInput) != parsingTable[top].end()) {
                    vector<string> production = parsingTable[top][currentInput];
                    cout << "Apply " << top << " -> ";
                    for (const auto& sym : production) cout << sym << " ";
                    cout << endl;

                    parseStack.pop();
                    if (!(production.size() == 1 && production[0] == "ε")) {
                        for (auto it = production.rbegin(); it != production.rend(); ++it) {
                            parseStack.push(*it);
                        }
                    }
                } 
                else {
                    cout << "\nError: No production for " << top << " on " << currentInput << endl;
                    return false;
                }
            } 
            else {
                cout << "\nError: Unexpected symbol " << top << endl;
                return false;
            }
        }

        return accepted;
    }

private:
    string stackToString(stack<string> s) {
        string result;
        while (!s.empty()) {
            result = s.top() + " " + result;
            s.pop();
        }
        return result;
    }
};

int main() {
    // Grammar productions
    unordered_map<string, vector<vector<string>>> grammar = {
        {"E", {{"T", "E'"}}},
        {"E'", {{"+", "T", "E'"}, {"ε"}}},
        {"T", {{"F", "T'"}}},
        {"T'", {{"*", "F", "T'"}, {"ε"}}},
        {"F", {{"(", "E", ")"}, {"id"}}}
    };

    // Create parser and compute parsing table
    LL1Parser parser(grammar, "E");

    // Test parsing
    string inputString = "id + id * id";  // Valid input
    // string inputString = "id + * id";   // Invalid input for testing

    cout << "\nParsing input: '" << inputString << "'" << endl;
    bool result = parser.parse(inputString);
    cout << "\nParsing result: " << (result ? "Accepted" : "Rejected") << endl;

    return 0;
}