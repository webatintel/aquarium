// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// The GitHub runner environment is optional for this script, so it can work in local.

// When running in GitHub hosted runners, the script translates
// mnemonics in the BUILD_CONFIG environment variable to GN arguments,
// writes the arguments to a staging file, and copies the file to
// args.gn during "gn args" time. This works because GN would launch the
// command given by the GN_EDITOR environment variable on POSIX, and
// with additional configuration it can work in a similar manner on
// Windows. Thus, path to args.gn will be passed to system-specific file
// copy commands as the destination, with the staging file being the
// source.

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
var temp_dir = fs.mkdtempSync(util.format('%s%s', work_dir, path.sep));
var sub_work_dir;

var cmd;
var env;
var shell;
var options;

switch (process.platform) {
    case 'win32':
        shell = true;
        break;
    case 'linux':
        shell = false;
        break;
    default:
        assert.fail();
}

var gn_args;
if ('BUILD_CONFIG' in process.env) {
    var build_config = process.env['BUILD_CONFIG'].split('-');

    gn_args = build_config.slice(1).map(function (arg) {
        switch (arg) {
            case 'angle':       return [ 'enable_angle',       'true'  ];
            case 'noangle':     return [ 'enable_angle',       'false' ];
            case 'x86':         return [ 'target_cpu',         '"x86"' ];
            case 'x64':         return [ 'target_cpu',         '"x64"' ];
            case 'component':   return [ 'is_component_build', 'true'  ];
            case 'nocomponent': return [ 'is_component_build', 'false' ];
            case 'debug':       return [ 'is_debug',           'true'  ];
            case 'release':     return [ 'is_debug',           'false' ];
            default: assert.fail();
        }
    });

    // Ensure there's no conflict or duplication in GN arguments
    gn_args.map(function (arg) {
        return arg[0];
    }).forEach(function (value, index, array) {
        assert(array.indexOf(value) == index);
    });

    gn_args = gn_args.map(function (arg) {
        return util.format("%s = %s", arg[0], arg[1]);
    }).join(os.EOL);
} else {
    gn_args = "";
}

fs.writeFileSync(path.join(temp_dir, 'gn.txt'), gn_args);
if (fs.existsSync(path.join(work_dir, 'gn.txt'))) {
    assert(fs.statSync(path.join(work_dir, 'gn.txt')).isFile());
    assert(fs.statSync(path.join(temp_dir, 'gn.txt')).mode == fs.statSync(path.join(work_dir, 'gn.txt')).mode);
}
fs.copyFileSync(path.join(temp_dir, 'gn.txt'), path.join(work_dir, 'gn.txt'));

if ('GITHUB_EVENT_NAME' in process.env && 'GITHUB_EVENT_PATH' in process.env) {
    var payload = fs.readFileSync(process.env['GITHUB_EVENT_PATH'], 'utf8');
    payload = JSON.parse(payload);
    switch (process.env['GITHUB_EVENT_NAME']) {
        case 'pull_request':
            sub_work_dir = path.join(work_dir, payload.pull_request.head.sha);
            if (!fs.existsSync(sub_work_dir)) {
                fs.mkdirSync(sub_work_dir);
            }
            break;
        default:
            assert.fail();
    }
} else {
    sub_work_dir = work_dir;
}

env = Object.create(process.env);
env['DEPOT_TOOLS_METRICS'] = '0';
env['DEPOT_TOOLS_UPDATE'] = '0';
switch (process.platform) {
    case 'win32':
        env['Path'] = util.format('%s%s%s', path.join(sub_work_dir, 'depot_tools'), path.delimiter, env['Path']);
        env['DEPOT_TOOLS_WIN_TOOLCHAIN'] = '0';
        env['GYP_MSVS_VERSION'] = '2019';
        break;
    case 'linux':
        env['PATH'] = util.format('%s%s%s', env['PATH'], path.delimiter, path.join(sub_work_dir, 'depot_tools'));
        break;
    default:
        assert.fail();
}
if ('GN_EDITOR' in env) {
    // Teach GN on Windows about GN_EDITOR by making EDITOR an alias of
    // it (GN_EDITOR takes precedence over EDITOR on POSIX, so
    // overwriting EDITOR is a no-op there)
    env['EDITOR'] = env['GN_EDITOR'];
}

cmd = [ 'gn', 'args', 'out' ];
options = {
    cwd: path.join(sub_work_dir, 'aquarium'),
    stdio: 'inherit',
    env: env,
    shell: shell,
};
child_process.execFileSync(cmd[0], cmd.slice(1), options);
