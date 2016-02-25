<?php

/**
 * @file
 * Contains \RpdProjectMigration
 */

/**
 * Migrates records from the project table into Research project nodes.
 */
class RpdProjectMigration extends Migration {

  /**
   * An array of Drupal uids keyed by user name.
   */
  protected $users = array();

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->dependencies = array('FSCategory', 'F2TCategory', 'ActivityStatus', 'ProjectType', 'Institution', 'Investigator', 'USFundingSource', 'IntlFundingSource', 'StateFundingSource');
    $this->description = 'Migrates records in the project table to Research project nodes.';

    // Connect to the Research project content database.
    db_set_active('project_data');
    $query = db_select('project', 'p')
      ->fields('p', array('ID', 'PROJECT_NUMBER', 'PROJECT_TITLE', 'PROJECT_START_DATE', 'PROJECT_END_DATE', 'PROJECT_FUNDING', 'PROJECT_TYPE', 'PROJECT_KEYWORDS', 'PROJECT_IDENTIFIERS', 'PROJECT_COOPORATORS', 'PROJECT_ABSTRACT', 'PROJECT_PUBLICATIONS', 'PROJECT_MORE_INFO', 'PROJECT_OBJECTIVE', 'PROJECT_ACCESSION_NUMBER', 'ACTIVITY_STATUS', 'DATE_ENTERED', 'COMMENTS', 'LAST_UPDATE', 'LAST_UPDATE_BY'));
    db_set_active();
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
      ->sourceMigration('FSCategory')
      ->defaultValue(0);
    $this->addFieldMapping('rpd_fscats:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('rpd_project_type', 'PROJECT_TYPE')
      ->sourceMigration('ProjectType')
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
      ->sourceMigration('ActivityStatus');
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
      ->sourceMigration('F2TCategory');
    $this->addFieldMapping('rpd_f2tcats:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('rpd_institutions', 'institutions')
      ->sourceMigration('Institution');
    $this->addFieldMapping('rpd_investigators', 'investigators')
      ->sourceMigration('Investigator');
    $this->addFieldMapping('rpd_funding_sources_us', 'us_funding_sources')
      ->sourceMigration('USFundingSource');
    $this->addFieldMapping('rpd_funding_sources_us:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('rpd_funding_sources_intl', 'intl_funding_sources')
      ->sourceMigration('IntlFundingSource');
    $this->addFieldMapping('rpd_funding_sources_intl:source_type')
      ->defaultValue('tid');
    $this->addFieldMapping('rpd_funding_sources_state', 'state_funding_sources')
      ->sourceMigration('StateFundingSource');
    $this->addFieldMapping('rpd_funding_sources_state:source_type')
      ->defaultValue('tid');
  }

  public function prepareRow($row) {
    db_set_active('project_data');

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

    db_set_active();
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
