#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <windows.h>

using namespace std;

const int COLOR_KEYWORD = 9;
const int COLOR_NUMERO = 11;
const int COLOR_STRING = 10;
const int COLOR_IDENT = 7;
const int COLOR_DELIM = 8;
const int COLOR_OP = 14;
const int COLOR_ERROR = 12;

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
    if (palabra.empty())
        return false;

    for (int i = 0; i < palabra.length(); i++) {
        if (!isdigit(palabra[i]))
            return false;
    }
    return true;
}

bool esIdentificador(string palabra) {
    if (palabra.empty())
        return false;

    if (!isalpha(palabra[0]) && palabra[0] != '_')
        return false;

    for (int i = 1; i < palabra.length(); i++) {
        if (!isalnum(palabra[i]) && palabra[i] != '_')
            return false;
    }
    return true;
}

bool esString(string palabra) {
    if (palabra.length() >= 2 &&
        palabra[0] == '"' &&
        palabra[palabra.length() - 1] == '"') {

        return true;
    }
    return false;
}

string obtenerTipo(string lexema) {
    if (esKeyword(lexema)) {
        return "KEYWORD";
    }
    else if (esNumero(lexema)) {
        return "NUMERO";
    }
    else if (esString(lexema)) {
        return "STRING";
    }
    else if (esIdentificador(lexema)) {
        return "IDENT";
    }
    else if (
        lexema == "+"  || lexema == "-"  ||
        lexema == "*"  || lexema == "/"  ||
        lexema == "="  || lexema == "==" ||
        lexema == "!=" || lexema == "<"  ||
        lexema == ">"  || lexema == "<=" ||
        lexema == ">=" || lexema == "++" ||
        lexema == "--"
    ) {
        return "OP";
    }
    else if (
        lexema == ";" ||
        lexema == "(" ||
        lexema == ")" ||
        lexema == "{" ||
        lexema == "}"
    ) {
        return "DELIM";
    }
    return "ERROR";
}


struct Token {
    string lexema;
    string tipo;
    int linea;
    int columna;
};

