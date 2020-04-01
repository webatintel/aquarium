// This script should be skipped when running locally.

var assert = require('assert');
var child_process = require('child_process');
var os = require('os');
var util = require('util');
switch (process.platform) {
    case 'win32':
        var dns = require('dns');
        break;
    case 'linux':
        var dbus = require('dbus-native');
        break;
    default:
        assert.fail();
}

var cmd;
var options;

util.promisify(function (callback) {
    callback();
})().then(function () {
    // Get FQDN
    switch (process.platform) {
        case 'win32':
            return util.promisify(function (callback) {
                callback();
            })().then(function () {
                return util.promisify(dns.lookup)(os.hostname());
            }).then(function (addr_info) {
                return util.promisify(dns.lookupService)(addr_info.address, 0);
            }).then(function (name_info) {
                return name_info.hostname;
            });
            break;
        case 'linux':
            // https://www.freedesktop.org/wiki/Software/systemd/resolved/
            var bus;
            return util.promisify(function (callback) {
                callback();
            })().then(function () {
                bus = dbus.systemBus();

                var msg = {
                    path:        '/org/freedesktop/resolve1',
                    interface:   'org.freedesktop.DBus.Properties',
                    destination: 'org.freedesktop.resolve1',
                    member:      'Get',
                    signature:   'ss',
                    body:        [ 'org.freedesktop.resolve1.Manager', 'Domains' ],
                };
                return util.promisify(bus.invoke).call(bus, msg);
            }).then(function (reply) {
                bus.connection.end();

                var signature = reply[0]; // 'a(isb)'
                var value = reply[1];
                var domains = value[0];
                var domain = domains[0]; // poor man's way to pick a domain name
                var ifindex = domain[0];
                var domain_name = domain[1];
                var route_only = domain[2];
                return util.format('%s.%s', os.hostname(), domain_name);
            });
            break;
        default:
            assert.fail();
    }
}).then(function (fqdn) {
    var user = os.userInfo().username;

    cmd = [ 'git', 'config', '--global', 'user.name', user ];
    options = {
        stdio: 'inherit',
    };
    child_process.execFileSync(cmd[0], cmd.slice(1), options);

    cmd = [ 'git', 'config', '--global', 'user.email', util.format('%s@%s', user, fqdn) ];
    options = {
        stdio: 'inherit',
    };
    child_process.execFileSync(cmd[0], cmd.slice(1), options);

    if (process.platform == 'win32') {
        cmd = [ 'git', 'config', '--global', 'core.longpaths', 'true' ];
        options = {
            stdio: 'inherit',
        };
        child_process.execFileSync(cmd[0], cmd.slice(1), options);
    }
});
