#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <windows.h>

using namespace std;

/*
const int COLOR_KEYWORD = 9;
const int COLOR_NUMERO  = 11;
const int COLOR_STRING  = 10;
const int COLOR_IDENT   = 7;
const int COLOR_DELIM   = 13;
const int COLOR_OP      = 14;
const int COLOR_ERROR   = 12;
const int COLOR_COMMENT = 2;
*/

// COLORES DE CONSOLA
const int COLOR_NEGRO         = 0;
const int COLOR_AZUL_OSCURO   = 1;
const int COLOR_VERDE_OSCURO  = 2;
const int COLOR_CIAN_OSCURO   = 3;
const int COLOR_ROJO_OSCURO   = 4;
const int COLOR_MAGENTA_OSCURO= 5;
const int COLOR_DORADO        = 6;
const int COLOR_GRIS_CLARO    = 7;
const int COLOR_GRIS_OSCURO   = 8;
const int COLOR_AZUL          = 9;
const int COLOR_VERDE         = 10;
const int COLOR_CELESTE       = 11;
const int COLOR_ROJO          = 12;
const int COLOR_MAGENTA       = 13;
const int COLOR_AMARILLO      = 14;
const int COLOR_BLANCO        = 15;

// COLORES
const int COLOR_KEYWORD = COLOR_AZUL;
const int COLOR_NUMERO  = COLOR_CELESTE;
const int COLOR_STRING  = COLOR_DORADO;
const int COLOR_IDENT   = COLOR_AMARILLO;
const int COLOR_OP      = COLOR_VERDE;
const int COLOR_DELIM   = COLOR_GRIS_CLARO;
const int COLOR_COMMENT = COLOR_GRIS_OSCURO;
const int COLOR_ERROR   = COLOR_MAGENTA;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// ==========================
// CLASIFICACION
// ==========================

bool esKeyword(string palabra) {
    return (
        palabra == "int"    ||
        palabra == "string" ||
        palabra == "float"  ||
        palabra == "double" ||
        palabra == "char"   ||
        palabra == "bool"   ||
        palabra == "if"     ||
        palabra == "else"   ||
        palabra == "while"  ||
        palabra == "for"    ||
        palabra == "return" ||
        palabra == "void"
    );
}

bool esNumero(string palabra) {
    if (palabra.empty()) return false;
    int digitos = 0;
    bool punto  = false;
    for (char c : palabra) {
        if (c == '.') {
            if (punto) return false;
            punto = true;
        } else if (isdigit(c)) {
            digitos++;
        } else {
            return false;
        }
    }
    return digitos > 0;
}

bool esIdentificador(string palabra) {
    if (palabra.empty()) return false;
    if (!isalpha(palabra[0]) && palabra[0] != '_')
        return false;
    for (int i = 1; i < (int)palabra.length(); i++) {
        if (!isalnum(palabra[i]) && palabra[i] != '_')
            return false;
    }
    return true;
}

bool esString(string palabra) {
    return (
        palabra.length() >= 2 &&
        palabra[0] == '"' &&
        palabra[palabra.length() - 1] == '"'
    );
}

string obtenerTipo(string lexema) {
    if (esKeyword(lexema))       return "KEYWORD";
    if (esNumero(lexema))        return "NUMERO";
    if (esString(lexema))        return "STRING";
    if (esIdentificador(lexema)) return "IDENT";

    if (
        lexema == "+"  || lexema == "-"  ||
        lexema == "*"  || lexema == "/"  ||
        lexema == "="  || lexema == "==" ||
        lexema == "!=" || lexema == "<"  ||
        lexema == ">"  || lexema == "<=" ||
        lexema == ">=" || lexema == "++" ||
        lexema == "--"
    ) return "OP";

    if (
        lexema == ";" ||
        lexema == "(" ||
        lexema == ")" ||
        lexema == "{" ||
        lexema == "}"
    ) return "DELIM";

    return "ERROR";
}

// ==========================
// TOKEN
// ==========================

struct Token {
    string lexema;
    string tipo;
    int    linea;
    int    columna;
};

// ==========================
// ANALISIS SINTACTICO
// ==========================

