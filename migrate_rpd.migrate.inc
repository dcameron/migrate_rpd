<?php
/**
 * @file
 * Defines Migration classes to be loaded.
 */

/**
 * Implements hook_migrate_api().
 */
function migrate_rpd_migrate_api() {
  $api = array(
    'api' => 2,
    'groups' => array(
      'rpd' => array(
        'title' => t('Research projects content'),
        'default_format' => 'filtered_html',
      ),
    ),
    'migrations' => array(
      'Project' => array(
        'class_name' => 'RpdProjectMigration',
        'group_name' => 'rpd',
      ),
      'FSCategory' => array(
        'class_name' => 'RpdFSCategoryMigration',
        'group_name' => 'rpd',
      ),
      'F2TCategory' => array(
        'class_name' => 'RpdF2TCategoryMigration',
        'group_name' => 'rpd',
      ),
      'ActivityStatus' => array(
        'class_name' => 'RpdActivityStatusMigration',
        'group_name' => 'rpd',
      ),
      'ProjectType' => array(
        'class_name' => 'RpdProjectTypeMigration',
        'group_name' => 'rpd',
      ),
      'Institution' => array(
        'class_name' => 'RpdInstitutionMigration',
        'group_name' => 'rpd',
      ),
      'Investigator' => array(
        'class_name' => 'RpdInvestigatorMigration',
        'group_name' => 'rpd',
      ),
      'USFundingSource' => array(
        'class_name' => 'RpdUSFundingSourceMigration',
        'group_name' => 'rpd',
      ),
      'IntlFundingSource' => array(
        'class_name' => 'RpdIntlFundingSourceMigration',
        'group_name' => 'rpd',
      ),
      'StateFundingSource' => array(
        'class_name' => 'RpdStateFundingSourceMigration',
        'group_name' => 'rpd',
      ),
    ),
  );
  return $api;
}
