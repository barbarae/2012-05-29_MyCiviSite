<?php

/**
 * @file Theme overrides for DrupalCon sites.
 */

/**
 * Override or insert variables into the html template.
 *
 * @param $variables
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("html" in this case.)
 */
function drupalcon_base_preprocess_html(&$variables, $hook) {
  if ($drupalorg_site_status = variable_get('drupalorg_site_status', FALSE)) {
    $variables['page']['page_top']['#prefix'] = '<div id="drupalorg-site-status">' . filter_xss_admin($drupalorg_site_status) . '</div>';
  }
}
