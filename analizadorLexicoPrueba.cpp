#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <windows.h>

using namespace std;

///COLORES
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

///CLASIFICACIÓN
bool esKeyword(string palabra) {
    return (
        palabra == "int" ||
        palabra == "string" ||
        palabra == "float" ||
        palabra == "double" ||
        palabra == "char" ||
        palabra == "bool" ||
        palabra == "if" ||
        palabra == "else" ||
        palabra == "while" ||
        palabra == "for" ||
        palabra == "return" ||
        palabra == "void"
    );
}

bool esNumero(string palabra) {
    for (int i = 0; i < palabra.length(); i++) {
        if (!isdigit(palabra[i]))
            return false;
    }
    return true;
}

bool esIdentificador(string palabra) {
    if (!isalpha(palabra[0]) && palabra[0] != '_')
        return false;

    for (int i = 1; i < palabra.length(); i++) {
        if (!isalnum(palabra[i]) && palabra[i] != '_')
            return false;
    }
    return true;
}


int main() {
    ifstream archivo("entrada.txt");
    if (!archivo) {
        cout << "Error al abrir el archivo entrada.txt" << endl;
        return 1;
    }
    string linea;
    vector<vector<string>> lineas_tokens;

    ///TOKENIZACIÓN
    while (getline(archivo, linea)) {
        vector<string> tokens_linea;
        string actual = "";

        for (int i = 0; i < linea.length(); i++) {
            char c = linea[i];
            if (c == ' ' || c == '\t') {
                if (actual != "") {
                    tokens_linea.push_back(actual);
                    actual = "";
                }
            }
            else if (c=='+'||c=='-'||c=='*'||c=='/'||c=='='||c==';'||c=='('||c==')'||c=='{'||c=='}') {
                if (actual != "") {
                    tokens_linea.push_back(actual);
                    actual = "";
                }
                tokens_linea.push_back(string(1, c));
            }
            else {
                actual = actual + c;
            }
        }
        if (actual != "") {
            tokens_linea.push_back(actual);
        }

        lineas_tokens.push_back(tokens_linea);
    }

    cout << "\n=============================\n";
    cout << "           EDITOR\n";
    cout << "=============================\n";

    ///CLASIf DE COLORES
    for (int i = 0; i < lineas_tokens.size(); i++) {
        for (int j = 0; j < lineas_tokens[i].size(); j++) {
            string t = lineas_tokens[i][j];
            if (esKeyword(t)) {
                setColor(9);
                cout << t;
                setColor(7);
            }
            else if (esIdentificador(t)) {
                setColor(8);
                cout << t;
                setColor(7);
            }
            else if (t=="+"||t=="-"||t=="*"||t=="/"||t=="=") {
                setColor(14);
                cout << t;
                setColor(7);
            }
            else if (t==";"||t=="("||t==")"||t=="{"||t=="}") {
                cout << t;
            }
            else if (esNumero(t)) {
                setColor(11);
                cout << t;
                setColor(7);
            }
            else {
                setColor(12);
                cout << "⚠" << t;
                setColor(7);
            }

            cout << " ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "\n=============================\n";
    cout << "          TOKENS\n";
    cout << "=============================\n\n";

    for (int i = 0; i < lineas_tokens.size(); i++) {
        cout << "Linea " << i + 1 << ":\n";
        for (int j = 0; j < lineas_tokens[i].size(); j++) {
            string t = lineas_tokens[i][j];
            if (esKeyword(t)) {
                setColor(9);
                cout << "[KEYWORD] " << t << endl;
                setColor(7);
            }
            else if (esNumero(t)) {
                setColor(11);
                cout << "[NUMERO]  " << t << endl;
                setColor(7);
            }
            else if (esIdentificador(t)) {
                setColor(7);
                cout << "[IDENT]   " << t << endl;
            }
            else if (t=="+"||t=="-"||t=="*"||t=="/"||t=="=") {
                setColor(14);
                cout << "[OP]      " << t << endl;
                setColor(7);
            }
            else if (t==";"||t=="("||t==")"||t=="{"||t=="}") {
                setColor(8);
                cout << "[DELIM]   " << t << endl;
                setColor(7);
            }
            else {
                setColor(12);
                cout << "[ERROR]   " << t << endl;
                setColor(7);
            }
        }
        cout << endl;
    }

    return 0;
}
