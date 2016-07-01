<?php
/**
 * Panelizer Functions.
 */

/**
 * Panelizer Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_panelizer($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Panelizer Entity Type';
  $headers[1] = 'Panelizer Entity Bundle';
  $headers[2] = 'Enabled View Mode';

  // print headers to CSV
  fputcsv($f, $headers);

  // Load up all panelized entities.

  ctools_include('plugins');
  $plugins = ctools_get_plugins('panelizer', 'entity');
  foreach ($plugins as $pname => $plugin) {
    // If there are variants, lets do this.
    if (isset($plugin['bundles'])) {
      // Check each bundle to see if active.
      foreach ($plugin['bundles'] as $bkey => $bundle) {
        // If the View mode on the bundle is enabled.
        if ($bundle['status']) {
          // Grab the first key for tabbing.
          reset($bundle['view modes']);
          $first = key($bundle['view modes']);

          // Then lets grab the name and set this up.
          foreach ($bundle['view modes'] as $mkey => $mode) {
            if ($mode['status']) {
              // Set up first row and then tab the rest.
              if ($mkey === $first) {
                $body['Panelizer Entity Type'] = $pname;
                $body['Panelizer Entity Bundle'] = $bkey;
                $body['Enabled View Mode'] = $mkey;
              }
              else {
                $body['Panelizer Entity Type'] = '';
                $body['Panelizer Entity Bundle'] = $bkey;
                $body['Enabled View Mode'] = $mkey;
              }

              fputcsv($f, $body);
            }
          }
        }
      }
    }
  }

  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
