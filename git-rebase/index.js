// This script should be skipped when running locally.

// When running in GitHub hosted runners, the script writes the todo
// list to a staging file first, and then a copy of the file is read by
// Git during interactive rebase. The latter works because Git would
// fire up the command given by the GIT_EDITOR environment variable,
// which has been set to system-specific file copy commands beforehand,
// with the staging file being the source.

var assert = require('assert');
var child_process = require('child_process');
var fs = require('fs');
var path = require('path');
var util = require('util');

var work_dir = 'WORKSPACE_OVERRIDE' in process.env ? process.env['WORKSPACE_OVERRIDE'] :
               'GITHUB_WORKSPACE' in process.env ? process.env['GITHUB_WORKSPACE'] :
               process.cwd();
work_dir = path.resolve(work_dir);
var temp_dir = fs.mkdtempSync(util.format('%s%s', work_dir, path.sep));
var sub_work_dir;

var cmd;
var options;

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

var merge_base;
cmd = [ 'git', 'merge-base', 'refs/tags/GITHUB_SHA^1', 'refs/tags/GITHUB_SHA^2' ];
options = {
    cwd: path.join(sub_work_dir, 'aquarium'),
    encoding: 'utf8',
};
merge_base = child_process.execFileSync(cmd[0], cmd.slice(1), options);
merge_base = merge_base.trim();

var commits; // commits for testing
cmd = [ 'git', 'rev-list', util.format('^%s', merge_base), 'refs/tags/GITHUB_SHA^2' ];
options = {
    cwd: path.join(sub_work_dir, 'aquarium'),
    encoding: 'utf8',
};
commits = child_process.execFileSync(cmd[0], cmd.slice(1), options);
commits = commits.trim().split('\n');

var commit; // the squashed commit
cmd = [ 'git', 'rev-parse', 'refs/tags/GITHUB_SHA^3' ];
options = {
    cwd: path.join(sub_work_dir, 'aquarium'),
    encoding: 'utf8',
};
commit = child_process.execFileSync(cmd[0], cmd.slice(1), options);
commit = commit.trim();

// Build the todo list for interactive rebase
var todo_list;
if (commits.length) {
    // Pick the squashed commit, and reword the commit message using that of the oldest unmerged commit
    todo_list = [
        util.format("pick %s", commit),
        util.format("exec git show --pretty=%%B --no-patch %s | git commit --amend --file=-", commits[commits.length - 1]),
    ].join('\n');
} else {
    // Maybe GITHUB_SHA refers to something already merged
    todo_list = "";
}

fs.writeFileSync(path.join(temp_dir, 'git.txt'), todo_list);
if (fs.existsSync(path.join(work_dir, 'git.txt'))) {
    assert(fs.statSync(path.join(work_dir, 'git.txt')).isFile());
    assert(fs.statSync(path.join(temp_dir, 'git.txt')).mode == fs.statSync(path.join(work_dir, 'git.txt')).mode);
}
fs.copyFileSync(path.join(temp_dir, 'git.txt'), path.join(work_dir, 'git.txt'));

cmd = [ 'git', 'rebase', '-i', '--onto', 'refs/tags/GITHUB_SHA^1', 'refs/tags/GITHUB_SHA^1' ];
options = {
    cwd: path.join(sub_work_dir, 'aquarium'),
    stdio: 'inherit',
};
child_process.execFileSync(cmd[0], cmd.slice(1), options);
