<?php

/**
 * @file
 * Contains \RpdInvestigatorMigration.
 */

/**
 * Migrates records from the investigator_data table into Investigator entities.
 */
class RpdInvestigatorMigration extends Migration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->dependencies = array('Institution');
    $this->description = 'Migrates records in the investigator_data table to RPD Investigator entities.';

    // Connect to the Research project content database.
    db_set_active('project_data');
    $query = db_select('investigator_data', 'i')
      ->fields('i', array('ID', 'name', 'EMAIL_ADDRESS', 'PHONE_NUMBER', 'INSTITUTION'));
    db_set_active();
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationEntityAPI('rpd_investigator', 'rpd_investigator');

    // Instantiate the MigrateMap.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'ID' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'alias' => 'i',
        ),
      ),
      MigrateDestinationEntityAPI::getKeySchema('rpd_investigator')
    );

    // Add field mappings.
    $this->addFieldMapping('type')
      ->defaultValue('rpd_investigator');
    $this->addFieldMapping('title', 'name');
    $this->addFieldMapping('uid')
      ->defaultValue(1);
    $this->addFieldMapping('created')
      ->defaultValue(time());
    $this->addFieldMapping('changed')
      ->defaultValue(time());
    $this->addFieldMapping('rpd_inv_email', 'EMAIL_ADDRESS');
    $this->addFieldMapping('rpd_inv_phone', 'PHONE_NUMBER');
    $this->addFieldMapping('rpd_inv_institution', 'INSTITUTION')
      ->sourceMigration('Institution');
  }

}
