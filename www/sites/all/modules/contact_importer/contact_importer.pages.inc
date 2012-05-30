<?php

/**
 * @file
 * User visible pages callbacks for Contact Importer.
 */

/**
 * Build the form only for modal.
 *
 * @return string HTML with no sidebars etc.
 */
function contact_importer_js() {
  drupal_add_css(drupal_get_path('module', 'contact_importer') . '/contact_importer.css', $options);
  echo theme('contact_importer_js', array(drupal_get_form('contact_importer_form', TRUE)));
  module_invoke_all('exit');
  exit();
}

/**
 * Fetches the form from the Retrieval Engine or confirmation form.
 *
 * @todo this isn't an implementation of hook_form, so we should change the name. Engines too.
 *
 * @param boolean $js_modal = FALSE
 *  Is the form being used as a javascript modal popup?
 *
 * @ingroup forms
 *
 * @see contact_importer_form_validate()
 * @see contact_importer_form_submit()
 * @see contact_importer_select_provider_form()
 * @see contact_importer_select_provider_form_submit()
 * @see contact_importer_select_contacts_form()
 * @see contact_importer_select_contacts_form_submit()
 */
function contact_importer_form($form, &$form_state, $js_modal = FALSE) {
  drupal_set_title(t('Import your contacts'));

  // Cleanup some things for JS.
  if ($js_modal || !empty($form_state['storage']['js_modal'])) {
    drupal_set_breadcrumb(array());
    // Disable admin_menu if present.
    if (function_exists('admin_menu_suppress')) {
      admin_menu_suppress();
    }
  }

  // This is a four-step form:
  // 1) The user chooses a provider (select_provider).
  if (empty($form_state['storage']['step']) || $form_state['storage']['step'] == 'select_provider') {

    // Step one, show the list of providers.
    $form_state['storage']['step'] = 'select_provider';

    // Sanity checking.
    if (_contact_importer_are_requirements_missing()) {
      return drupal_access_denied();
    }

    // Remove any emails stored from a previous attempt.
    contact_importer_set_contacts(NULL);

    // Build the form.
    $form_state['storage']['js_modal'] = $js_modal;
    $form_state['storage']['providers'] = _contact_importer_providers_enabled();
    $form = contact_importer_select_provider_form($form, $form_state);

  }

  // 2) The user enters credentials (credentials).
  elseif ($form_state['storage']['step'] == 'credentials') {

    // Build the form.
    $engine_form_function = $form_state['storage']['provider']['engine_enabled'] . '_form';
    if (!function_exists($engine_form_function)) {
      return drupal_not_found();
    }
    $form = $engine_form_function($form, $form_state);
    $form['#attributes']['autocomplete'] = 'off';

    // This step can take a loooong time.  Make sure the user can't re-click the button.
    // We can't use drupal_add_js() due to the form cache.
    $form['#suffix'] = (isset($form['#suffix']) ? $form['#suffix'] : '') . <<<EOD
      <script type="text/javascript">
        <!--//--><![CDATA[//><!--
          jQuery(window).load(function(){
            jQuery("#contact-importer-form").submit(function() {
              jQuery("#edit-submit", "#contact-importer-form").after('<p class="loading">Please wait&hellip;</p>').hide();
              jQuery('body').css('cursor', 'wait');
            });
          });
        //--><!]]>
      </script>
EOD;

  }

  // 3) The user selects which contacts to work with (select_contacts).
  elseif ($form_state['storage']['step'] == 'select_contacts') {
    $form = contact_importer_select_contacts_form($form, $form_state);
  }

  // 4) The form is closed and data is passed back to the parent window (close).
  else {

    $modal = variable_get('contact_importer_modal', 'none');
    $is_admin = path_is_admin($_SESSION['contact_importer_destination']);
    $js_modal = ($modal == 'overlay' && $is_admin ? FALSE : $form_state['storage']['js_modal']);
    $emails = contact_importer_get_contacts('string_emails', 'default', $js_modal);
    $js = '';

    switch ($modal) {
      case 'lightbox2':
        $js = "window.parent.Drupal.contact_importer.contactImporterFillEmails('$emails');\n";
        $js .= "window.parent.Lightbox.end('forceClose');\n";
        break;

      case 'automodal':
        // Send the information from iframe modalframe to main window
        modalframe_close_dialog($emails);
        break;

      case 'overlay':
        if (!$is_admin) {
          $js = "window.parent.Drupal.contact_importer.contactImporterFillEmails('$emails');\n";
          $js .= "window.parent.Drupal.overlay.refreshPage = false;\n";
          $js .= "window.parent.Drupal.overlay.close();\n";
          break;
        }
        // If the render param is passed to drupal_goto() and the overlay
        // isn't enabled, Drupal will reload the page and contact data will be
        // lost.
        unset($_SESSION['contact_importer_destination_query']['render']);
        // Intentional pass-through.

      default:
        drupal_goto($_SESSION['contact_importer_destination'], array(
          'query' => $_SESSION['contact_importer_destination_query']));
    }
    drupal_add_js($js, array('type' => 'inline', 'scope' => 'footer'));

    contact_importer_set_contacts();

    // Do not unset($_SESSION['contact_importer_destination']) since this is used in other places
    // for sanity checking.

  }

  return $form;
}

