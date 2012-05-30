<?php

/**
 * Implements hook_token_info_alter().
 */
function date_partofday_token_info_alter(&$info) {
  $info['tokens']['date']['partofday'] = array(
    'name' => t('Part of day'),
    'description' => t('Part of day: %morning, %afternoon or %evening', array(
      '%morning' => t('morning'),
      '%afternoon' => t('afternoon'),
      '%evening' => t('evening'),
    )),
    'module' => 'date_partofday',
  );
}

/**
 * Implements hook_tokens().
 */
function date_partofday_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }

  if ($type == 'date') {
    $date = !empty($data['date']) ? $data['date'] : REQUEST_TIME;

    if (isset($tokens['partofday'])) {
      $hour = (int) format_date($date, 'custom', 'G', NULL, $language_code);
      if ($hour < 12) {
        $partofday = t('morning');
      }
      elseif ($hour < 18) {
        $partofday = t('afternoon');
      }
      elseif ($hour < 21) {
        $partofday = t('evening');
      }
      $replacements[$tokens['partofday']] = $partofday;
    }
  }

  return $replacements;
}
