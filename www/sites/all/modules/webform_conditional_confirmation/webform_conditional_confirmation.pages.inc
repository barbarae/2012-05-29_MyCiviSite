<?php

/**
 * @file
 * Menu callbacks and functions for configuring conditional webform messages.
 */

/**
 * List conditional confirmation messages.
 */
function webform_conditional_confirmation_list_confirmations($node) {
  $output = drupal_get_form('webform_conditional_confirmation_list_form', $node);
  return array(
    '#theme' => 'webform_conditional_confirmation_list_confirmations',
    '#node' => $node,
    '#form' => $output,
  );
}

/**
 * Theme the output of the main messages listing page.
 */
function theme_webform_conditional_confirmation_list_confirmations($variables) {
  $node = $variables['node'];
  $form = $variables['form'];

  return drupal_render($form);
}

/**
 * The table-based listing of all confirmation messages for this webform.
 */
function webform_conditional_confirmation_list_form($form, &$form_state, $node) {
  $form = array(
    '#tree' => TRUE,
    '#node' => $node,
    'messages' => array(),
  );

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  // Get all the confirmation messages.
  $messages = webform_conditional_confirmation_message_load($node);
  if (empty($messages)) {
    return $form;
  }

  foreach ($messages as $msgid => $message) {
    $form['messages'][$msgid]['msgid'] = array(
      '#type' => 'hidden',
      '#default_value' => $message->msgid,
    );
    $form['messages'][$msgid]['name'] = array(
      '#type' => 'hidden',
      '#default_value' => $message->name,
    );
    $form['messages'][$msgid]['weight'] = array(
      '#type' => 'textfield',
      '#size' => 4,
      '#title' => t('Weight'),
      '#default_value' => $message->conditional_weight,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 45,
    '#access' => count($messages) > 0,
  );

  return $form;
}

/**
 * Theme the node confirmation messages listing form.
 */
function theme_webform_conditional_confirmation_list_form($variables) {
  $form = $variables['form'];
  $node = $form['#node'];

  $form['messages']['#attached']['library'][] = array('webform', 'admin');

  // TODO: Attach these. See http://drupal.org/node/732022.
  drupal_add_tabledrag('webform-conditional-confirmations', 'order', 'sibling', 'webform-confirmation-weight');

  // Init headers.
  $headers = array(
    t('Name'),
    t('Weight'),
    array('data' => t('Operations'), 'colspan' => 3),
  );

  // Populate the rows.
  $rows = array();
  $messages = element_children($form['messages']);
  if (!empty($form['messages'])) {
    foreach (element_children($form['messages']) as $msgid) {
      $message = $form['messages'][$msgid];
      $form['messages'][$msgid]['weight']['#attributes']['class'] = array('webform-confirmation-weight');
      $row = array(
        filter_xss($message['name']['#default_value']),
        //$message->conditional_weight,
        drupal_render($form['messages'][$msgid]['weight']),
        l(t('Edit'), 'node/' . $node->nid . '/webform/confirmation/' . $msgid . '/edit', array('query' => drupal_get_destination())),
        l(t('Delete'), 'node/' . $node->nid . '/webform/confirmation/' . $msgid . '/delete', array('query' => drupal_get_destination())),
      );
      $rows[] = array('data' => $row, 'class' => array('draggable', 'tabledrag-leaf'));
    }
  }
  
  // Format the output.
  $output = '';
  $output .= theme('table', array('header' => $headers, 'rows' => $rows, 'attributes' => array('id' => 'webform-conditional-confirmations')));
  $output .= drupal_render_children($form);
  return $output;
}

/**
 * Save the new weights to the database.
 */
function webform_conditional_confirmation_list_form_submit($form, &$form_state) {
  foreach ($form_state['values']['messages'] as $msgid => $data) {
    db_update('webform_conditional_confirmation')
      ->fields(array(
        'conditional_weight' => $data['weight'],
      ))
      ->condition('nid', $form_state['values']['nid'])
      ->condition('msgid', $msgid)
      ->execute();
  }
  drupal_set_message(t('Configuration saved.'));
}

/**
 * Add/edit conditional confirmation message.
 */
function webform_conditional_confirmation_edit_confirmation_form($form, &$form_state, $node, $msgid) {
  $form['#attached']['library'][] = array('webform', 'admin');

  $defaults = webform_conditional_confirmation_message_load($node, $msgid);

  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );
  $form['msgid'] = array(
    '#type' => 'value',
    '#value' => $msgid == 'add' ? NULL : $msgid,
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => t('Internal name assigned to this confirmation message. Used on administration screens only.'),
    '#default_value' => $defaults->name,
    '#maxlength' => 255,
    '#weight' => -10,
    '#required' => TRUE,
  );

  // Generate list of conditional fields.
  $conditional_components = array();
  foreach ($node->webform['components'] as $cid => $test_component) {
    if (webform_component_feature($test_component['type'], 'conditional')) {
      $conditional_components[$cid] = $test_component;
    }
  }
  if (empty($conditional_components)) {
    drupal_set_message(t('No conditional components found.  You need to add some conditional components to your webform before you can configure conditional confirmation messages.'), 'error');
    return $form;
  }


  // Conditional rule.
  $form['conditional'] = array(
    '#weight' => 0,
    '#type' => 'fieldset',
    '#title' => t('Conditional rule'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('Create a rule to control when this confirmation message is used.'),
  );
  $form['conditional']['conditional_component'] = array(
    '#type' => 'select',
    '#title' => t('Component'),
    '#options' => webform_component_list($node, $conditional_components, FALSE, TRUE),
    '#description' => t('Select a component to decide when to use these confirmation message settings.'),
    '#default_value' => $defaults->conditional_component,
    '#required' => TRUE,
  );
  $form['conditional']['conditional_operator'] = array(
    '#type' => 'select',
    '#title' => t('Operator'),
    '#options' => array(
      '=' => t('Is one of'),
      '!=' => t('Is not one of')
    ),
    '#description' => t('Determines whether the list below is inclusive or exclusive.'),
    '#default_value' => $defaults->conditional_operator,
    '#required' => TRUE,
  );
  $form['conditional']['conditional_values'] = array(
    '#type' => 'textarea',
    '#title' => t('Values'),
    '#description' => t('List values, one per line, that will trigger this action.'),
    '#default_value' => $defaults->conditional_values,
    '#required' => TRUE,
  );
  $form['conditional']['conditional_weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#description' => t('The order in which rules are to be checked. Lighter items will be processed first and the first matching rule will be used.'),
    '#default_value' => $defaults->conditional_weight,
    '#required' => TRUE,
    '#delta' => 20,
  );


  // Confirmation message settings.
  $form['message_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Confirmation message settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#weight' => 1,
  );

  $form['message_settings']['confirmation'] = array(
    '#type' => 'text_format',
    '#title' => t('Confirmation message'),
    '#description' => t('Message to be shown upon successful submission. If the redirection location is set to <em>Confirmation page</em> it will be shown on its own page, otherwise this displays as a message.'),
    '#default_value' => $defaults->confirmation,
    '#cols' => 40,
    '#rows' => 10,
    '#format' => $defaults->confirmation_format,
  );

  // Redirection settings.
  if (strpos($defaults->redirect_url, '<') === 0) {
    $redirect = trim($defaults->redirect_url, '<>');
    // Redirection is set to front page.
    if ($redirect == 'front') {
      $redirect = 'url';
      $redirect_url = $defaults->redirect_url;
    }
    else {
      $redirect_url = '';
    }
  }
  else {
    $redirect = 'url';
    $redirect_url = $defaults->redirect_url;
  }

  $form['message_settings']['redirection'] = array(
    '#type' => 'item',
    '#title' => t('Redirection location'),
    '#theme' => 'webform_advanced_redirection_form',
    '#description' => t('Choose where to redirect the user upon successful submission.') . ' ' . t('The <em>Custom URL</em> option supports Webform token replacements.') . theme('webform_token_help', array('groups' => array('basic', 'node', 'special', 'submission'))),
  );
  $form['message_settings']['redirection']['redirect']= array(
    '#type' => 'radios',
    '#default_value' => $redirect,
    '#options' => array(
      'confirmation' => t('Confirmation page'),
      'url' => t('Custom URL'),
      'none' => t('No redirect (reload current page)'),
    ),
  );
  $form['message_settings']['redirection']['redirect_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect URL'),
    '#description' => t('URL to redirect the user to upon successful submission.'),
    '#default_value' => $redirect_url,
    '#maxlength' => 255,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save confirmation message'),
    '#weight' => 300,
  );

  $form['#submit'][] = 'webform_conditional_confirmation_edit_confirmation_form_submit';

  return $form;
}

