#include <iostream>
#include <map>
#include <fstream>
#include <cctype>
#include <map>
#include "lex.h"
#include <cstring>



using namespace std;

// Mapping of keywords
map<string, Token> keywordMap = {
    {"if", IF},
    {"IF", IF},
    {"else", ELSE},
    {"ELSE", ELSE},
    {"print", PRINT},
    {"PRINT", PRINT},
    {"int", INT},
    {"INT", INT},
    {"float", FLOAT},
    {"FLOAT", FLOAT},
    {"char", CHAR},
    {"CHAR", CHAR},
    {"string", STRING},
    {"STRING", STRING},
    {"bool", BOOL},
    {"BOOL", BOOL},
    {"program", PROGRAM},
    {"PROGRAM", PROGRAM},
    {"true", TRUE},
    {"TRUE", TRUE},
    {"false", FALSE},
    {"FALSE", FALSE},
    // Add other keywords if needed
};
//LexItem getNextToken(istream& in, int & linenum){
//    return LexItem;
//}
// functions
LexItem id_or_kw(const string& lexeme, int linenum)
{
    
    auto it = keywordMap.find(lexeme);
    // If not found
    if (it != keywordMap.end()) {
        if((it->second == TRUE || it->second == FALSE) || (it->second == true || it->second == false)) {
            // True false
            return LexItem(BCONST, lexeme, linenum);
        }
        // Other keywords
        return LexItem(it->second, lexeme, linenum);
    }
    // If not should be Identifier
    return LexItem(IDENT, lexeme, linenum);
}