int indice = 0;
vector<string> erroresSintacticos;
vector<string> declaracionesCorrectas;

Token actualToken(vector<Token>& tokens) {
    if (indice < (int)tokens.size())
        return tokens[indice];
    return {"", "EOF", -1, -1};
}

void avanzar() {
    indice++;
}

// Saltear comentarios
void saltarComentarios(vector<Token>& tokens) {
    while (indice < (int)tokens.size() && actualToken(tokens).tipo == "COMMENT")
        avanzar();
}

bool consumir(vector<Token>& tokens, string esperado) {
    saltarComentarios(tokens);
    Token t = actualToken(tokens);
    if (t.lexema == esperado) {
        avanzar();
        return true;
    }
    setColor(COLOR_ERROR);
    cout << "\n=====================================\n";
    cout << "         ERROR SINTACTICO";
    cout << "\n=====================================\n";
    cout << "Se esperaba: " << esperado << endl;
    cout << "Se encontro: " << t.lexema << endl;
    cout << "Linea: "       << t.linea  << endl;
    cout << "Columna: "     << t.columna << endl;
    setColor(7);

    string err = "Se esperaba <b>" + esperado + "</b>, se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
    erroresSintacticos.push_back(err);
    avanzar();
    return false;
}

bool consumirTipo(vector<Token>& tokens, string tipo, string desc) {
    saltarComentarios(tokens);
    Token t = actualToken(tokens);
    if (t.tipo == tipo) {
        avanzar();
        return true;
    }
    setColor(COLOR_ERROR);
    cout << "\n=====================================\n";
    cout << "         ERROR SINTACTICO";
    cout << "\n=====================================\n";
    cout << "Se esperaba: " << desc     << endl;
    cout << "Se encontro: " << t.lexema << endl;
    cout << "Linea: "       << t.linea  << endl;
    cout << "Columna: "     << t.columna << endl;
    setColor(7);

    string err = "Se esperaba " + desc + ", se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
    erroresSintacticos.push_back(err);
    avanzar();
    return false;
}

// ─────────────────────────────────────────
// CONDICION:  IDENT/NUM  OP  IDENT/NUM
//             o simplemente IDENT/NUM
// ─────────────────────────────────────────
bool condicion(vector<Token>& tokens) {
    saltarComentarios(tokens);
    Token t = actualToken(tokens);

    // lado izquierdo
    if (t.tipo == "IDENT" || t.tipo == "NUMERO") {
        avanzar();
    } else {
        string err = "Se esperaba IDENT o NUMERO en condicion, se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
        erroresSintacticos.push_back(err);
        setColor(COLOR_ERROR);
        cout << "\nERROR: se esperaba IDENT o NUMERO en condicion, se encontro: " << t.lexema << endl;
        setColor(7);
        avanzar();
        return false;
    }

    // operador relacional (opcional, pero si existe lo consumimos)
    saltarComentarios(tokens);
    t = actualToken(tokens);
    if (t.tipo == "OP" && (
        t.lexema == "==" || t.lexema == "!=" ||
        t.lexema == "<"  || t.lexema == ">"  ||
        t.lexema == "<=" || t.lexema == ">=" ))
    {
        avanzar();
        // lado derecho
        saltarComentarios(tokens);
        t = actualToken(tokens);
        if (t.tipo == "IDENT" || t.tipo == "NUMERO") {
            avanzar();
        } else {
            string err = "Se esperaba IDENT o NUMERO tras operador, se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
            erroresSintacticos.push_back(err);
            avanzar();
            return false;
        }
    }
    return true;
}

// ─────────────────────────────────────────
// CUERPO:  { sentencias* }
// ─────────────────────────────────────────
bool sentencia(vector<Token>& tokens);   // forward

