<?php
/**
 * @file theme-settings.php
 */
if (module_exists('noggin')) {
  $default_var = variable_get('noggin:header_selector', 'div#header');
  if ($default_var == 'div#header') {
    variable_set('noggin:header_selector', 'header#header');
  }
}
/**
 * Implements hook_form_system_theme_settings_alter().
 *
 * @param $form['at']
 *   Nested array of form elements that comprise the form.
 * @param $form['at']_state
 *   A keyed array containing the current state of the form.
 */
function pixture_reloaded_form_system_theme_settings_alter(&$form, &$form_state)  {

  // Create the form using Forms API: http://api.drupal.org/api/7
  if (theme_get_setting('enable_styles') == 'on') {
    $form['at']['font'] = array(
      '#type' => 'fieldset',
      '#title' => t('Fonts'),
    );
    $form['at']['font']['font_family'] = array(
      '#type' => 'select',
      '#title' => t('Font family'),
      '#default_value' => theme_get_setting('font_family'),
      '#options' => array(
        'ff-sss' => t('Helvetica Nueue, Trebuchet MS, Arial, Nimbus Sans L, FreeSans, sans-serif'),
        'ff-ssl' => t('Verdana, Geneva, Arial, Helvetica, sans-serif'),
        'ff-a'   => t('Arial, Helvetica, sans-serif'),
        'ff-ss'  => t('Garamond, Perpetua, Nimbus Roman No9 L, Times New Roman, serif'),
        'ff-sl'  => t('Baskerville, Georgia, Palatino, Palatino Linotype, Book Antiqua, URW Palladio L, serif'),
        'ff-m'   => t('Myriad Pro, Myriad, Arial, Helvetica, sans-serif'),
        'ff-l'   => t('Lucida Sans, Lucida Grande, Lucida Sans Unicode, Verdana, Geneva, sans-serif'),
      ),
    );
    $form['at']['font']['headings_font_family'] = array(
      '#type' => 'select',
      '#title' => t('Headings Font family'),
      '#default_value' => theme_get_setting('headings_font_family'),
      '#options' => array(
        'hff-sss' => t('Helvetica Nueue, Trebuchet MS, Arial, Nimbus Sans L, FreeSans, sans-serif'),
        'hff-ssl' => t('Verdana, Geneva, Arial, Helvetica, sans-serif'),
        'hff-a'   => t('Arial, Helvetica, sans-serif'),
        'hff-ss'  => t('Garamond, Perpetua, Nimbus Roman No9 L, Times New Roman, serif'),
        'hff-sl'  => t('Baskerville, Georgia, Palatino, Palatino Linotype, Book Antiqua, URW Palladio L, serif'),
        'hff-m'   => t('Myriad Pro, Myriad, Arial, Helvetica, sans-serif'),
        'hff-l'   => t('Lucida Sans, Lucida Grande, Lucida Sans Unicode, Verdana, Geneva, sans-serif'),
      ),
    );
    $form['at']['font']['font_size'] = array(
      '#type' => 'select',
      '#title' => t('Base Font Size'),
      '#default_value' => theme_get_setting('font_size'),
      '#description' => t('This sets a base font-size on the body element - all text will scale relative to this value.'),
      '#options' => array(
        'fs-10' => t('0.833em'),
        'fs-11' => t('0.917em'),
        'fs-12' => t('1em'),
        'fs-13' => t('1.083em'),
        'fs-14' => t('1.167em'),
        'fs-15' => t('1.25em'),
        'fs-16' => t('1.333em'),
      ),
    );
    $form['at']['font']['headings_styles'] = array(
      '#type' => 'container',
      '#title' => t('Heading Styles'),
      '#description' => t('Add extra styles to headings. Shadows ony work for CSS3 capable browsers such as Firefox, Safari, IE9 etc.'),
    );
    $form['at']['font']['headings_styles']['headings_styles_caps'] = array(
      '#type' => 'checkbox',
      '#title' => t('All Caps'),
      '#default_value' => theme_get_setting('headings_styles_caps'),
    );
    $form['at']['font']['headings_styles']['headings_styles_weight'] = array(
      '#type' => 'checkbox',
      '#title' => t('Font weight normal'),
      '#default_value' => theme_get_setting('headings_styles_weight'),
    );
    $form['at']['font']['headings_styles']['headings_styles_shadow'] = array(
      '#type' => 'checkbox',
      '#title' => t('Text shadows'),
      '#default_value' => theme_get_setting('headings_styles_shadow'),
    );
    $form['at']['corners'] = array(
      '#type' => 'fieldset',
      '#title' => t('Rounded corners'),
    );
    $form['at']['corners']['corner_radius'] = array(
      '#type' => 'select',
      '#title' => t('Corner radius'),
      '#default_value' => theme_get_setting('corner_radius'),
      '#description' => t('Change the corner radius for blocks, node teasers and comments.'),
      '#options' => array(
        'rc-0' => t('none'),
        'rc-4' => t('4px'),
        'rc-8' => t('8px'),
        'rc-12' => t('12px'),
      ),
    );
    $form['at']['pagestyles'] = array(
      '#type' => 'fieldset',
      '#title' => t('Page Box Shadow'),
    );
    $form['at']['pagestyles']['box_shadows'] = array(
      '#type' => 'radios',
      '#title' => t('Box shadow'),
      '#default_value' => theme_get_setting('box_shadows'),
      '#description' => t('Add styles for CSS3 browsers.'),
      '#options' => array(
        'bs-n' => t('None'),
        'bs-l' => t('Box shadow - light'),
        'bs-d' => t('Box shadow - dark'),
      ),
    );
  } // endif styles
  // Collapse all other forms.
  $form['theme_settings']['#collapsible'] = TRUE;
  $form['theme_settings']['#collapsed'] = TRUE;
  $form['logo']['#collapsible'] = TRUE;
  $form['logo']['#collapsed'] = TRUE;
  $form['favicon']['#collapsible'] = TRUE;
  $form['favicon']['#collapsed'] = TRUE;
  // Noggin
  if (module_exists('noggin')) {
    // Rewrite the selector to suit Adaptivetheme and HTML5
    $form['noggin']['settings']['header_selector']['#default_value'] = variable_get('noggin:header_selector', 'header#header');
    // Extra fields for noggin settings
    $form['atnoggin'] = array(
      '#type' => 'fieldset',
      '#title' => t('Header Image Extra Styles'),
      '#description' => t('These settings extend the Noggin module Header Image Settings.'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#states' => array(
        'invisible' => array(
          'input[name="use_header"]' => array('checked' => FALSE),
        ),
      ),
    );
    $form['atnoggin']['noggin_image_vertical_alignment'] = array(
      '#type' => 'radios',
      '#title' => t('Image alignment - vertical'),
      '#default_value' => theme_get_setting('noggin_image_vertical_alignment'),
      '#options' => array(
        't' => t('Top'),
        'c' => t('Middle'),
        'b' => t('Bottom'),
      ),
    );
    $form['atnoggin']['noggin_image_horizontal_alignment'] = array(
      '#type' => 'radios',
      '#title' => t('Image alignment - horizontal'),
      '#default_value' => theme_get_setting('noggin_image_horizontal_alignment'),
      '#options' => array(
        'l' => t('Left'),
        'r' => t('Right'),
        'c' => t('Center'),
      ),
    );
    $form['atnoggin']['noggin_image_repeat'] = array(
      '#type' => 'radios',
      '#title' => t('Image repeat'),
      '#default_value' => theme_get_setting('noggin_image_repeat'),
      '#options' => array(
        'ni-r-r' => t('Repeat'),
        'ni-r-rx' => t('Horizontal repeat'),
        'ni-r-ry' => t('Vertical repeat'),
        'ni-r-nr' => t('No repeat'),
      ),
    );
    $form['atnoggin']['noggin_image_width'] = array(
      '#type' => 'radios',
      '#title' => t('Image width'),
      '#default_value' => theme_get_setting('noggin_image_width'),
      '#options' => array(
        'ni-w-a' => t('Auto <span class="description">- use actual image dimensions</span>'),
        'ni-w-ftw' => t('100% width <span class="description"> - stretch to fit, this only works in modern CSS3 capable browsers</span>'),
      ),
    );
  }
}
