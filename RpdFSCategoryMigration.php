<?php

/**
 * @file
 * Contains \RpdFSCategoryMigration.
 */

/**
 * Migrates records from the category table into Food safety category terms.
 */
class RpdFSCategoryMigration extends Migration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = 'Migrates records in the category table to RPD Food safety category taxonomy terms.';

    // Connect to the Research project content database.
    db_set_active('project_data');
    $query = db_select('category', 'c')
      ->fields('c', array('ID', 'CATEGORY_NAME', 'KEYWORDS', 'IDENTIFIERS', 'CATEGORY_DESCRIPTION'))
      ->orderBy('ID', 'ASC');
    db_set_active();
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_food_safety_categories');

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
    $this->addFieldMapping('name', 'CATEGORY_NAME')
      ->callbacks('html_entity_decode');
    $this->addFieldMapping('description', 'CATEGORY_DESCRIPTION');
    $this->addFieldMapping('parent')
      ->defaultValue(0);
    $this->addFieldMapping('format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('weight')
      ->defaultValue(0);
    $this->addFieldMapping('rpd_fscat_keywords', 'KEYWORDS')
      ->defaultValue('');
    $this->addFieldMapping('rpd_fscat_keywords:format')
      ->defaultValue('plain_text');
    $this->addFieldMapping('rpd_fscat_keywords:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('rpd_fscat_identifiers', 'IDENTIFIERS')
      ->defaultValue('');
    $this->addFieldMapping('rpd_fscat_identifiers:format')
      ->defaultValue('plain_text');
    $this->addFieldMapping('rpd_fscat_identifiers:language')
      ->defaultValue(LANGUAGE_NONE);
  }

}
