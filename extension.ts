import * as vscode from 'vscode';
import * as url from 'url';

export function activate(context: vscode.ExtensionContext) {
 let disposable = vscode.commands.registerCommand('pythonDocs.openDocs', () => {
  const editor = vscode.window.activeTextEditor;
  if (!editor) {
   return;
  }

  const selection = editor.selection;
  const word = editor.document.getText(selection);
  if (!word) {
   return;
  }

  const docsUrl = getDocsUrl(word);
  if (docsUrl) {
   vscode.env.openExternal(vscode.Uri.parse(docsUrl));
  } else {
   vscode.window.showErrorMessage(`Документация для функции "${word}" не найдена.`);
  }
 });

 context.subscriptions.push(disposable);
}

function getDocsUrl(functionName: string): string | undefined {
 if (!/^(?:(?!_)\w+)+$/.test(functionName)) {
  return undefined;
 }

 const baseUrl = 'https://docs.python.org/3/library/';
 return url.resolve(baseUrl, `functions.html#${functionName}`);
}

export function deactivate() {}
