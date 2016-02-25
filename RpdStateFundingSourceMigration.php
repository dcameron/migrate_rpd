<?php

/**
 * @file
 * Contains \RpdStateFundingSourceMigration.
 */

/**
 * Migrates records from the agency_data table into State Funding Source terms.
 */
class RpdStateFundingSourceMigration extends Migration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = 'Migrates records in the agency_data table to RPD State funding source taxonomy terms.';

    // Connect to the Research project content database.
    db_set_active('project_data');
    $query = db_select('agency_data', 'a')
      ->fields('a', array('ID', 'AGENCY_FULL_NAME', 'AGENCY_ACRONYM', 'AGENCY_URL'))
      ->condition('US_GOVT', 0)
      ->orderBy('ID', 'ASC');
    $query->leftJoin('agency_hierarchy', 'h', 'a.ID = h.aid');
    $query->fields('h', array('parent'));
    db_set_active();
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_funding_sources_state_private');

    // Instantiate the MigrateMap.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'ID' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'alias' => 'a',
        ),
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    // Add field mappings.
    $this->addFieldMapping('name', 'AGENCY_FULL_NAME');
    $this->addFieldMapping('description')
      ->defaultValue('');
    $this->addFieldMapping('parent', 'parent')
      ->sourceMigration('StateFundingSource');
    $this->addFieldMapping('format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('weight')
      ->defaultValue(0);
    $this->addFieldMapping('rpd_funding_acronym', 'AGENCY_ACRONYM');
    $this->addFieldMapping('rpd_funding_acronym:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('rpd_funding_url', 'AGENCY_URL');
    $this->addFieldMapping('rpd_funding_url:title')
      ->defaultValue('');
    $this->addFieldMapping('rpd_funding_url:language')
      ->defaultValue(LANGUAGE_NONE);
  }

}
