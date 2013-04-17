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
    ),
  );
  return $api;
}
