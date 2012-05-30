<?php

/**
 * @file
 * Contains page callbacks for webform_template
 */

function webform_template_config_form($form, &$form_state) {
  $webform_types = webform_variable_get('webform_node_types', array());
  
  if (!empty($webform_types)) {
    foreach ($webform_types as $type) {
      $types[$type] =  node_type_get_name($type);
    }
    $form['destination']['webform_template_lang'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display language'),
      '#default_value' => variable_get('webform_template_lang', FALSE),
      '#description' => t("Check this box to show the template's language in the attach form."),
    );
    $form['source']['webform_template_src'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Template source(s)'),
      '#default_value' => variable_get('webform_template_src', array()),
      '#options' => $types,
      '#description' => t("Select any node types which will function as a template. Typically you will want to use a specific node type that isn't used for anything else."),
    );
    $form['destination']['webform_template_dest'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Template destination(s)'),
      '#default_value' => variable_get('webform_template_dest', array()),
      '#options' => $types,
      '#description' => t('Select any node types that should have the option to apply webform templates to them.'),
    );
  }
  else {
    drupal_set_message(t('Please go to the webform settings first and enable webform for at least one content type.'));
  }

  return system_settings_form($form);
}

