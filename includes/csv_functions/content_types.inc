<?php

/**
 * Content Type Functions.
 */

/**
 * Content Types Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_content_types($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Type Name';
  $headers[1] = 'Type Machine Name';
  $headers[2] = 'Title Label';
  $headers[3] = 'Description';
  $headers[4] = 'Publish';
  $headers[5] = 'Feature';
  $headers[6] = 'Field Name';
  $headers[7] = 'Field Machine Name';
  $headers[8] = 'Pathauto';

  // print headers to CSV
  fputcsv($f, $headers);

  // Grab pathauto default pattern.
  if (module_exists('pathauto')) {
    $default_pattern = _kala_migrate_pathauto_default_pattern('node');
  }

  // Load up all the features.
  if (module_exists('features')) {
    $features = features_get_features();
    foreach ($features as $featname => $feat) {
      // If there are content types in the feature.
      if (isset($feat->info['features']['node'])) {
        foreach ($feat->info['features']['node'] as $node) {
          // Grab the node to check for later in array_search this way.
          // We can just output the key which is the feature name.
          $feature[$featname] = $node;
        }
      }
    }
  }


  // Grab all Content Types
  $types = node_type_get_types();
  foreach ($types as $type) {

    // Checks for overridden patterns.
    if (isset($default_pattern)) {
      $pattern = _kala_migrate_pathauto_overidden_pattern($type->type, $default_pattern);
    }

    // Get the default node options to check for default publishing.
    $workflows = variable_get('node_options_' . $type->type, '');
    $publish = FALSE;
    if (!empty($workflows)) {
      foreach ($workflows as $workflow) {
        if ($workflow == 'status') {
          $publish = TRUE;
        }
      }
    }

    // Check the feature array.
    if (isset($feature)) {
      $check = array_search($type->type, $feature);
      $infeature = $check !== FALSE ? $check : '';
    }

    // Trill in the fields.
    $fields = field_info_instances('node', $type->type);
    // Grab the first row to make this format happier.
    reset($fields);
    $first = key($fields);
    // Go through the loop now.
    foreach ($fields as $key => $field) {
      // Set up first row and then tab the rest.
      if ($key === $first) {
        // Add in all the data on the first row.
        $body['Type Name'] = $type->name;
        $body['Type Machine Name'] = $type->type;
        $body['Title Label'] = $type->has_title ? $type->title_label : '';
        $body['Description'] = $type->description;
        $body['Publish'] = $publish ? 'Y' : 'N';
        $body['Feature'] = isset($infeature) ? $infeature : '';
        $body['Field Name'] = $field['label'];
        $body['Field Machine Name'] = $field['field_name'];
        $body['Pathauto'] = isset($pattern) ? $pattern : '';
      }
      else {
        // Just the fields within the content type.
        $body['Type Name'] = '';
        $body['Type Machine Name'] = '';
        $body['Title Label'] = '';
        $body['Description'] = '';
        $body['Publish'] = '';
        $body['Feature'] = '';
        $body['Field Name'] = $field['label'];
        $body['Field Machine Name'] = $field['field_name'];
        $body['Pathauto'] = '';
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
