#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cctype>

// Token types
enum class TokenType {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    LPAREN,
    RPAREN,
    END_OF_FILE
};

// Token structure
struct Token {
    TokenType type;
    double value;
    
    Token(TokenType t, double v = 0) : type(t), value(v) {}
};

// Lexer class - converts input string to tokens
class Lexer {
private:
    std::string input;
    size_t position;
    char current_char;
    
    void advance() {
        position++;
        if (position >= input.length()) {
            current_char = '\0';
        } else {
            current_char = input[position];
        }
    }
    
    void skip_whitespace() {
        while (current_char != '\0' && std::isspace(current_char)) {
            advance();
        }
    }
    
    double number() {
        std::string num_str = "";
        while (current_char != '\0' && (std::isdigit(current_char) || current_char == '.')) {
            num_str += current_char;
            advance();
        }
        return std::stod(num_str);
    }
    
public:
    Lexer(const std::string& text) : input(text), position(0) {
        current_char = input.empty() ? '\0' : input[0];
    }
    
    Token get_next_token() {
        while (current_char != '\0') {
            if (std::isspace(current_char)) {
                skip_whitespace();
                continue;
            }
            
            if (std::isdigit(current_char) || current_char == '.') {
                return Token(TokenType::NUMBER, number());
            }
            
            if (current_char == '+') {
                advance();
                return Token(TokenType::PLUS);
            }
            
            if (current_char == '-') {
                advance();
                return Token(TokenType::MINUS);
            }
            
            if (current_char == '*') {
                advance();
                return Token(TokenType::MULTIPLY);
            }
            
            if (current_char == '/') {
                advance();
                return Token(TokenType::DIVIDE);
            }
            
            if (current_char == '(') {
                advance();
                return Token(TokenType::LPAREN);
            }
            
            if (current_char == ')') {
                advance();
                return Token(TokenType::RPAREN);
            }
            
            throw std::runtime_error("Invalid character: " + std::string(1, current_char));
        }
        
        return Token(TokenType::END_OF_FILE);
    }
};

// Abstract Syntax Tree node types
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual double evaluate() = 0;
};

class NumberNode : public ASTNode {
private:
    double value;
    
public:
    NumberNode(double val) : value(val) {}
    
    double evaluate() override {
        return value;
    }
};

class BinaryOpNode : public ASTNode {
private:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    TokenType op;
    
public:
    BinaryOpNode(std::unique_ptr<ASTNode> l, TokenType operation, std::unique_ptr<ASTNode> r)
        : left(std::move(l)), op(operation), right(std::move(r)) {}
    
    double evaluate() override {
        double left_val = left->evaluate();
        double right_val = right->evaluate();
        
        switch (op) {
            case TokenType::PLUS:
                return left_val + right_val;
            case TokenType::MINUS:
                return left_val - right_val;
            case TokenType::MULTIPLY:
                return left_val * right_val;
            case TokenType::DIVIDE:
                if (right_val == 0) {
                    throw std::runtime_error("Division by zero");
                }
                return left_val / right_val;
            default:
                throw std::runtime_error("Unknown binary operator");
        }
    }
};

class UnaryOpNode : public ASTNode {
private:
    std::unique_ptr<ASTNode> operand;
    TokenType op;
    
public:
    UnaryOpNode(TokenType operation, std::unique_ptr<ASTNode> node)
        : op(operation), operand(std::move(node)) {}
    
    double evaluate() override {
        double val = operand->evaluate();
        
        switch (op) {
            case TokenType::PLUS:
                return val;
            case TokenType::MINUS:
                return -val;
            default:
                throw std::runtime_error("Unknown unary operator");
        }
    }
};

// Parser class - builds AST from tokens
class Parser {
private:
    Lexer lexer;
    Token current_token;
    
    void eat(TokenType token_type) {
        if (current_token.type == token_type) {
            current_token = lexer.get_next_token();
        } else {
            throw std::runtime_error("Unexpected token");
        }
    }
    
    // Grammar rules (using recursive descent parsing)
    // expr: term ((PLUS | MINUS) term)*
    // term: factor ((MULTIPLY | DIVIDE) factor)*
    // factor: (PLUS | MINUS) factor | NUMBER | LPAREN expr RPAREN
    
