import * as vscode from 'vscode';
import * as fs from 'fs';
import * as path from 'path';
import { execFile } from 'child_process';


let temporizador: NodeJS.Timeout | undefined;
let ejecutando = false;

// Panel único: si ya existe, se reutiliza y se actualiza.
// Si no existe, se crea una sola vez.
let panel: vscode.WebviewPanel | undefined;


export function activate(context: vscode.ExtensionContext) {


    console.log("=== Inclusive Compiler iniciado ===");


    // ==============================
    // MOSTRAR / ACTUALIZAR EL PANEL
    // ==============================

    function mostrarEnPanel(html: string) {

        if (!panel) {

            // Se crea SOLO la primera vez
            panel = vscode.window.createWebviewPanel(
                'inclusiveCompiler',
                'Inclusive Compiler',
                vscode.ViewColumn.Beside,
                {
                    enableScripts: true,
                    retainContextWhenHidden: true
                }
            );

            panel.onDidDispose(() => {
                // Si el usuario lo cierra, permitimos que se vuelva a crear
                // la próxima vez que se guarde/escriba.
                panel = undefined;
            }, null, context.subscriptions);

        }

        // Ya sea que se acabe de crear o que ya existiera,
        // solo actualizamos el contenido. Esto NO abre nada nuevo
        // ni roba el foco del editor.
        panel.webview.html = html;
    }


    // ==============================
    // FUNCIÓN PRINCIPAL DE ANÁLISIS
    // ==============================

    function analizarCodigo(editor: vscode.TextEditor) {


        if (ejecutando) {
            return;
        }


        ejecutando = true;


        const codigo = editor.document.getText();


        const carpetaCompilador =
            "C:\\user\\Desktop\\proyecto\\compiladorDaltonismo";


        const rutaCompilador = path.join(
            carpetaCompilador,
            "analizadorLexicoPrueba.exe"
        );


        const rutaEntrada = path.join(
            carpetaCompilador,
            "entrada.txt"
        );


        const rutaSalida = path.join(
            carpetaCompilador,
            "salida.html"
        );



        // Verificar compilador

        if (!fs.existsSync(rutaCompilador)) {

            vscode.window.showErrorMessage(
                "No existe analizadorLexicoPrueba.exe"
            );

            ejecutando = false;
            return;
        }



        try {


            // Guardar código actual

            fs.writeFileSync(
                rutaEntrada,
                codigo,
                "utf8"
            );


            console.log(
                "Analizando código..."
            );



            // Ejecutar compilador
            // IMPORTANTE: se le pasan las rutas y el flag --no-open
            // para que NO abra el navegador en cada análisis.

            execFile(

                rutaCompilador,

                [rutaEntrada, rutaSalida, "--no-open"],

                {
                    cwd: carpetaCompilador
                },


                (error, stdout, stderr) => {


                    ejecutando = false;



                    if (error) {

                        console.error(error);

                        return;
                    }



                    console.log(
                        "Análisis terminado"
                    );



                    if (fs.existsSync(rutaSalida)) {


                        console.log(
                            "HTML actualizado"
                        );

                        // En vez de abrir el navegador, leemos el HTML
                        // y actualizamos el panel ya abierto en VS Code.
                        const html = fs.readFileSync(rutaSalida, "utf8");
                        mostrarEnPanel(html);


                    }



                }

            );



        }
        catch(error){


            ejecutando = false;


            console.error(error);

        }



    }



    // =====================================
    // ANALIZAR AL ESCRIBIR EN EL EDITOR
    // =====================================


    const cambioDocumento =
        vscode.workspace.onDidChangeTextDocument(
            
            (evento)=>{


                const editor =
                    vscode.window.activeTextEditor;



                if(!editor){
                    return;
                }



                // cancelar análisis anterior

                if(temporizador){

                    clearTimeout(
                        temporizador
                    );

                }



                // esperar 500 ms

                temporizador =
                    setTimeout(()=>{


                        analizarCodigo(
                            editor
                        );


                    },500);



            }

        );




    context.subscriptions.push(
        cambioDocumento
    );




    // =====================================
    // COMANDO MANUAL
    // =====================================


    const disposable =
        vscode.commands.registerCommand(

            'inclusive-compiler.helloWorld',

            ()=>{


                const editor =
                    vscode.window.activeTextEditor;



                if(!editor){

                    vscode.window.showErrorMessage(
                        "No hay archivo abierto"
                    );

                    return;

                }


                analizarCodigo(
                    editor
                );


            }

        );



    context.subscriptions.push(
        disposable
    );


}



export function deactivate() {


}