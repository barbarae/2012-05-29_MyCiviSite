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
class MigrateDestinationCiviCRMMembership extends MigrateDestinationCiviCRMEntity {

  public function __construct() {
    parent::__construct('membership');
  }

  public function __toString() {
    $output = t('CiviCRM membership');
    return $output;
  }

  /**
  * Returns a list of fields available to be mapped.
  *
  * @return array
  *  Keys: machine names of the fields (to be passed to addFieldMapping)
  *  Values: Human-friendly descriptions of the fields.
  */
  public function fields() {
    $fields = parent::fields();
    foreach (array('contact_id', 'start_date', 'end_date', 'join_date', 'source') as $which) {
      $fields[$which] = $which;
      unset($fields['membership_' . $which]);
    }
    return $fields;
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
    return parent::prepare($entity, $source_row);
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
}
