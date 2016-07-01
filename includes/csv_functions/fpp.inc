<?php
/**
 * Fieldable Panels Panes Functions.
 */

/**
 * Fieldable Panels Panes Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_fpp($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'FPP Bundle Machine Name';
  $headers[1] = 'FPP Bundle Count';
  $headers[2] = 'Field Label';
  $headers[3] = 'Field Machine Name';
  $headers[4] = 'Help Text';
  $headers[5] = 'Settings';

  // print headers to CSV
  fputcsv($f, $headers);

  // Load all the fpps.
  $fpps = entity_load('fieldable_panels_pane');

  // Grab all the bundles
  $bundles = array();
  foreach ($fpps as $info) {
    if (isset($info->bundle)) {
      $bundles[] = $info->bundle;
    }
  }

  // Grab the count of each FPP.
  $counts = array_count_values($bundles);

  // Remove Duplicate
  $bundles = array_unique($bundles);

  foreach ($bundles as $bundle) {
    // Grab the count.
    if (array_key_exists($bundle, $counts)) {
      $count = $counts[$bundle];
    }

    // Grab the fields for the FPP.
    $fields = field_info_instances('fieldable_panels_pane', $bundle);

    // Grab the first row to make this format happier.
    reset($fields);
    $first = key($fields);

    // Go through the loop now.
    foreach ($fields as $key => $field) {

      // Grab the settings if they exist.
      if (isset($field['settings'])) {
        $settings = _kala_migrate_get_field_settings($field['settings']);
      }

      // Set up first row and then tab the rest.
      if ($key === $first) {
        $body['FPP Bundle Machine Name'] = $bundle;
        $body['FPP Bundle Count'] = $count;
        $body['Field Label'] = $field['label'];
        $body['Field Machine Name'] = $field['field_name'];
        $body['Help Text'] = $field['description'];
        $body['Settings'] = isset($settings) ? $settings : '';
      }
      else {
        $body['FPP Bundle Machine Name'] = '';
        $body['FPP Bundle Count'] = '';
        $body['Field Label'] = $field['label'];
        $body['Field Machine Name'] = $field['field_name'];
        $body['Help Text'] = $field['description'];
        $body['Settings'] = isset($settings) ? $settings : '';
      }

      // Throw this in.
      fputcsv($f, $body);
    }
  }
  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