bool cuerpo(vector<Token>& tokens) {
    if (!consumir(tokens, "{")) return false;
    saltarComentarios(tokens);
    // parsear sentencias hasta llegar a '}'
    while (indice < (int)tokens.size() &&
           actualToken(tokens).lexema != "}" &&
           actualToken(tokens).tipo   != "EOF")
    {
        saltarComentarios(tokens);
        if (actualToken(tokens).lexema == "}") break;
        if (!sentencia(tokens)) {
            // recuperacion: avanzar hasta '}' o ';'
            while (indice < (int)tokens.size() &&
                   actualToken(tokens).lexema != "}" &&
                   actualToken(tokens).lexema != ";")
                avanzar();
            if (actualToken(tokens).lexema == ";") avanzar();
            if (actualToken(tokens).lexema == "}") break;
        }
    }
    return consumir(tokens, "}");
}

// ─────────────────────────────────────────
// DECLARACION:  TIPO IDENT [= VALOR] ;
// ─────────────────────────────────────────
bool declaracion(vector<Token>& tokens) {
    Token t = actualToken(tokens);

    if (t.lexema == "int" || t.lexema == "float" || t.lexema == "string") {
        avanzar();
    } else {
        string err = "Se esperaba TIPO (int/float/string), se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
        erroresSintacticos.push_back(err);
        setColor(COLOR_ERROR);
        cout << "\nERROR SINTACTICO: se esperaba TIPO, se encontro: " << t.lexema << endl;
        setColor(7);
        avanzar();
        return false;
    }

    t = actualToken(tokens);
    if (t.tipo == "IDENT") {
        avanzar();
    } else {
        string err = "Se esperaba IDENTIFICADOR, se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
        erroresSintacticos.push_back(err);
        avanzar();
        return false;
    }

    t = actualToken(tokens);
    if (t.lexema == "=") {
        avanzar();
        t = actualToken(tokens);
        if (t.tipo == "NUMERO" || t.tipo == "STRING" || t.tipo == "IDENT") {
            avanzar();
        } else {
            string err = "Se esperaba VALOR, se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
            erroresSintacticos.push_back(err);
            avanzar();
            return false;
        }
    }

    t = actualToken(tokens);
    if (t.lexema == ";") {
        avanzar();
        return true;
    } else {
        string err = "Se esperaba <b>;</b>, se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
        erroresSintacticos.push_back(err);
        avanzar();
        return false;
    }
}

// ─────────────────────────────────────────
// IF:  if ( condicion ) cuerpo [else cuerpo]
// ─────────────────────────────────────────
bool sentenciaIf(vector<Token>& tokens) {
    avanzar(); // consumir 'if'
    if (!consumir(tokens, "("))   return false;
    if (!condicion(tokens))       return false;
    if (!consumir(tokens, ")"))   return false;
    if (!cuerpo(tokens))          return false;

    saltarComentarios(tokens);
    Token t = actualToken(tokens);
    if (t.lexema == "else") {
        avanzar(); // consumir 'else'
        if (!cuerpo(tokens)) return false;
    }
    return true;
}

// ─────────────────────────────────────────
// WHILE:  while ( condicion ) cuerpo
// ─────────────────────────────────────────
bool sentenciaWhile(vector<Token>& tokens) {
    avanzar(); // consumir 'while'
    if (!consumir(tokens, "("))  return false;
    if (!condicion(tokens))      return false;
    if (!consumir(tokens, ")"))  return false;
    if (!cuerpo(tokens))         return false;
    return true;
}

