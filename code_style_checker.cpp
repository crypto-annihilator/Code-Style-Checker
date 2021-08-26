#include <iostream>
#include <fstream>
#include <exception>
#include <unordered_map>
#include <string>
#include <vector>


std::vector<std::string> type {"int", "char", "std::string", "double", "float", "bool", "warchar_t", "long"};
const std::vector<std::string> typeFirstWord {"const", "unsigned", "short", "long"};
const std::vector<char> simbols {' ', '(', ')', '=', '+', '-', '*', '/', '%', '!', ';', ',', '>', '<', '{', '}'};
const std::vector<std::string> namespaceWords{"cin", "cout", "string", "endl", "vector", "map", "unordered_map"};
std::unordered_map<std::string, std::string> wrongAndRightName;
std::vector<std::string> codeLine;

bool isSimbol (char ch) {
    for (const auto& elem : simbols) {
        if (ch == elem) {
            return true;
        }
    }
    return false;
}

bool isNamespaceWord(std::string& word)
{
    for (auto& str : namespaceWords) {
        if (str == word) {
            return true;
        }
    }
    return false;
}

void checkAndChangeNamespaceWord(int indexLine)
{
    for (auto& word : codeLine) {
        if (isNamespaceWord(word)) {
            std::cout << "line:" << indexLine << ": error:  Maybe you wanted to write '" << ("std::" + word) << "'" << std::endl;
                word = "std::" + word;
        }
    }
}

void parseToken(std::string& line)
{
    std::string wordOfLine = "";
    for (int i = 0; i < line.size(); ++i) {
        if (isSimbol(line[i])) {
            if (wordOfLine.size() == 0) {
                continue;
            } else {
                codeLine.push_back(wordOfLine);
                wordOfLine.clear();
            }
            if (line[i] != ' ') {
                wordOfLine += line[i];
                codeLine.push_back(wordOfLine);
                wordOfLine.clear();
            }
        } else {
            wordOfLine += line[i];
        }
    }
    if (wordOfLine.size() != 0) {
        codeLine.push_back(wordOfLine);
    }
}

bool isNamesClassOrStruct()
{
    if (codeLine[0] == "class" || codeLine[0] == "struct")
        return true;
    return false;
}

bool parseClassName(bool wrongName = true)
{
    if (codeLine[1][0] >= 'A' && codeLine[1][0] <= 'Z') {
            return wrongName;
    }
    if (codeLine[1][0] >= 'a' || codeLine[1][0] <= 'z') {
        codeLine[1][0] -= 32;
    } else {
        codeLine[1] = codeLine[1].substr(1);
        parseClassName(false);
    }
    return false;
}

void checkNamesClassOrStruct(int indexLine)
{
    type.push_back(codeLine[1]);
    if (!parseClassName()) {
        wrongAndRightName[type[type.size() - 1]] = codeLine[1];
        std::cout << "line:" << indexLine << ": error: Maybe you wanted to write " << codeLine[1] << "?" << std::endl;
    }
}

bool isTypeTwoAnnouncement(std::string& str)
{
    for (const auto& elem : typeFirstWord) {
        if (elem == str) {
            return true;
        }
    }
    return false;
}

bool isTypeAnnouncement(std::string& str)
{
    for (const auto& elem : type) {
        if (elem == str) {
            return true;
        }
    }
    return false;
}

void parseName(std::string& name, int indexLine)
{
    if (name.size() == 1) {
        if (!isSimbol(name[0])) {
            std::cout << "line:" << indexLine << ": error: The name of a function or variable cannot be a single character " << std::endl;
            std::string oldName = name;
            name = name + (name[0] -= 32);
            wrongAndRightName[oldName] = name;
        }
    } else {
        std::string oldName = name;
        if (name[0] >= 'A' && name[0] <= 'Z') {
            name[0] += 32;
        }
        for (int  i = 0; i < name.size(); ++i) {
            if (name[i] == '_') {
                if ((i < name.size() - 1) && name[i + 1] >= 'a' && name[i + 1] <= 'z') {
                    name[i + 1] -= 32;
                }
                name = name.substr(0, i) + name.substr(i + 1);
            }
        }
        if (oldName != name) {
            std::cout << "line:" << indexLine << ": error: Maybe you wanted to write " << name << " ?" << std::endl;
            wrongAndRightName[oldName] = name;
        }
    }
}