/**
 * Validation handler for primary form builder.
 *
 * @ingroup forms
 *
 * @see contact_importer_form()
 * @see contact_importer_form_submit()
 * @see contact_importer_select_provider_form()
 * @see contact_importer_select_provider_form_submit()
 * @see contact_importer_select_contacts_form()
 * @see contact_importer_select_contacts_form_submit()
 */
function contact_importer_form_validate($form, &$form_state) {
  // @todo Not sure that we need this in a validation handler.
  // Disable admin_menu if present.
  if (!empty($form_state['storage']['js_modal']) && function_exists('admin_menu_suppress')) {
    admin_menu_suppress();
  }

  switch ($form_state['storage']['step']) {
    case 'select_provider':
      break;
    case 'credentials':
      $engine_validation_function = $form_state['storage']['provider']['engine_enabled'] . '_form_validate';
      if (function_exists($engine_validation_function)) {
        $engine_validation_function($form, $form_state);
      }
      break;
  }
}

/**
 * Submit handler for the primary form builder.
 *
 * @ingroup forms
 *
 * @see contact_importer_form()
 * @see contact_importer_form_validate()
 * @see contact_importer_select_provider_form()
 * @see contact_importer_select_provider_form_submit()
 * @see contact_importer_select_contacts_form()
 * @see contact_importer_select_contacts_form_submit()
 */
function contact_importer_form_submit($form, &$form_state) {
  // @todo Not sure that we need this in a submit handler.
  // Disable admin_menu if present.
  if (!empty($form_state['storage']['js_modal']) && function_exists('admin_menu_suppress')) {
    admin_menu_suppress();
  }

  switch ($form_state['storage']['step']) {
    case 'select_provider':
      contact_importer_select_provider_form_submit($form, $form_state);
      $form_state['storage']['step'] = 'credentials';
      $form_state['rebuild'] = TRUE;
      break;

    case 'credentials':
      $engine_submit_function = $form_state['storage']['provider']['engine_enabled'] . '_form_submit';
      if (function_exists($engine_submit_function)) {
        $engine_submit_function($form, $form_state);
      }
      $form_state['storage']['step'] = 'select_contacts';
      $form_state['storage']['email'] =
        isset($form_state['values']['email']) ? $form_state['values']['email'] : '';
      $form_state['rebuild'] = TRUE;
      break;

    case 'select_contacts':
      contact_importer_select_contacts_form_submit($form, $form_state);
      $form_state['storage']['step'] = 'close';

      // Log the import as completed.
      $log = array(
        'session_id' => session_id(),
        'email' => $form_state['storage']['email'],
        'authentication_success' => TRUE,
        'completed' => TRUE,
      );
      drupal_write_record('contact_importer_log', $log, array('session_id', 'email', 'authentication_success'));

      // Don't set a destination, form builder will take care of things.
      // (submit-like code is there and not here because we cannot manipulate the page during the
      // submit handler).

      $form_state['rebuild'] = TRUE;
      break;
  }
}

/**
 * Step 1) The select_provider form.
 *
 * @ingroup forms
 *
 * @see contact_importer_form()
 * @see contact_importer_form_validate()
 * @see contact_importer_form_submit()
 * @see contact_importer_select_provider_form_submit()
 * @see contact_importer_select_contacts_form()
 * @see contact_importer_select_contacts_form_submit()
 */
function contact_importer_select_provider_form($form, &$form_state) {
  $form = array();
  foreach ($form_state['storage']['providers'] as $provider_name => $provider) {
    $favicon = !empty($provider['favicon']) ? $provider['favicon'] : '';
    $form[$provider['type']][$provider_name] = array(
      '#type' => 'submit',
      '#value' => check_plain($provider['human_readable']),
      '#prefix' => '<div class="contact-importer-button-wrapper">',
      '#suffix' => $favicon . '</div>',
      '#attributes' => array('style' => 'width:10em; margin:0.5em'),
      // We need some way of identifying the button in the submit handler.
      '#provider_name' => $provider_name,
    );
  }
  foreach ($form as $type => &$fieldset) {
    $fieldset['#type'] = 'fieldset';
    $fieldset['#title'] = ucfirst($type);
  }
  $form['#prefix'] = t('Please choose where to import your contacts from:');
  return $form;
}