int main() {
    
    //HTML
    ofstream html("salida.html");
    html << "<!DOCTYPE html>";
    html << "<html>";
    html << "<head>";
    html << "<meta charset='UTF-8'>";
    html << "<title>Compilador Inclusivo</title>";
    
    //CSS
    html << "<style>";
    html << "body {";
    html << "font-family: Arial;";
    html << "background-color: #1e1e1e;";
    html << "color: white;";
    html << "padding: 20px;";
    html << "}";

    html << ".contenedor {";
    html << "display: flex;";
    html << "gap: 20px;";
    html << "}";

    html << ".panel {";
    html << "width: 50%;";
    html << "background: #2b2b2b;";
    html << "padding: 15px;";
    html << "border-radius: 10px;";
    html << "white-space: pre-wrap;";
    html << "box-shadow: 0 0 10px rgba(0,0,0,0.5);";
    html << "}";
    html << "th { background-color: #444; }";
    html << "tr:nth-child(even) { background-color: #2a2a2a; }";

    html << ".keyword { color: #569CD6; font-weight: bold; }";
    html << ".numero { color: #4EC9B0; }";
    html << ".string { color: #CE9178; }";
    html << ".ident { color: #DCDCAA; }";
    html << ".op { color: #FFD700; }";
    html << ".delim { color: #C586C0; }";
    html << ".error { color: #FF5555; font-weight: bold; }";
    
    html << "table {";
    html << "width: 100%;";
    html << "border-collapse: collapse;";
    html << "margin-top: 20px;";
    html << "}";
    
    html << "th, td {";
    html << "border: 1px solid gray;";
    html << "padding: 8px;";
    html << "}";
    
    html << "</style>";
    
    //BODY
    html << "</head>";
    html << "<body>";
    
    html << "<h1>Compilador Inclusivo</h1>";
    
    //PANELES
    html << "<div class='contenedor'>";
    //izquierda
    html << "<div class='panel'>";
    html << "<h2>Codigo Original</h2>";
    html << "<pre>";

    ifstream archivo("entrada.txt");
    if (!archivo) {
        cout << "Error al abrir el archivo entrada.txt" << endl;
        return 1;
    }

    string linea;
    vector<Token> tokens;
    int numeroLinea = 1;

    ///TOKENIZACIÓN
    while (getline(archivo, linea)) {
        string actual = "";
        html << linea << "<br>";
        
        for (int i = 0; i < linea.length(); i++) {
            char c = linea[i];

            // COMENTARIOS //
            if (c == '/' && i + 1 < linea.length() && linea[i + 1] == '/') {
                break;
            }
            // SALTOS DE LÍNEA, TABULACIONES Y ESPACIOS
            else if (c == ' ' || c == '\t') {
                if (actual != "") {
                    Token nuevo;
                    nuevo.lexema = actual;
                    nuevo.tipo = obtenerTipo(nuevo.lexema);
                    nuevo.linea = numeroLinea;
                    nuevo.columna = i - actual.length() + 1;
                    tokens.push_back(nuevo);

                    actual = "";
                }
                continue;
            }
            // STRINGS
            else if (c == '"') {
                if (actual != "") {
                    Token nuevo;
                    nuevo.lexema = actual;
                    nuevo.tipo = obtenerTipo(nuevo.lexema);
                    nuevo.linea = numeroLinea;
                    nuevo.columna = i - actual.length() + 1;
                    tokens.push_back(nuevo);

                    actual = "";
                }
                string cadena = "\"";
                i++;
                while (i < linea.length() && linea[i] != '"') {
                    cadena += linea[i];
                    i++;
                }
                if (i < linea.length()) {
                    cadena += "\"";
                }
                Token nuevo;
                nuevo.lexema = cadena;
                nuevo.tipo = obtenerTipo(nuevo.lexema);
                nuevo.linea = numeroLinea;
                nuevo.columna = i - cadena.length() + 2;
                tokens.push_back(nuevo);

                continue;
            }
            // OPERADORES Y DELIMITADORES
            else if (c=='+'||c=='-'||c=='*'||c=='/'||c=='='||c=='<'||c=='>'||c==';'||c=='('||c==')'||c=='{'||c=='}') {
                if (actual != "") {
                    Token nuevo;
                    nuevo.lexema = actual;
                    nuevo.tipo = obtenerTipo(nuevo.lexema);
                    nuevo.linea = numeroLinea;
                    nuevo.columna = i - actual.length() + 1;
                    tokens.push_back(nuevo);
                    actual = "";
                }

                // OPERADORES DOBLES
                if (i + 1 < linea.length()) {
                    string doble = string(1, c) + linea[i + 1];
                    if (
                        doble == "==" ||
                        doble == "!=" ||
                        doble == "<=" ||
                        doble == ">=" ||
                        doble == "++" ||
                        doble == "--"
                    ) {
                        Token nuevo;
                        nuevo.lexema = doble;
                        nuevo.tipo = obtenerTipo(nuevo.lexema);
                        nuevo.linea = numeroLinea;
                        nuevo.columna = i + 1;
                        tokens.push_back(nuevo);

                        i++;
                        continue;
                    }
                }
                // OPERADOR SIMPLE
                Token nuevo;
                nuevo.lexema = string(1, c);
                nuevo.tipo = obtenerTipo(nuevo.lexema);
                nuevo.linea = numeroLinea;
                nuevo.columna = i + 1;
                tokens.push_back(nuevo);
                continue;
            }
            // OTROS CARACTERES
            else {
                actual = actual + c;
            }
        }
        if (actual != "") {
                Token nuevo;
                nuevo.lexema = actual;
                nuevo.tipo = obtenerTipo(nuevo.lexema);
                nuevo.linea = numeroLinea;
                nuevo.columna = linea.length() - actual.length() + 1;
                tokens.push_back(nuevo);
        }
        numeroLinea++;
    }

    html << "</pre>";
    html << "</div>";

    cout << "\n=============================\n";
    cout << "           EDITOR\n";
    cout << "=============================\n";

    ///CLASIFICACIÓN DE COLORES
    int lineaActual = 1;

    //Panel derecha
    html << "<div class='panel'>";
    html << "<h2>Codigo Resaltado</h2>";
    html << "<pre>";

    for (int i = 0; i < tokens.size(); i++) {
        Token t = tokens[i];
        if (t.linea != lineaActual) {
            cout << endl;
            html << "<br>";
            lineaActual = t.linea;
        }

        string mostrar = t.lexema;
        if (mostrar == "<")
            mostrar = "&lt;";
        else if (mostrar == ">")
            mostrar = "&gt;";

        if (t.tipo == "KEYWORD") {
            setColor(COLOR_KEYWORD);
            cout << t.lexema;
            setColor(7);
            html << "<span class='keyword'> "
                 << mostrar
                 << "</span> ";
        }
        else if (t.tipo == "NUMERO") {
            setColor(COLOR_NUMERO);
            cout << t.lexema;
            setColor(7);
            html << "<span class='numero'> "
                 << mostrar
                 << "</span> ";
        }
        else if (t.tipo == "STRING") {
            setColor(COLOR_STRING);
            cout << t.lexema;
            setColor(7);
            html << "<span class='string'> "
                 << mostrar
                 << "</span> ";
        }
        else if (t.tipo == "IDENT") {
            setColor(COLOR_IDENT);
            cout << t.lexema;
            setColor(7);
            html << "<span class='ident'> "
                 << mostrar
                 << "</span> ";
        }
        else if (t.tipo == "OP") {
            setColor(COLOR_OP);
            cout << t.lexema;
            setColor(7);
            html << "<span class='op'> "
                 << mostrar
                 << "</span> ";
        }
        else if (t.tipo == "DELIM") {
            cout << t.lexema;
            html << "<span class='delim'>"
                 << mostrar
                 << "</span> ";
        }
        else {
            setColor(COLOR_ERROR);
            cout << "[ERROR] " << t.lexema;
            setColor(7);
            html << "<span class='error'>⚠ "
                 << mostrar
                 << "</span> ";
        }

        cout << " ";
    }
    cout << endl;
    html << "</pre>";
    html << "</div>";

    html << "</div>";


    cout << "\n=============================\n";
    cout << "          TOKENS\n";
    cout << "=============================\n\n";

    int lineaActualTokens = -1;

    html << "<hr>";
    html << "<h2>Tabla de Tokens</h2>";

    html << "<table>";

    html << "<tr>";
    html << "<th>Lexema</th>";
    html << "<th>Tipo</th>";
    html << "<th>Linea</th>";
    html << "<th>Columna</th>";
    html << "</tr>";

    for (int i = 0; i < tokens.size(); i++) {

        Token t = tokens[i];
        html << "<tr>";

        if (t.linea != lineaActualTokens) {
            lineaActualTokens = t.linea;
            cout << "\nLinea " << lineaActualTokens << ":\n";
        }

        if (t.tipo == "KEYWORD"){
            setColor(COLOR_KEYWORD);
            cout << "[KEYWORD] ";
        }
        else if (t.tipo == "NUMERO") {
            setColor(COLOR_NUMERO);
            cout << "[NUMERO]  ";
        }
        else if (t.tipo == "STRING") {
            setColor(COLOR_STRING);
            cout << "[STRING]  ";
        }
        else if (t.tipo == "IDENT") {
            setColor(COLOR_IDENT);
            cout << "[IDENT]   ";
        }
        else if (t.tipo == "OP") {
            setColor(COLOR_OP);
            cout << "[OP]      ";
        }
        else if (t.tipo == "DELIM") {
            setColor(COLOR_DELIM);
            cout << "[DELIM]   ";
        }
        else {
            setColor(COLOR_ERROR);
            cout << "[ERROR]   ";
        }

        cout << t.lexema;
        setColor(7);

        cout << "  (Linea: " << t.linea << ", Columna: " << t.columna << ")";
        cout << endl;

        html << "<td>" << t.lexema << "</td>";
        html << "<td>" << t.tipo << "</td>";
        html << "<td>" << t.linea << "</td>";
        html << "<td>" << t.columna << "</td>";

        html << "</tr>";
    }

    html << "</table>";
    

    cout << "\n===============================================\n";
    cout << "              TABLA DE TOKENS\n";
    cout << "===============================================\n\n";

    cout << "LEXEMA\t\tTIPO\t\tLINEA\tCOLUMNA\n";
    cout << "------------------------------------------------\n";

    for (int i = 0; i < tokens.size(); i++) {

        Token t = tokens[i];

        // COLORES SEGÚN TIPO
        if (t.tipo == "KEYWORD") {
            setColor(COLOR_KEYWORD);
        }
        else if (t.tipo == "NUMERO") {
            setColor(COLOR_NUMERO);
        }
        else if (t.tipo == "STRING") {
            setColor(COLOR_STRING);
        }
        else if (t.tipo == "IDENT") {
            setColor(COLOR_IDENT);
        }
        else if (t.tipo == "OP") {
            setColor(COLOR_OP);
        }
        else if (t.tipo == "DELIM") {
            setColor(COLOR_DELIM);
        }
        else {
            setColor(COLOR_ERROR);
        }

        cout << t.lexema;

        // ESPACIADO
        if (t.lexema.length() < 8)
            cout << "\t\t";
        else
            cout << "\t";

        cout << t.tipo << "\t\t";
        cout << t.linea << "\t";
        cout << t.columna << endl;

        setColor(7);
    }


    html << "</body>";
    html << "</html>";

    html.close();
    system("start salida.html");

    return 0;
}

