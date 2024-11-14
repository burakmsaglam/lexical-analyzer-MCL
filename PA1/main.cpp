//
//  main.cpp
//  PA1
//
//  Created by Burak Mert Saglam on 10/17/24.
//

#include <iostream>
#include <map>
#include <fstream>
#include <cctype>
#include <map>
#include "lex.h"


using namespace std;


int main(int argc, const char * argv[]) {
    
    // Variables
    
    LexItem token;
    map<string, int> strMap, operatorsMap, idMap, keywordMap, boolMap, chrMap;
    map<Token, string> keywordMapOut;
    map<double,int> numMap;
    
    bool outAll = false, outNum = false, outStr = false, outChr = false, outID = false, outKw = false, outBool = false;
    int lineNum = 0, tokensC = 0;
    
    if(argc == 1){
        cout << "No specified input file." << endl;
        return 1;
    }
    string fileName = argv[1];
    ifstream inFile(fileName);
    
    if(!inFile.is_open()){
        cout << "CANNOT OPEN THE FILE " << fileName << endl;
        return 1;
    }
    else if (inFile.peek() == EOF){
        cout << "Empty file." << endl;
        return 1;
    }
    
    // Process command-line arguments
    bool fileProvided = false; // Track if filename is already given

    for (int i = 1; i < argc; ++i) {
        string argument = argv[i];

        if (argument == "-all") outAll = true;
        else if (argument == "-num") outNum = true;
        else if (argument == "-str") outStr = true;
        else if (argument == "-char") outChr = true;
        else if (argument == "-id") outID = true;
        else if (argument == "-kw") outKw = true;
        else if (argument == "-bool") outBool = true;
        else if (argument[0] != '-') {
            if (fileProvided) { // If a filename was already provided
                cout << "Only one file name is allowed." << endl;
                exit(1);
            }
            fileProvided = true; // Mark that a filename was given
        } else {
            cout << "Unrecognized flag {" << argument <<"}" << endl;
            exit(1);
        }
    }
    
    token = getNextToken(inFile, lineNum);
    while (token.GetToken() != DONE) {
        if (token.GetToken() == ERR) {
            // Displaying error message.
            cout << token << endl;
            // Stopping program.
            return 1;
        }
        
        // Where flag is all:
        if (outAll) cout << token << endl;
        tokensC++;  // Increment token count

        // Process token types
        switch (token.GetToken()) {
            case IDENT:
                idMap[token.GetLexeme()]++;
                break;
            case ICONST:
            case RCONST:
                numMap[stod(token.GetLexeme())]++;
                break;
            case SCONST:
                strMap[token.GetLexeme()]++;
                break;
            case CCONST:
                chrMap[token.GetLexeme()]++;
                break;
            case BOOL:
            case BCONST:
                boolMap[token.GetLexeme()]++;
                break;
            case PLUS: case MINUS: case MULT: case DIV:
            case ASSOP: case EQ: case NEQ: case GTHAN:
            case LTHAN: case AND: case OR: case NOT:
            case ADDASSOP: case SUBASSOP: case MULASSOP:
            case DIVASSOP: case REMASSOP:
                operatorsMap[token.GetLexeme()]++;
                break;
            case COMMA: case SEMICOL: case LPAREN: case RPAREN:
            case LBRACE: case RBRACE: case DOT:
                break;
            default:
                if (token.GetToken() >= IF && token.GetToken() <= PROGRAM) {
                    keywordMap[token.GetLexeme()]++;
                    keywordMapOut[token.GetToken()] = token.GetLexeme();
                }
                break;
        }

        // Update the token by calling getNextToken() to advance
        token = getNextToken(inFile, lineNum);
    }
    
    cout << "\nLines: " << lineNum << endl;
    cout << "Total Tokens: " << tokensC << endl;
    cout << "Identifiers and Keywords: " << idMap.size() + keywordMap.size() << endl;
    cout << "Numerics: " << numMap.size() << endl;
    cout << "Booleans: " << boolMap.size() << endl;
    cout << "Strings and Characters: " << strMap.size() + chrMap.size() << endl;
    
    // Process additional flags.
    if (outNum & !numMap.empty()) {
        cout << "NUMERIC CONSTANTS:" << endl;
        bool first = true;
        for (auto it = numMap.begin(); it != numMap.end(); ++it) {
            if (!first) {
                cout << ", ";
            }
            cout << it->first;
            first = false;
        }
        cout << endl;
    }
    if (outBool & !boolMap.empty()) {
        cout << "BOOLEAN CONSTANTS:" << endl;
        for (auto it = boolMap.begin(); it != boolMap.end(); ++it) {
            cout << it->first << endl;
        }
    }
    if (outChr & !chrMap.empty()) {
        cout << "CHARACTER CONSTANTS:" << endl;
        bool first = true;
        for (auto it = chrMap.begin(); it != chrMap.end(); ++it) {
            if (!first) {
                cout << ", ";
            }
            cout << "'" << it->first << "'";
            first = false;
        }
        cout << endl;
    }
    if (outStr & !strMap.empty()) {
        cout << "STRINGS:" << endl;
        bool first = true;
        for (auto it = strMap.begin(); it != strMap.end(); ++it) {
            if (!first) {
                cout << ", ";
            }
            cout << it->first << "\"";
            first = false;
        }
        cout << endl;
    }
    if (outID & !idMap.empty()) {
        cout << "IDENTIFIERS:" << endl;
        bool first = true;
        for (auto it = idMap.begin(); it != idMap.end(); ++it) {
            if(!first) cout << ", ";
            cout << it->first;
            first = false;
        }
        cout << endl;
     }

     if (outKw & !keywordMapOut.empty()) {
         cout << "KEYWORDS:" << endl;
         bool first = true;
         
         for (auto it = keywordMapOut.begin(); it != keywordMapOut.end(); ++it) {
             // Implementing comma
             if (!first) cout << ", ";
             cout << it->second;
             first = false;
         }
         cout << endl;
     }
     
   
    
    
    
    
    inFile.close();
    
    return 0;
}

