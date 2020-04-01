// The GitHub runner environment is optional for this script, so it can work in local.

var assert = require('assert');
var child_process = require('child_process');
var path = require('path');
var util = require('util');

var work_dir = 'WORKSPACE_OVERRIDE' in process.env ? process.env['WORKSPACE_OVERRIDE'] :
               'GITHUB_WORKSPACE' in process.env ? process.env['GITHUB_WORKSPACE'] :
               process.cwd();
work_dir = path.resolve(work_dir);

var shell;
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

var cmd;
switch (process.platform) {
    case 'win32':
        cmd = [ 'npm', 'install', 'glob', '@actions/core' ];
        break;
    case 'linux':
        // Git needs a fake email address in this workflow. During the
        // construction of the address, the D-Bus protocol will be used
        // for the domain name part of the FQDN.
        cmd = [ 'npm', 'install', 'glob', '@actions/core', 'dbus-native' ];
        break;
    default:
        assert.fail();
}
var options = {
    cwd: work_dir,
    stdio: 'inherit',
    shell: shell,
};
child_process.execFileSync(cmd[0], cmd.slice(1), options);
