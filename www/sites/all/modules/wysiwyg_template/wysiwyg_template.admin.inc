<?php

/**
 * @file
 * Administrative page callbacks for the Wysiwyg Template module.
 */

/**
 * Wysiwyg template overview page - view templates and edit them
 */
function wysiwyg_template_overview(){
  $rows = array();
  // load the templates and display them on the admin form
  $templateResults = db_query("SELECT * FROM {wysiwyg_templates}");
  // Show export link when ctools is enabled.
  $export_enabled = module_exists('ctools');
  foreach ($templateResults as $template){
    $row = array();
    $row[] = $template->title;
    $row[] = $template->description;
    $row[] = l(t('edit'), "admin/config/content/wysiwyg-templates/edit/" . $template->name);
    if ($export_enabled) {
      $row[] = l(t('export'), "admin/config/content/wysiwyg-templates/export/" . $template->name);
    }
    $row[] = l(t('delete'), "admin/config/content/wysiwyg-templates/delete/" . $template->name);
    
    $rows[] = $row;
  }
  
  if (empty($rows)){
    $rows[] = array(array(
      'data' => t('No templates available.'), 
      'colspan' => ($export_enabled ? 5 : 4),
    ));
  }
  $rows[] = array(array(
    'data' => l(t('Create a new template'), 'admin/config/content/wysiwyg-templates/add'),
    'colspan' => ($export_enabled ? 5 : 4),
  ));
  
  $header = array(t('Name'), t('Description'), array('data' => t('Operations'), 'colspan' => ($export_enabled ? 3 : 2)));
  
  $help = t('<p>The Wysiwyg Template module allows you to create templates to be used with a Wysiwyg editor of your choice. The template button for the editor will need to be enabled from the !profiles settings. The currently supported editors are FCK Editor, CK Editor and TinyMCE.</p>', array('!profiles' => l(t('WYSIWYG Profile'), 'admin/config/content/filters')));
  
  return $help . theme('table', array(
    'header' => $header,
    'rows' => $rows, 
    'id' => 'wysiwyg_template'
  ));
}

/**
 * Wysiwyg template create/modify form
 */
function wysiwyg_template_template_form($something, $form_state, $template = NULL) {
  if(isset($form_state['confirm_delete'])){
    // Rebuild the form to confirm term deletion.
    $form['name'] = array('#type' => 'value', '#value' => $form_state['values']['name']);
    $form['delete'] = array('#type' => 'value', '#value' => true);
    return confirm_form($form, 
      t('Are you sure you want to delete the template %title?', array('%title' => $form_state['values']['title'])),
      'admin/content/wysiwyg-template',
      t('This action cannot be undone.'),
      t('Delete'),
      t('Cancel')
    );
  }

  if(!empty($template)) {
    // Add the current values as defaults to the form, if editing an existing item.
    $form_state['values'] = $template;
  }
  
  $form = array();

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Template Name'),
    '#default_value' => isset($form_state['values']['title']) ? $form_state['values']['title'] : '',
    '#description' => t('Select a name for this template.'),
    '#maxlength' => 80,
    '#required' => true
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'wysiwyg_template_name_exists',
      'source' => array('title'),
    ),
    '#description' => t('A unique machine-readable name for this template. It must only contain lowercase letters, numbers, and underscores.'),
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Template Description'),
    '#default_value' => isset($form_state['values']['description']) ? $form_state['values']['description'] : '',
    '#description' => t('A description to be shown with the template.')
  );
  $form['body'] = array(
    '#type' => 'textarea',
    '#title'  => t('HTML Template'),
    '#rows'  =>  10,
    '#default_value' => isset($form_state['values']['body']) ? $form_state['values']['body'] : '',
    '#required' => true
  );
  if(module_exists('token')){
    $form['token_help'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array(),
      '#global_types' => true
    );
    $form['body']['#title'] = theme('token_help', 'node');
  }
  $form['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Save')
  );
  
  if(!empty($template)) {
    if (!empty($form_state['values']['name'])) {
      $form['name']['#default_value'] = $form_state['values']['name'];
      $form['name']['#disabled'] = TRUE;
      $form['name']['#value'] = $form_state['values']['name'];
    }

    // If it's an existing template, offer a delete button.
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete')
    );
  }
  return $form;
}

/**
 * Wysiwyg template form submit - delete and save handlers
 */
function wysiwyg_template_template_form_submit($form, &$form_state) {
  //if we're deleting the template
  if ($form_state['clicked_button']['#id'] == 'edit-delete') {
    //show the confirmation
    $form_state['rebuild'] = true;
    $form_state['confirm_delete'] = true;
    return;
  }
  // delete confirmation provided
  if (isset($form_state['values']['delete'])){
    if ($form_state['values']['delete'] === TRUE) {
      wysiwyg_template_delete_template($form_state['values']['name']);
      drupal_set_message(t('The template has been deleted.'));    
      $form_state['redirect'] = 'admin/config/content/wysiwyg-templates';
      return;
    }
  }

  // save the template
  if(wysiwyg_template_save_template($form_state['values'])){
    drupal_set_message(t('The template has been saved.'));
  }
  else{
    drupal_set_message(t('There was an error saving the template to the database.'));
  }
  // redirect back to the overview page
  $form_state['redirect'] = 'admin/config/content/wysiwyg-templates';
}

/**
 * Menu callback -- ask for confirmation of rule deletion.
 */
function wysiwyg_template_delete_confirm($something, &$form_state) {
  
  $form['name'] = array(
    '#type' => 'value',
    '#value' => isset($form_state['build_info']['args'][0]) ? $form_state['build_info']['args'][0] : 0,
  );

  $template = wysiwyg_template_load_template(isset($form_state['build_info']['args'][0]) ? $form_state['build_info']['args'][0] : 0);
  return confirm_form($form,
    t('Are you sure you want to delete the template %title?', array('%title' => $template['title'])),
    isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/content/wysiwyg-templates',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute node deletion.
 */
function wysiwyg_template_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    wysiwyg_template_delete_template($form_state['values']['name']);
  }

  $form_state['redirect'] = 'admin/config/content/wysiwyg-templates';
  return;
}

/**
 * Generate exporting code for a template.
 */
function wysiwyg_template_export_get_code($template, $indent = '') {
  ctools_include('export');
  return ctools_export_object('wysiwyg_templates', $template, $indent);
}

/**
 * Generate a form for exporting a template.
 */
function wysiwyg_template_export_form($form, &$form_state, $template) {
  $template = (object) $template;
  drupal_set_title(check_plain($template->title));
  $code = wysiwyg_template_export_get_code($template);
  $lines = substr_count($code, "\n")  + 1;
  $form['export'] = array(
    '#title' => t('Export data'),
    '#type' => 'textarea',
    '#value' => $code,
    '#rows' => $lines,
    '#description' => t('Copy the export text and paste it into another site using the import function.'),
  );

  return $form;
}

/**
 * Generate a form for importing a template.
 */
function wysiwyg_template_import_form($form, &$form_state) {
  $form['import'] = array(
    '#title' => t('Import data'),
    '#type' => 'textarea',
    '#rows' => 20,
    '#description' => t('Paste the code from template export function.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

function wysiwyg_template_import_form_submit($form, &$form_state) {
  ctools_include('export');
  $template = ctools_export_crud_import('wysiwyg_templates', $form_state['input']['import']);
  if (is_object($template)) {
    $template = (array) $template;
    wysiwyg_template_save_template($template);
    $form_state['redirect'] = 'admin/config/content/wysiwyg-templates';
  }
  else {
    form_set_error('import', $template);
  }
}
