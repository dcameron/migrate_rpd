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
    'migrations' => array(
      'RPDProject' => array('class_name' => 'RPDProjectMigration'),
      'RPDFSCategory' => array('class_name' => 'RPDFSCategoryMigration'),
      'RPDF2TCategory' => array('class_name' => 'RPDF2TCategoryMigration'),
      'RPDActivityStatus' => array('class_name' => 'RPDActivityStatusMigration'),
      'RPDProjectType' => array('class_name' => 'RPDProjectTypeMigration'),
      'RPDInstitution' => array('class_name' => 'RPDInstitutionMigration'),
      'RPDInvestigator' => array('class_name' => 'RPDInvestigatorMigration'),
      'RPDUSFundingSource' => array('class_name' => 'RPDUSFundingSourceMigration'),
      'RPDIntlFundingSource' => array('class_name' => 'RPDIntlFundingSourceMigration'),
      'RPDStateFundingSource' => array('class_name' => 'RPDStateFundingSourceMigration'),
    ),
  );
  return $api;
}
