<?php
/**
 * @file
 * Migrates FSRIO's Research Projects Database to Drupal7.
 */

/**
 * Migrates records from the project table into Research project nodes.
 */
class RPDProjectMigration extends Migration {
  /**
   * An array of Drupal uids keyed by user name.
   */
  protected $users = array();

  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $this->dependencies = array('RPDFSCategory', 'RPDF2TCategory', 'RPDActivityStatus', 'RPDProjectType', 'RPDInstitution', 'RPDInvestigator', 'RPDUSFundingSource', 'RPDIntlFundingSource', 'RPDStateFundingSource');
    $this->description = 'Migrates records in the project table to Research project nodes.';

    $query = db_select('project', 'p')
      ->fields('p', array('ID', 'PROJECT_NUMBER', 'PROJECT_TITLE', 'PROJECT_START_DATE', 'PROJECT_END_DATE', 'PROJECT_FUNDING', 'PROJECT_TYPE', 'PROJECT_KEYWORDS', 'PROJECT_IDENTIFIERS', 'PROJECT_COOPORATORS', 'PROJECT_ABSTRACT', 'PROJECT_PUBLICATIONS', 'PROJECT_MORE_INFO', 'PROJECT_OBJECTIVE', 'PROJECT_ACCESSION_NUMBER', 'ACTIVITY_STATUS', 'DATE_ENTERED', 'COMMENTS', 'LAST_UPDATE', 'LAST_UPDATE_BY'))
    // @todo Remove the query range which limits the number of records during
    // testing.
      ->range(0, 250);
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationNode('research_project');

    // Instantiate the MigrateMap.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'ID' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'alias' => 'p',
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // Query for the list of users.
    $results = db_query("SELECT uid, name FROM users WHERE uid > 0");
    foreach ($results as $result) {
      $this->users[$result->name] = $result->uid;
    }

