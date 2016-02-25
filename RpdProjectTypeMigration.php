<?php

/**
 * @file
 * Contains \RPDProjectTypeMigration.
 */

/**
 * Migrates records from the projecttype table into Project type terms.
 */
class RpdProjectTypeMigration extends Migration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = 'Migrates records in the projecttype table to RPD Project type taxonomy terms.';

    // Connect to the Research project content database.
    db_set_active('project_data');
    $query = db_select('projecttype', 't')
      ->fields('t', array('ID', 'NAME', 'COMMENTS'))
      ->orderBy('ID', 'ASC');
    db_set_active();
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_project_types');

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
    $this->addFieldMapping('name', 'NAME');
    $this->addFieldMapping('description', 'COMMENTS');
    $this->addFieldMapping('parent')
      ->defaultValue(0);
    $this->addFieldMapping('format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('weight')
      ->defaultValue(0);
  }

}