// ─────────────────────────────────────────
// FOR:  for ( TIPO IDENT = VAL ; condicion ; IDENT OP ) cuerpo
//       Forma simplificada: for ( init ; cond ; inc ) cuerpo
//       init  -> declaracion sin ';' final   (ej: int i = 0)
//             o  asignacion                  (ej: i = 0)
//       cond  -> condicion
//       inc   -> IDENT OP  (ej: i++ / i--)
// ─────────────────────────────────────────
bool sentenciaFor(vector<Token>& tokens) {
    avanzar(); // consumir 'for'
    if (!consumir(tokens, "(")) return false;

    // ── init ──
    saltarComentarios(tokens);
    Token t = actualToken(tokens);

    if (t.lexema == "int" || t.lexema == "float" || t.lexema == "string") {
        // declaracion de variable (sin punto y coma final, lo consumimos nosotros)
        avanzar(); // tipo
        if (!consumirTipo(tokens, "IDENT", "IDENTIFICADOR")) return false;
        saltarComentarios(tokens);
        if (actualToken(tokens).lexema == "=") {
            avanzar();
            saltarComentarios(tokens);
            t = actualToken(tokens);
            if (t.tipo == "NUMERO" || t.tipo == "IDENT") avanzar();
            else {
                string err = "Se esperaba valor inicial en for, se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
                erroresSintacticos.push_back(err);
                avanzar(); return false;
            }
        }
    } else if (t.tipo == "IDENT") {
        // asignacion simple: i = 0
        avanzar();
        if (actualToken(tokens).lexema == "=") {
            avanzar();
            saltarComentarios(tokens);
            t = actualToken(tokens);
            if (t.tipo == "NUMERO" || t.tipo == "IDENT") avanzar();
            else {
                string err = "Se esperaba valor en asignacion de for, se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
                erroresSintacticos.push_back(err);
                avanzar(); return false;
            }
        }
    }

    if (!consumir(tokens, ";")) return false;

    // ── condicion ──
    if (!condicion(tokens))      return false;
    if (!consumir(tokens, ";"))  return false;

    // ── incremento: IDENT (OP | = EXPR) ──
    saltarComentarios(tokens);
    t = actualToken(tokens);
    if (t.tipo == "IDENT") {
        avanzar();
        saltarComentarios(tokens);
        t = actualToken(tokens);
        if (t.tipo == "OP") {
            avanzar(); // ++ / -- / etc.
        } else if (t.lexema == "=") {
            avanzar();
            saltarComentarios(tokens);
            t = actualToken(tokens);
            if (t.tipo == "IDENT" || t.tipo == "NUMERO") avanzar();
        }
    }

    if (!consumir(tokens, ")")) return false;
    if (!cuerpo(tokens))        return false;
    return true;
}

// ─────────────────────────────────────────
// VOID:  void IDENT ( ) cuerpo
// ─────────────────────────────────────────
bool sentenciaVoid(vector<Token>& tokens) {
    avanzar(); // consumir 'void'
    if (!consumirTipo(tokens, "IDENT", "NOMBRE DE FUNCION")) return false;
    if (!consumir(tokens, "("))  return false;
    if (!consumir(tokens, ")"))  return false;
    if (!cuerpo(tokens))         return false;
    return true;
}

// ─────────────────────────────────────────
// SENTENCIA (dispatcher)
// ─────────────────────────────────────────
bool sentencia(vector<Token>& tokens) {
    saltarComentarios(tokens);
    Token t = actualToken(tokens);

    if (t.lexema == "if")    return sentenciaIf(tokens);
    if (t.lexema == "while") return sentenciaWhile(tokens);
    if (t.lexema == "for")   return sentenciaFor(tokens);
    if (t.lexema == "void")  return sentenciaVoid(tokens);

    if (t.lexema == "int"    ||
        t.lexema == "float"  ||
        t.lexema == "string")
        return declaracion(tokens);

    // sentencia de expresion / return / etc.  → consumir hasta ';'
    while (indice < (int)tokens.size() &&
           actualToken(tokens).lexema != ";" &&
           actualToken(tokens).lexema != "}" &&
           actualToken(tokens).tipo   != "EOF")
        avanzar();
    if (actualToken(tokens).lexema == ";") avanzar();
    return true;
}

// ==========================
// MAIN
// ==========================

