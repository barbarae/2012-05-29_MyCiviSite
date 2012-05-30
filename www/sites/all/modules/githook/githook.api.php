\<?php

/**
 * @file
 *  Git Hook Plugin API.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Implementation of githook_PLUGIN_event_action_allowed().
 */
function githook_plugin_event_action_allowed($event) {
  if ($event == 'pre-commit') {
    return strpos(githook_last_tag(), '7.x') !== FALSE;
  }
  return TRUE;
}

/**
 * Implementation of githook_PLUGIN_event_action().
 *
 * Invalidate a command by using drush_set_error() at any time.
 *
 * @param $event
 *  The active git event.
 * @param $arguments
 *  Arguments passed in by git related to the current operation.
 */
function githook_plugin_event_action($event, $arguments) {
  switch ($event) {
    case 'pre-commit':
    case 'pre-receive':
      _githook_plugin_approve_the_code();
  }
}

/**
 * Implementation of githook_PLUGIN_event_EVENT_action().
 *
 * Invalidate a command by using drush_set_error() at any time.
 *
 * @param $arguments
 *  Arguments passed in by git related to the current operation.
 */
function githook_plugin_event_pre_commit_action($arguments) {
  if (!_githook_plugin_demand_money_for_commit()) {
    drush_set_error('GITHOOK_NO_BRIBE', dt('Did not bribe the git daemon. Goodbye.'));
  }
}

/**
 * Implementation of githook_PLUGIN_event_EVENT_post_action().
 *
 * Invalidate a command by using drush_set_error() at any time.
 *
 * This command is intended not for actual validation, but to perform
 * any kind of feedback or post-workflow cleanup.
 *
 * @param $arguments
 *  Arguments passed in by git related to the current operation.
 */
function githook_plugin_event_pre_commit_post_action($arguments) {
  drush_print(dt('Thank you for running githook!'));
}

/**
 * @} End of "addtogroup hooks".
 */
