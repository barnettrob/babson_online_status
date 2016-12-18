# babson_online_status
Handles users online status across multiple properties.

Uses PDO (PHP Data Objects) instead of the Drupal api layer for connecting to database.
This enables a user's online status changes to be propagated across web properties.
db_set_active Drupal api caused issues with cache.
