<?php
/**
 * Views Functions.
 */

/**
 * Views Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_views($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'View Name';
  $headers[1] = 'View Machine Name';
  $headers[2] = 'Base';
  $headers[3] = 'Description';
  $headers[4] = 'Display Type';
  $headers[5] = 'Display Type Name';
  $headers[6] = 'Display Type Machine name';
  $headers[7] = 'Display Type Title';

  // print headers to CSV
  fputcsv($f, $headers);

  // Load up all menus.
  $views = views_get_all_views();

  foreach ($views as $key => $view) {

    $body['View Name'] = $view->human_name;
    $body['View Machine Name'] = $view->name;
    $body['Base'] = $view->base_table;
    $body['Description'] = $view->description;
    $body['Display Type'] = '';
    $body['Display Type Name'] = '';
    $body['Display Type Machine name'] = '';
    $body['Display Type Title'] = '';

    // throw it in the file meow.
    fputcsv($f, $body);

    if (isset($view->display)) {
      foreach ($view->display as $display) {
        if ($display->id == 'default') {
          $body['View Name'] = '';
          $body['View Machine Name'] = '';
          $body['Base'] = '';
          $body['Description'] = '';
          $body['Display Type'] = $display->display_title;
          $body['Display Type Name'] = '';
          $body['Display Type Machine name'] = 'N/A';
          $body['Display Type Title'] = '';
          $master = $display;
        }
        else {
          $test = '1';
          $body['View Name'] = '';
          $body['View Machine Name'] = '';
          $body['Base'] = '';
          $body['Description'] = '';
          $body['Display Type'] = $display->display_plugin;
          $body['Display Type Name'] = $display->display_title;
          $body['Display Type Machine name'] = $display->id;
          $body['Display Type Title'] = $display->display_title;
        }

        fputcsv($f, $body);
      }
    }


    // throw it in the file meow.
    // fputcsv($f, $body);

  }
  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
