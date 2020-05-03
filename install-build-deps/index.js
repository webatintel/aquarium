// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// The GitHub runner environment is optional for this script, so it can work in local.

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
var sub_work_dir;

var cmd;
var options;

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

cmd = [ 'sh', '-c', '$(realpath install-build-deps.sh) --no-syms --no-chromeos-fonts' ];
if (os.userInfo().uid != 0) {
    cmd = [ 'sudo' ].concat(cmd);
}
options = {
    cwd: path.join(sub_work_dir, 'aquarium', 'build'),
    stdio: 'inherit',
};
child_process.execFileSync(cmd[0], cmd.slice(1), options);
