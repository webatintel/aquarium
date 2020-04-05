// This script should be skipped when running locally.

var assert = require('assert');
var path = require('path');
var util = require('util');
var actions = require('@actions/core');

var work_dir = 'WORKSPACE_OVERRIDE' in process.env ? process.env['WORKSPACE_OVERRIDE'] :
               'GITHUB_WORKSPACE' in process.env ? process.env['GITHUB_WORKSPACE'] :
               process.cwd();
work_dir = path.resolve(work_dir);

var editor;
switch (process.platform) {
    case 'win32':
        editor = [ 'COPY', path.join(work_dir, 'gn.txt') ];
        editor = editor.map(function (arg) {
            if (/^\w+$/.test(arg)) return arg;
            return arg.replace(/((\\+)("|$))/g, '$2$1').replace(/"/g, '\\"').replace(/(.*)/, '"$1"');
        }).join(' ');
        break;
    case 'linux':
        editor = [ 'cp', path.join(work_dir, 'gn.txt') ];
        editor = editor.map(function (arg) {
            if (/^\w+$/.test(arg)) return arg;
            return arg.replace(/([$`"\\])/g, '\\$1').replace(/(.*)/, '"$1"');
        }).join(' ');
        break;
    default:
        assert.fail();
}
actions.exportVariable('GN_EDITOR', editor);