    std::unique_ptr<ASTNode> factor() {
        Token token = current_token;
        
        if (token.type == TokenType::PLUS) {
            eat(TokenType::PLUS);
            return std::make_unique<UnaryOpNode>(TokenType::PLUS, factor());
        } else if (token.type == TokenType::MINUS) {
            eat(TokenType::MINUS);
            return std::make_unique<UnaryOpNode>(TokenType::MINUS, factor());
        } else if (token.type == TokenType::NUMBER) {
            eat(TokenType::NUMBER);
            return std::make_unique<NumberNode>(token.value);
        } else if (token.type == TokenType::LPAREN) {
            eat(TokenType::LPAREN);
            auto node = expr();
            eat(TokenType::RPAREN);
            return node;
        }
        
        throw std::runtime_error("Invalid syntax");
    }
    
    std::unique_ptr<ASTNode> term() {
        auto node = factor();
        
        while (current_token.type == TokenType::MULTIPLY || current_token.type == TokenType::DIVIDE) {
            Token token = current_token;
            if (token.type == TokenType::MULTIPLY) {
                eat(TokenType::MULTIPLY);
            } else if (token.type == TokenType::DIVIDE) {
                eat(TokenType::DIVIDE);
            }
            
            node = std::make_unique<BinaryOpNode>(std::move(node), token.type, factor());
        }
        
        return node;
    }
    
    std::unique_ptr<ASTNode> expr() {
        auto node = term();
        
        while (current_token.type == TokenType::PLUS || current_token.type == TokenType::MINUS) {
            Token token = current_token;
            if (token.type == TokenType::PLUS) {
                eat(TokenType::PLUS);
            } else if (token.type == TokenType::MINUS) {
                eat(TokenType::MINUS);
            }
            
            node = std::make_unique<BinaryOpNode>(std::move(node), token.type, term());
        }
        
        return node;
    }
    
public:
    Parser(const std::string& text) : lexer(text), current_token(TokenType::END_OF_FILE) {
        current_token = lexer.get_next_token();
    }
    
    std::unique_ptr<ASTNode> parse() {
        auto node = expr();
        if (current_token.type != TokenType::END_OF_FILE) {
            throw std::runtime_error("Unexpected token at end of expression");
        }
        return node;
    }
};
// Compiler class - combines lexer, parser, and evaluator
class ArithmeticCompiler {
public:
    double compile_and_evaluate(const std::string& expression) {
        try {
            Parser parser(expression);
            auto ast = parser.parse();
            return ast->evaluate();
        } catch (const std::exception& e) {
            throw std::runtime_error("Compilation error: " + std::string(e.what()));
        }
    }
    
    void print_tokens(const std::string& expression) {
        std::cout << "Tokens for \"" << expression << "\":" << std::endl;
        Lexer lexer(expression);
        Token token = lexer.get_next_token();
        
        while (token.type != TokenType::END_OF_FILE) {
            switch (token.type) {
                case TokenType::NUMBER:
                    std::cout << "NUMBER(" << token.value << ") ";
                    break;
                case TokenType::PLUS:
                    std::cout << "PLUS ";
                    break;
                case TokenType::MINUS:
                    std::cout << "MINUS ";
                    break;
                case TokenType::MULTIPLY:
                    std::cout << "MULTIPLY ";
                    break;
                case TokenType::DIVIDE:
                    std::cout << "DIVIDE ";
                    break;
                case TokenType::LPAREN:
                    std::cout << "LPAREN ";
                    break;
                case TokenType::RPAREN:
                    std::cout << "RPAREN ";
                    break;
                default:
                    break;
            }
            token = lexer.get_next_token();
        }
        std::cout << "EOF" << std::endl << std::endl;
    }
};

int main() {
    ArithmeticCompiler compiler;
    std::string expression;
    
    std::cout << "=== Simple Arithmetic Expression Compiler ===" << std::endl;
    std::cout << "Supports: +, -, *, /, parentheses, and decimal numbers" << std::endl;
    std::cout << "Enter 'quit' to exit, 'tokens <expr>' to see tokenization" << std::endl;
    std::cout << "Examples: 2 + 3 * 4, (10 - 5) / 2.5, -3 + 4" << std::endl << std::endl;
    
    while (true) {
        std::cout << "Enter expression: ";
        std::getline(std::cin, expression);
        
        if (expression == "quit") {
            break;
        }
        
        if (expression.substr(0, 6) == "tokens") {
            if (expression.length() > 7) {
                std::string expr = expression.substr(7);
                compiler.print_tokens(expr);
            } else {
                std::cout << "Usage: tokens <expression>" << std::endl;
            }
            continue;
        }
        
        if (expression.empty()) {
            continue;
        }
        
        try {
            double result = compiler.compile_and_evaluate(expression);
            std::cout << "Result: " << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    std::cout << "Goodbye!" << std::endl;
    return 0;
}