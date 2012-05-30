<?php

/**
 * @file
 * NewsFlash teheme-settings.php
 */
function newsflash_form_system_theme_settings_alter(&$form, $form_state) {

$form['newsflash_style'] = array
  (
    '#type' => 'select',
    '#title' => t('Style'),
    '#default_value' => theme_get_setting('newsflash_style'),
    '#description' => t('Choose your favorite color.'),
    '#options' => array
    (
      'copper' => t('Copper'),
      'green' => t('Green'),
      'blue' => t('Blue'),
      'black' => t('Black'),
      'red' => t('Red'),
      'violet' => t('Violet'),
      'aqua' => t('Aqua'),
    ),
  );

  $form['newsflash_pickstyle'] = array
  (
    '#type' => 'checkbox',
    '#title' => t('Enable StylePicker Javasrcipt'),
    '#description' => t('If enabled then you can use stylepicker see README.txt.'),
    '#default_value' => theme_get_setting('newsflash_pickstyle'),
  );
  
  $form['newsflash_themelogo'] = array
  (
    '#type' => 'checkbox',
    '#title' => t('Use Themed Logo'),
    '#description' => t('If enabled then will the themed logo use the color you have selected above.'),
    '#default_value' => theme_get_setting('newsflash_themelogo'),
  );

  $form['newsflash_width'] = array
  (
    '#type' => 'textfield',
    '#title' => t('Width Size'),
    '#description' => t('Set the width. You  can set the width in percent for dynamic width or in px for fixed width.<br /><b>eg.: 850px or 60%</b>'),
    '#default_value' => theme_get_setting('newsflash_width'),
    '#size' => 7,
    '#maxlength' => 7,
  );

  $form['newsflash_breadcrumb'] = array
  (
    '#type' => 'checkbox',
    '#title' => t('Show Breadcrumbs'),
    '#default_value' => theme_get_setting('newsflash_breadcrumb'),
  );

  $form['newsflash_permalink'] = array
  (
    '#type' => 'checkbox',
    '#title' => t('Show Permanent Links'),
    '#description' => t('Enable/disable permanent links in comments'),
    '#default_value' => theme_get_setting('newsflash_permalink'),
  );

  $form['newsflash_fontfamily'] = array
  (
    '#type' => 'select',
    '#title' => t('Font Family'),
    '#description' => t('Choose your favorite Fonts'),
    '#default_value' => theme_get_setting('newsflash_fontfamily'),
    '#options' => array
    (
      'Arial, Verdana, sans-serif' => t('Arial, Verdana, sans-serif'),
      '"Arial Narrow", Arial, Helvetica, sans-serif' => t('"Arial Narrow", Arial, Helvetica, sans-serif'),
      '"Times New Roman", Times, serif' => t('"Times New Roman", Times, serif'),
      '"Lucida Sans", Verdana, Arial, sans-serif' => t('"Lucida Sans", Verdana, Arial, sans-serif'),
      '"Lucida Grande", Verdana, sans-serif' => t('"Lucida Grande", Verdana, sans-serif'),
      'Tahoma, Verdana, Arial, Helvetica, sans-serif' => t('Tahoma, Verdana, Arial, Helvetica, sans-serif'),
      'Georgia, "Times New Roman", Times, serif' => t('Georgia, "Times New Roman", Times, serif'),
      'Custom' => t('Custom (specify below)'),
    ),
  );

  $form['newsflash_customfont'] = array
  (
    '#type' => 'textfield',
    '#title' => t('Custom Font-Family Setting'),
    '#description' => t('type your fonts separated by ,<br />eg. <b>"Lucida Grande", Verdana, sans-serif</b>'),
    '#default_value' => theme_get_setting('newsflash_customfont'),
    '#size' => 40,
    '#maxlength' => 75,
  );

  $form['newsflash_uselocalcontent'] = array
  (
    '#type' => 'checkbox',
    '#title' => t('Include Custom Stylesheet'),
    '#default_value' => theme_get_setting('newsflash_uselocalcontent'),
  );

  $form['newsflash_localcontentfile'] = array
  (
    '#type' => 'textfield',
    '#title' => t('Path to Custom Stylesheet'),
    '#description' => t('type the location of your custom css without leading slash<br />eg. <b>sites/all/themes/newsflash/css/icons.css</b>'),
    '#default_value' => theme_get_setting('newsflash_localcontentfile'),
    '#size' => 40,
    '#maxlength' => 75,
  );

  $form['newsflash_leftsidebarwidth'] = array
  (
    '#type' => 'textfield',
    '#title' => t('Left Sidebar Width'),
    '#description' => t('Set the width in Pixel.'),
    '#default_value' => theme_get_setting('newsflash_leftsidebarwidth'),
    '#size' => 5,
    '#maxlength' => 5,
  );

  $form['newsflash_rightsidebarwidth'] = array
  (
    '#type' => 'textfield',
    '#title' => t('Right Sidebar Width'),
    '#description' => t('Set the width in Pixel.'),
    '#default_value' => theme_get_setting('newsflash_rightsidebarwidth'),
    '#size' => 5,
    '#maxlength' => 5,
  );

  $form['newsflash_suckerfish'] = array
  (
    '#type' => 'checkbox',
    '#title' => t('Use Suckerfish Menus'),
    '#default_value' => theme_get_setting('newsflash_suckerfish'),
  );

  $form['newsflash_usecustomlogosize'] = array
  (
    '#type' => 'checkbox',
    '#title' => t('Specify Custom Logo Size'),
    '#default_value' => theme_get_setting('newsflash_usecustomlogosize'),
  );

  $form['newsflash_logowidth'] = array
  (
    '#type' => 'textfield',
    '#title' => t('Logo Width'),
    '#description' => t('Set the width in Pixel.'),
    '#default_value' => theme_get_setting('newsflash_logowidth'),
    '#size' => 5,
    '#maxlength' => 5,
  );

  $form['newsflash_logoheight'] = array
  (
    '#type' => 'textfield',
    '#title' => t('Logo Height'),
    '#description' => t('Set the width in Pixel.'),
    '#default_value' => theme_get_setting('newsflash_logoheight'),
    '#size' => 5,
    '#maxlength' => 5,
  );
  
  $form['newsflash_banner'] = array
  (
    '#type' => 'checkbox',
    '#title' => t('Show RoopleTheme Footer Logo'),
    '#description' => t('if unchecked then roopletheme logo in the footer will disapear<br />so you don\'t need touch the code'),
    '#default_value' => theme_get_setting('newsflash_banner'),
  );
  
  return $form;
}
