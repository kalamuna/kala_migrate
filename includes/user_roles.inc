<?php
/**
 * User Roles Functions.
 */

/**
 * User Roles Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_user_roles($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Role ID';
  $headers[1] = 'Role Name';

  // print headers to CSV
  fputcsv($f, $headers);

  // Load up user roles.
  $roles = user_roles();

  foreach ($roles as $key => $role) {
    $body['Role ID'] = $key;
    $body['Role Name'] = $role;

    // throw it in the file meow.
    fputcsv($f, $body);

  }
  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
