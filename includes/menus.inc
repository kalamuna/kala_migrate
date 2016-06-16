<?php
/**
 * Menus Functions.
 */

/**
 * Menus Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_menus($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Menu Name';
  $headers[1] = 'Menu Machine Name';

  // print headers to CSV
  fputcsv($f, $headers);

  // Load up all menus.
  $menus = menu_get_menus();

  foreach ($menus as $key => $menu_name) {
    $body['Menu Name'] = $menu_name;
    $body['Menu Machine Name'] = $key;

    // throw it in the file meow.
    fputcsv($f, $body);

  }
  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
