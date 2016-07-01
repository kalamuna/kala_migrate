<?php
/**
 * Mini Panels Functions.
 */

/**
 * Mini Panels Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_mini_panels($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'MP Name';
  $headers[1] = 'MP Machine Name';
  $headers[2] = 'Description';
  $headers[3] = 'In Feature';
  $headers[4] = 'CTX Filter';
  $headers[5] = 'Layout Machine Name';
  $headers[6] = 'Layout Settings';

  // print headers to CSV
  fputcsv($f, $headers);

  // Grab all the mini panels.
  $minis = panels_mini_load_all();
  foreach ($minis as $mini) {
    // Grab Contexts.
    $ctx = '';
    if (isset($mini->contexts)) {
      $ctx = _kala_migrate_get_field_settings($mini->contexts);
    }

    // Display Settings.
    $layout = $settings = '';
    if (isset($mini->display)) {
      $layout = $mini->display->layout;
      if (isset($mini->display->layout_settings)) {
        $settings = _kala_migrate_get_field_settings($mini->display->layout_settings);
      }
    }

    $body['MP Name'] = $mini->admin_title;
    $body['MP Machine Name'] = $mini->name;
    $body['Description'] = isset($mini->admin_description) ? $mini->admin_description : '';
    $body['In Feature'] = isset($mini->export_module) ? $mini->export_module : '';
    $body['CTX Filter'] = $ctx;
    $body['Layout Machine Name'] = $layout;
    $body['Layout Settings'] = $settings;

    // Throw it all in now.
    fputcsv($f, $body);

  }

  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
