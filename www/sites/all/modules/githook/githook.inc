<?php
// $Id$

/**
 * @file
 *  Library of Git wrapper functions.
 */

/**
 * Get path to current git repository.
 * 
 * @param $reset
 *  Reset static cache.
 *
 * @return
 *  Path to current git repo, FALSE if not a repo
 */
function githook_repository_path($reset = FALSE) {
  static $repo;

  if (!isset($repo) || $reset) {
    $output = array();
    $repo = exec('git rev-parse --git-dir 2> /dev/null', $output);
    if ($repo) {
      $repo = $output[0];
    }
  }

  return $repo;
}

/**
 * Return a list of the names of all staged files.
 */
function githook_staged_files($reset = FALSE) {
  static $staged_files;
  if (empty($staged_files) || $reset) {
    $output = array();
    $against = githook_reference_last_commit();
    exec("git diff-index --cached --name-only {$against}", $output);
    $staged_files = $output;
  }
  return $staged_files;
}

/**
 * Indenfity whether this is the first commit to the repository.
 *
 * @param $reset
 *  Reset the static cache.
 *
 * @return boolean
 *  TRUE if there is no commit to the repository, FALSE if there is a commit record.
 */
function githook_is_first_commit($reset = FALSE) {
  static $status;
  if (!isset($status) || $reset) {
    $output = array();
    exec('git rev-parse --verify HEAD 2> /dev/null', $output, $status);
  }
  return $status;
}

/**
 * Retrieve a pointer to the last commit.
 *
 * While HEAD normally refers to the last commit in the repo, if there has
 * never been a sucessful commit, it has nothing to point to.
 */
function githook_reference_last_commit() {
  if (githook_is_first_commit()) {
    // Magic commit for 'blank' repo.
    return '4b825dc642cb6eb9a060e54bf8d69288fbee4904';
  }
  return 'HEAD';
}

/**
 * Retrieve the name of the last tag on this branch.
 *
 * @param $reset
 *  (default: FALSE) Reset static cache.
 *
 * @see _githook_describe
 */
function githook_last_tag($reset = FALSE) {
  return _githook_describe('tag');
}

/**
 * Retrieve the number of commits since the last tag.
 *
 * @param $reset
 *  (default: FALSE) Reset static cache.
 *
 * @see _githook_describe
 */
function git_commits_since_last_tag($reset = FALSE) {
  return _git_describe('since_last_tag');
}

/**
 * Retrieve the name of the last commit.
 *
 * @param $reset
 *  (default: FALSE) Reset static cache.
 *
 * @see _githook_describe
 */
function githook_last_commit($reset = FALSE) {
  return _githook_describe('commit');
}

/**
 * Get the parsed results of git describe.
 *
 * @param $key
 *  (default: NULL) May be set to one of following to determine return.
 *  - tag: The most recent tag on the current branch.
 *  - since_last_tag: The number of commits since the most recent tag.
 *  - commit: The short commit name.
 * @param $reset
 *  (default: FALSE) Reset the static cache.
 */
function _githook_describe($key = NULL, $reset = FALSE) {
  static $describe;

  if (empty($describe) || $reset) {
    $describe = $output = array();
    exec('git describe --long --always --tags', $output);
    if(preg_match('/(.*?)\-(\d+)\-([\d\w]+)$/', $output[0], $matches)) {
      $describe = array(
        'all' => $matches[0],
        'tag' => $matches[1],
        'since_last_tag' => $matches[2],
        'commit' => $matches[3],
      );
    }
    else {
      $describe = array(
        'all' => '-0-' . $output[0],
        'tag' => NULL,
        'since_last_tag' => '0',
        'commit' => $output[0],
      );
    }
  }

  return isset($key) ? $describe[$key] : $describe;
}

/**
 * Get a list of all git events.
 */
function githook_git_events() {
  static $events;

  if (empty($events)) {
    $events = array(
      'applypatch-msg' => 'applypatch-msg',
      'commit-msg' => 'commit-msg',
      'post-applypatch' => 'post-applypatch',
      'post-checkout' => 'post-checkout',
      'post-commit' => 'post-commit',
      'post-merge' => 'post-merge',
      'post-receive' => 'post-receive',
      'post-rewrite' => 'post-rewrite',
      'post-update' => 'post-update',
      'pre-applypatch' => 'pre-applypatch',
      'pre-auto-gc' => 'pre-auto-gc',
      'pre-commit' => 'pre-commit',
      'prepare-commit-msg' => 'prepare-commit-msg',
      'pre-rebase' => 'pre-rebase',
      'pre-receive' => 'pre-receive',
      'update' => 'update',
    );
    $events = array_diff($events, _githook_events_blacklist());
  }
  return $events;
}

