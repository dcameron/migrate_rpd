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
      ->fields('p', array('ID', 'PROJECT_TITLE'));
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationNode('research_project');

    // Instantiate the MigrateMap
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

    // Finally we add simple field mappings
    $this->addFieldMapping('title', 'PROJECT_TITLE');
  }
}