void checkNamesFunctionsOrVariables(int indexLine)
{
    for (int i = 0; i < codeLine.size(); ++i) {
        if (isTypeTwoAnnouncement(codeLine[i])) {
            if (isTypeAnnouncement(codeLine[i + 1])) {
                if (codeLine[i + 2] != "," || codeLine[i + 2] != ")" || codeLine[i + 2] != ">") {
                    parseName(codeLine[i + 2], indexLine);
                }
            } else {
                if (codeLine[i + 1] != "," || codeLine[i + 1] != ")" || codeLine[i + 2] != ">") {
                    parseName(codeLine[i + 1], indexLine);
                }
            }
        } else if (isTypeAnnouncement(codeLine[i])) {
            if (codeLine[i + 1] != "," || codeLine[i + 1] != ")" || codeLine[i + 2] != ">") {
                parseName(codeLine[i + 1], indexLine);
            }
        }
    }
}

void checkAndchangeWrongNames(int indxeLine)
{
    for (auto& word : codeLine) {
        if (wrongAndRightName.find(word) != wrongAndRightName.end()) {
            word = wrongAndRightName[word];
            std::cout << "line:" << indxeLine << ": error: Maybe you wanted to write " << word << " ?" << std::endl;
        }
    }
}

void withParseToken(std::string& line)
{
    std::string wordOfLine = "";
    int indexTokenElem = 0;
    int indexChangedLine = 0;
    for (int i = 0; i < line.size(); ++i) {
        if (isSimbol(line[i])) {
            if (wordOfLine.size() == 0) {
                ++indexChangedLine;
                continue;
            } else {
                std::string startLine = line.substr(0, indexChangedLine);
                std::string endLine = line.substr(indexChangedLine + wordOfLine.size());
                line = startLine + codeLine[indexTokenElem] + endLine;
                indexChangedLine= indexChangedLine + codeLine[indexTokenElem].size() + 1;
                ++indexTokenElem;
                wordOfLine.clear();
                i = indexChangedLine - 1;
            }
            if (line[i] != ' ') {
                ++indexTokenElem;
            }
        } else {
            wordOfLine += line[i];
        }
    }
    if (wordOfLine.size() != 0) {
        if (indexChangedLine < line.size()) {
        line = line.substr(0, indexChangedLine) + codeLine[indexTokenElem] + line.substr(indexChangedLine + wordOfLine.size());
        }
    }
}

void testStart(std::string& fileName)
{
    std::ifstream righ(fileName);
    std::ofstream writ("new_" + fileName);
    std::string currentLine;
    if (righ.is_open()) {
        long int numberOfLine = 1;
        while (std::getline(righ, currentLine)) {
            parseToken(currentLine);
            checkAndChangeNamespaceWord(numberOfLine);
            if (isNamesClassOrStruct()) {
                checkNamesClassOrStruct(numberOfLine);
            } else if (isTypeAnnouncement(codeLine[0]) || isTypeTwoAnnouncement(codeLine[0])) {
                checkNamesFunctionsOrVariables(numberOfLine);
            }
            checkAndchangeWrongNames(numberOfLine);
            withParseToken(currentLine);
            if (codeLine.size() > 2 && codeLine[1] == "namespace") {
                std::cout << "line:" << numberOfLine << ": error: What???????" << std::endl;
            } else {
                writ << currentLine;
                writ << '\n';
            }
            codeLine.clear();
            ++numberOfLine;
            currentLine.clear();
        }
    } else {
        std::cout << "The file could not be opened" << std::endl;
        abort();
    }
    righ.close();
    writ.close();
}


int main(int argc, char** argv)
{
    if (argc == 1) {
        std::cout << "error: no input file" << std::endl;
        abort();
    } else if (argc > 2){
        std::cout << "error: too many arguments" << std::endl;
        abort();
    }
    std::string fileName = argv[1];
    size_t findPoint = fileName.find(".");
    if(findPoint != std::string::npos) {
        if(fileName.substr(findPoint + 1) != "cpp") {
            std::cout << "Wrong file extention" << std::endl;
            abort();
        }
    }
    testStart(fileName);
    return 0;
}