    // Add field mappings.
    $this->addFieldMapping('title', 'PROJECT_TITLE');
    $this->addFieldMapping('uid', 'LAST_UPDATE_BY')
      ->defaultValue(1)
      ->callbacks(array($this, 'getUID'));
    $this->addFieldMapping('comment')
      ->defaultValue(1);
    $this->addFieldMapping('status')
      ->defaultValue(1);
    $this->addFieldMapping('promote')
      ->defaultValue(0);
    $this->addFieldMapping('sticky')
      ->defaultValue(0);
    $this->addFieldMapping('language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('tnid')
      ->defaultValue(0);
    $this->addFieldMapping('translate')
      ->defaultValue(0);
    $this->addFieldMapping('rpd_project_number', 'PROJECT_NUMBER');
    $this->addFieldMapping('rpd_project_number:format')
      ->defaultValue('plain_text');
    $this->addFieldMapping('rpd_project_number:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('rpd_start_date', 'PROJECT_START_DATE');
    $this->addFieldMapping('rpd_end_date', 'PROJECT_END_DATE');
    $this->addFieldMapping('rpd_funding', 'PROJECT_FUNDING');
    $this->addFieldMapping('rpd_fscats', 'category')
      ->sourceMigration('RPDFSCategory')
      ->defaultValue(0);
    $this->addFieldMapping('rpd_fscats:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('rpd_project_type', 'PROJECT_TYPE')
      ->sourceMigration('RPDProjectType')
      ->defaultValue(0);
    $this->addFieldMapping('rpd_project_type:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('rpd_keywords', 'PROJECT_KEYWORDS')
      ->defaultValue('');
    $this->addFieldMapping('rpd_keywords:format')
      ->defaultValue('plain_text');
    $this->addFieldMapping('rpd_keywords:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('rpd_identifiers', 'PROJECT_IDENTIFIERS')
      ->defaultValue('');
    $this->addFieldMapping('rpd_identifiers:format')
      ->defaultValue('plain_text');
    $this->addFieldMapping('rpd_identifiers:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('rpd_cooperators', 'PROJECT_COOPORATORS')
      ->defaultValue('')
      ->callbacks(array($this, 'splitCooperators'));
    $this->addFieldMapping('rpd_cooperators:format')
      ->defaultValue('plain_text');
    $this->addFieldMapping('rpd_cooperators:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('rpd_abstract', 'PROJECT_ABSTRACT')
      ->defaultValue('')
      ->callbacks('html_entity_decode', 'utf8_encode');
    $this->addFieldMapping('rpd_abstract:format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('rpd_abstract:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('rpd_publications', 'PROJECT_PUBLICATIONS')
      ->defaultValue('')
      ->callbacks(array($this, 'splitPublications'));
    $this->addFieldMapping('rpd_publications:format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('rpd_publications:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('rpd_more_info', 'PROJECT_MORE_INFO')
      ->defaultValue('')
      ->callbacks('html_entity_decode', 'utf8_encode');
    $this->addFieldMapping('rpd_more_info:format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('rpd_more_info:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('body', 'PROJECT_OBJECTIVE')
      ->defaultValue('')
      ->callbacks('html_entity_decode', 'utf8_encode');
    $this->addFieldMapping('body:summary')
      ->defaultValue('');
    $this->addFieldMapping('body:format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('body:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('rpd_accession_number', 'PROJECT_ACCESSION_NUMBER')
      ->defaultValue('');
    $this->addFieldMapping('rpd_accession_number:format')
      ->defaultValue('plain_text');
    $this->addFieldMapping('rpd_accession_number:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('rpd_activity_status', 'ACTIVITY_STATUS')
      ->sourceMigration('RPDActivityStatus');
    $this->addFieldMapping('rpd_activity_status:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('created', 'DATE_ENTERED')
      ->callbacks('strtotime');
    $this->addFieldMapping('rpd_comments', 'COMMENTS')
      ->defaultValue('');
    $this->addFieldMapping('rpd_comments:format')
      ->defaultValue('plain_text');
    $this->addFieldMapping('rpd_comments:language')
      ->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('changed', 'LAST_UPDATE')
      ->callbacks('strtotime');
    $this->addFieldMapping('rpd_f2tcats', 'farm_to_table')
      ->sourceMigration('RPDF2TCategory');
    $this->addFieldMapping('rpd_f2tcats:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('rpd_institutions', 'institutions')
      ->sourceMigration('RPDInstitution');
    $this->addFieldMapping('rpd_investigators', 'investigators')
      ->sourceMigration('RPDInvestigator');
    $this->addFieldMapping('rpd_funding_sources_us', 'us_funding_sources')
      ->sourceMigration('RPDUSFundingSource');
    $this->addFieldMapping('rpd_funding_sources_us:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('rpd_funding_sources_intl', 'intl_funding_sources')
      ->sourceMigration('RPDIntlFundingSource');
    $this->addFieldMapping('rpd_funding_sources_intl:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('rpd_funding_sources_state', 'state_funding_sources')
      ->sourceMigration('RPDStateFundingSource');
    $this->addFieldMapping('rpd_funding_sources_state:source_type')
      ->defaultValue('tid');
  }

  public function prepareRow($row) {
    // Create an array of related food safety categories.
    $row->category = db_select('project_category', 'c')
      ->fields('c', array('category_id'))
      ->condition('project_id', $row->ID)
      ->execute()
      ->fetchCol();

    // Create an array of related farm-to-table categories.
    $row->farm_to_table = db_select('project_farm_to_table', 'f')
      ->fields('f', array('cid'))
      ->condition('pid', $row->ID)
      ->execute()
      ->fetchCol();

    // Create an array of related institution categories.
    $row->institutions = db_select('institution_index', 'i')
      ->fields('i', array('inst_id'))
      ->condition('pid', $row->ID)
      ->execute()
      ->fetchCol();

    // Create an array of related investigator categories.
    $row->investigators = db_select('investigator_index', 'i')
      ->fields('i', array('inv_id'))
      ->condition('pid', $row->ID)
      ->execute()
      ->fetchCol();

    // Create an array of related US funding source categories.
    $query = db_select('agency_index', 'i');
    $query->join('agency_data', 'a', 'i.aid = a.ID');
    $query->fields('i', array('aid'))
      ->condition('pid', $row->ID)
      ->condition('US_GOVT', 1);
    $row->us_funding_sources = $query->execute()
      ->fetchCol();

    // Create an array of related international funding source categories.
    $query = db_select('agency_index', 'i');
    $query->join('agency_data', 'a', 'i.aid = a.ID');
    $query->fields('i', array('aid'))
      ->condition('pid', $row->ID)
      ->condition('US_GOVT', 2);
    $row->intl_funding_sources = $query->execute()
      ->fetchCol();

    // Create an array of related state/private org. funding source categories.
    $query = db_select('agency_index', 'i');
    $query->join('agency_data', 'a', 'i.aid = a.ID');
    $query->fields('i', array('aid'))
      ->condition('pid', $row->ID)
      ->condition('US_GOVT', 0);
    $row->state_funding_sources = $query->execute()
      ->fetchCol();
  }

  /**
   * Returns the uid of a given username.
   *
   * @param string $name
   *   The name of a user.
   * @return int
   *   The uid of the user.
   */
  protected function getUID($name) {
    if (isset($this->users[$name])) {
      return $this->users[$name];
    }
    return 1;
  }

  /**
   * Splits source cooperator fields into multiple values.
   *
   * @param string $source
   *   A field value from the source table record.
   * @return array
   *   A formatted, exploded array of strings derived from the source field.
   */
  protected function splitCooperators($source) {
    // Decode the HTML entities in the source string.
    $html_decoded = html_entity_decode($source);
    // Encode the strings as UTF8 to prevent insert errors due to text encoding.
    $utf8_encoded = utf8_encode($html_decoded);
    // Convert HTML entities to separator characters.
    $replaced = str_replace(array('<BR>', '<LI>', '<li>'), '|', $utf8_encoded);
    // Strip all remaining HTML tags.
    $stripped = strip_tags($replaced);
    $values = explode('|', $stripped);
    // Filter any empty values in the array.
    $trimmed = array_map('trim', $values);
    // Filter any empty values in the array before returning.
    return array_filter($trimmed);
  }

  /**
   * Splits source publication fields into multiple values.
   *
   * @param string $source
   *   A field value from the source table record.
   * @return array
   *   A formatted, exploded array of strings derived from the source field.
   */
  protected function splitPublications($source) {
    // Decode the HTML entities in the source string.
    $html_decoded = html_entity_decode($source);
    // Encode the strings as UTF8 to prevent insert errors due to text encoding.
    $utf8_encoded = utf8_encode($html_decoded);
    $values = explode('<P>', $utf8_encoded);
    // Filter any empty values in the array before returning.
    return array_filter($values);
  }
}

/**
 * Migrates records from the category table into Food safety category terms.
 */
class RPDFSCategoryMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $this->description = 'Migrates records in the category table to RPD Food safety category taxonomy terms.';

    $query = db_select('category', 'c')
      ->fields('c', array('ID', 'CATEGORY_NAME', 'KEYWORDS', 'IDENTIFIERS', 'CATEGORY_DESCRIPTION'))
      ->orderBy('ID', 'ASC');
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

/**
 * Migrates records from the farm_to_table_categories table into terms.
 */
class RPDF2TCategoryMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $this->description = 'Migrates records in the farm_to_table_categories table to RPD Farm to table category taxonomy terms.';

    $query = db_select('farm_to_table_categories', 'c')
      ->fields('c', array('cid', 'name'))
      ->orderBy('cid', 'ASC');
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

/**
 * Migrates records from the status table into Activity status terms.
 */
class RPDActivityStatusMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $this->description = 'Migrates records in the status table to RPD Activity status taxonomy terms.';

    $query = db_select('status', 's')
      ->fields('s', array('ID', 'TEXT'))
      ->orderBy('ID', 'ASC');
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

/**
 * Migrates records from the projecttype table into Project type terms.
 */
class RPDProjectTypeMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $this->description = 'Migrates records in the projecttype table to RPD Project type taxonomy terms.';

    $query = db_select('projecttype', 't')
      ->fields('t', array('ID', 'NAME', 'COMMENTS'))
      ->orderBy('ID', 'ASC');
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

/**
 * Migrates records from the institution_data table into Institution entities.
 */
class RPDInstitutionMigration extends Migration {
  /**
   * An array of country names keyed by their two-letter code.
   */
  protected $countries = array();

  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $this->description = 'Migrates institution data into institution entities';

    $query = db_select('institution_data', 'i')
      ->fields('i', array('ID', 'INSTITUTION_NAME', 'INSTITUTION_DEPARTMENT', 'INSTITUTION_ADDRESS1', 'INSTITUTION_ADDRESS2', 'INSTITUTION_CITY', 'INSTITUTION_ZIP', 'INSTITUTION_URL'));
    $query->leftJoin('states', 's', 'i.INSTITUTION_STATE = s.id');
    $query->fields('s', array('abbrv'));
    $query->leftJoin('countries', 'c', 'i.INSTITUTION_COUNTRY = c.ID');
    $query->fields('c', array('COUNTRY_NAME'))
      ->orderBy('ID', 'ASC');
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
    $arguments = array(
      'organisation_name' => array('source_field' => 'INSTITUTION_DEPARTMENT'),
      'thoroughfare' => array('source_field' => 'INSTITUTION_ADDRESS1'),
      'premise' => array('source_field' => 'INSTITUTION_ADDRESS2'),
      'locality' => array('source_field' => 'INSTITUTION_CITY'),
      'administrative_area' => array('source_field' => 'abbrv'),
      'postal_code' => array('source_field' => 'INSTITUTION_ZIP'),
    );
    $this->addFieldMapping('rpd_inst_address', 'COUNTRY_NAME')
      ->arguments($arguments)
      ->callbacks(array($this, 'getCountryCode'));
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

/**
 * Migrates records from the investigator_data table into Investigator entities.
 */
class RPDInvestigatorMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $this->dependencies = array('RPDInstitution');
    $this->description = 'Migrates records in the investigator_data table to RPD Investigator entities.';

    $query = db_select('investigator_data', 'i')
      ->fields('i', array('ID', 'name', 'EMAIL_ADDRESS', 'PHONE_NUMBER', 'INSTITUTION'));
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
      ->sourceMigration('RPDInstitution');
  }
}

/**
 * Migrates records from the agency_data table into US Funding Source terms.
 */
class RPDUSFundingSourceMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $this->description = 'Migrates records in the agency_data table to RPD US Funding Source taxonomy terms.';

    $query = db_select('agency_data', 'a')
      ->fields('a', array('ID', 'AGENCY_FULL_NAME', 'AGENCY_ACRONYM', 'AGENCY_URL'))
      ->condition('US_GOVT', 1)
      ->orderBy('ID', 'ASC');
    $query->leftJoin('agency_hierarchy', 'h', 'a.ID = h.aid');
    $query->fields('h', array('parent'));
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_funding_sources_us');

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
      ->sourceMigration('RPDUSFundingSource');
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

/**
 * Migrates records from the agency_data table into Intl. Funding Source terms.
 */
class RPDIntlFundingSourceMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $this->description = 'Migrates records in the agency_data table to RPD Intl. funding source taxonomy terms.';

    $query = db_select('agency_data', 'a')
      ->fields('a', array('ID', 'AGENCY_FULL_NAME', 'AGENCY_ACRONYM', 'AGENCY_URL'))
      ->condition('US_GOVT', 2)
      ->orderBy('ID', 'ASC');
    $query->leftJoin('agency_hierarchy', 'h', 'a.ID = h.aid');
    $query->fields('h', array('parent'));
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_funding_sources_international');

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
      ->sourceMigration('RPDIntlFundingSource');
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

/**
 * Migrates records from the agency_data table into State Funding Source terms.
 */
class RPDStateFundingSourceMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $this->description = 'Migrates records in the agency_data table to RPD State funding source taxonomy terms.';

    $query = db_select('agency_data', 'a')
      ->fields('a', array('ID', 'AGENCY_FULL_NAME', 'AGENCY_ACRONYM', 'AGENCY_URL'))
      ->condition('US_GOVT', 0)
      ->orderBy('ID', 'ASC');
    $query->leftJoin('agency_hierarchy', 'h', 'a.ID = h.aid');
    $query->fields('h', array('parent'));
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
      ->sourceMigration('RPDStateFundingSource');
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
