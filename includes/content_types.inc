<?php

/**
 * Content Type Functions.
 */

/**
 * Content Types Processing function.
 *
 * @param  string $name file name
 * @return string       file name
 */
function _kala_migrate_content_types($name) {
  // Open The CSV
  $f = fopen($name, 'w');
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
  // @see pathauto_patterns_form in pathauto.admin.inc
  if (module_exists('pathauto')) {
    $all_settings = module_invoke_all('pathauto', 'settings');
    foreach ($all_settings as $settings) {
      if ($settings->token_type == 'node') {
        $default_pattern = $settings->patterndefault;
        break;
      }
    }
  }

  // Load up all the features.
  $features = features_get_features();
  foreach ($features as $name => $feat) {
    // If there are content types in the feature.
    if (isset($feat->info['features']['node'])) {
      foreach ($feat->info['features']['node'] as $node) {
        // Grab the node to check for later in array_search this way.
        // We can just output the key which is the feature name.
        $feature[$name] = $node;
      }
    }
  }


  // Grab all Content Types
  $types = node_type_get_types();
  foreach ($types as $type) {

    // Checks for overridden patterns.
    // @see pathauto_patterns_form in pathauto.admin.inc
    if (isset($default_pattern)) {
      $variable = 'pathauto_node_' . $type->type . '_pattern';
      $pattern = !empty(variable_get($variable, '')) ?
                 variable_get($variable, '') : $default_pattern;
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
    $check = array_search($type->type, $feature);
    $infeature = $check !== FALSE ? $check : 'none';

    // Grab all the fields of the type.
    $body['Type Name'] = $type->name;
    $body['Type Machine Name'] = $type->type;
    $body['Title Label'] = $type->has_title ? $type->title_label : '';
    $body['Description'] = $type->description;
    $body['Publish'] = $publish ? 'Y' : 'N';
    $body['Feature'] = $infeature;
    $body['Field Name'] = '';
    $body['Field Machine Name'] = '';
    $body['Pathauto'] = isset($pattern) ? $pattern : '';

    // Main rowz for types.
    fputcsv($f, $body);

    // Trill in the fields.
    $fields = field_info_instances('node', $type->type);
    foreach ($fields as $field) {
      // Tabbed stuff for fields.
      $body['Type Name'] = '';
      $body['Type Machine Name'] = '';
      $body['Title Label'] = '';
      $body['Description'] = '';
      $body['Publish'] = '';
      $body['Feature'] = '';
      $body['Field Name'] = $field['label'];
      $body['Field Machine Name'] = $field['field_name'];
      $body['Pathauto'] = '';

      // Seperate rows for fieldz.
      fputcsv($f, $body);
    }
  }

  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $name;
}
