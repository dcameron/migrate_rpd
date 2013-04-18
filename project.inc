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
      ->fields('p', array('ID', 'PROJECT_NUMBER', 'PROJECT_TITLE', 'PROJECT_START_DATE', 'PROJECT_END_DATE', 'PROJECT_FUNDING', 'JIFSR_CATEGORIES', 'PROJECT_TYPE', 'PROJECT_KEYWORDS', 'PROJECT_IDENTIFIERS', 'PROJECT_ABSTRACT', 'PROJECT_PUBLICATIONS', 'PROJECT_MORE_INFO', 'PROJECT_OBJECTIVE', 'DATE_ENTERED', 'COMMENTS', 'LAST_UPDATE'))
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

    // Add field mappings.
    $this->addFieldMapping('title', 'PROJECT_TITLE');
    $this->addFieldMapping('rpd_project_number', 'PROJECT_NUMBER');
    $this->addFieldMapping('rpd_start_date', 'PROJECT_START_DATE');
    $this->addFieldMapping('rpd_end_date', 'PROJECT_END_DATE');
    $this->addFieldMapping('rpd_funding', 'PROJECT_FUNDING');
    $this->addFieldMapping('rpd_jifsrcats', 'JIFSR_CATEGORIES')
      ->arguments(array('source_type' => 'tid'))
      ->sourceMigration('RPDJIFSRCategory')
      ->defaultValue(0);
    $this->addFieldMapping('rpd_project_type', 'PROJECT_TYPE')
      ->arguments(array('source_type' => 'tid'))
      ->sourceMigration('RPDProjectType')
      ->defaultValue(0);
    $this->addFieldMapping('rpd_keywords', 'PROJECT_KEYWORDS')
      ->arguments(array('format' => 'plain_text'))
      ->defaultValue('');
    $this->addFieldMapping('rpd_identifiers', 'PROJECT_IDENTIFIERS')
      ->arguments(array('format' => 'plain_text'))
      ->defaultValue('');
    $this->addFieldMapping('rpd_abstract', 'PROJECT_ABSTRACT')
      ->defaultValue('');
    $this->addFieldMapping('rpd_abstract:format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('rpd_publications', 'PROJECT_PUBLICATIONS')
      ->defaultValue('')
      ->callbacks('html_entity_decode');
    $this->addFieldMapping('rpd_publications:format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('rpd_more_info', 'PROJECT_MORE_INFO')
      ->defaultValue('')
      ->callbacks('html_entity_decode');
    $this->addFieldMapping('rpd_more_info:format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('body', 'PROJECT_OBJECTIVE')
      ->defaultValue('')
      ->callbacks('html_entity_decode');
    $this->addFieldMapping('body:summary')
      ->defaultValue('');
    $this->addFieldMapping('body:format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('created', 'DATE_ENTERED');
    $this->addFieldMapping('rpd_comments', 'COMMENTS')
      ->arguments(array('format' => 'plain_text'))
      ->defaultValue('');
    $this->addFieldMapping('changed', 'LAST_UPDATE');
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
    $this->addFieldMapping('format')
      ->defaultValue('filtered_html');
    $this->addFieldMapping('rpd_fscat_keywords', 'KEYWORDS')
      ->arguments(array('format' => 'plain_text'))
      ->defaultValue('');
    $this->addFieldMapping('rpd_fscat_identifiers', 'IDENTIFIERS')
      ->arguments(array('format' => 'plain_text'))
      ->defaultValue('');
  }
}

/**
 * Migrates records from the farm_to_table_categories table into terms.
 */
class RPDF2TCategoryMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $query = db_select('farm_to_table_categories', 'c')
      ->fields('c', array('cid', 'name'))
      ->orderBy('cid', 'ASC');
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_farm_to_table_categories');

    // Instantiate the MigrateMap
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
    $this->addFieldMapping('format')
      ->defaultValue('filtered_html');
  }
}

/**
 * Migrates records from the jifsrcategory table into JIFSR category terms.
 */
class RPDJIFSRCategoryMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $query = db_select('jifsrcategory', 'c')
      ->fields('c', array('ID', 'CATEGORY_NAME'))
      ->orderBy('ID', 'ASC');
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_jifsr_categories');

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
    $this->addFieldMapping('name', 'CATEGORY_NAME');
    $this->addFieldMapping('description')
      ->defaultValue('');
    $this->addFieldMapping('format')
      ->defaultValue('filtered_html');
  }
}

/**
 * Migrates records from the projecttype table into Project type terms.
 */
class RPDProjectTypeMigration extends Migration {
  public function __construct() {
    parent::__construct(MigrateGroup::getInstance('rpdproject'));
    $query = db_select('projecttype', 't')
      ->fields('t', array('ID', 'NAME', 'COMMENTS'))
      ->orderBy('ID', 'ASC');
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTerm('rpd_project_types');

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
    $this->addFieldMapping('name', 'NAME');
    $this->addFieldMapping('description', 'COMMENTS');
    $this->addFieldMapping('format')
      ->defaultValue('filtered_html');
  }
}
