// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// The GitHub runner environment is optional for this script, so it can work in local.

var assert = require('assert');
var child_process = require('child_process');
var fs = require('fs');
var glob = require('glob');
var path = require('path');
var util = require('util');

var work_dir = 'WORKSPACE_OVERRIDE' in process.env ? process.env['WORKSPACE_OVERRIDE'] :
               'GITHUB_WORKSPACE' in process.env ? process.env['GITHUB_WORKSPACE'] :
               process.cwd();
work_dir = path.resolve(work_dir);
var temp_dir = fs.mkdtempSync(util.format('%s%s', work_dir, path.sep));
var sub_work_dir;

var pattern;
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

fs.mkdirSync(path.join(temp_dir, 'git-templates'));

// Sync directories
pattern = 'git-templates/**';
options = {
    cwd: temp_dir,
    dot: true,
    absolute: true,
};
var src_dir = glob.sync(pattern, options);
options = {
    cwd: work_dir,
    dot: true,
    absolute: true,
};
var dst_dir = glob.sync(pattern, options);
while (src_dir[0] || dst_dir[0]) {
    if (src_dir[0] && dst_dir[0] && path.relative(temp_dir, src_dir[0]) == path.relative(work_dir, dst_dir[0])) {
        if (fs.statSync(src_dir[0]).isFile() && fs.statSync(dst_dir[0]).isFile()) {
            fs.copyFileSync(src_dir[0], dst_dir[0]);
        } else {
            assert(fs.statSync(src_dir[0]).isDirectory());
            assert(fs.statSync(dst_dir[0]).isDirectory());
        }
        assert(fs.statSync(src_dir[0]).mode == fs.statSync(dst_dir[0]).mode);
        src_dir = src_dir.slice(1);
        dst_dir = dst_dir.slice(1);
    } else if (dst_dir[0] && (!src_dir[0] || path.relative(temp_dir, src_dir[0]) > path.relative(work_dir, dst_dir[0]))) {
        var i = 0;
        while (dst_dir[i] && dst_dir[i].startsWith(dst_dir[0])) {
            i++;
        }
        var j = i - 1;
        while (j >= 0) {
            if (fs.statSync(dst_dir[j]).isFile()) {
                fs.unlinkSync(dst_dir[j]);
            } else {
                assert(fs.statSync(dst_dir[j]).isDirectory());
                fs.rmdirSync(dst_dir[j]);
            }
            j--;
        }
        dst_dir = dst_dir.slice(i);
    } else if (src_dir[0] && (!dst_dir[0] || path.relative(temp_dir, src_dir[0]) < path.relative(work_dir, dst_dir[0]))) {
        if (fs.statSync(src_dir[0]).isFile()) {
            fs.copyFileSync(src_dir[0], path.join(work_dir, path.relative(temp_dir, src_dir[0])));
        } else {
            assert(fs.statSync(src_dir[0]).isDirectory());
            fs.mkdirSync(path.join(work_dir, path.relative(temp_dir, src_dir[0])));
            fs.chmodSync(path.join(work_dir, path.relative(temp_dir, src_dir[0])), fs.statSync(src_dir[0]).mode);
        }
        src_dir = src_dir.slice(1);
    } else {
        assert.fail();
    }
}

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

cmd = [ 'gclient', 'sync' ];
options = {
    cwd: path.join(sub_work_dir, 'aquarium'),
    stdio: 'inherit',
    env: env,
    shell: shell,
};
child_process.execFileSync(cmd[0], cmd.slice(1), options);
