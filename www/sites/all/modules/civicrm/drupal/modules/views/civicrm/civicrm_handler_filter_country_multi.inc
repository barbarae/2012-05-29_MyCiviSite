<?php
  /*
   +--------------------------------------------------------------------+
   | CiviCRM version 4.0                                                |
   +--------------------------------------------------------------------+
   | This file is a part of CiviCRM.                                    |
   |                                                                    |
   | CiviCRM is free software; you can copy, modify, and distribute it  |
   | under the terms of the GNU Affero General Public License           |
   | Version 3, 19 November 2007 and the CiviCRM Licensing Exception.   |
   |                                                                    |
   | CiviCRM is distributed in the hope that it will be useful, but     |
   | WITHOUT ANY WARRANTY; without even the implied warranty of         |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               |
   | See the GNU Affero General Public License for more details.        |
   |                                                                    |
   | You should have received a copy of the GNU Affero General Public   |
   | License and the CiviCRM Licensing Exception along                  |
   | with this program; if not, contact CiviCRM LLC                     |
   | at info[AT]civicrm[DOT]org. If you have questions about the        |
   | GNU Affero General Public License or the licensing of CiviCRM,     |
   | see the CiviCRM license FAQ at http://civicrm.org/licensing        |
   +--------------------------------------------------------------------+
   
   **
   * @file Provides Views integration for CiviCRM Multi-Select Country custom fields
   *
   * @author DaveJ
   * based on civicrm_handler_filter_custom_option and civicrm_handler_filter_country
   */
class civicrm_handler_filter_country_multi extends views_handler_filter_in_operator {

    function construct( ) {
        parent::construct();
        if ( ! civicrm_initialize( ) ) {
            return;
        }
    }

    function get_value_options() {
        if (!isset($this->value_options)) {
            require_once 'CRM/Core/PseudoConstant.php';
            $countries = CRM_Core_PseudoConstant::country( );
            $options = array( );
            if (is_array($countries)) {
                foreach ($countries as $id => $name ) {
                    $options[$id] = $name;
                }
            }
            $this->value_options = $options;
        }
    }


    function op_simple() {
        if (empty($this->value)) {
            return;
        }
        
        $this->ensure_my_table();
        
        $sep = CRM_Core_DAO::VALUE_SEPARATOR;
        
        // negated operator uses AND, positive uses OR
        $op = ($this->operator == 'in') ? 'LIKE' : 'NOT LIKE';
        $glue = ($this->operator == 'in') ? 'OR ' : 'AND ';
        foreach ($this->value as $value){
            $clauses[] = "$this->table_alias.$this->real_field " . $op . " '%" . $sep . $value . $sep . "%' ";
        }
        $clause = implode($glue, $clauses);
        $this->query->add_where_expression($this->options['group'], $clause);
    }

}


