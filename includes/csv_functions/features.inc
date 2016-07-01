<?php

/**
 * Features Functions.
 */

/**
 * Features Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_features($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Feature Name';
  $headers[1] = 'Feature Machine Name';
  $headers[2] = 'Description';
  $headers[3] = 'Sub Directores';
  $headers[4] = 'Has Custom Functionality in .module';
  $headers[5] = 'Requires';
  $headers[6] = 'Content Types';
  $headers[7] = 'Fields';
  $headers[8] = 'Views';

  // print headers to CSV
  fputcsv($f, $headers);

  // Load up all menus.
  $features = features_get_features();

  foreach ($features as $feature) {

    // Check for various file based items.
    $custom_content = FALSE;
    if (isset($feature->filename)) {

      // Check for subdirectories.
      $path = dirname($feature->filename);
      $subdirs = glob($path . '/*' , GLOB_ONLYDIR);
      if ($subdirs) {
        $sub = '';
        foreach ($subdirs as $subdir) {
          $folder = explode('/', $subdir);
          $sub .= end($folder) . ', ';
        }
        $sub = rtrim($sub, ', ');
      }

      // Check for custom functions, methods, etc.
      $module = file_get_contents($feature->filename);
      if (!empty($module) && strpos($module, 'function') !== FALSE) {
        $custom_content = TRUE;
      }
    }

    // Grab what modules this feature requires.
    $requires = '';
    if (isset($feature->requires)) {
      foreach ($feature->requires as $module_name => $value) {
        $requires .= $module_name . ', ';
      }
      $requires = rtrim($requires, ', ');
    }
    $test = '1';
    // Grab the info of the feature.
    if (isset($feature->info)) {
      // Grab basic info.
      $name = $feature->info['name'];
      $description = isset($feature->info['description']) ?
                     $feature->info['description'] : '';

      // Content Types this uses.
      $nodes = '';
      if (isset($feature->info['features']['node'])) {
        foreach ($feature->info['features']['node'] as $node) {
          $nodes .= $node . ', ';
        }
        $nodes = rtrim($nodes, ', ');
      }

      // Field Instances this uses.
      $fields = '';
      if (isset($feature->info['features']['field_instance'])) {
        foreach ($feature->info['features']['field_instance'] as $field) {
          $fields .= $field . ', ';
        }
        $fields = rtrim($fields, ', ');
      }

      // Views this uses.
      $views = '';
      if (isset($feature->info['features']['views_view'])) {
        foreach ($feature->info['features']['views_view'] as $view) {
          $views .= $view . ', ';
        }
        $views = rtrim($views, ', ');
      }
    }

    // Set up all the body fields.
    $body['Feature Name'] = isset($name) ? $name : '';
    $body['Feature Machine Name'] = $feature->name;
    $body['Description'] = isset($description) ? $description : '';
    $body['Sub Directores'] = isset($sub) ? $sub : '';
    $body['Has Custom Functionality in .module'] = $custom_content ? 'Yes' : 'No';
    $body['Requires'] = $requires;
    $body['Content Types'] = isset($nodes) ? $nodes : '';
    $body['Fields'] = isset($fields) ? $fields : '';
    $body['Views'] = isset($views) ? $views : '';

    // throw it in the file meow.
    fputcsv($f, $body);

  }
  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
