"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.deactivate = exports.activate = void 0;
const vscode = __importStar(require("vscode"));
const url = __importStar(require("url"));
function activate(context) {
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
        }
        else {
            vscode.window.showErrorMessage(`Документация для функции "${word}" не найдена.`);
        }
    });
    context.subscriptions.push(disposable);
}
exports.activate = activate;
function getDocsUrl(functionName) {
    if (!/^(?:(?!_)\w+)+$/.test(functionName)) {
        return undefined;
    }
    const baseUrl = 'https://docs.python.org/3/library/';
    return url.resolve(baseUrl, `functions.html#${functionName}`);
}
function deactivate() { }
exports.deactivate = deactivate;

