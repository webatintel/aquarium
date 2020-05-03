// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This script should be skipped when running locally.

var assert = require('assert');
var util = require('util');
var actions = require('@actions/core');

var build_config;
switch (process.env['GITHUB_EVENT_NAME']) {
    case 'pull_request':
        build_config = actions.getInput('build-config');
        break;
    default:
        assert.fail();
}
actions.exportVariable('BUILD_CONFIG', build_config);