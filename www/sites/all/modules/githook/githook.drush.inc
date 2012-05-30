<?php
/**
 * @file
 *  Drush commands to facilitate git event workflows in Drupal development.
 */

define('GITHOOK_API_VERSION', 1);

/**
 * Implementation of hook_drush_command().
 */
function githook_drush_command() {
  $items = array();

  $items['githook'] = array(
    'description' => 'Install & configure drush control of local git repository hooks.',
    'options' => array(
      'install' => 'Install git hook integration to your local git repo.',
      'uninstall' => 'Uninstall git hook integration and restore original hooks.',
    ),
    'sub-options' => array(
      'install' => array(
        'set-debug' => 'Tell git to run githook-event in --debug mode. Incompatible with --template.',
        'template' => "Select a file for use as the template for git script replacement. See 'resources/default.hook_template.php' for an example. Incompatible with --set-debug.",
        'event-blacklist' => 'Do not install githook wrappers for a comma or space-delimited list of git hooks. If existing event scripts are found, keep them live.',
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  $items['githook-event'] = array(
    'description' => 'React to triggered Git events and test workflows.',
    'arguments' => array(
      'event' => 'Specify which git event to trigger.',
      'arguments' => 'Arguments passed on by Git for the specific event.'
    ),
    'options' => array(
      'live' => 'githook-event is always in --simulate mode unless overriden with --live. Used by git.',
    ),
    'examples' => array(
      'drush githook-event pre-commit' => 'A git commit is about to happen, react or reject the commit.',
      'drush githook-event post-commit' => 'Signal that a git commit just completed.',
      // Fill in examples for each event to document them.
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  return $items;
}

/**
 * Implementaton of hook_drush_help().
 */
function githook_drush_help($section) {
  switch ($section) {
    case 'drush:githook':
      return dt('Install & configure drush control of local git repository hooks. To learn more about hooks, type `git help hooks`.');
    case 'drush:githook-event':
      return dt('React to triggered Git events and test workflows.');
  }
}

/**
 * Implementation of hook_drush_help_alter().
 */
function githook_drush_help_alter(&$command) {
  // If any githook-internal plugins want to modify options, let them.
  if ($command['commandfile'] == 'githook') {
    $plugins = githook_plugins_load();
    foreach ($plugins as $plugin) {
      $function = "githook_{$plugin}_drush_help_alter";
      if (function_exists($function)) {
        $function($command);
      }
    }
  }
}

/**
 * Implementation of hook_drush_init().
 */
function githook_drush_init() {
  $command_info = drush_get_command();
  if ($command_info['commandfile'] == 'githook') {
    include_once(dirname(__FILE__) . '/githook.inc');
    _drush_githook_git_repo_validate();
  }
}

/**
 * Implementation of drush_COMMAND_validate() for githook-event.
 */
function drush_githook_event_validate($event = NULL) {
  drush_set_context('DRUSH_SIMULATE', !drush_get_option('live', FALSE));
}

/**
 * Validate that drush is being invoked from within a git repo.
 */
function _drush_githook_git_repo_validate() {
  $status = githook_repository_path();
  if (!$status) {
    return drush_set_error('GIT_NO_REPO', dt('Must be run inside a git repository. To make the current directory into the root of a new git repository, run `git init; git add .; git commit -m "Initial commit."`'));
  }
  return TRUE;
}

/**
 * Implementation of drush_COMMAND() for githook-event.
 */
function drush_githook() {
  $path = githook_repository_path();
  $backup = TRUE;

  if (drush_get_option('install', FALSE)) {
    if (file_exists($path . '/hooks/GITHOOK_VERSION.txt')) {
      $version_info = file_get_contents($path . '/hooks/GITHOOK_VERSION.txt');
      if ($version_info == GITHOOK_API_VERSION) {
        return drush_set_error('GITHOOK_ALREADY_INSTALLED', dt('Githook is already properly installed in this repository.'));
      }
      else {
        drush_log('An older version of githook has been installed. Updating...', 'notice');
        $backup = FALSE;
      }
    }
    // Backup original hooks.
    if ($backup) {
      if (drush_get_context('DRUSH_VERBOSE')) {
        drush_log('Backing up original .git/hooks directory.', 'notice');
      }
      _githook_backup_hooks($path);
    }

    // Overwrite hooks directory.
    if (_githook_install_githook($path)) {
      drush_log(dt('Successfully installed githook to !repo', array('!repo' => $path)), 'success');
    }
    else {
      _githook_restore_hooks($path);
    }
    return;
  }
  elseif (drush_get_option('uninstall', FALSE)) {
    if (file_exists($path . '/hooks/GITHOOK_VERSION.txt')) {
      _githook_restore_hooks($path);
      drush_log('Successfully reverted to original git hooks directory. Drush is no longer involved in this repository.', 'success');
    }
    else {
      drush_log('Githook is not installed, nothing to do here.', 'ok');
    }
  }
}

/**
 * Implementation of drush_COMMAND() for githook-event.
 */
function drush_githook_event($event = NULL, $arguments = NULL) {
  if (isset($event)) {
    drush_log(dt('githook-event triggered with !event', array('!event' => $event)), 'notice');
  }

  $event = _drush_githook_get_event($event);
  if ($event === FALSE) {
    return drush_set_error('GITHOOK_INVALID_EVENT', dt('You must specify a valid git event.'));
  }

  // Invoking hooks like githook_pre_commit.
  githook_plugins_invoke($event, $arguments);
}

/**
 * Invoke all event actions for githook plugins in the plugins/ directory.
 *
 * Actions have no return value because all they need to do is use drush_set_error()
 * and it will count as a failure for validation hooks.
 */
function githook_plugins_invoke($event, $arguments = NULL) {
  $plugins = githook_plugins_load($event, TRUE);

  // Process git arguments back into an array.  
  if (isset($arguments)) {
    $arguments = explode(':', $arguments);
  }

  $event_raw = $event;
  $event = strtr($event, '-', '_');
  foreach ($plugins as $plugin) {
    // Invoke hook_githook_PLUGIN_event_action().
    $function = 'githook_' . $plugin . '_event_action';
    if (function_exists($function)) {
      drush_log(dt('Dispatching githook_PLUGIN_event_action() for !plugin:!event.', array('!plugin' => $plugin, '!event' => $arg)), 'notice');
      $function($event_raw, $arguments);
    }
  }
  foreach ($plugins as $plugin) {
    // Invoke hook_githook_PLUGIN_event_EVENT_action().
    $function = 'githook_' . $plugin . '_event_' . $event . '_action';
    if (function_exists($function)) {
      drush_log(dt('Dispatching githook_PLUGIN_event_EVENT_action() for !plugin:!event.', array('!plugin' => $plugin, '!event' => $arg)), 'notice');
      $function($arguments);
    }
  }
  foreach ($plugins as $plugin) {
    // Invoke hook_githook_PLUGIN_event_EVENT_post_action().
    $function = 'githook_' . $plugin . '_event_' . $event . '_post_action';
    if (function_exists($function)) {
      drush_log(dt('Dispatching githook_PLUGIN_event_EVENT_post_action() for !plugin:!event.', array('!plugin' => $plugin, '!event' => $arg)), 'notice');
      $function($arguments);
    }
  }
}

/**
 * Include all githook plugin files.
 *
 * @param $event
 *  The current git event. If NULL will skip event-specific files.
 * @param $filter
 *  (default: FALSE) Filter out plugins which are blacklisted or opt-out of running.
 * @param $reset
 *  (default: FALSE) Reset the static cache.
 *
 * @return
 *  Array of plugins.
 */
function githook_plugins_load($event = NULL, $filter = FALSE, $reset = FALSE) {
  static $plugins;

  if (empty($plugins) || $reset) {
    $dir = dirname(__FILE__) . '/plugins';
    $plugins = array();

    $dh = opendir($dir);
    if (!$dh) {
      return array();
    }
    while (($filename = readdir($dh)) !== FALSE) {
      $parts = explode('.', $filename);
      $ext = array_pop($parts);
      if ($ext != 'inc') {
        continue;
      }
      if (isset($event)) {
        $file_event = array_pop($parts);
        if (_githook_is_git_event($file_event) && $file_event != $event) {
          continue;
        }
        elseif (!_githook_is_git_event($file_event)) {
          array_push($parts, $file_event);
        }
      }
      include_once($dir . '/' . $filename);

      // Sanitize file name for use as plugin name.
      $plugin = strtr(implode('_', $parts), '-', '_');
      $plugins[$plugin] = $plugin;
    }
    asort($plugins);
    drush_log('Loaded plugins: ' . implode(', ', $plugins), 'notice');
  }
  if ($filter) {
    _githook_filter_plugins($plugins, $event);
  }
  return $plugins;
}

/**
 * Allow plugins to dynamically opt-out of being invoked at this time.
 *
 * If a plugin returns FALSE for an implementation of
 * hook_githook_PLUGIN_event_action_allowed(), that plugin will not be invoked
 * for any further events.
 *
 * This is the place for any option-level blacklisting of specific plugins.
 *
 * @param $plugins
 *  List of all plugins in the githook directory.
 * @param $event
 *  Git event being processed.
 */
function _githook_filter_plugins(&$plugins, $event) {
  foreach ($plugins as $key => $plugin) {
    // Invoke hook_githook_event_action_allowed()
    $function = 'githook_' . $plugin . '_event_action_allowed';
    if (function_exists($function) && !$function($event)) {
      unset($plugins[$key]);
    }
  }
}

/**
 * Helper function to assign or guide user selection of a git event.
 */
function _drush_githook_get_event($event = NULL) {
  if (isset($event) && _githook_is_git_event($event)) {
    return $event;
  }
  return drush_choice(githook_git_events());
}

/**
 * Validate that a specified term is a git event.
 */
function _githook_is_git_event($event) {
  return array_key_exists($event, githook_git_events());
}

/**
 * Set up files that point git events to launch drush.
 */
function _githook_install_githook($path) {
  $path .= '/hooks';
  $template = _githook_hook_template_path();
  if (!$template) {
    return FALSE;
  }

  // If code is ever "1", a file operation failed.
  $code = 0;
  foreach (githook_git_events() as $event) {
    $file = $path . "/$event";
    $code |= drush_op_system("cp $template $file");
    $code |= drush_op_system("chmod +x $file");   
  }

  // Tag the githook version in the hooks directory.
  $code |= drush_op_system('echo "' . GITHOOK_API_VERSION . '" > ' . $path . '/GITHOOK_VERSION.txt');

  // Add a brief explanation of what's going on in the directory.
  $readme = "This directory is under the management of the Drush shell script to facilitate Drupal development.\n\n"
          . "To revert it to it's state before drush took over, please run \`drush githook --uninstall\`\n"
          . "from the command line within the git directory.\n\n"
          . "Please do not change, delete, or move the GITHOOK_VERSION.txt file, as it is used to identify installation status.";

  $file = $path . '/README.txt';
  $code |= drush_op_system("echo \"$readme\" > $file");

  // Preserve blacklisted event scripts.
  foreach (_githook_events_blacklist() as $event) {
    $source = $path . ".backup/$event";
    if (file_exists($source)) {
      $code |= !copy($path . ".backup/$event", $path . "/$event");
    }
  }

  if ($code) {
    drush_set_error('GITHOOK_INSTALL_FAILURE', 'Did not install githooks correctly. Check your permissions and try again.');
  }

  // File operations return "1" on failure and "0" on success, therefore we reverse for better logic.
  return !$code;
}

/**
 * Get the path to the hook template script for githook installation.
 */
function _githook_hook_template_path() {
  $template = drush_get_option('template', FALSE);
  if (!$template) {
    if (drush_get_option('set-debug', FALSE)) {
      $path = dirname(__FILE__) . '/resources/debug.hook_template.php';
    }
    else {
      $path = dirname(__FILE__) . '/resources/default.hook_template.php';
    }
    if (file_exists($path)) {
      $template = $path;
    }
    else {
      return drush_set_error('GITHOOK_NO_TEMPLATE', "There is no template file for the git hook script. Please restore the file 'resources/default.hook_template.php'.");
    }
  }
  return $template;
}

/**
 * Backup the original git hooks directory before installing drush githook.
 *
 * @param $path
 *  Path to the current git repository.
 */
function _githook_backup_hooks($path) {
  $path .= '/hooks';
  drush_op_system("mv $path $path.backup");
  drush_op_system("mkdir $path");
}

/**
 * Restore the git hooks directory in place before drush githook.
 *
 * @param $path
 *  Path to the current git repository.
 1*/
function _githook_restore_hooks($path) {
  $path .= '/hooks';
  drush_op_system("rm -Rf $path");
  drush_op_system("mv $path.backup $path");
}

/**
 * Get the array of all events that should not be handled by githook.
 */
function _githook_events_blacklist() {
  $raw = drush_get_option('events-blacklist', '');
  if (empty($raw)) {
    return array();
  }
  return preg_split('/[\s,]+/', $raw);
}