LexItem getNextToken(istream& in,int& lineNum)
{
    char character;
    while(in.get(character))
    {
        // Skipping spaces while counting lines.
        if (isspace(character))
        {
            if (character == '\n') lineNum++;
            continue;
        }
        
        // Single line comments
        if (character == '/' && in.peek() == '/')
        {
            while(character != '\n') in.get(character);
            lineNum++;
            continue;
        }
        
        // Multi Line Comments
        if (character == '/' && in.peek() == '*')
        {
            while(in.get(character))
            {
                if(character == '*' && in.peek() == '/')
                {
                    // To get '/'
                    in.get(character);
                    break;
                }
                if (character == '\n') lineNum++;
            }
            continue;
        }
        // Logic identifiers, constant, etc.
        
        // String literals
        if (character == '"')
        {
            string lexeme;
            char nextChar;
            lexeme += character;
            while(in.get(nextChar))
            {
                if(nextChar == '"')
                    return LexItem(SCONST, lexeme, lineNum); // When Double Quote
                if(nextChar == '\n')
                    return LexItem(ERR, " Invalid string constant " + lexeme, lineNum);
                lexeme+= nextChar;
            }
            // If end of file reached without closing
            return LexItem(ERR, "String is not closed!!", lineNum);
        }
        
        // Characters
      if (character == '\'') 
      {
          string lexeme; // Initialize lexeme to store character constant
          char nextChar;

          // Read the next character
          in.get(nextChar);

          // Check if the next character is the end of input or whitespace
          if (nextChar == EOF || isspace(nextChar)) 
          {
              lexeme = "New line is an invalid character constant.";
              return LexItem(ERR, lexeme, lineNum); // Error: No character after opening quote
          }

          // Add the character to lexeme
          lexeme += nextChar;
          
          // Read the next character again to check for closing symbol
          in.get(nextChar);
          
          // Check for closing symbol
          if (nextChar == '\'') 
          {
              return LexItem(CCONST, lexeme, lineNum); // Valid character constant
          } 
          else
          {
              // If we reach here, it means there's no closing quote
              lexeme += nextChar; // Add the character that wasn't a closing quote
              lexeme = " Invalid character constant '" + lexeme + "'";
              return LexItem(ERR, lexeme, lineNum); // Error: Invalid character constant
          }
      }
        
        
        // Identifiers
        if (isalpha(character) || character == '_')
        {
            // Concatenating single character with empty string
            string lexeme = string() + character;
            while(in.peek() && (isalnum(in.peek()) || in.peek() == '_')) lexeme += in.get();
            return id_or_kw(lexeme, lineNum);
        }
        
        //  Int or real const
        if (isdigit(character) || character == '.' || character == '+' || character == '-')
        {
            string lexeme(1, character);
            bool isReal = (character == '.');  // Track if it's a real number
            bool hasSign = (character == '+' || character == '-');  // Track if there's a sign
            bool isValid = true;  // Track if the format is valid

            // Look ahead to see if the next character forms an assignment operator (+= or -=)
            if (hasSign && in.peek() == '=') {
                in.get();  // Consume '='
            //
                return (character == '+') ? LexItem(ADDASSOP, "+=", lineNum)
                                          : LexItem(SUBASSOP, "-=", lineNum);
            }

            // Handle standalone + or - (not followed by a digit or dot)
            if (hasSign && !isdigit(in.peek()) && in.peek() != '.') {
                return (character == '+') ? LexItem(PLUS, "+", lineNum)
                                          : LexItem(MINUS, "-", lineNum);
            }
                
            
            // Read the rest of the number
            while (isdigit(in.peek()) || in.peek() == '.') {
                char next = in.get();
                if (next == '.') {
                    if (isReal) {  // Multiple dots indicate an error
                        isValid = false;
                        break;
                    }
                    isReal = true;
                }
                lexeme += next;
            }
            
            if (lexeme == ".") return LexItem(DOT, ".", lineNum);

            // If the number is invalid, return an error
            if (!isValid) {
                return LexItem(ERR, lexeme, lineNum);
            }

            // Return the appropriate token based on type
            return isReal ? LexItem(RCONST, lexeme, lineNum) : LexItem(ICONST, lexeme, lineNum);
        }


        if (character == '+') {
            if (in.peek() == '=') {
                in.get();
                return LexItem(ADDASSOP, "+=", lineNum);
            }
            return LexItem(PLUS, "+", lineNum);
        }
        
        if (character == '-') {
            if (in.peek() == '=') {
                in.get();
                return LexItem(SUBASSOP, "-=", lineNum);
            }
            return LexItem(MINUS, "-", lineNum);
        }

        if (character == '*') {
            if (in.peek() == '=') {
                in.get();
                return LexItem(MULASSOP, "*=", lineNum);
            }
            return LexItem(MULT, "*", lineNum);
        }

        if (character == '/') {
            if (in.peek() == '=') {
                in.get();
                return LexItem(DIVASSOP, "/=", lineNum);
            }
            return LexItem(DIV, "/", lineNum);
        }

        if (character == '=') {
            if (in.peek() == '=') {
                in.get();
                return LexItem(EQ, "==", lineNum);
            }
            return LexItem(ASSOP, "=", lineNum);
        }

        if (character == '!') {
            if (in.peek() == '=') {
                in.get();
                return LexItem(NEQ, "!=", lineNum);
            }
            return LexItem(NOT, "!", lineNum);
        }

        if (character == '&') {
            if (in.peek() == '&') {
                in.get();
                return LexItem(AND, "&&", lineNum);
            }
            return LexItem(ERR, "&", lineNum);
        }

        if (character == '|') {
            if (in.peek() == '|') {
                in.get();
                return LexItem(OR, "||", lineNum);
            }
            return LexItem(ERR, "|", lineNum);
        }
        if (character == '%') {
            if (in.peek() == '=') {
                in.get();
                return LexItem(REMASSOP, "%=", lineNum);
            }
            return LexItem(REM, "%", lineNum);
        }
      
        // Delimeters and Operators
        switch(character)
        {
            case '<': return LexItem(LTHAN, "<", lineNum);
            case '>': return LexItem(GTHAN, ">", lineNum);
            case '(': return LexItem(LPAREN, "(", lineNum);
            case ')': return LexItem(RPAREN, ")", lineNum);
            case '{': return LexItem(LBRACE, "{", lineNum);
            case '}': return LexItem(RBRACE, "}", lineNum);
            case ',': return LexItem(COMMA, ",", lineNum);
            case '.': return LexItem(DOT, ".", lineNum);
            case ';': return LexItem(SEMICOL, ";", lineNum);
        }
        // If nothing its error
        // Convert character to string
        string characterS = string() + character;
        return LexItem(ERR, characterS, lineNum);
    }
    // If reached to EOF
    return LexItem(DONE, "", lineNum);
}

