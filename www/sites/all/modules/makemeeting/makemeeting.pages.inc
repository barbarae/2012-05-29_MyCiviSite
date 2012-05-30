<?php

/**
 * Show the poll pages - scheduler and regular poll
 *
 * @param string $url
 */
function makemeeting_show_pollpage($url) {
  // Try to retrieve poll URL
  $node_id = db_query("SELECT nid FROM {makemeeting_poll_heads} WHERE url = :url", array(':url' => $url))->fetchfield();
  if (is_numeric($node_id) && $node_id > 0) {
    // Show poll form
    $node = node_load($node_id);
    if ($node->uid == 0) {
      $node->name = $node->anonym_name == "" ? t('Anonymous') : $node->anonym_name;
    }
    drupal_set_title(t("@name's question: @question", array("@name" => $node->name, "@question" => $node->title)));
    $breadcrumb = array(
      l(t('Home'), NULL),
      l(t('Meetings'), 'makemeeting'),
      l(t("Question: @question", array('@question' => $node->title)), 'makemeeting/' . $node->poll_url)
    );
    drupal_set_breadcrumb($breadcrumb);
    return node_view($node);
  }

  // Try to retrieve admin URL
  $node_id = db_query("SELECT nid FROM {makemeeting_poll_heads} WHERE admin_url = :admin_url", array(':admin_url' => $url))->fetchfield();
  if (is_numeric($node_id) && $node_id > 0) {
    // Show the admin panel
    $node = node_load($node_id);
    drupal_set_title(t("Admin page for: @question", array("@question" => $node->title)));
    $breadcrumb = array(
      l(t('Home'), NULL),
      l(t('Make Meetings'), 'makemeeting'),
      l(t("Admin page: @question", array('@question' => $node->title)), 'makemeeting/' . $node->poll_admin_url)
    );
    drupal_set_breadcrumb($breadcrumb);
    return theme("makemeeting_poll_admin", array('node' => $node, 'admin_form' => drupal_render(drupal_get_form("makemeeting_update_poll", $node))));
  }
  // No URL found, display 404
  return drupal_not_found();
}


/**
 * Showing voters logpage.
 *
 * @param string $admin_url
 */
function makemeeting_logpage($admin_url) {
  // Retrieve the node
  $node = node_load(db_query("SELECT nid FROM {makemeeting_poll_heads} WHERE admin_url = :admin_url", array(':admin_url' => $admin_url))->fetchfield());
  if (!$node) {
    return drupal_not_found();
  }
  $breadcrumb = array(
    l(t('Home'), NULL),
    l(t('Make Meetings'), 'makemeeting'),
    l(t("Admin page: @question", array('@question' => $node->title)), 'makemeeting/' . $node->poll_admin_url),
    l(t("View logs"), 'makemeeting/' . $node->poll_admin_url . "/log")
  );
  drupal_set_breadcrumb($breadcrumb);
  // Retrieve the votes
  $result = db_query("SELECT username, dt FROM {makemeeting_poll_logs} WHERE nid = :nid", array(':nid' => $node->nid));
  if (!$result->rowCount()) {
    return t('No vote has been recorded yet.');
  }
  $rows = array();
  foreach ($result as $r) {
    $rows[] = array($r->username, format_date($r->dt));
  }
  return theme('table', array('header' => array(t('Name'), t('Date')), 'rows' => $rows));
}


/**
 * The send link forward page.
 *
 * @param string $poll_url
 */
function makemeeting_sendfw($poll_url) {
  // Retrieve the node
  $node_id = db_query("SELECT nid FROM {makemeeting_poll_heads} WHERE url = :url", array(':url' => $poll_url))->fetchfield();
  $node = node_load($node_id);

  // Breadcrumb navigation
  drupal_set_breadcrumb(array(
                             l(t('Home'), NULL),
                             l(t('Make Meetings'), 'makemeeting'),
                             l(t("Question: @question", array('@question' => $node->title)), 'makemeeting/' . $node->poll_url),
                             l(t("Send forward"), 'makemeeting/' . $node->poll_url . "/sendfw")
                        ));

  if (variable_get('makemeeting_send_email_enabled', '1') == 0) {
    return t('Send forward function is disabled by admin.');
  }
  return drupal_get_form("makemeeting_sendfw_form", $node);
}


/**
 * The default page for /makemeeting url, now showing some basic
 * information.
 */
function makemeeting_infopage() {
  // Breadcrumb navigation
  $breadcrumb = array(
    l(t('Home'), NULL),
    l(t('Make Meetings'), 'makemeeting')
  );
  drupal_set_breadcrumb($breadcrumb);
  return t("Use the navigation menu to create polls.");
}

/**
 * Show own polls page
 * Only authenticated users can access this page
 */
function makemeeting_mypage() {
  global $user;
  $breadcrumb = array(
    l(t('Home'), NULL),
    l(t('Make Meetings'), 'makemeeting'),
    l(t('List of my polls'), 'makemeeting/mypage'),
  );
  drupal_set_breadcrumb($breadcrumb);

  // Get user's polls from the database
  $query = db_select('node', 'n')->condition('type', 'makemeeting')->condition('n.uid', $user->uid);
  $query->fields('n', array('title'))->join('makemeeting_poll_heads', 'p', 'p.nid = n.nid');
  $results = $query->fields('p', array('url', 'admin_url'))->execute();
  $header = array(t('Title'), t('Poll URL'), t('Admin URL'));
  $rows = array();
  foreach ($results as $r) {
    $rows[] = array($r->title, l(t('show poll'), 'makemeeting/' . $r->url), l(t('edit poll'), 'makemeeting/' . $r->admin_url));
  }
  return theme('table', array('header' => $header, 'rows' => $rows));
}