int main() {

    ofstream html("salida.html");

    html << "<!DOCTYPE html>";
    html << "<html>";
    html << "<head>";
    html << "<meta charset='UTF-8'>";
    html << "<title>Compilador Inclusivo</title>";

    // CSS
    html << "<style>";
    html << "body { font-family:Arial; background-color:#1e1e1e; color:white; padding:20px; }";
    html << ".contenedor { display:flex; gap:20px; }";
    html << ".panel { width:50%; background:#2b2b2b; padding:15px; border-radius:10px; white-space:pre-wrap; box-shadow:0 0 10px rgba(0,0,0,0.5); }";
    html << "table { width:100%; border-collapse:collapse; margin-top:20px; }";
    html << "th, td { border:1px solid gray; padding:8px; }";
    html << "th { background-color:#444; }";
    html << ".keyword { color:#569CD6; font-weight:bold; }";
    /*html << ".numero  { color:#4EC9B0; }";
    html << ".string  { color:#CE9178; }";
    html << ".ident   { color:#DCDCAA; }";
    html << ".op      { color:#FFD700; }";
    html << ".delim   { color:#C586C0; }";
    html << ".comment { color:#9E9E9E; font-style:italic; }";
    html << ".error   { color:#FF00FF; font-weight:bold; text-decoration:underline; }";*/
    html << ".keyword { color:#569CD6; font-weight:bold; }";      // Azul
    html << ".numero  { color:#4EC9B0; }";                        // Celeste
    html << ".string  { color:#FFD700; font-style:italic; }";     // Dorado
    html << ".ident   { color:#FFFF00; }";                        // Amarillo
    html << ".op      { color:#4CAF50; }";                        // Verde
    html << ".delim   { color:#D4D4D4; }";                        // Gris claro
    html << ".comment { color:#808080; font-style:italic; }";     // Gris oscuro
    html << ".error   { color:#FF00FF; font-weight:bold; text-decoration:underline; }"; // Magenta
    html << ".syntax-ok    { background:#1e3a1e; padding:10px; border-left:5px solid #4CAF50; margin-bottom:10px; border-radius:5px; }";
    html << ".syntax-error { background:#3a1e1e; padding:10px; border-left:5px solid #FF5555; margin-bottom:10px; border-radius:5px; }";
    html << "</style>";
    html << "</head>";
    html << "<body>";

    html << "<h1>Compilador Inclusivo</h1>";

    // LEYENDA
    html << "<h2>Leyenda de Tokens</h2>";
    html << "<p><span class='keyword'>KEYWORD</span> &rarr; Palabras reservadas</p>";
    html << "<p><span class='numero'>NUMERO</span>   &rarr; Numeros enteros y flotantes</p>";
    html << "<p><span class='string'>STRING</span>   &rarr; Cadenas de texto</p>";
    html << "<p><span class='ident'>IDENT</span>     &rarr; Identificadores</p>";
    html << "<p><span class='op'>OP</span>           &rarr; Operadores</p>";
    html << "<p><span class='delim'>DELIM</span>     &rarr; Delimitadores</p>";
    html << "<p><span class='comment'>COMMENT</span> &rarr; Comentarios</p>";
    html << "<p><span class='error'>ERROR</span>     &rarr; Error lexico</p>";

    /*// JUSTIFICACION
    html << "<h2>Justificacion de Colores</h2>";
    html << "<ul>";
    html << "<li><b style='color:#569CD6'>Azul:</b>    keywords importantes.</li>";
    html << "<li><b style='color:#4EC9B0'>Celeste:</b> numeros faciles de distinguir.</li>";
    html << "<li><b style='color:#CE9178'>Naranja:</b> strings resaltan texto.</li>";
    html << "<li><b style='color:#DCDCAA'>Amarillo:</b> identificadores visibles.</li>";
    html << "<li><b style='color:#FFD700'>Dorado:</b>  operadores importantes.</li>";
    html << "<li><b style='color:#C586C0'>Morado:</b>  delimitadores estructurales.</li>";
    html << "<li><b style='color:#6A9955'>Verde:</b>   comentarios auxiliares.</li>";
    html << "<li><b style='color:#FF5555'>Rojo:</b>    errores visibles inmediatamente.</li>";
    */

    html << "<h2>Justificacion de Colores</h2>";
    html << "<ul>";
    html << "<li><b style='color:#569CD6'>Azul:</b> palabras reservadas. "
            "Se eligio por su alta diferenciacion visual y porque permite identificar rapidamente las estructuras principales del lenguaje.</li>";
    html << "<li><b style='color:#4EC9B0'>Celeste:</b> numeros enteros y flotantes. "
            "Facilita distinguir los valores numericos de otros elementos del codigo fuente.</li>";
    html << "<li><b style='color:#FFD700'>Dorado:</b> cadenas de texto. "
            "Presenta un contraste perceptible respecto a las palabras reservadas e identificadores, favoreciendo su reconocimiento.</li>";
    html << "<li><b style='color:#FFFF00'>Amarillo:</b> identificadores. "
            "Mantiene buena visibilidad y facilita el seguimiento de variables y funciones durante la lectura del programa.</li>";
    html << "<li><b style='color:#4CAF50'>Verde:</b> operadores. "
            "Permite una lectura clara de las expresiones sin generar sobrecarga visual.</li>";
    html << "<li><b style='color:#D4D4D4'>Gris claro:</b> delimitadores. "
            "Al ser elementos estructurales de alta frecuencia, se representan con un color neutro para evitar distracciones visuales.</li>";
    html << "<li><b style='color:#808080'>Gris oscuro:</b> comentarios. "
            "Al corresponder a informacion auxiliar, se muestran con menor prominencia visual para reducir la carga cognitiva.</li>";
    html << "<li><b style='color:#FF00FF'>Magenta:</b> errores lexicos. "
            "Se eligio por su alta visibilidad para usuarios con deuteranopia y se complementa con estilos adicionales para no depender exclusivamente del color.</li>";
    html << "</ul>";

    // ==========================
    // APERTURA DE ARCHIVO
    // ==========================

    ifstream archivo("entrada.txt");

    if (!archivo) {
        cout << "Error al abrir entrada.txt" << endl;
        return 1;
    }

    vector<Token> tokens;

    string linea;
    int    numeroLinea        = 1;
    bool   enComentarioBloque = false;

    // ==========================
    // PANEL IZQUIERDO
    // ==========================

    html << "<div class='contenedor'>";
    html << "<div class='panel'>";
    html << "<h2>Codigo Original</h2>";
    html << "<pre style='tab-size:4;'>";

    vector<string> todasLineas;
    while (getline(archivo, linea)) {
        todasLineas.push_back(linea);
    }
    archivo.close();

    for (const string& l : todasLineas) {
        for (char ch : l) {
            if      (ch == ' ')  html << "&nbsp;";
            else if (ch == '\t') html << "&nbsp;&nbsp;&nbsp;&nbsp;";
            else                 html << ch;
        }
        html << "<br>";
    }

    html << "</pre>";
    html << "</div>";

    // ==========================
    // ANALISIS LEXICO
    // ==========================

    numeroLinea = 1;

    for (const string& linea : todasLineas) {

        string actual = "";

        for (int i = 0; i < (int)linea.length(); i++) {

            char c = linea[i];

            // ── DENTRO DE COMENTARIO BLOQUE ──
            if (enComentarioBloque) {
                if (c == '*' && i + 1 < (int)linea.length() && linea[i + 1] == '/') {
                    enComentarioBloque = false;
                    i++;
                }
                continue;
            }

            // ── INICIO COMENTARIO BLOQUE /* ──
            if (c == '/' && i + 1 < (int)linea.length() && linea[i + 1] == '*') {
                if (actual != "") {
                    Token nuevo;
                    nuevo.lexema  = actual;
                    nuevo.tipo    = obtenerTipo(actual);
                    nuevo.linea   = numeroLinea;
                    nuevo.columna = i - (int)actual.length() + 1;
                    tokens.push_back(nuevo);
                    actual = "";
                }
                string comentario = "/*";
                i += 2;
                enComentarioBloque = true;
                while (i < (int)linea.length()) {
                    if (linea[i] == '*' && i + 1 < (int)linea.length() && linea[i + 1] == '/') {
                        comentario += "*/";
                        i++;
                        enComentarioBloque = false;
                        break;
                    }
                    comentario += linea[i];
                    i++;
                }
                Token nuevo;
                nuevo.lexema  = comentario;
                nuevo.tipo    = "COMMENT";
                nuevo.linea   = numeroLinea;
                nuevo.columna = i - (int)comentario.length() + 2;
                tokens.push_back(nuevo);
                continue;
            }

            // ── COMENTARIO DE LINEA // ──
            if (c == '/' && i + 1 < (int)linea.length() && linea[i + 1] == '/') {
                string comentario = linea.substr(i);
                Token nuevo;
                nuevo.lexema  = comentario;
                nuevo.tipo    = "COMMENT";
                nuevo.linea   = numeroLinea;
                nuevo.columna = i + 1;
                tokens.push_back(nuevo);
                break;
            }

            // ── ESPACIOS / TABULACIONES ──
            if (c == ' ' || c == '\t') {
                if (actual != "") {
                    Token nuevo;
                    nuevo.lexema  = actual;
                    nuevo.tipo    = obtenerTipo(actual);
                    nuevo.linea   = numeroLinea;
                    nuevo.columna = i - (int)actual.length() + 1;
                    tokens.push_back(nuevo);
                    actual = "";
                }
                continue;
            }

            // ── CADENAS DE TEXTO " " ──
            if (c == '"') {
                if (actual != "") {
                    Token nuevo;
                    nuevo.lexema  = actual;
                    nuevo.tipo    = obtenerTipo(actual);
                    nuevo.linea   = numeroLinea;
                    nuevo.columna = i - (int)actual.length() + 1;
                    tokens.push_back(nuevo);
                    actual = "";
                }
                string cadena = "\"";
                i++;
                while (i < (int)linea.length() && linea[i] != '"') {
                    cadena += linea[i];
                    i++;
                }
                if (i < (int)linea.length()) cadena += "\"";
                Token nuevo;
                nuevo.lexema  = cadena;
                nuevo.tipo    = "STRING";
                nuevo.linea   = numeroLinea;
                nuevo.columna = i - (int)cadena.length() + 2;
                tokens.push_back(nuevo);
                continue;
            }

            // ── OPERADORES Y DELIMITADORES ──
            if (
                c=='+' || c=='-' || c=='*' || c=='/' ||
                c=='=' || c=='<' || c=='>' || c=='!' ||
                c==';' || c=='(' || c==')' || c=='{' || c=='}'
            ) {
                if (actual != "") {
                    Token nuevo;
                    nuevo.lexema  = actual;
                    nuevo.tipo    = obtenerTipo(actual);
                    nuevo.linea   = numeroLinea;
                    nuevo.columna = i - (int)actual.length() + 1;
                    tokens.push_back(nuevo);
                    actual = "";
                }
                if (i + 1 < (int)linea.length()) {
                    string doble = string(1, c) + linea[i + 1];
                    if (
                        doble == "==" || doble == "!=" ||
                        doble == "<=" || doble == ">=" ||
                        doble == "++" || doble == "--"
                    ) {
                        Token nuevo;
                        nuevo.lexema  = doble;
                        nuevo.tipo    = "OP";
                        nuevo.linea   = numeroLinea;
                        nuevo.columna = i + 1;
                        tokens.push_back(nuevo);
                        i++;
                        continue;
                    }
                }
                Token nuevo;
                nuevo.lexema  = string(1, c);
                nuevo.tipo    = obtenerTipo(nuevo.lexema);
                nuevo.linea   = numeroLinea;
                nuevo.columna = i + 1;
                tokens.push_back(nuevo);
                continue;
            }

            // ── CUALQUIER OTRO CARACTER ──
            actual += c;
        }

        if (actual != "") {
            Token nuevo;
            nuevo.lexema  = actual;
            nuevo.tipo    = obtenerTipo(actual);
            nuevo.linea   = numeroLinea;
            nuevo.columna = (int)linea.length() - (int)actual.length() + 1;
            tokens.push_back(nuevo);
            actual = "";
        }

        numeroLinea++;
    }

    // ==========================
    // PANEL DERECHO: CODIGO RESALTADO
    // ==========================

    html << "<div class='panel'>";
    html << "<h2>Codigo Resaltado</h2>";
    html << "<pre style='tab-size:4;'>";

    int lineaActual = 1;

    for (int i = 0; i < (int)tokens.size(); i++) {

        Token t = tokens[i];

        while (lineaActual < t.linea) {
            cout << "\n";
            html << "<br>";
            lineaActual++;
        }

        string mostrar = t.lexema;
        string salida  = "";
        for (char ch : mostrar) {
            if      (ch == '<') salida += "&lt;";
            else if (ch == '>') salida += "&gt;";
            else                salida += ch;
        }

        string clase = "error";
        int    color = COLOR_ERROR;

        if      (t.tipo == "KEYWORD")  { clase = "keyword"; color = COLOR_KEYWORD; }
        else if (t.tipo == "NUMERO")   { clase = "numero";  color = COLOR_NUMERO;  }
        else if (t.tipo == "STRING")   { clase = "string";  color = COLOR_STRING;  }
        else if (t.tipo == "IDENT")    { clase = "ident";   color = COLOR_IDENT;   }
        else if (t.tipo == "OP")       { clase = "op";      color = COLOR_OP;      }
        else if (t.tipo == "DELIM")    { clase = "delim";   color = COLOR_DELIM;   }
        else if (t.tipo == "COMMENT")  { clase = "comment"; color = COLOR_COMMENT; }

        setColor(color);
        cout << t.lexema << " ";
        setColor(7);

        html << "<span class='" << clase << "'>" << salida << "</span> ";
    }

    html << "</pre>";
    html << "</div>";
    html << "</div>";

    // ==========================
    // TABLA DE TOKENS
    // ==========================

    html << "<hr>";
    html << "<h2>Tabla de Tokens</h2>";
    html << "<table>";
    html << "<tr><th>Lexema</th><th>Tipo</th><th>Linea</th><th>Columna</th></tr>";

    for (int i = 0; i < (int)tokens.size(); i++) {
        Token t = tokens[i];
        html << "<tr>";
        html << "<td>" << t.lexema  << "</td>";
        html << "<td>" << t.tipo    << "</td>";
        html << "<td>" << t.linea   << "</td>";
        html << "<td>" << t.columna << "</td>";
        html << "</tr>";
    }

    html << "</table>";

    // ==========================
    // ANALISIS SINTACTICO
    // ==========================

    cout << "\n\n=====================================\n";
    cout << "       ANALISIS SINTACTICO";
    cout << "\n=====================================\n\n";

    html << "<hr>";
    html << "<h2>Analisis Sintactico</h2>";

    indice = 0;

    while (indice < (int)tokens.size()) {

        saltarComentarios(tokens);
        if (indice >= (int)tokens.size()) break;

        int    inicio = indice;
        Token  t      = actualToken(tokens);

        // Reglas reconocidas en nivel superior
        bool ok = false;

        if (t.lexema == "if")    { ok = sentenciaIf(tokens);    }
        else if (t.lexema == "while") { ok = sentenciaWhile(tokens); }
        else if (t.lexema == "for")   { ok = sentenciaFor(tokens);   }
        else if (t.lexema == "void")  { ok = sentenciaVoid(tokens);  }
        else if (t.lexema == "int"   ||
                 t.lexema == "float" ||
                 t.lexema == "string") { ok = declaracion(tokens); }
        else {
            // token no reconocido en nivel superior: recuperacion
            string err = "Token no reconocido en nivel superior: <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
            erroresSintacticos.push_back(err);
            setColor(COLOR_ERROR);
            cout << "ERROR: token no reconocido: " << t.lexema << " (linea " << t.linea << ")\n";
            setColor(7);
            avanzar();
            continue;
        }

        // Armar texto de la construccion
        string texto = "";
        for (int i = inicio; i < indice; i++) {
            texto += tokens[i].lexema + " ";
        }

        if (ok) {
            setColor(10);
            cout << "Construccion correcta: " << texto << endl;
            setColor(7);

            html << "<div class='syntax-ok'>";
            html << "&#10004; <b>Construccion correcta</b><br><br>";
            html << "<code>" << texto << "</code>";
            html << "</div>";
        } else {
            html << "<div class='syntax-error'>";
            html << "&#10006; <b>Error Sintactico</b><br><br>";
            if (!erroresSintacticos.empty()) {
                html << erroresSintacticos.back() << "<br>";
            }
            html << "</div>";
        }
    }

    html << "</body></html>";
    html.close();

    system("start salida.html");

    return 0;
}