<?php

/**
 * @file
 * Provides rules integrations for the Context module
 */

/**
 * Implements hook_rules_condition_info().
 */
function context_rules_rules_condition_info() {
  return array(
    'rules_condition_context_rules_context_isset' => array(
      'label' => t('Is a context set'),
      'parameter' => array(
        'context' => array(
          'type' => 'text',
          'label' => t('Is the following Context set?'),
          'options list' => 'rules_context_rules_context_options',
          'restriction' => 'input',
        ),
      ),
      'help' => t('Evaluates to TRUE, if a given Context is set.'),
      'group' => 'Context Rules',
    ),
  );
}

/**
 * returns list of context of Context dropdown
 */
function rules_context_rules_context_options() {
  if ($contexts = context_context_list()) {
    return $contexts;
  }
  return array();
}

/**
 * determines if a given Context is currently set
 */
function rules_condition_context_rules_context_isset($context) {
  if (!empty($context)) {
    //all other hooks should get got but theme level ones will be missed otherwise
    module_invoke_all('context_page_condition');
    //module_invoke_all('context_page_reaction');
    return context_isset('context', $context);
  }
  return FALSE;
}

/**
 * Implements hook_rules_action_info().
 * @ingroup rules
 * 
 * TODO: Offer option to force or respect context conditions
 */
function context_rules_rules_action_info() {
  return array(
    'rules_action_context_rules_set_context' => array(
      'label' => t('Set a context module context'),
      'parameter' => array(
        'context' => array(
          'type' => 'text',
          'label' => t('Select a context to set'),
          'options list' => 'rules_context_rules_context_options',
          'restriction' => 'input',
        ),
      ),
      'group' => 'Context Rules',
    ),
  );
}

/**
 * Set a context via rules action
 *
 * @param $context
 *   String of context name 
 * @return
 *   An array of the newly set context
 *
 * TODO: Offer option to force or respect context conditions
 */
function rules_action_context_rules_set_context($context) {
  if ($context = context_load($context)) {
    if (context_set('context', $context->name, $context)) {
      return TRUE;
    }
  }
}
