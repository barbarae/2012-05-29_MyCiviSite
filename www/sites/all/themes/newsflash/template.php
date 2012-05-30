<?php

/**
 * @file
 * NewsFlash template.php
 *
 */

// get setted color style and save it in a cookie
function get_newsflash_style() {
  $style = theme_get_setting('newsflash_style');
  if (!$style) {
    $style = 'blue';
  }
  if (theme_get_setting('newsflash_pickstyle')) {
    if (isset($_COOKIE["newsflashstyle"])) {
      $style = $_COOKIE["newsflashstyle"];
    }
  }
  return $style;
}

/**
 * Implements hook_preprocess_html()
 *
 */
function newsflash_preprocess_html(&$variables) {
// set the color style themes
  $style = get_newsflash_style();
  drupal_add_css(path_to_theme() . '/style.css'); //is not settet in info file because it's loades to late
  drupal_add_css(path_to_theme() . '/css/' . $style . '.css');

  if (theme_get_setting('newsflash_suckerfish')) {
    drupal_add_css(path_to_theme() . '/css/suckerfish_' . $style . '.css');
  }
  else {
    drupal_add_css(path_to_theme() . '/css/nosuckerfish.css');
  }
  
  // set for custome css
  if (theme_get_setting('newsflash_uselocalcontent')) {
    $local_content =  theme_get_setting('newsflash_localcontentfile');
    if (file_exists($local_content)) {
      drupal_add_css($local_content);
    }
  }
  
  // add ie css fixing some px settings in css
  drupal_add_css
  (
    path_to_theme() . '/css/ie.css', 
    array
    (
      'group' => CSS_THEME, 
      'browsers' => array
      (
      'IE' => 'lte IE 7', 
      '!IE' => FALSE
      ), 
    'preprocess' => FALSE
    )
  );
  if (theme_get_setting('newsflash_pickstyle')) {
    drupal_add_js(path_to_theme() . '/js/pickstyle.js');
  }
}
/**
 * Implements hook_preprocess_page()
 */
function newsflash_preprocess_page(&$variables) {
  if (theme_get_setting('newsflash_themelogo')) {
    $variables['logo'] = base_path() . path_to_theme() . '/images/' . get_newsflash_style() . '/logo.png';
  }
}
// in sukerfishmenu, searchbox. and header region visually hide block titles.
function newsflash_preprocess_block(&$variables) {
  // In the suckerfishmenu region visually hide block titles if use suckerfish enabled.
  if (theme_get_setting('newsflash_suckerfish') && $variables['block']->region == 'suckerfish') {
    $variables['title_attributes_array']['class'][] = 'element-invisible';
  }
  if ($variables['block']->region == 'header') {
    $variables['title_attributes_array']['class'][] = 'element-invisible';
  }
    //adds in blocks title the class .title
    $variables['title_attributes_array']['class'][] = 'title';
}

/**
 * Implements hook_process_html()
 */
function newsflash_process_html(&$variables) {
  $variables['styles'] .= '<script type="text/javascript">'/* Needed to avoid Flash of Unstyle Content in IE */ . '</script>';
  // here are the witdh calculatings and some scrips addings 
  if (theme_get_setting('newsflash_width')) { 
    $variables['styles'] .= '<style type="text/css">
      #page 
      {
        width : ' . theme_get_setting('newsflash_width') . ';
      }
    </style>';
  }
  else {
    $variables['styles'] .= '<style type="text/css">
      #page 
      {
        width: 95%;
      }
    </style>';
  }

  if ($left_sidebar_width = theme_get_setting('newsflash_leftsidebarwidth')) {
    $variables['styles'] .= '<style type="text/css">
      body.sidebar-first #main 
      {
        margin-left: -' . $left_sidebar_width . 'px;
      }
      body.two-sidebars #main 
      {
        margin-left: -' . $left_sidebar_width . 'px;
      }
      body.sidebar-first #squeeze 
      {
        margin-left: ' . $left_sidebar_width . 'px;
      }
      body.two-sidebars #squeeze 
      {
        margin-left: ' . $left_sidebar_width . 'px;
      }
      #sidebar-left 
      {
        width: ' . $left_sidebar_width . 'px;
      }
    </style>';
  }

  if ($right_sidebar_width = theme_get_setting('newsflash_rightsidebarwidth')) {
    $variables['styles'] .= '<style type="text/css">
      body.sidebar-second #main 
      {
        margin-right: -' . $right_sidebar_width . 'px;
      }
      body.two-sidebars #main 
      {
        margin-right: -' . $right_sidebar_width . 'px;
      }
      body.sidebar-second #squeeze 
      {
        margin-right: ' . $right_sidebar_width . 'px;
      }
      body.two-sidebars #squeeze 
      {
        margin-right: ' . $right_sidebar_width . 'px;
      }
      #sidebar-right 
      {
        width: ' . $right_sidebar_width . 'px;
      }
    </style>';
  }

  if (theme_get_setting('newsflash_fontfamily') != 'Custom') {
    $variables['styles'] .= '<style type="text/css">
      body 
      {
        font-family : ' . theme_get_setting('newsflash_fontfamily') . ';
      }
    </style>';
  } 
  elseif (theme_get_setting('newsflash_customfont')) {
      $variables['styles'] .= '<style type="text/css">
        body 
        {
          font-family : ' . theme_get_setting('newsflash_customfont') . ';
        }
      </style>';
  }

  if (theme_get_setting('newsflash_usecustomlogosize')) {
    $variables['styles'] .= '<style type="text/css">
      img#logo 
      {
        width : ' . theme_get_setting('newsflash_logowidth') . 'px;
        height: ' . theme_get_setting('newsflash_logoheight') . 'px;
      }
    </style>';
  }
  if (theme_get_setting('newsflash_suckerfish')) {
  $variables['styles'] .= '<style type="text/css">
      #suckerfishmenu div .contextual-links-wrapper {
         display:none;
      }
  </style>';
  }
  
  if (theme_get_setting('newsflash_suckerfish')) {
      $variables['scripts'] .= '<!--[if lte IE 6]>
        <script type="text/javascript" src="' . path_to_theme() . '/js/suckerfish.js"></script>
      <![endif]-->';
  }

  $variables['styles'] .= '<!--[if lte IE 6]>
  <style type="text/css">
      #primarymenu {
         float:none;
      }
  </style>
  <![endif]-->';
}
