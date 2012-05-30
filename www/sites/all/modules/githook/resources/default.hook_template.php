#!/usr/bin/env php
<?php
/**
 * This hook is managed by Drush to facilitate Drupal development in Git.
 */
$event = basename(__FILE__);

// Get the arguments passed by Git.
$arguments = $argv;
array_shift($arguments); // drush_shift
if (!empty($arguments)) {
  $arguments = implode(':', $arguments);
}

$command = "drush githook-event $event '$arguments' --live";
passthru($command, $status);
exit((int) ($status == 1));

