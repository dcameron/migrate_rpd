<?php
/**
 * @file
 * Migrates FSRIO's Research Projects Database to Drupal7.
 */

/**
 * Migrates records from the project table into Research project nodes.
 */
class RPDProjectMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $query = db_select('project', 'p')
      ->fields('p', array('ID', 'PROJECT_NUMBER', 'PROJECT_TITLE', 'PROJECT_START_DATE', 'PROJECT_END_DATE', 'PROJECT_FUNDING', 'PROJECT_KEYWORDS', 'PROJECT_IDENTIFIERS', 'PROJECT_MORE_INFO', 'PROJECT_OBJECTIVE', 'COMMENTS'))
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
        ),
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // Add field mappings.
    $this->addFieldMapping('title', 'PROJECT_TITLE');
    $this->addFieldMapping('rpd_project_number', 'PROJECT_NUMBER');
    $this->addFieldMapping('rpd_start_date', 'PROJECT_START_DATE');
    $this->addFieldMapping('rpd_end_date', 'PROJECT_END_DATE');
    $this->addFieldMapping('rpd_funding', 'PROJECT_FUNDING');
    $this->addFieldMapping('rpd_keywords', 'PROJECT_KEYWORDS')
      ->arguments(array('format' => 'plain_text'))
      ->defaultValue('');
    $this->addFieldMapping('rpd_identifiers', 'PROJECT_IDENTIFIERS')
      ->arguments(array('format' => 'plain_text'))
      ->defaultValue('');
    $this->addFieldMapping('rpd_abstract', 'PROJECT_ABSTRACT')
      ->arguments(array('format' => 'filtered_html'))
      ->defaultValue('');
    $this->addFieldMapping('rpd_publications', 'PROJECT_PUBLICATIONS')
      ->arguments(array('format' => 'filtered_html'))
      ->defaultValue('');
    $this->addFieldMapping('rpd_more_info', 'PROJECT_MORE_INFO')
      ->arguments(array('format' => 'filtered_html'))
      ->defaultValue('')
      ->callbacks('html_entity_decode');
    $this->addFieldMapping('body', 'PROJECT_OBJECTIVE')
      ->arguments(array('format' => 'filtered_html'))
      ->defaultValue('')
      ->callbacks('html_entity_decode');
    $this->addFieldMapping('rpd_comments', 'COMMENTS')
      ->arguments(array('format' => 'plain_text'))
      ->defaultValue('');
  }
}

/**
 * Migrates records from the category table into Food safety category terms.
 */
class RPDFSCategoryMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $query = db_select('category', 'c')
      ->fields('c', array('ID', 'CATEGORY_NAME', 'KEYWORDS', 'IDENTIFIERS', 'CATEGORY_DESCRIPTION'))
      ->orderBy('ID', 'ASC');
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_food_safety_categories');

    // Instantiate the MigrateMap
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
    $this->addFieldMapping('rpd_fscat_keywords', 'KEYWORDS')
      ->arguments(array('format' => 'plain_text'))
      ->defaultValue('');
    $this->addFieldMapping('rpd_fscat_identifiers', 'IDENTIFIERS')
      ->arguments(array('format' => 'plain_text'))
      ->defaultValue('');
  }
}

