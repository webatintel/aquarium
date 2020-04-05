// This script should be skipped when running locally.

var assert = require('assert');
var child_process = require('child_process');
var fs = require('fs');
var os = require('os');
var path = require('path');
var util = require('util');

var work_dir = 'WORKSPACE_OVERRIDE' in process.env ? process.env['WORKSPACE_OVERRIDE'] :
               'GITHUB_WORKSPACE' in process.env ? process.env['GITHUB_WORKSPACE'] :
               process.cwd();
work_dir = path.resolve(work_dir);

var cmd;
var options;

switch (process.platform) {
    case 'win32':
        // GN does not respect the EDITOR environment variable on
        // Windows. Instead it opens the program associated with .txt
        // files. The association is modified below, so that a launcher
        // script will be used as the default text editor, where the
        // environment variable is properly honored.

        var node_exe = process.execPath;
        if (!/^\w+$/.test(node_exe)) {
            node_exe = node_exe.replace(/((\\+)("|$))/g, '$2$1').replace(/"/g, '\\"').replace(/(.*)/, '"$1"');
        }

        var editor_bat = path.join(work_dir, 'editor.bat');
        if (!/^\w+$/.test(editor_bat)) {
            editor_bat = editor_bat.replace(/((\\+)("|$))/g, '$2$1').replace(/"/g, '\\"').replace(/(.*)/, '"$1"');
        }

        // It looks rather difficult to handle environment variables
        // containing quotes or whitespaces in batch, so the launcher
        // script is mainly implemented in JS here.
        var editor = (function () {
            var child_process = require('child_process');
            var path = require('path');
            var util = require('util');
            var editor;
            if ('EDITOR' in process.env) {
                editor = process.env['EDITOR'];
            } else {
                // Fallback to Notepad if EDITOR is not set
                editor = path.join('%SystemRoot%', 'system32', 'NOTEPAD.EXE');
            }
            var args = process.argv.slice(2).map(function (arg) {
                if (/^\w+$/.test(arg)) return arg;
                return arg.replace(/((\\+)("|$))/g, '$2$1').replace(/"/g, '\\"').replace(/(.*)/, '"$1"');
            }).join(' ');
            var options = {
                stdio: 'inherit',
            };
            child_process.execSync(util.format("%s %s", editor, args), options);
        }).toString();

        // Get the function body
        editor = editor.substring(editor.indexOf('{') + 1, editor.lastIndexOf('}')).trim().split('\n').map(function (line) {
            return line.trim();
        }).join(os.EOL);

        // Embed JS into a batch script
        editor = [
            "REM (); /*",                                   // This is a comment in batch, but a function call in JS.
                                                            // Note it's totally valid to place a JS function call before its declaration, even in strict mode.
            "@ECHO OFF",
            util.format("%s %s %%*", node_exe, editor_bat), // self-execute as a Node.js script
            "EXIT /B",                                      // Batch script ends here. The following JS will be ignored by CMD.EXE.
            "*/",
            "function REM() {",
            editor,
            "}",
        ].join(os.EOL);
        fs.writeFileSync(path.join(work_dir, 'editor.bat'), editor);

        // Change current user's default editor in registry
        var key_name = 'HKCU\\Software\\Classes\\txtfile\\shell\\open\\command';
        var type = 'REG_EXPAND_SZ';
        var data = util.format('%s "%%0" %%*', editor_bat);
        cmd = [ 'REG', 'ADD', key_name, '/ve', '/t', type, '/d', data, '/f' ];
        options = {
            stdio: 'inherit',
        };
        child_process.execFileSync(cmd[0], cmd.slice(1), options);

        break;
    case 'linux':
        break;
    default:
        assert.fail();
}
