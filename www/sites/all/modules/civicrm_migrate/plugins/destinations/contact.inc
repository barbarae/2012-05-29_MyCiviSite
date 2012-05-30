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
class MigrateDestinationCiviCRMContact extends MigrateDestinationCiviCRMEntity {

  public function __construct() {
    parent::__construct('contact');
  }

  public function __toString() {
    $output = t('CiviCRM contact');
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
    $phone_types = MigrateDestinationCiviCRMEntity::getPhoneTypes();
    $location_types = MigrateDestinationCiviCRMEntity::getLocationTypes();
    foreach ($location_types as $id => $value) {
      foreach ($phone_types as $phoneid => $phone_value) {
        $phones['phone__' . strtolower($id) . '__' . strtolower($phoneid)] = $phone_value . ' (' . $value . ')';
      }
    }
    $fields += $phones;

    $address_fields = array(
      'street_address' => 'Street address',
      'supplemental_address_1' => 'Supplemental Address 1',
      'supplemental_address_2' => 'Supplemental Address 2',
      'city' => 'City',
      'state_province' => 'State/Province',
      'postal_code' => 'Postal code',
      'postal_code_ext' => 'Postal code extension',
      'country' => 'Country',
    );

    foreach ($location_types as $id => $value) {
      foreach ($address_fields as $field => $name) {
        $fields[$field . '__' . strtolower($id)] = $name . ' (' . $value . ')';
      }
    }

    $tags = MigrateDestinationCiviCRMEntity::getTagTypes();
    foreach ($tags as $id => $value) {
      $fields["tag_$id"] = $value;
    }
    return $fields + array(
      'note' => 'Note',
      'email' => 'Email',
    );
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
    // contact specific munging goes here
    $location_types = MigrateDestinationCiviCRMEntity::getLocationTypes();

    // addresses
    $addresses = 0;
    $loop = 0;
    foreach ($location_types as $id => $location) {
      if (isset($entity->{'street_address__' . $id}) &&
          isset($entity->{'city__' . $id}) &&
          isset($entity->{'state_province__' . $id}) &&
          isset($entity->{'postal_code__' . $id})) {

        $address_fields = array(
          'street_address',
          'supplemental_address_1',
          'supplemental_address_2',
          'city',
          'state_province',
          'postal_code',
          'postal_code_ext',
          'country',
        );
        $address = array();
        foreach ($address_fields as $field) {
          $f = $field . '__' . $id;
          $address[$field] = isset($entity->{$f}) ? $entity->{$f} : null;
          unset($entity->{$f});
        }
        $address['location_type_id'] = $id; // address location type
        $address['is_primary'] = ($addresses == 0) ? 1 : 0;
        $entity->{'street_address__' . $id} = array('api.address.create' . (($addresses == 0) ? '' : '__' . $addresses) => $address);
        $addresses++;
      }
      $loop++;
    }

    // phone numbers
    $phone_count = 0;
    $loop = 0;

    $phone_types = MigrateDestinationCiviCRMEntity::getPhoneTypes();
    foreach ($location_types as $id => $location) {
      foreach ($phone_types as $phone_id => $phone_value) {
        $field = implode('__', array('phone', $id, $phone_id));
        if (isset($entity->{$field}) && !empty($entity->{$field})) {
          $entity->{$field} = array('api.phone.create' . (($phone_count == 0) ? '' : '__' . $phone_count) => array (
            'location_type_id' => $id,
            'phone_type_id' => $phone_id,
            'phone' => $entity->{$field},
            'is_primary' => ($phone_count == 0) ? 1 : 0,
          ));
          $phone_count++;
        }
        $loop++;
      }
    }

    $tag_count = 0;
    foreach (MigrateDestinationCiviCRMEntity::getTagTypes() as $id => $value) {
      if (isset($entity->{"tag_$id"}) && $entity->{"tag_$id"}) {
        $entity->{"tag_$id"} = array('api.entity_tag.create' . (($tag_count == 0) ? '' : '__' . $tag_count) => array(
          'tag_id' => $id,
        ));
        $tag_count++;
      }
    }

    // then get on with it
    parent::prepare($entity, $source_row);
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
