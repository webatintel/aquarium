// This script should be skipped when running locally.

var assert = require('assert');
var path = require('path');
var util = require('util');
var actions = require('@actions/core');

var workspace_override = actions.getInput('workspace-override');
workspace_override = path.resolve(workspace_override);
actions.exportVariable('WORKSPACE_OVERRIDE', workspace_override);
