<?php
// $Id$

/**
 * @file
 * Support for tables defined through the Schema API.
 */

/**
 * Destination class implementing migration into a single table defined through
 * the Schema API.
 */
class MigrateDestinationCiviCRMEntity extends MigrateDestination {
  /**
   * The name of the current table.
   *
   * @var string
   */
  protected $entityName = NULL;

  public function __construct($entity_name) {
    $this->entityName = $entity_name;
    civicrm_initialize();
    require_once 'api/api.php';
  }

  static public function getKeySchema($entity_name = NULL) {
    $keys = array();
    $keys['id'] = array(
      'type' => 'int',
      'unsigned' => TRUE,
      'not null' => TRUE,
      'description' => 'ID of destination record',
    );
    return $keys;
  }

  public function __toString() {
    $output = t('CiviCRM !entity', array('!entity' => $this->entityName));
    return $output;
  }

  /**
   * Delete a single row.
   *
   * @param $id
   *  Primary key values.
   */
  public function rollback(array $id) {
    migrate_instrument_start('civicrm rollback');
    $params = array('version' => 3, 'id' => reset($id), 'skip_undelete' => true);
    $results = civicrm_api($this->entityName, 'delete', $params);
    if ($results['is_error'] != 0) {
      throw new MigrateException(t('Error occurred deleting !entity: !message', array('!entity' => $this->entityName, '!message' => $results['error_message'])), Migration::MESSAGE_ERROR);
    }
    migrate_instrument_stop('civicrm rollback');
  }

  /**
   * Import a single row.
   *
   * @param $entity
   *  Object object to build. Prefilled with any fields mapped in the Migration.
   * @param $row
   *  Raw source data object - passed through to prepare/complete handlers.
   * @return array
   *  Array of key fields of the object that was saved if
   *  successful. FALSE on failure.
   */
  public function import(stdClass $entity, stdClass $row) {

    $migration = Migration::currentMigration();
    // Updating previously-migrated content?
    if (isset($row->migrate_map_destid1)) {
      if (isset($entity->id)) {
        if ($entity->id != $row->migrate_map_destid1) {
          throw new MigrateException(t("Incoming id !id and map destination id !destid1 don't match",
            array('!id' => $entity->{$this->id}, '!destid1' => $row->migrate_map_destid1)));
        }
      }
      else {
        $entity->id = $row->migrate_map_destid1;
      }
    }

    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      if (!isset($entity->id)) {
        throw new MigrateException(t('System-of-record is DESTINATION, but no destination id provided'));
      }
      // Load the entity that's being updated, update its values, then
      // substitute the (fake) passed in entity with that one.
      $old_entity = entity_load($this->entityType, $entity->{$this->id});
      foreach ($entity as $field => $value) {
        $old_entity->$field = $entity->$field;
      }
      $entity = $old_entity;
    }
    else {
      // Create a real entity object, update its values with the ones we have
      // and pass it along.
      $new_entity = new stdClass();
      foreach ($entity as $field => $value) {
        $new_entity->$field = $entity->$field;
      }

      $entity = $new_entity;
    }
    $this->prepare($entity, $row);

    $updating = TRUE;
    if (!isset($entity->id)) {
      $updating = FALSE;
    }

    $values = array();
    foreach ($entity as $field => $value) {
      if (is_array($value)) {
        // this needs to be an array with a single key/value -- the key is the api string, the value is array
        $key = reset(array_keys($value));
        if (substr($key, 0, 4) == 'api.') {
          if (strpos($key, '__')) {
            $api = explode('__', $key);
            $api = $api[0];
          }
          else {
            $api = $key;
          }
          $values[$api][] = $value[$key];
        }
      }
      else {
        $values[$field] = $value;
      }
    }

    $values['version'] = 3;
    $op = ($updating) ? 'UPDATE' : 'CREATE';

    migrate_instrument_start('entity_save');
    $result = civicrm_api($this->entityName, $op, $values);
    migrate_instrument_stop('entity_save');

