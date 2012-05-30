<?php
/**
 * Override or insert variables into page templates.
 */
function adaptivetheme_admin_preprocess_page(&$vars) {
  global $user;
  $vars['datetime_rfc'] = '';
  $vars['datetime_iso'] = '';
  $vars['datetime_rfc'] = date("r" , time()); // RFC2822 date format
  $vars['datetime_iso'] = date("c" , time()); // ISO 8601 date format
}
