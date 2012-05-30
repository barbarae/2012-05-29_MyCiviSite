<?php
/**
 * implimentation of mycustom_form_form_alter()
 */
function mycustom_forms_form_alter(&$form, &$form_state, $form_id){

// add all mycustom modules below

	if ($form_id == 'webform_client_form_42') {
		switch($form_page_num){
		case '1' :
				if ($form['submitted']['number_selection_one']['#value'] == 191) {
					if ($form['submitted']['number_selection_two']['#value'] == 161) { 
	  			  drupal_goto('content/2012-number-puzzle-winner');
					}		
	  		}
	  break;
	  case '2' :
				if ($form['submitted']['number_one_pg2']['#value'] == 191) {
					if ($form['submitted']['number_two_pg2']['#value'] == 161) { 
	  			$form_state['redirect'] = 'content/2012-number-puzzle-winner';
						}		
	  		}
	  break;
	  case '3' :
				if ($form['submitted']['number_one_pg3']['#value'] == 191) {
					if ($form['submitted']['number_two_pg3']['#value'] == 161) { 
	  			$form_state['redirect'] = 'content/2012-number-puzzle-winner';
	  			}
	  		}
	  break;
	  case '4' :
				if ($form['submitted']['number_one_pg4']['#value'] == 191) {
					if ($form['submitted']['number_two_pg4']['#value'] == 161) { 
	  			$form_state['redirect'] = 'content/2012-number-puzzle-winner';
	  			}
	  		}
	  break;
	     }
	   }
	 }   
}