    if ($result['is_error'] == 1) {
      throw new MigrateException(t('Error processing record: !entityName; !op; !error', array('!entityName' => $this->entityName, '!op' => $op, '!error' => $result['error_message'])), Migration::MESSAGE_ERROR);
    }
    else {
      $entity->id = $result['id'];
      if ($updating) {
        $this->numUpdated++;
      }
      else {
        $this->numCreated++;
      }

      $this->complete($entity, $row);
    }

    $return = (isset($entity->id) && $entity->id > 0)
      ? array($entity->id) : FALSE;
    return $return;
  }

  /**
   * Returns a list of fields available to be mapped.
   *
   * @return array
   *  Keys: machine names of the fields (to be passed to addFieldMapping)
   *  Values: Human-friendly descriptions of the fields.
   */
  public function fields() {
    $fields = civicrm_api($this->entityName, 'getfields', array('version' => 3));
    if (isset($fields['values'])) {
      $fields = $fields['values'];
    }
    $return = array();
    foreach ($fields as $field => $info) {
      $label = 'Unnamed field';
      foreach (array('title', 'label', 'name') as $which) {
        if (isset($info[$which])) {
          $label = $info[$which];
          break;
        }
      }
      $return[$field] = $label;
    }
    return $return;
  }

  /**
   * Give handlers a shot at modifying the object before saving it.
   *
   * @param $entity
   *  Entity object to build. Prefilled with any fields mapped in the Migration.
   * @param $source_row
   *  Raw source data object - passed through to prepare handlers.
   */
  public function prepare($entity, stdClass $source_row) {
    $migration = Migration::currentMigration();
    $entity->migrate = array(
      'machineName' => $migration->getMachineName(),
    );

    // Call any prepare handler for this specific Migration.
    if (method_exists($migration, 'prepare')) {
      $migration->prepare($entity, $source_row);
    }
  }

  /**
   * Give handlers a shot at modifying the object (or taking additional action)
   * after saving it.
   *
   * @param $object
   *  Entity object to build. This is the complete object after saving.
   * @param $source_row
   *  Raw source data object - passed through to complete handlers.
   */
  public function complete($entity, stdClass $source_row) {
    $migration = Migration::currentMigration();

    // Call any complete handler for this specific Migration.
    if (method_exists($migration, 'complete')) {
      $migration->complete($entity, $source_row);
    }
  }

  public static function getLocationTypes() {
    $params = array(
      'name' => 'locationType',
      'version' => 3,
    );

    $result = civicrm_api('constant', 'get', $params);
    return $result['values'];
  }

  public static function getPhoneTypes() {
    $params = array(
      'name' => 'phoneType',
      'version' => 3,
    );

    $result = civicrm_api('constant', 'get', $params);
    return $result['values'];
  }

  public static function getTagTypes() {
    $params = array(
      'name' => 'tag',
      'version' => 3,
    );

    $result = civicrm_api('constant', 'get', $params);
    return $result['values'];
  }

  public static function getSchemaForType($type) {
    switch ( $type ) {
      case     1: $return = array('type' => 'int', 'size' => 'normal',); break;
      case     2: $return = array('type' => 'varchar', 'size' => 'normal',) ; break;
      case     3: $return = array('type' => 'varchar', 'size' => 'normal',); break;
      case     4: $return = array('type' => 'datetime', 'size' => 'normal',); break;
      case     8: $return = array('type' => 'datetime', 'size' => 'normal',); break;
      case    16: $return = array('type' => 'int', 'size' => 'tiny',); break;
      case    32: $return = array('type' => 'text', 'size' => 'normal',); break;
      case    64: $return = array('type' => 'blob', 'size'  => 'normal',); break;
      case   256: $return = array('type' => 'int', 'size'  => 'normal',); break;
      case   512: $return = array('type' => 'float', 'size'  => 'normal',); break;
      case  1024: $return = array('type' => 'decimal', 'size' =>'normal', 'precision' => '2',); break;
      case  2048: $return = array('type' => 'datetime', 'size'  => 'normal',); break;
      case  4096: $return = array('type' => 'varchar', 'size' => 'normal',); break;
      case 16384: $return = array('type' => 'blob', 'size' => 'big'); break;
    }
    return $return;
  }
}
