
# 🧮 C++ Arithmetic Expression Compiler & Evaluator

A lightweight arithmetic expression compiler written in modern **C++** using a **recursive descent parser**. It tokenizes, parses, and evaluates mathematical expressions supporting `+`, `-`, `*`, `/`, parentheses, and unary operations.

---

## 🚀 Features

- 📌 Tokenizer (Lexer) for recognizing numbers, operators, and parentheses
- 🧠 Parser builds an Abstract Syntax Tree (AST) using recursive descent
- 🔁 Evaluator computes the result from the AST
- ➕ Supports floating-point numbers and operator precedence
- ✅ Handles unary `+` and `-` operators
- 📤 Token visualization mode (`tokens <expression>`)
- 🧪 Error handling for invalid expressions and division by zero

---

## 🧠 Example Input/Output

```bash
=== Simple Arithmetic Expression Compiler ===
Supports: +, -, *, /, parentheses, and decimal numbers
Enter 'quit' to exit, 'tokens <expr>' to see tokenization
Examples: 2 + 3 * 4, (10 - 5) / 2.5, -3 + 4

Enter expression: (2 + 3) * 4
Result: 20

Enter expression: tokens -5 + 8 / (2.5)
Tokens for "-5 + 8 / (2.5)":
MINUS NUMBER(5) PLUS NUMBER(8) DIVIDE LPAREN NUMBER(2.5) RPAREN EOF

Enter expression: quit
Goodbye!
````

---

## 📦 File Structure

```
/ExpressionCompiler
├── main.cpp            # Full implementation (lexer, parser, AST, evaluator)
└── README.md
```

---

## 🔧 How to Compile

### 🔹 Linux / macOS

```bash
g++ -std=c++17 -o Compiler main.cpp
./Compiler
```

### 🔹 Windows (MinGW / MSVC)

```bash
g++ -std=c++17 -o Compiler.exe main.cpp
Compiler.exe
```

---

## ✅ Supported Grammar

The program implements the following grammar rules:

```
expr   → term ((PLUS | MINUS) term)*
term   → factor ((MUL | DIV) factor)*
factor → (PLUS | MINUS) factor | NUMBER | LPAREN expr RPAREN
```

---

## 💡 Example Expressions

* `2 + 3 * 4` → `14`
* `(10 - 5) / 2.5` → `2`
* `-3 + 4` → `1`
* `tokens 3 + 4 * (2 - 1)` → Shows token stream

---

## ⚠️ Error Handling

The compiler gracefully reports:

* Unexpected characters
* Invalid syntax (e.g., unbalanced parentheses)
* Division by zero

---

## 📜 License

This project is free to use for learning, modification, or distribution. Attribution is appreciated but not required.

---


```
