<?php

/**
 * @file
 *  Helpful messages and guidance for use by GitHook command.
 */

/**
 * Implementation of githook_PLUGIN_event_EVENT_action() for post-commit.
 */
function githook_help_event_post_commit_action() {
  if (githook_is_first_commit()) {
    print("Congratulations on your first commit! To host this project as a Drupal.Org project, visit http://drupal.org/node/add/project, fill in the form and submit. Visit the Git Instructions tab to see how to push your code to the public repository.\n");
  }
}


/**
 * Implementation of githook_PLUGIN_event_EVENT_action() for pre-commit.
 */
function githook_help_event_pre_commit_post_action() {
  if (drush_get_error()) {
    // Doing this here is a cheat that takes advantage of the fact that
    // event-specific hook comes after general hook implementation, and
    // currently all defined validations happen in the general hook.
    print("Your prospective commit did not pass githook validations. To commit anyway, re-run `git commit` with --no-verify option.\n");
  }
}
