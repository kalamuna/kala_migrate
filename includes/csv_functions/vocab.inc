<?php
/**
 * Vocab Functions.
 */

/**
 * Vocab Processing function.
 *
 * @param  string $filename
 *   The file name for this function.
 *
 * @return string
 *   The file name for this function.
 */
function _kala_migrate_vocab($filename) {
  // Open The CSV
  $f = fopen($filename, 'w');
  // Clear out vars.
  $headers = $body = array();

  $headers[0] = 'Vocab Name';
  $headers[1] = 'Vocab Machine Name';
  $headers[2] = 'Description';
  $headers[3] = 'Pathauto';
  $headers[4] = 'Vocab Field Name';
  $headers[5] = 'Vocab Field Machine Name';
  $headers[6] = 'Vocab Field Description';
  $headers[7] = 'Vocab Field Help Text';
  $headers[8] = 'Vocab Field Settings';

  // print headers to CSV
  fputcsv($f, $headers);

  // Grab the Default Pattern.
  if (module_exists('pathauto')) {
    $default_pattern = _kala_migrate_pathauto_default_pattern('term');
  }

  // Load up all the vocabs.
  $vocabs = taxonomy_vocabulary_load_multiple(FALSE);
  foreach ($vocabs as $vocab) {
    // Checks for overridden patterns.
    if (isset($default_pattern)) {
      $pattern = _kala_migrate_pathauto_overidden_pattern($vocab->machine_name, $default_pattern);
    }

    // Grab the field names in thy vocab.
    $fields = field_info_instances('taxonomy_term', $vocab->machine_name);
    // Grab the first row to make this format happier.
    reset($fields);
    $first = key($fields);
    if (isset($fields) && count($fields) > 0) {
      foreach ($fields as $key => $field) {
        // Grab the settings if they exist.
        if (isset($field['widget']['settings'])) {
          $settings = _kala_migrate_get_field_settings($field['widget']['settings']);
        }

        // Set up first row and then tab the rest.
        if ($key === $first) {
          // Add in all the data on the first row.
          $body['Vocab Name'] = $vocab->name;
          $body['Vocab Machine Name'] = $vocab->machine_name;
          $body['Description'] = $vocab->description;
          $body['Pathauto'] = isset($pattern) ? $pattern : '';
          $body['Vocab Field Name'] = $field['label'];
          $body['Vocab Field Machine Name'] = $field['field_name'];
          $body['Vocab Field Help Text'] = $field['description'];
          $body['Vocab Field Settings'] = isset($settings) ? $settings : '';
        }
        else {
          // Just the fields within the vocab type.
          $body['Vocab Name'] = '';
          $body['Vocab Machine Name'] = '';
          $body['Description'] = '';
          $body['Pathauto'] = '';
          $body['Vocab Field Name'] = $field['label'];
          $body['Vocab Field Machine Name'] = $field['field_name'];
          $body['Vocab Field Help Text'] = $field['description'];
          $body['Vocab Field Settings'] = isset($settings) ? $settings : '';
        }

        // Throw this in.
        fputcsv($f, $body);
      }
    }
    else {
      // No fields, just put this in the CSV.
      $body['Vocab Name'] = $vocab->name;
      $body['Vocab Machine Name'] = $vocab->machine_name;
      $body['Description'] = $vocab->description;
      $body['Pathauto'] = isset($pattern) ? $pattern : '';
      $body['Vocab Field Name'] = '';
      $body['Vocab Field Machine Name'] = '';
      $body['Vocab Field Help Text'] = '';
      $body['Vocab Field Settings'] = '';

      // Throw this in.
      fputcsv($f, $body);
    }
  }

  // Close this and trill this out.
  fclose($f);

  // Return File Name
  return $filename;
}
