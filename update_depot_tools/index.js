// The GitHub runner environment is optional for this script, so it can work in local.

var assert = require('assert');
var child_process = require('child_process');
var fs = require('fs');
var path = require('path');
var util = require('util');

var work_dir = 'WORKSPACE_OVERRIDE' in process.env ? process.env['WORKSPACE_OVERRIDE'] :
               'GITHUB_WORKSPACE' in process.env ? process.env['GITHUB_WORKSPACE'] :
               process.cwd();
work_dir = path.resolve(work_dir);
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

cmd = [ 'update_depot_tools' ];
options = {
    cwd: path.join(sub_work_dir, 'depot_tools'),
    stdio: 'inherit',
    env: env,
    shell: shell,
};
child_process.execFileSync(cmd[0], cmd.slice(1), options);