/**
 * Validate handler; check the redirection settings.
 */
function webform_conditional_confirmation_edit_confirmation_form_validate($form, &$form_state) {
  // Ensure the entered redirect URL is valid.
  if ($form_state['values']['redirect'] == 'url') {
    $redirect_url = trim($form_state['values']['redirect_url']);
    if (empty($redirect_url)) {
      form_error($form['message_settings']['redirection']['redirect_url'], t('A valid URL is required for custom redirection.'));
    }
    elseif (strpos($redirect_url, 'http') === 0 && !valid_url($redirect_url, TRUE)) {
      form_error($form['message_settings']['redirection']['redirect_url'], t('The entered URL is not a valid address.'));
    }
    else {
      form_set_value($form['message_settings']['redirection']['redirect_url'], $redirect_url, $form_state);
    }
  }
  elseif ($form_state['values']['redirect'] == 'confirmation') {
    form_set_value($form['message_settings']['redirection']['redirect_url'], '<confirmation>', $form_state);
  }
  else {
    form_set_value($form['message_settings']['redirection']['redirect_url'], '<none>', $form_state);
  }
}

/**
 * Submit handler; save the new or updated confirmation message to the database.
 */
function webform_conditional_confirmation_edit_confirmation_form_submit($form, &$form_state) {
  $message = new stdClass;
  $message->nid = $form_state['values']['nid'];
  $message->msgid = $form_state['values']['msgid'];
  $message->name = $form_state['values']['name'];
  $message->conditional_component = $form_state['values']['conditional_component'];
  $message->conditional_operator = $form_state['values']['conditional_operator'];
  $message->conditional_values = $form_state['values']['conditional_values'];
  $message->conditional_weight = $form_state['values']['conditional_weight'];
  $message->confirmation = $form_state['values']['confirmation']['value'];
  $message->confirmation_format = $form_state['values']['confirmation']['format'];
  $message->redirect_url = $form_state['values']['redirect_url'];
  $msgid = webform_conditional_confirmation_message_save($message);
  if ($msgid) {
    drupal_set_message(t('Configuration saved.'));
  }
  else {
    drupal_set_message(t('There was a problem saving your configuration.', 'error'));
  }
  $form_state['redirect'] = 'node/' . $message->nid . '/webform/confirmation';
}

/**
 * Delete conditional confirmation message.
 */
function webform_conditional_confirmation_delete_confirmation_form($form, &$form_state, $node, $msgid) {
  $form = array();
  $form['nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );
  $form['msgid'] = array(
    '#type' => 'value',
    '#value' => $msgid,
  );
  $defaults = webform_conditional_confirmation_message_load($node, $msgid);

  $question = t('Delete the %name confirmation message?', array('%name' => $defaults->name));
  $description = t('This will immediately delete the %name confirmation message from the %webform webform. This cannot be undone.', array('%name' => $defaults->name, '%webform' => $node->title));

  return confirm_form($form, $question, 'node/' . $node->nid . '/webform/components', $description, t('Delete'));
}

/**
 * Submit handler; delete confirmation message.
 */
function webform_conditional_confirmation_delete_confirmation_form_submit($form, &$form_state) {
  $nid = $form_state['values']['nid'];
  webform_conditional_confirmation_message_delete($nid, $form_state['values']['msgid']);
  drupal_set_message(t('Confirmation message deleted.'));
  $form_state['redirect'] = 'node/' . $nid . '/webform/confirmation';
}

