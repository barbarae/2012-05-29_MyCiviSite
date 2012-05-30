<?php

/**
 * @file
 * Engine classes.
 */

/**
 * Interface defining the methods of datasource engine.
 */
interface DatasourceEngineInterface {
  
  /**
   * The method to import a record of the datasource input.
   * 
   * @return boolean
   *   - self::IMPORTED : record imported
   *   - self::NOT_IMPORTED : record not imported
   *   - self::EOF : no more record
   */
  public function importRecord();
   
  /**
   * Initialize import.
   * 
   * @return boolean
   *   - TRUE : import can begin.
   *   - FALSE : import cannot begin.
   */
  public function initImport();
  
  /**
   * Some cleanup stuff.
   */
  public function completeImport();
}

/**
 * Abstract datasource engine.
 */
abstract class DatasourceEngineAbstract implements DatasourceEngineInterface {

  const IMPORTED = 1;
  const NOT_IMPORTED = 0;
  const EOF = -1;
  
  public $input_id;
  
  public function __construct($input_id) {
    $this->input_id = $input_id;
  }

  public function initImport() {
    return TRUE;
  }
  
  public function completeImport() {
    // nothing
  }
  
  /**
   * Default importRecord().
   * Call getRecord() and try to import te datas matching with udid fields.
   * 
   * @see DatasourceEngineInterface::importRecord()
   * @see getRecord()
   * 
   * @return integer
   *   The status of the import.
   *   - self::IMPORTED : record imported
   *   - self::NOT_IMPORTED : record not imported
   *   - self::EOF : no more record
   */
  public function importRecord() {
    try {
      // keep track of new entities
      $new_entities = array();
      
      $to_save_context=array();
      $record=$this->getRecord();
      
      if (!$record) return self::EOF;
      
      $input = datasources_inputs_info($this->input_id);
  
      $entities=array();
      $exists = FALSE;
      
      if ($input['has_key']) {
      
        $ids = array();
        // we try to match existing entity in the specified order
        $flat_udid=NULL;
        foreach ($input['keys'] as $field => $key_info) {
          // use only some key
          if ($key_info['use']) {
            $res=udid_entity_ids($record[$field], array('entities' => $input['entity']));
            if (isset($res[$input['entity']])) {
              $exists = TRUE;
              if (!$flat_udid) {
                // store the first udid for watchdog...
                $flat_udid="{$record[$field]['source']}/{$record[$field]['id']}";
              }
              foreach ($res[$input['entity']] as $id => $item) {
                $entity=entity_load($input['entity'], array($id));
                if ($entity) {
                  $entity = reset($entity);
                  $ids[$id] = array('entity' => $entity);
                  // test if smart keys are ok
                  foreach ($item->fields as $udid_field) {
                    $current = field_get_items($input['entity'], $entity, $udid_field);
                    if ($current&&$key_info['smart']) {
                      if (!isset($ids[$id]['smart'][$udid_field])) $ids[$id]['smart'][$udid_field] = TRUE;
                      $cmp = udid_compare($current[0], $record[$field]);
                      if ($cmp!==FALSE) {
                        $ids[$id]['smart'][$udid_field] = ($cmp <= 0) && $ids[$id]['smart'][$udid_field];
                      }
                    }
                  }
                }
              }
            }
          }
        }
    
        $n=0;
        foreach($ids as $id => $item) {
          $entity = $item['entity'];
          list(, , $bundle)=entity_extract_ids($input['entity'], $entity);
          if ($input['bundle']==$bundle || $input['policies']['bad_bundle'] = 'update') {
            // entity is updatable
            
            // test if all smart keys are ok
            $smartok = TRUE;
            if (isset($item['smart'])) {
              foreach($item['smart'] as $smart) {
                if (!$smart) $smartok = FALSE;
              }
            }
            
            if ($smartok) {
              $entities[$id]=$entity;
            }
            else {
              watchdog('datasource', t('Smart key policy : a record from datasource input !input was not imported because the existing udid version is greater than the record (!udid).'), array('!input' => $this->input_id, '!udid' => $flat_udid), WATCHDOG_WARNING);
            }
            
            if ($input['policies']['duplicate']=='first') {
              // retrieve and update only the first entity
              break;
            }
            elseif ($n>0 && $input['policies']['duplicate']=='abort') {
              watchdog('datasource', t('Duplicate policy : a record from datasource input !input was not imported because it matches multiple entities (!udid).'), array('!input' => $this->input_id, '!udid' => $flat_udid), WATCHDOG_WARNING);
              return self::NOT_IMPORTED;
            }
            $n++;
          }
        }
      
        if ($exists && !$input['policies']['update']) {
          watchdog('datasource', t('Update policy : a record from datasource input !input was not imported (!udid).'), array('!input' => $this->input_id, '!udid' => $flat_udid), WATCHDOG_WARNING);
          return self::NOT_IMPORTED;
        }
      }

      if (empty($entities) && !$input['policies']['insert']) {   
        watchdog('datasource', t('Insert policy : a record from datasource input !input was not imported (!udid).'), array('!input' => $this->input_id, '!udid' => $flat_udid), WATCHDOG_WARNING);
        return self::NOT_IMPORTED;
      }
      
      if (empty($entities) || !$exists) {
        $entities=array();
        // create an empty entity
        $entity=entity_create_stub_entity($input['entity'], array(0, NULL, $input['bundle']));
        datasources_set_language($input['entity'], $entity);
        entity_save($input['entity'], $entity); // saving seams easier to handle id
        list($id, , ) = entity_extract_ids($input['entity'], $entity);      
        // get a cached version for consistency with later wrapper acces to entity
        $entity = entity_load($input['entity'], array($id));
        $entity = reset($entity);
        $entities[$id]=$entity;
        $new_entities[$id]=$entity;
      }
      
      $to_save=array();
      
      foreach ($entities as $id => $entity) {
        $fields_info=datasources_entity_fields($this->input_id, $entity, TRUE);
        
        foreach ($fields_info['fields'] as $field => $field_info) {
    
          if (isset($field_info['map']['field']) && isset($record[$field_info['map']['field']])) {
            // TODO add EntityDrupalWrapper true support currently only basic structure are supported
            $value=$record[$field_info['map']['field']];
            $context = array(
              'source_type' => $input['fields'][$field_info['map']['field']]['type'],
              'target_type' => $field_info['type'],
              'source_field' => $field_info['map']['field'],
              'target_field' => $field,
              'input' => $input,
              'record' => $record,
              'engine' => $this,
            );
            
            $w=$field_info['wrapper'];
            
            // try to get the underlaying entity
            do {
              $info = $w->info();
              if (isset($info['parent'])) {
                $w = $info['parent'];
              }
              else {
                $w = NULL;
                break;
              }
            } while(!$w instanceof EntityDrupalWrapper);
            
            if ($w) {
              
              // got it !
              $w->language(language_default('language'));
              
              $list_mode = isset($field_info['map']['list']) ? $field_info['map']['list'] : '';
  
              $l_target = datasources_list_nesting_level($field_info['type']);
              $l_source = datasources_list_nesting_level($input['fields'][$field_info['map']['field']]['type']);
              if ($l_target>0 && $list_mode) {
                $context['list'] = $list_mode;
                if ($l_target > 1||$l_source > 1) {
                   watchdog('datasource', t('Datasource !input : can only manage 1 level list field (!field).', array('!input' => $this->input_id, '!field' => $field_info['map']['field'])), WATCHDOG_WARNING);
                   return self::NOT_IMPORTED;
                 }
                 if ($l_target<$l_source) {
                   watchdog('datasource', t('Datasource !input : cannot put a list source field in a non list target field (!field)', array('!input' => $this->input_id, '!field' => $field_info['map']['field'])), WATCHDOG_WARNING);
                   return self::NOT_IMPORTED;
                 }
                 if ($list_mode == 'replace') {
                   $field_info['wrapper']->set(array());
                 }
                 if ($l_source == 0) {
                   $values = array($value);
                 }
                 else {
                   $values = $value;
                 }
                 
                 foreach ($values as $value) {
                   // append a new value
                   $value = datasources_apply_preprocessor($field_info['map']['preprocessor'], $value, $context);
                   // NULL means nothing so it's not added
                   if ($value!==NULL) $field_info['wrapper']->get(NULL)->set($value);
                 }
              }
              else {
                // raw set by default
                $value = datasources_apply_preprocessor($field_info['map']['preprocessor'], $value, $context);
                $field_info['wrapper']->set($value);
              }
  
              // store wrapper of updated entities
              if (!in_array($w, $to_save, TRUE)) {
                $to_save[]=$w;
              }
              
              // store some usefull (?) infos
              if (!isset($to_save_context[$id])) {
                $to_save_context[$id]=array(
                  'entity' => $entity,
                  'fields' => array(),
                );
              }
              $to_save_context[$id]['fields'][$field] = $field_info['map'];
            }
            else {
              // oops
              watchdog('datasource', t('Error : while importing datasource input field !input::!field, cannot find underlaying entity for on record (!udid).'), array('!input' => $this->input_id, '!field' => $field, '!udid' => $flat_udid), WATCHDOG_ERROR);
              return self::NOT_IMPORTED;
            }
          }
        }     
      }
      
      // last chance to modify entities
      foreach ($to_save_context as $id => $context) {
        $entity = $context['entity'];
        $context['record'] = $record;
        $this->beforeSaveEntity($entity, $context);
      }
      
      try {
                
        // save underlaying entities using wrapper
        foreach ($to_save as $w) {
          $w->save();
        }
        
        // allow module to do more stuff
        $this->postImportRecord($record, $to_save_context);
        
        return self::IMPORTED;
      }
      catch(EntityMetadataWrapperException $e) {
        throw new DatasourceEngineException('EntityMetadataWrapperException : ' . $e->getMessage());
      }
    }
    catch (DatasourceEngineException $e) {
      watchdog('datasource', t('DatasourceEngineException while importing !input::!field (udid = !udid) : !msg', array('!msg' => $e->getMessage())), array('!input' => $this->input_id, '!field' => isset($field) ? $field : '*unknown*', '!udid' => isset($flat_udid) ? $flat_udid : '*unknown*'), WATCHDOG_ERROR);
      $to_save_context['exception'] = $e;
      
      $this->onErrorImportRecord(isset($record) ? $record : NULL, $to_save_context);
      
      // some cleanup
      if (!empty($new_entities)) entity_delete_multiple($input['entity'], array_keys($new_entities));

    }
    
    return self::NOT_IMPORTED;
  }

  /**
   * Last chance to modify the imported entity.
   * @param stdClass $entity
   * @param array $context
   */
  protected function beforeSaveEntity($entity, array $context) {
    // nothing by default
  }
  
  /**
   * Some post import record stuff.
   * Only executed if record import was successfull.
   * 
   * @param array $record
   * @param array $context
   *  A "entity ids" keyed array of arrays with keys :
   *    - entity : the updated entity
   *    - fields : the updated fields (target => source)
   *  
   */
  protected function postImportRecord(array $record, array $context) {
    // nothing by default
  }
  
  /**
   * Some stuff to do on error.
   * Only executed if record import was not successfull.
   * 
   * @param array $record
   * @param array $context
   *  A "entity ids" keyed array of arrays with keys :
   *    - entity : the updated entity
   *    - fields : the updated fields (target => source)
   *    - exception : the raised exception if any
   *  
   */
  protected function onErrorImportRecord($record, array $context) {
    // nothing by default
  }  
  
  /**
   * Must provide an input record.
   * The input record is an array with values keyed with fields defined in the datasource input.
   * 
   * @see hook_datasource_inputs()
   */
  abstract protected function getRecord();
  
}

class DatasourceEngineException extends Exception {}
