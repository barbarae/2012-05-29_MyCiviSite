<?php
// Pixture Reloaded by Adaptivethemes.com
/**
 * Override or insert variables into the html template.
 */
function pixture_reloaded_preprocess_html(&$vars) {

  global $theme, $theme_key;

  // Set a body class based on the color scheme name
  if (module_exists('color')) {
    $info = color_get_info($theme);
    $info['schemes'][''] = array('title' => t('Custom'), 'colors' => array());
    $schemes = array();
    foreach ($info['schemes'] as $key => $scheme) {
      $schemes[$key] = $scheme['colors'];
    }
    $current_scheme = variable_get('color_' . $theme . '_palette', array());
    foreach ($schemes as $key => $scheme) {
      if ($current_scheme == $scheme) {
        $scheme_name = $key;
        break;
      }
    }
    if (empty($scheme_name)) {
      if (empty($current_scheme)) {
        $scheme_name = 'default';
      }
      else {
        $scheme_name = 'custom';
      }
    }
    $vars['classes_array'][] = 'color-scheme-' . drupal_html_class($scheme_name);
  }

  // Set a body class based on the theme name
  $vars['classes_array'][] = drupal_html_class($theme_key);
  
  // Theme settings classes
  $vars['classes_array'][] = theme_get_setting('font_family');
  $vars['classes_array'][] = theme_get_setting('headings_font_family');
  $vars['classes_array'][] = theme_get_setting('font_size');
  $vars['classes_array'][] = theme_get_setting('box_shadows');
  $vars['classes_array'][] = theme_get_setting('corner_radius');
  if (module_exists('noggin')) {
    $va = theme_get_setting('noggin_image_vertical_alignment');
    $ha = theme_get_setting('noggin_image_horizontal_alignment');
    $vars['classes_array'][] = 'ni-a-' . $va . $ha;
    $vars['classes_array'][] = theme_get_setting('noggin_image_repeat');
    $vars['classes_array'][] = theme_get_setting('noggin_image_width');
  }
  if (theme_get_setting('headings_styles_caps') == 1) {
    $vars['classes_array'][] = 'hs-caps';
  }
  if (theme_get_setting('headings_styles_weight') == 1) {
    $vars['classes_array'][] = 'hs-fwn';
  }
  if (theme_get_setting('headings_styles_shadow') == 1) {
    $vars['classes_array'][] = 'hs-ts';
  }
}

/**
 * Override or insert variables into the html template.
 */
function pixture_reloaded_process_html(&$vars) {
  // Hook into color.module
  if (module_exists('color')) {
    _color_html_alter($vars);
  }
}

/**
 * Override or insert variables into the page template.
 */
function pixture_reloaded_process_page(&$vars) {
  // Hook into color.module
  if (module_exists('color')) {
    _color_page_alter($vars);
  }
}

/**
 * Override or insert variables into the block template.
 */
function pixture_reloaded_preprocess_block(&$vars) {
  if($vars['block']->module == 'superfish' || $vars['block']->module == 'nice_menu') {
    $vars['content_attributes_array']['class'][] = 'clearfix';
  }
}
