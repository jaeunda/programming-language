import sys
import re

symbol_table = {}

def prompt():
    print(">> ", end="")

def tokenize(line):
    tokens = re.findall(r"[a-zA-Z_]+|\d+|==|!=|<=|>=|[+\-*/=();{}<>]", line)
    return tokens

class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.pos = 0
        self.variables = {}
        self.output_buffer = []

    def current(self):
        return self.tokens[self.pos] if self.pos < len(self.tokens) else None

    def eat(self, expected=None):
        if self.current() is None:
            raise SyntaxError("Unexpected EOF")
        token = self.current()
        if expected and token != expected:
            raise SyntaxError(f"Expected '{expected}' but found '{token}'")
        self.pos += 1
        return token

    def parse_program(self):
        try:
            while self.current() == "integer":
                self.parse_declaration()

            while self.current():
                if self.current() == "integer":
                    raise SyntaxError("Declaration not allowed after statements")
                self.parse_statement()
            if self.output_buffer:
                print(" ".join(self.output_buffer))
        except SyntaxError:
            print("Syntax Error!")

    def parse_declaration(self):
        self.eat("integer")
        while True:
            var = self.eat()
            if not var.isalpha():
                raise SyntaxError("Invalid variable name")
            self.variables[var] = 0
            symbol_table[var] = 0
            if self.current() == ";":
                self.eat(";")
                break
            elif self.current() == ",":
                self.eat(",")
            else:
                raise SyntaxError("Expected ',' or ';'")

    def parse_statement(self):
        if self.current() == "integer":
            raise SyntaxError("Declaration not allowed after statements")
        if self.current() == "print":
            self.eat("print")
            val = self.parse_expression()
            self.eat(";")
            self.output_buffer.append(str(val))
        elif self.current() == "if":
            self.eat("if")
            self.eat("(")
            cond = self.parse_boolean()
            self.eat(")")
            self.eat("{")
            if cond:
                while self.current() != "}":
                    self.parse_statement()
                self.eat("}")
                if self.current() == "else":
                    self.eat("else")
                    self.eat("{")
                    while self.current() != "}":
                        self.skip_statement()
                    self.eat("}")
            else:
                while self.current() != "}":
                    self.skip_statement()
                self.eat("}")
                self.eat("else")
                self.eat("{")
                while self.current() != "}":
                    self.parse_statement()
                self.eat("}")
            self.eat(";")
        elif self.current() == "while":
            self.eat("while")
            self.eat("(")
            cond_pos = self.pos
            cond = self.parse_boolean()
            self.eat(")")
            self.eat("do")
            self.eat("{")
            body_pos = self.pos
            while cond:
                temp_pos = self.pos
                while self.current() != "}":
                    self.parse_statement()
                self.pos = cond_pos
                cond = self.parse_boolean()
                self.eat(")")
                self.eat("do")
                self.eat("{")
                self.pos = body_pos
            while self.current() != "}":
                self.skip_statement()
            self.eat("}")
            self.eat(";")
        else:
            var = self.eat()
            if var not in self.variables:
                raise SyntaxError("Undeclared variable")
            self.eat("=")
            val = self.parse_expression()
            self.variables[var] = val
            symbol_table[var] = val
            self.eat(";")

    def parse_boolean(self):
        lhs_var = self.eat()
        if lhs_var not in self.variables:
            raise SyntaxError("Undeclared variable")
        op = self.eat()
        rhs_var = self.eat()
        if rhs_var not in self.variables:
            raise SyntaxError("Undeclared variable")

        lhs = self.variables[lhs_var]
        rhs = self.variables[rhs_var]

        if op == "==":
            return lhs == rhs
        elif op == "!=":
            return lhs != rhs
        elif op == "<":
            return lhs < rhs
        elif op == ">":
            return lhs > rhs
        else:
            raise SyntaxError("Invalid boolean operator")

    def parse_expression(self):
        result = self.parse_term()
        while self.current() in {"+", "-"}:
            op = self.eat()
            rhs = self.parse_term()
            if op == "+":
                result += rhs
            else:
                result -= rhs
        return result

    def parse_term(self):
        result = self.parse_factor()
        while self.current() == "*":
            self.eat("*")
            rhs = self.parse_factor()
            result *= rhs
        return result

    def parse_factor(self):
        if self.current() == "(":
            self.eat("(")
            val = self.parse_expression()
            self.eat(")")
            return val
        elif self.current().isdigit():
            return int(self.eat())
        elif self.current().isalpha():
            var = self.eat()
            if var not in self.variables:
                raise SyntaxError("Undeclared variable")
            return self.variables[var]
        else:
            raise SyntaxError("Invalid factor")

    def skip_statement(self):
        # crude way to skip a statement for else/while skipping
        brace = 0
        while self.current() and (self.current() != ";" or brace != 0):
            if self.current() == "{":
                brace += 1
            elif self.current() == "}":
                brace -= 1
            self.pos += 1
        if self.current() == ";":
            self.pos += 1

while True:
    try:
        prompt()
        line = input()
        if not line.strip():
            break
        tokens = tokenize(line)
        parser = Parser(tokens)
        parser.parse_program()
    except EOFError:
        break

