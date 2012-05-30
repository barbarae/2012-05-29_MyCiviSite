<?php
// $Id$

/**
 * Implementation of githook_PLUGIN_event_action().
 */
function githook_php_event_action($event) {
  if ($event == 'pre-commit' || $event == 'post-receive') {
    _githook_php_validate_php_syntax();
  }
}

/**
 * Check that all PHP files have valid syntax.
 * @see http://phpadvent.org/2008/dont-commit-that-error-by-travis-swicegood
 */
function _githook_php_validate_php_syntax() {
  $test = 0;
  $regex = php_get_extensions(TRUE);

  foreach (githook_staged_files() as $file) {
    if (!preg_match($regex, $file)) {
        // Only check PHP files.
        continue;
    }
    if (drush_get_context('DRUSH_DEBUG')) {
      drush_log(dt("Proceeding with PHP syntax checking for '!file'", array('!file' => $file)), 'notice');
    }

    // Now to check PHP sytax.
    $lint_output = array();
    exec("php -l " . escapeshellarg($file), $lint_output, $test);
    if ($test == 0) {
        continue;
    }
    drush_set_error('GITHOOK_PHP_INVALID', implode("\n", $lint_output));
  }
}

/**
 * Helper function to identify all PHP file extensions.
 *
 * @param $regex
 *  (default: FALSE) Arrange file extensions as a regular expression.
 * @param $reset
 *  (default: FALSE) Recalculate all extensions.
 *
 * @return
 *  Array of file extensions.
 */
function php_get_extensions($regex = FALSE, $reset = FALSE) {
  static $ext;
  if (empty($ext) || $reset) {
    $ext['raw'] = array(
      'php' => 'php',
      'phps' => 'phps',
      'module' => 'module',
      'inc' => 'inc',
      'test' => 'test',
      'install' => 'install',
      'engine' => 'engine',
      'theme' => 'theme',
    );
    $ext['regex'] = '/\.(' . implode('|', $ext['raw']) . ')$/S';
  }
  return $regex ? $ext['regex'] : $ext['raw']; 
}

