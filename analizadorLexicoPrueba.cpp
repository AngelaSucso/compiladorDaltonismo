#include <iostream>
#include <fstream>
#include <sstream>
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

string escaparHtml(const string& texto) {
    string salida;
    for (char c : texto) {
        if      (c == '<') salida += "&lt;";
        else if (c == '>') salida += "&gt;";
        else if (c == '&') salida += "&amp;";
        else if (c == '"') salida += "&quot;";
        else                salida += c;
    }
    return salida;
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
    while (indice < (int)tokens.size() &&
           (actualToken(tokens).tipo == "COMMENT" || actualToken(tokens).tipo == "PREPROC"))
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
// EXPRESION:  VALOR ((+|-|*|/) VALOR)*
// ─────────────────────────────────────────
bool expresion(vector<Token>& tokens) {
    saltarComentarios(tokens);
    Token t = actualToken(tokens);
    if (t.tipo == "NUMERO" || t.tipo == "STRING" || t.tipo == "IDENT") {
        avanzar();
    } else {
        string err = "Se esperaba VALOR, se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
        erroresSintacticos.push_back(err);
        avanzar();
        return false;
    }
    while (true) {
        saltarComentarios(tokens);
        t = actualToken(tokens);
        if (t.tipo == "OP" && (t.lexema=="+" || t.lexema=="-" || t.lexema=="*" || t.lexema=="/")) {
            avanzar();
            saltarComentarios(tokens);
            t = actualToken(tokens);
            if (t.tipo == "NUMERO" || t.tipo == "STRING" || t.tipo == "IDENT") {
                avanzar();
            } else {
                string err = "Se esperaba VALOR tras operador, se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
                erroresSintacticos.push_back(err);
                avanzar();
                return false;
            }
        } else break;
    }
    return true;
}
 
// ─────────────────────────────────────────
// DECLARACION:  TIPO IDENT [= VALOR] ;
// ─────────────────────────────────────────
bool declaracion(vector<Token>& tokens) {
    Token t = actualToken(tokens);
 
    if (t.lexema == "int" || t.lexema == "float" || t.lexema == "string" || t.lexema == "char" || t.lexema == "bool") {
        avanzar();
    } else {
        string err = "Se esperaba TIPO (int/float/string/char/bool), se encontro <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
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
        if (!expresion(tokens)) return false;
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
// FUNCION CON TIPO DE RETORNO:  TIPO IDENT ( ) cuerpo
// ─────────────────────────────────────────
bool sentenciaFuncion(vector<Token>& tokens) {
    avanzar(); // TIPO
    if (!consumirTipo(tokens, "IDENT", "NOMBRE DE FUNCION")) return false;
    if (!consumir(tokens, "(")) return false;
    if (!consumir(tokens, ")")) return false;   // sin parametros por ahora
    if (!cuerpo(tokens))        return false;
    return true;
}
 
// Decide si "TIPO IDENT (" es una funcion o una declaracion normal
bool declaracionOFuncion(vector<Token>& tokens) {
    int checkpoint = indice;
    avanzar(); // TIPO
    Token siguiente = actualToken(tokens);
    Token trasIdent = {"", "", -1, -1};
    if (siguiente.tipo == "IDENT" && indice + 1 < (int)tokens.size())
        trasIdent = tokens[indice + 1];
    indice = checkpoint; // restaurar
 
    if (siguiente.tipo == "IDENT" && trasIdent.lexema == "(")
        return sentenciaFuncion(tokens);
    return declaracion(tokens);
}
 
// ─────────────────────────────────────────
// ENCABEZADO DE FUNCION (solo firma, sin tragarse el cuerpo):
//   TIPO/void IDENT ( ) {
// Se usa en el nivel superior del analisis sintactico para poder
// seguir mostrando cada sentencia del cuerpo por separado, en vez
// de mostrar toda la funcion como un unico bloque.
// ─────────────────────────────────────────
bool encabezadoFuncion(vector<Token>& tokens) {
    avanzar(); // TIPO o 'void'
    if (!consumirTipo(tokens, "IDENT", "NOMBRE DE FUNCION")) return false;
    if (!consumir(tokens, "("))  return false;
    if (!consumir(tokens, ")"))  return false;  // sin parametros por ahora
    if (!consumir(tokens, "{"))  return false;
    return true;
}
 
// Mira hacia adelante (sin consumir) si el token actual inicia una
// funcion: TIPO/void IDENT (
bool esInicioFuncion(vector<Token>& tokens) {
    Token t = actualToken(tokens);
    if (t.lexema != "void" && t.lexema != "int" && t.lexema != "float" &&
        t.lexema != "string" && t.lexema != "char" && t.lexema != "bool")
        return false;
 
    if (indice + 2 >= (int)tokens.size()) return false;
    Token ident  = tokens[indice + 1];
    Token parent = tokens[indice + 2];
    return (ident.tipo == "IDENT" && parent.lexema == "(");
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
 
    if (t.lexema == "int" || t.lexema == "float" || t.lexema == "string" || t.lexema == "char" || t.lexema == "bool") {
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
        t.lexema == "string" ||
        t.lexema == "char"   ||
        t.lexema == "bool")
        return declaracionOFuncion(tokens);
 
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
 
int main(int argc, char* argv[]) {

    // ==========================
    // ARGUMENTOS DE LINEA DE COMANDOS
    // La extension de VS Code llama:
    //   analizadorLexicoPrueba.exe entrada.txt salida.html --no-open
    // Se aceptan las rutas de entrada/salida como argumentos, y el flag
    // --no-open evita que el programa abra el navegador (para que la
    // extension solo actualice el panel interno de VS Code).
    // ==========================

    string rutaEntradaArg = "entrada.txt";
    string rutaSalidaArg  = "salida.html";
    bool   noAbrir        = false;
    int    posicional     = 0;

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--no-open") {
            noAbrir = true;
        } else if (posicional == 0) {
            rutaEntradaArg = arg;
            posicional++;
        } else if (posicional == 1) {
            rutaSalidaArg = arg;
            posicional++;
        }
    }
 
    ofstream html(rutaSalidaArg);
 
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
    html << ".keyword { color:#FF0000; font-weight:bold; }";      // Rojo
    html << ".numero  { color:#00FFFF; }";                        // Cian
    html << ".string  { color:#00FF00; font-style:italic; }";     // Verde
    html << ".ident   { color:#FFFFFF; }";                        // Blanco
    html << ".op      { color:#FFFF00; }";                        // Amarillo
    html << ".delim   { color:#C0C0C0; }";                        // Gris plata
    html << ".comment { color:#808080; font-style:italic; }";     // Gris
    html << ".error   { color:#FF00FF; font-weight:bold; text-decoration:underline; }"; // Magenta
    html << ".preproc { color:#0000FF; font-style:italic; }";     // Azul
    html << ".syntax-ok { background:#003b46; padding:10px; border-left:5px solid cyan; margin-bottom:10px; border-radius:5px; }";
    html << ".syntax-error { background:#3b003b; padding:10px; border-left:5px solid magenta; margin-bottom:10px; border-radius:5px; }";
    html << ".syntax-summary { padding:10px 15px; margin-bottom:15px; border-radius:5px; font-weight:bold; }";
    html << ".syntax-summary.ok { background:#003b1a; border-left:5px solid #00e676; }";
    html << ".syntax-summary.error { background:#3b003b; border-left:5px solid #8404a0; }";
    html << ".num-linea { color:#666; display:inline-block; width:30px; text-align:right; margin-right:8px; user-select:none; }";
    html << ".panel-completo { width:100%; }";
    // Paleta deuteranomalia (dificultad rojo-verde, la forma mas comun
    // y habitualmente mas leve de daltonismo), basada en la paleta
    // Okabe-Ito, validada para uso accesible.
    html << "body.deuteranomalia .keyword  { color:#0072B2; }";  // azul
    html << "body.deuteranomalia .numero   { color:#56B4E9; }";  // celeste
    html << "body.deuteranomalia .string   { color:#E69F00; }";  // naranja
    html << "body.deuteranomalia .ident    { color:#F0E442; }";  // amarillo
    html << "body.deuteranomalia .op       { color:#CC79A7; }";  // purpura rosado
    html << "body.deuteranomalia .delim    { color:#E5E5E5; }";  // gris claro
    html << "body.deuteranomalia .comment  { color:#9E9E9E; }";  // gris medio
    html << "body.deuteranomalia .preproc  { color:#009E73; }";  // verde azulado
    html << "body.deuteranomalia .error    { color:#FF3EA5; }";  // rosa magenta
    // Paleta tritanopia (dificultad azul-amarillo): se evita el eje
    // azul/amarillo por completo y se trabaja sobre rojos, verdes y
    // violetas, que siguen siendo distinguibles para este tipo de
    // daltonismo.
    html << "body.tritanopia .keyword  { color:#EF476F; }";  // rojo rosado
    html << "body.tritanopia .numero   { color:#06D6A0; }";  // verde azulado
    html << "body.tritanopia .string   { color:#F3722C; }";  // naranja calido
    html << "body.tritanopia .ident    { color:#FFFFFF; }";  // blanco (se evita el amarillo puro)
    html << "body.tritanopia .op       { color:#9D4EDD; }";  // violeta
    html << "body.tritanopia .delim    { color:#B0BEC5; }";  // gris azulado claro
    html << "body.tritanopia .comment  { color:#9E9E9E; }";  // gris medio
    html << "body.tritanopia .preproc  { color:#43AA8B; }";  // verde teal
    html << "body.tritanopia .error    { color:#FF006E; }";  // magenta
    // ── NAVEGACION POR PESTAÑAS ──
    html << "h1 { margin-bottom:10px; }";
    html << ".tabs { display:flex; gap:8px; border-bottom:2px solid #444; margin-bottom:20px; flex-wrap:wrap; }";
    html << ".tab-btn { background:#2b2b2b; color:#eee; border:none; padding:10px 18px; font-size:15px; cursor:pointer; border-radius:8px 8px 0 0; transition:background 0.2s; }";
    html << ".tab-btn:hover { background:#3a3a3a; }";
    html << ".tab-btn:focus-visible { outline:3px solid cyan; outline-offset:2px; }";
    html << ".tab-btn.active { background:#444; border-bottom:3px solid cyan; font-weight:bold; }";
    html << ".tab-content { display:none; }";
    html << ".tab-content.active { display:block; }";
 
    html << "</style>";
    html << "</head>";
    html << "<body>";
 
    html << "<h1>Compilador Inclusivo</h1>";
 
    // ── BOTONES DE NAVEGACION ──
    html << "<div class='tabs' role='tablist' aria-label='Secciones del compilador'>";
    html << "<button class='tab-btn active' id='btn-codigo' role='tab' aria-selected='true' aria-controls='codigo' onclick=\"abrirTab('codigo')\">Codigo</button>";
    html << "<button class='tab-btn' id='btn-tabla' role='tab' aria-selected='false' aria-controls='tabla' onclick=\"abrirTab('tabla')\">Tabla de Tokens</button>";
    html << "<button class='tab-btn' id='btn-sintactico' role='tab' aria-selected='false' aria-controls='sintactico' onclick=\"abrirTab('sintactico')\">Analisis Sintactico</button>";
    html << "<button class='tab-btn' id='btn-accesibilidad' role='tab' aria-selected='false' aria-controls='accesibilidad' onclick=\"abrirTab('accesibilidad')\">Accesibilidad</button>";
    html << "</div>";
 
    // ==========================
    // PESTAÑA: ACCESIBILIDAD
    // (Leyenda de tokens + justificacion de colores)
    // ==========================
 
    html << "<div class='tab-content' id='accesibilidad' role='tabpanel' aria-labelledby='btn-accesibilidad'>";
 
    // LEYENDA
    html << "<h2>Leyenda de Tokens</h2>";
    html << "<p><span class='keyword'>KEYWORD</span> &rarr; Palabras reservadas</p>";
    html << "<p><span class='numero'>NUMERO</span>   &rarr; Numeros enteros y flotantes</p>";
    html << "<p><span class='string'>STRING</span>   &rarr; Cadenas de texto</p>";
    html << "<p><span class='ident'>IDENT</span>     &rarr; Identificadores</p>";
    html << "<p><span class='op'>OP</span>           &rarr; Operadores</p>";
    html << "<p><span class='delim'>DELIM</span>     &rarr; Delimitadores</p>";
    html << "<p><span class='comment'>COMMENT</span> &rarr; Comentarios</p>";
    html << "<p><span class='preproc'>PREPROC</span> &rarr; Directivas de preprocesador (#include, #define, etc.)</p>";
    html << "<p><span class='error'>ERROR</span>     &rarr; Error lexico</p>";
 
    html << "<h2>Justificacion de Colores (paleta Normal)</h2>";
    html << "<ul>";
    html << "<li><b style='color:#FF0000'>Rojo:</b> palabras reservadas. "
            "Color basico de alto contraste que permite identificar rapidamente las estructuras principales del lenguaje.</li>";
    html << "<li><b style='color:#00FFFF'>Cian:</b> numeros enteros y flotantes. "
            "Facilita distinguir los valores numericos de otros elementos del codigo fuente.</li>";
    html << "<li><b style='color:#00FF00'>Verde:</b> cadenas de texto. "
            "Presenta un contraste perceptible respecto a las palabras reservadas e identificadores, favoreciendo su reconocimiento.</li>";
    html << "<li><b style='color:#FFFFFF'>Blanco:</b> identificadores. "
            "Mantiene buena visibilidad y facilita el seguimiento de variables y funciones durante la lectura del programa.</li>";
    html << "<li><b style='color:#FFFF00'>Amarillo:</b> operadores. "
            "Permite una lectura clara de las expresiones sin generar sobrecarga visual.</li>";
    html << "<li><b style='color:#C0C0C0'>Gris plata:</b> delimitadores. "
            "Al ser elementos estructurales de alta frecuencia, se representan con un color neutro para evitar distracciones visuales.</li>";
    html << "<li><b style='color:#808080'>Gris:</b> comentarios. "
            "Al corresponder a informacion auxiliar, se muestran con menor prominencia visual para reducir la carga cognitiva.</li>";
    html << "<li><b style='color:#FF00FF'>Magenta:</b> errores lexicos. "
            "Se eligio por su alta visibilidad y se complementa con negrita y subrayado para no depender exclusivamente del color.</li>";
    html << "</ul>";
 
    html << "<h2>Paletas alternativas</h2>";
    html << "<p>El selector <b>Paleta de color</b> (en la pestaña Codigo) permite cambiar toda la vista "
            "a un esquema alternativo pensado para dos tipos de daltonismo:</p>";
    html << "<ul>";
    html << "<li><b>Deuteranomalia:</b> dificultad para distinguir rojos y verdes, la forma de daltonismo "
            "mas comun. Se reemplazan esos tonos por azules, naranjas y amarillos, basados en la paleta "
            "Okabe-Ito, disenada especificamente para ser distinguible en este tipo de vision.</li>";
    html << "<li><b>Tritanopia:</b> dificultad para distinguir azules y amarillos. En este caso se evita "
            "por completo ese eje de color y se trabaja con rojos, verdes, naranjas y violetas, que siguen "
            "siendo distinguibles entre si.</li>";
    html << "</ul>";
    html << "<p>Ademas del color, los errores lexicos siempre se muestran en <b>negrita y subrayados</b> "
            "y las cadenas de texto/comentarios en <i>cursiva</i>, para que el significado no dependa "
            "unicamente del color en ninguna paleta.</p>";
 
    html << "</div>"; // fin tab-content accesibilidad
 
    // ==========================
    // APERTURA DE ARCHIVO
    // ==========================
 
    ifstream archivo(rutaEntradaArg);
 
    if (!archivo) {
        cout << "Error al abrir " << rutaEntradaArg << endl;
        return 1;
    }
 
    vector<Token> tokens;
 
    string lineaLectura;
    int    numeroLinea        = 1;
    bool   enComentarioBloque = false;
 
    // ==========================
    // PESTAÑA: CODIGO
    // ==========================
 
html << "<div class='tab-content active' id='codigo' role='tabpanel' aria-labelledby='btn-codigo'>";
 
    html << "<div class='contenedor'>";
 
    vector<string> todasLineas;
    while (getline(archivo, lineaLectura)) {
        todasLineas.push_back(lineaLectura);
    }
    archivo.close();
 
    // ==========================
    // ANALISIS LEXICO
    // ==========================
 
    numeroLinea = 1;
 
    for (const string& linea : todasLineas) {
 
        string actual = "";
 
        // ── DIRECTIVA DE PREPROCESADOR (#include, #define, etc.) ──
        {
            size_t primerNoEspacio = linea.find_first_not_of(" \t");
            if (primerNoEspacio != string::npos && linea[primerNoEspacio] == '#') {
                Token nuevo;
                nuevo.lexema  = linea;
                nuevo.tipo    = "PREPROC";
                nuevo.linea   = numeroLinea;
                nuevo.columna = (int)primerNoEspacio + 1;
                tokens.push_back(nuevo);
                numeroLinea++;
                continue; // pasar directamente a la siguiente linea
            }
        }
 
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
 
            // ── CARACTERES ' ' ──
            if (c == '\'') {
                if (actual != "") {
                    Token nuevo;
                    nuevo.lexema  = actual;
                    nuevo.tipo    = obtenerTipo(actual);
                    nuevo.linea   = numeroLinea;
                    nuevo.columna = i - (int)actual.length() + 1;
                    tokens.push_back(nuevo);
                    actual = "";
                }
                string caracter = "'";
                i++;
                while (i < (int)linea.length() && linea[i] != '\'') {
                    caracter += linea[i];
                    i++;
                }
                if (i < (int)linea.length()) caracter += "'";
                Token nuevo2;
                nuevo2.lexema  = caracter;
                nuevo2.tipo    = "STRING";
                nuevo2.linea   = numeroLinea;
                nuevo2.columna = i - (int)caracter.length() + 2;
                tokens.push_back(nuevo2);
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
                        doble == "++" || doble == "--" ||
                        doble == "<<" || doble == ">>"
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
 
    // ── PANEL DERECHO: CODIGO RESALTADO ──
 
    html << "<div class='panel panel-completo'>";
    html << "<h2>Codigo Resaltado</h2>";
    html << "<div style='margin-bottom:10px;'>";
    html << "<label for='paleta'>Paleta de color: </label>";
    html << "<select id='paleta' onchange=\"document.body.className = this.value\">";
    html << "<option value=''>Normal</option>";
    html << "<option value='deuteranomalia'>Deuteranopía</option>";
    html << "<option value='tritanopia'>Tritanopia</option>";
    html << "</select>";
    html << "</div>";
    html << "<pre style='tab-size:4;'>";
 
    int lineaActual = 1;
    html << "<span class='num-linea'>" << lineaActual << "</span> ";
 
    for (int i = 0; i < (int)tokens.size(); i++) {
 
        Token t = tokens[i];
 
        while (lineaActual < t.linea) {
            cout << "\n";
            html << "<br>";
            lineaActual++;
            html << "<span class='num-linea'>" << lineaActual << "</span> ";
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
        else if (t.tipo == "PREPROC")  { clase = "preproc"; color = COLOR_MAGENTA_OSCURO; }
 
        setColor(color);
        cout << t.lexema << " ";
        setColor(7);
 
        html << "<span class='" << clase << "'>" << salida << "</span> ";
    }
 
    html << "</pre>";
    html << "</div>";  // fin panel derecho
    html << "</div>";  // fin contenedor
    html << "</div>";  // fin tab-content codigo
 
    // ==========================
    // PESTAÑA: TABLA DE TOKENS
    // ==========================
 
    html << "<div class='tab-content' id='tabla' role='tabpanel' aria-labelledby='btn-tabla'>";
    html << "<h2>Tabla de Tokens</h2>";
    html << "<table>";
    html << "<tr><th>Lexema</th><th>Tipo</th><th>Linea</th><th>Columna</th></tr>";
 
    for (int i = 0; i < (int)tokens.size(); i++) {
        Token t = tokens[i];
        html << "<tr>";
        html << "<td>" << escaparHtml(t.lexema) << "</td>";
        html << "<td>" << t.tipo    << "</td>";
        html << "<td>" << t.linea   << "</td>";
        html << "<td>" << t.columna << "</td>";
        html << "</tr>";
    }
 
    html << "</table>";
    html << "</div>"; // fin tab-content tabla
 
    // ==========================
    // PESTAÑA: ANALISIS SINTACTICO
    // ==========================
 
    cout << "\n\n=====================================\n";
    cout << "       ANALISIS SINTACTICO";
    cout << "\n=====================================\n\n";
 
    html << "<div class='tab-content' id='sintactico' role='tabpanel' aria-labelledby='btn-sintactico'>";
    html << "<h2>Analisis Sintactico</h2>";
 
    indice = 0;
    ostringstream bufferSintactico;
 
    // Cuenta cuantas funciones (nivel superior) siguen abiertas.
    // Se usa para reconocer la '}' que cierra una funcion y no
    // mostrarla como "token no reconocido", sin volver a tragarse
    // todo el cuerpo de la funcion como un unico bloque (asi cada
    // sentencia interna se sigue mostrando por separado, como antes).
    int profundidadFuncion = 0;
 
    while (indice < (int)tokens.size()) {
 
        saltarComentarios(tokens);
        if (indice >= (int)tokens.size()) break;
 
        // Si estamos dentro de una funcion y llegamos a la '}' que
        // la cierra, simplemente la consumimos y seguimos con el
        // resto del archivo (no genera tarjeta propia).
        if (profundidadFuncion > 0 && actualToken(tokens).lexema == "}") {
            avanzar();
            profundidadFuncion--;
            continue;
        }
 
        int    inicio = indice;
        Token  t      = actualToken(tokens);
 
        // Reglas reconocidas en nivel superior
        bool ok = false;
 
        if (esInicioFuncion(tokens)) {
            // Solo se valida y muestra la firma (TIPO/void IDENT ( ) {).
            // El cuerpo se sigue analizando sentencia por sentencia
            // en las siguientes vueltas del while, igual que antes.
            ok = encabezadoFuncion(tokens);
            if (ok) profundidadFuncion++;
        }
        else if (t.lexema == "if")    { ok = sentenciaIf(tokens);    }
        else if (t.lexema == "while") { ok = sentenciaWhile(tokens); }
        else if (t.lexema == "for")   { ok = sentenciaFor(tokens);   }
        else if (t.lexema == "void")  { ok = sentenciaVoid(tokens);  }
        else if (t.lexema == "int"   ||
                 t.lexema == "float" ||
                 t.lexema == "string"||
                 t.lexema == "char"  ||
                 t.lexema == "bool")  { ok = declaracion(tokens); }
        else if (t.lexema == "return" || t.tipo == "IDENT") {
            // return, llamada a funcion (cout, cin, etc.) u otra sentencia de expresion:
            // se consume hasta el ';' de cierre.
            while (indice < (int)tokens.size() &&
                   actualToken(tokens).lexema != ";" &&
                   actualToken(tokens).lexema != "}" &&
                   actualToken(tokens).tipo   != "EOF")
                avanzar();
            if (actualToken(tokens).lexema == ";") avanzar();
            ok = true;
        }
        else {
            // token no reconocido en nivel superior: recuperacion
            string err = "Token no reconocido en nivel superior: <b>" + t.lexema + "</b> (linea " + to_string(t.linea) + ")";
            erroresSintacticos.push_back(err);
            setColor(COLOR_ERROR);
            cout << "ERROR: token no reconocido: " << t.lexema << " (linea " << t.linea << ")\n";
            setColor(7);

            // Se muestra tambien su propia tarjeta de error, para que
            // el numero de tarjetas coincida siempre con el contador
            // de "error(es) encontrado(s)" del resumen.
            bufferSintactico << "<div class='syntax-error'>";
            bufferSintactico << "&#10006; <b>Error Sintactico</b><br><br>";
            bufferSintactico << err << "<br>";
            bufferSintactico << "</div>";

            avanzar();
            continue;
        }
 
        // Armar texto de la construccion
        string texto = "";
        for (int i = inicio; i < indice; i++) {
            texto += tokens[i].lexema + " ";
        }
 
        if (ok) {
            setColor(11);
            cout << "Construccion correcta: " << texto << endl;
            setColor(7);
 
            bufferSintactico << "<div class='syntax-ok'>";
            bufferSintactico << "&#10004; <b>Construccion correcta</b><br><br>";
            bufferSintactico << "<code>" << texto << "</code>";
            bufferSintactico << "</div>";
        } else {
            // MAGENTA
            setColor(13);
            cout << "Error Sintactico: " << texto << endl;
            setColor(7);
            bufferSintactico << "<div class='syntax-error'>";
            bufferSintactico << "&#10006; <b>Error Sintactico</b><br><br>";
            if (!erroresSintacticos.empty()) {
                bufferSintactico << erroresSintacticos.back() << "<br>";
            }
            bufferSintactico << "</div>";
        }
    }
 
    if (erroresSintacticos.empty()) {
        html << "<div class='syntax-summary ok'>&#9989; 0 errores encontrados</div>";
    } else {
        html << "<div class='syntax-summary error'>&#10060; " << erroresSintacticos.size() << " error(es) encontrado(s)</div>";
    }
    html << bufferSintactico.str();
 
    html << "</div>"; // fin tab-content sintactico
 
    // ── SCRIPT DE NAVEGACION POR PESTAÑAS ──
    html << "<script>";
    html << "function abrirTab(nombre) {";
    html << "  var contenidos = document.getElementsByClassName('tab-content');";
    html << "  for (var i = 0; i < contenidos.length; i++) contenidos[i].classList.remove('active');";
    html << "  var botones = document.getElementsByClassName('tab-btn');";
    html << "  for (var i = 0; i < botones.length; i++) { botones[i].classList.remove('active'); botones[i].setAttribute('aria-selected','false'); }";
    html << "  document.getElementById(nombre).classList.add('active');";
    html << "  var btn = document.getElementById('btn-' + nombre);";
    html << "  btn.classList.add('active');";
    html << "  btn.setAttribute('aria-selected','true');";
    html << "}";
    html << "</script>";
 
    html << "</body></html>";
    html.close();
 
    // Solo abrimos el navegador si NO vino el flag --no-open.
    // La extension de VS Code manda --no-open y lee salida.html
    // directamente para actualizar su panel interno, asi que aqui
    // ya no hace falta (ni conviene) lanzar el navegador.
    if (!noAbrir) {
        string comando = "start " + rutaSalidaArg;
        system(comando.c_str());
    }
 
    return 0;
}