<?php

/**
 * @file
 * Contains \RpdF2TCategoryMigration.
 */

/**
 * Migrates records from the farm_to_table_categories table into terms.
 */
class RpdF2TCategoryMigration extends Migration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = 'Migrates records in the farm_to_table_categories table to RPD Farm to table category taxonomy terms.';

    // Connect to the Research project content database.
    db_set_active('project_data');
    $query = db_select('farm_to_table_categories', 'c')
      ->fields('c', array('cid', 'name'))
      ->orderBy('cid', 'ASC');
    db_set_active();
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_farm_to_table_categories');

    // Instantiate the MigrateMap.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'cid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    // Add field mappings.
    $this->addFieldMapping('name', 'name');
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
