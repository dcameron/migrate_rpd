<?php

/**
 * @file
 * Contains \RpdActivityStatusMigration.
 */

/**
 * Migrates records from the status table into Activity status terms.
 */
class RpdActivityStatusMigration extends Migration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = 'Migrates records in the status table to RPD Activity status taxonomy terms.';

    // Connect to the Research project content database.
    db_set_active('project_data');
    $query = db_select('status', 's')
      ->fields('s', array('ID', 'TEXT'))
      ->orderBy('ID', 'ASC');
    db_set_active();
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_activity_statuses');

    // Instantiate the MigrateMap.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'ID' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    // Add field mappings.
    $this->addFieldMapping('name', 'TEXT');
    $this->addFieldMapping('description')
      ->defaultValue('');
    $this->addFieldMapping('parent')
      ->defaultValue(0);
    $this->addFieldMapping('format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('weight')
      ->defaultValue(0);
  }

}
