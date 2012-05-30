<?php

/**
 * makemeeting_admin_form()
 *
 * @return
 */
function makemeeting_admin_form() {
  $form['send_email'] = array(
    '#type' => 'fieldset',
    '#title' => t('E-mail sending options'),
    '#tree' => TRUE,
  );

  $form['send_email']['enable'] = array(
    '#type' => 'radios',
    '#title' => t('Enable e-mail sending function'),
    '#default_value' => variable_get('makemeeting_send_email_enabled', '1'),
    '#options' => array('1' => t('Enabled'), '0' => t('Disabled')),
    '#description' => t('If you choose yes, then the users can send e-mails to their friends with the link of the poll.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save')
  );

  return $form;
}

/**
 * makemeeting_admin_form_submit()
 *
 * @return void
 */
function makemeeting_admin_form_submit($form, &$form_state) {
  variable_set("makemeeting_send_email_enabled", $form_state['values']['send_email']['enable']);
}
