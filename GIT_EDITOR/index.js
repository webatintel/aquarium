// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
        // Git for Windows runs in an MSYS2 environment, so the editor
        // command is built for that environment.
        var sep;
        sep = path.sep;
        work_dir = work_dir.split(sep).map(function (segment) {
            if (/^\w+$/.test(segment)) return segment;
            return segment.replace(/([$`"\\])/g, '\\$1').replace(/(.*)/, '"$1"');
        });
        sep = path.posix.sep;
        editor = util.format("cp $(cygpath %s)%s%s", work_dir[0], sep, work_dir.slice(1).concat('git.txt').join(sep));
        editor = [ 'sh', '-c', util.format('%s "$@"', editor), 'sh' ];
        editor = editor.map(function (arg) {
            if (/^\w+$/.test(arg)) return arg;
            return arg.replace(/([$`"\\])/g, '\\$1').replace(/(.*)/, '"$1"');
        }).join(' ');
        break;
    case 'linux':
        editor = [ 'cp', path.join(work_dir, 'git.txt') ];
        editor = editor.map(function (arg) {
            if (/^\w+$/.test(arg)) return arg;
            return arg.replace(/([$`"\\])/g, '\\$1').replace(/(.*)/, '"$1"');
        }).join(' ');
        break;
    default:
        assert.fail();
}
actions.exportVariable('GIT_EDITOR', editor);
