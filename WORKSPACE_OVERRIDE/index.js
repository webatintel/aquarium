// Copyright (c) 2020 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This script should be skipped when running locally.

var assert = require('assert');
var path = require('path');
var util = require('util');
var actions = require('@actions/core');

var workspace_override = actions.getInput('workspace-override');
workspace_override = path.resolve(workspace_override);
actions.exportVariable('WORKSPACE_OVERRIDE', workspace_override);