/**
 * Step 1) The select_provider form submit handler.
 *
 * @ingroup forms
 *
 * @see contact_importer_form()
 * @see contact_importer_form_validate()
 * @see contact_importer_form_submit()
 * @see contact_importer_select_provider_form()
 * @see contact_importer_select_contacts_form()
 * @see contact_importer_select_contacts_form_submit()
 */
function contact_importer_select_provider_form_submit($form, &$form_state) {
  $provider_name = $form_state['clicked_button']['#provider_name'];
  $form_state['storage']['provider'] = $form_state['storage']['providers'][$provider_name];
  $form_state['storage']['provider']['provider_name'] = $provider_name;
  return;
}

/**
 * Step 3) The select_contacts form.
 *
 * @ingroup forms
 *
 * @see contact_importer_form()
 * @see contact_importer_form_validate()
 * @see contact_importer_form_submit()
 * @see contact_importer_select_provider_form()
 * @see contact_importer_select_provider_form_submit()
 * @see contact_importer_select_contacts_form_submit()
 */
function contact_importer_select_contacts_form($form, &$form_state) {

  // Log the number of contacts that were retrieved from the provider.
  $log = array(
    'session_id' => session_id(),
    'email' => $form_state['storage']['email'],
    'authentication_success' => TRUE,
    'completed' => FALSE,
    'contacts_retrieved' => count($form_state['storage']['contacts']),
  );
  drupal_write_record('contact_importer_log', $log,
    array('session_id', 'email', 'authentication_success', 'completed'));

  if (empty($form_state['storage']['contacts'])) {
    drupal_set_message(t('No contacts could be retrieved from your account.
      Please try again with a different provider.'), 'error');
    $path = contact_importer_form_path($form_state);
    unset($form_state['storage']);
    drupal_goto($path);
  }

  // Create the form.
  $email_list = array();
  $email_list['select_all'] = '<em>' . t('Select/Deselect All') . '</em>';
  foreach ($form_state['storage']['contacts'] as $contact) {
    $name = $contact['name'];
    $email = $contact['email'];
    $email_list[check_plain($email)] = check_plain($name) . ' &lt;' . check_plain($email) . '&gt;';
  }

  $form['confirmed_list'] = array(
    '#type' => 'checkboxes',
    '#options' => $email_list,
    '#required' => TRUE,
  );
  $form['add_contact'] = array(
    '#type' => 'submit',
    '#value' => t('Add Contacts'),
  );

  // Pass the original contact list along so that it can be used
  // by the contact_importer_contacts hook.
  $form['#contacts'] = $form_state['storage']['contacts'];

  // Select all / deselect all JS.
  $js = <<<EOD
    jQuery(document).ready(function(){
      jQuery('#edit-confirmed-list-select-all').click(function() {
        var checked_status = this.checked;
        jQuery("#contact-importer-form input").each(function() {
          this.checked = checked_status;
        });
      });
      jQuery('#contact-importer-form input').not('#edit-confirmed-list-select-all').click(function() {
        jQuery('#edit-confirmed-list-select-all').each(function(){
          this.checked = false;
        });
      });
    });
EOD;
  drupal_add_js($js, array('type' => 'inline', 'scope' => 'footer'));
  return $form;
}

/**
 * Step 3) The select_contacts form submit handler.
 *
 * @ingroup forms
 *
 * @see contact_importer_form()
 * @see contact_importer_form_validate()
 * @see contact_importer_form_submit()
 * @see contact_importer_select_provider_form()
 * @see contact_importer_select_provider_form_submit()
 * @see contact_importer_select_contacts_form()
 */
function contact_importer_select_contacts_form_submit($form, &$form_state) {

  // Remove select_all if it is set.
  $list = $form_state['values']['confirmed_list'];
  if (isset($list['select_all'])) {
    if ($list['select_all']) {
      foreach ($list as $email => &$contact) {
        $contact = $email;
      }
    }
    unset($list['select_all']);
  }
  $list = array_filter($list);

  // Log the number of contacts that were imported from the provider.
  $log = array(
    'session_id' => session_id(),
    'email' => $form_state['storage']['email'],
    'authentication_success' => TRUE,
    'completed' => FALSE,
    'contacts_imported' => count($list),
  );
  drupal_write_record('contact_importer_log', $log,
          array('session_id', 'email', 'authentication_success', 'completed'));

  // Allow contrib to modify the results getting sent back to the form.
  drupal_alter('contact_importer_contacts', $list, $form_state['storage']['#contacts']);

  // Add the results to the session.
  contact_importer_set_contacts($list);

}
