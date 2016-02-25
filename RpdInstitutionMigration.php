<?php

/**
 * @file
 * Contains \RpdInstitutionMigration.
 */

/**
 * Migrates records from the institution_data table into Institution entities.
 */
class RpdInstitutionMigration extends Migration {

  /**
   * An array of country names keyed by their two-letter code.
   */
  protected $countries = array();

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = 'Migrates institution data into institution entities';

    // Connect to the Research project content database.
    db_set_active('project_data');
    $query = db_select('institution_data', 'i')
      ->fields('i', array('ID', 'INSTITUTION_NAME', 'INSTITUTION_DEPARTMENT', 'INSTITUTION_ADDRESS1', 'INSTITUTION_ADDRESS2', 'INSTITUTION_CITY', 'INSTITUTION_ZIP', 'INSTITUTION_URL'));
    $query->leftJoin('states', 's', 'i.INSTITUTION_STATE = s.id');
    $query->fields('s', array('abbrv'));
    $query->leftJoin('countries', 'c', 'i.INSTITUTION_COUNTRY = c.ID');
    $query->fields('c', array('COUNTRY_NAME'))
      ->orderBy('ID', 'ASC');
    db_set_active();
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationEntityAPI('rpd_institution', 'rpd_institution');

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
      MigrateDestinationEntityAPI::getKeySchema('rpd_institution')
    );

    // Get the country list from the Locale module.
    require_once DRUPAL_ROOT . '/includes/locale.inc';
    $this->countries = country_get_list();

    // Add field mappings.
    $this->addFieldMapping('type')
      ->defaultValue('rpd_institution');
    $this->addFieldMapping('title', 'INSTITUTION_NAME');
    $this->addFieldMapping('uid')
      ->defaultValue(1);
    $this->addFieldMapping('created')
      ->defaultValue(time());
    $this->addFieldMapping('changed')
      ->defaultValue(time());
    $this->addFieldMapping('rpd_inst_address', 'COUNTRY_NAME')
      ->callbacks(array($this, 'getCountryCode'));
    $this->addFieldMapping('rpd_inst_address:organisation_name', 'INSTITUTION_DEPARTMENT');
    $this->addFieldMapping('rpd_inst_address:thoroughfare', 'INSTITUTION_ADDRESS1');
    $this->addFieldMapping('rpd_inst_address:premise', 'INSTITUTION_ADDRESS2');
    $this->addFieldMapping('rpd_inst_address:locality', 'INSTITUTION_CITY');
    $this->addFieldMapping('rpd_inst_address:administrative_area', 'abbrv');
    $this->addFieldMapping('rpd_inst_address:postal_code', 'INSTITUTION_ZIP');
    $this->addFieldMapping('rpd_inst_url', 'INSTITUTION_URL');
    $this->addFieldMapping('rpd_inst_url:title')
      ->defaultValue('');
    $this->addFieldMapping('rpd_inst_url:language')
      ->defaultValue(LANGUAGE_NONE);
  }

  /**
   * Finds a country's two-letter code from its full name.
   *
   * @param type $country
   *   A country's full name.
   * @return string
   *   A two-letter country code.
   */
  protected function getCountryCode($country) {
    $key = array_search($country, $this->countries);
    if ($key !== FALSE) {
      return $key;
    }
    return 'US';
  }

}