ostream& operator<<(ostream& out, const LexItem& tok){
      // Need to filter them out based on token.
    Token tk = tok.GetToken();
    string lexeme = tok.GetLexeme();
    int lineNum = tok.GetLinenum();
    
    if(tk == IDENT){
        out << "IDENT: <" << lexeme <<">";
    }
    else if(tk == PROGRAM){
        out << "PROGRAM";
    }
    else if(tk == PRINT){
        out << "PRINT";
    }
    else if(tk == FLOAT){
        out << "FLOAT" ;
    }
    else if(tk == INT){
        out << "INT" ;
    }
    else if(tk == IF){
        out << "IF";
    }
    else if(tk == ELSE){
        out << "ELSE";
    }
    else if(tk == CHAR){
        out << "CHAR";
    }
    else if(tk == STRING){
        out << "STRING" ;
    }
    else if(tk == BOOL){
        out << "BOOL";
    }
    else if(tk == TRUE){
        out << "TRUE";
    }
    else if(tk == FALSE){
        out << "FALSE";
    }
    else if(tk == ICONST){
        out << "ICONST: (" << lexeme << ")";
        
    }
    else if(tk == RCONST){
        out << "RCONST: (" << lexeme << ")";
    }
    else if(tk == SCONST){
        out << "SCONST: " << lexeme <<"\"";
    }
    else if(tk == BCONST){
        out << "BCONST: (" << lexeme << ")";
    }
    else if(tk == CCONST){
        out << "CCONST: '" << lexeme << "'";
    }
    else if(tk == PLUS){
        out << "PLUS";
    }
    else if(tk == MINUS){
        out << "MINUS";
    }
    else if(tk == MULT){
        out << "MULT";
    }
    else if(tk == DIV){
        out << "DIV";
    }
    else if(tk == ASSOP){
        out << "ASSOP";
    }
    else if(tk == EQ){
        out << "EQ";
    }
    else if(tk == NEQ){
        out << "NEQ";
    }
    else if(tk == ADDASSOP){
        out << "ADDASSOP";
    }
    else if(tk == SUBASSOP){
        out << "SUBASSOP";
    }
    else if(tk == MULASSOP){
        out << "MULASSOP";
    }
    else if(tk == DIVASSOP){
        out << "DIVASSOP";
    }
    else if(tk == REMASSOP){
        out << "REMASSOP";
    }
    else if(tk == GTHAN){
        out << "GTHAN";
    }
    else if(tk == LTHAN){
        out << "LTHAN";
    }
    else if(tk == AND){
        out << "AND";
    }
    else if(tk == OR){
        out << "OR";
    }
    else if(tk == NOT){
        out << "NOT";
    }
    else if(tk == REM){
        out << "REM";
    }
    else if(tk == COMMA){
        out << "COMMA" ;
    }
    else if(tk == SEMICOL){
        out << "SEMICOL" ;
    }
    else if(tk == LPAREN){
        out << "LPAREN: [" << lexeme << "]" ;
    }
    else if(tk == RPAREN){
        out << "RPAREN: [" << lexeme << "]" ;
    }
    else if(tk == DOT){
        out << "DOT";
    }
    else if(tk == LBRACE){
        out << "LBRACE: [" << lexeme << "]" ;
    }
    else if(tk == RBRACE){
        out << "RBRACE: [" << lexeme << "]" ;
    }
    else if(tk == ERR)
    {
        out << "ERR: In line " << lineNum+1 << ", Error message {" << lexeme << "}" << endl;
    }
    else{
        out << "ERROR!! " << lexeme;
    }
    return out;
}
