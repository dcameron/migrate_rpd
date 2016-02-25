
CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation

INTRODUCTION
------------

Current maintainer: David Cameron
Original author: David Cameron <david.a.cameron@sbcglobal.net>

The Migrate FSRIO Research Projects Database was created for the Food Safety
Research Information Office at the National Agricultural Library.  It migrates
FSRIO's legacy MySQL Research Projects Database to Drupal7.

INSTALLATION
------------

Install as usual, see http://drupal.org/node/895232 for further information.

Configure a new database connection in the site's settings.php file.  Give this
database the connection key 'project_data', e.g.
$databases['project_data']['default'] = array(...);.  Import the data to be
migrated into the configured database.  The migration classes will automatically
look for the data tables in this database.  The connection can be removed after
the migrations have been completed.
