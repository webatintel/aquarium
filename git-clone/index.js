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
var options;

var post_checkout;
if ('GITHUB_SHA' in process.env && 'BUILD_CONFIG' in process.env && 'GITHUB_EVENT_NAME' in process.env) {
    // The post-checkout hook will be executed at the end of git-clone.
    // The purpose of this hook is to create a special commit tagged as
    // GITHUB_SHA, which has three parent commits:
    //
    // * GITHUB_SHA^1: head of the base branch when the workflow starts
    // * GITHUB_SHA^2: head of the commits for testing
    // * GITHUB_SHA^3: squash of the commits for testing

    var build_config = process.env['BUILD_CONFIG'].split('-');

    assert(build_config[0]);
    assert(/^[0-9a-f]+$/.test(build_config[0]));

    switch (process.env['GITHUB_EVENT_NAME']) {
        case 'pull_request':
            post_checkout = [
                util.format("#!/bin/sh"),

                // Do nothing if there's already a tag named GITHUB_SHA
                util.format("SHA=$(git rev-parse --verify --quiet refs/tags/GITHUB_SHA)"),
                util.format("if [ -n \"$SHA\" ]"),
                util.format("then"),
                util.format("    exit"),
                util.format("fi"),

                // Validate the first field of the build config gives a
                // valid commit hash, either complete or abbreviated
                util.format("SHA=$(git rev-parse --verify --quiet \"%s^{commit}\")", build_config[0]),
                util.format("if [ -n \"$SHA\" ]"),
                util.format("then"),
                util.format("    if [ \"$SHA\" != \"%s\" ]", build_config[0]),
                util.format("    then"),
                util.format("        SHA=$(git show --pretty=%%h --no-patch \"%s\")", build_config[0]),
                util.format("        if [ \"$SHA\" != \"%s\" ]", build_config[0]),
                util.format("        then"),
                util.format("            exit 1"),
                util.format("        fi"),
                util.format("    fi"),
                util.format("else"),
                util.format("    exit 1"),
                util.format("fi"),

                // Create the squashed commit
                util.format("MERGE_BASE=$(git merge-base \"%s^1\" \"%s^2\")", process.env['GITHUB_SHA'], build_config[0]),
                util.format("SHA=$(printf \"\" | git commit-tree \"%s^2^{tree}\" -p \"$MERGE_BASE\")", build_config[0]),

                // Calculate the SHA of an empty tree (but no need to
                // write to the object database since it's hardcoded in
                // Git), and create a merge commit based on that (in
                // fact the tree object of the commit is unimportant)
                util.format("EMPTY_TREE=$(printf \"\" | git hash-object -t tree --stdin)"),
                util.format("SHA=$(printf \"\" | git commit-tree \"$EMPTY_TREE\" -p \"%s^1\" -p \"%s^2\" -p \"$SHA\")", process.env['GITHUB_SHA'], build_config[0]),
                util.format("git tag GITHUB_SHA \"$SHA\""),
            ].join('\n');
            break;
        default:
            assert.fail();
    }
} else {
    post_checkout = "#!/bin/true";
}

// Populate the template directory
var fd;
fs.mkdirSync(path.join(temp_dir, 'git-templates'));
fs.mkdirSync(path.join(temp_dir, 'git-templates', 'hooks'));
fd = fs.openSync(path.join(temp_dir, 'git-templates', 'hooks', 'post-checkout'), 'w');
fs.writeSync(fd, post_checkout);
fs.fchmodSync(fd, fs.fstatSync(fd).mode | 0o111);
fs.closeSync(fd);

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

cmd = [
    'git',
    'clone',
    '-c', 'remote.origin.fetch=+refs/pull/*/merge:refs/pull/origin/*/merge',
    util.format('https://github.com/%s.git', process.env['GITHUB_REPOSITORY']),
    'aquarium',
];
options = {
    cwd: sub_work_dir,
    stdio: 'inherit',
};
child_process.execFileSync(cmd[0], cmd.slice(1), options);
